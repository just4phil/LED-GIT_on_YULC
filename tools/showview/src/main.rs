// main.rs - showview: zeigt den Ablauf eines Songs der LED-Show als Zeitstrahl (Windows-Programm, nur ansehen).
//
// Aufbau des Fensters:
//   links   Liste der Song-Ordner unter songs/ (nur Songs mit show.yaml lassen sich öffnen)
//   Mitte   der Zeitstrahl (timeline.rs): Parts, Übergänge, eine Spur je Gerät mit Blindern, Overlay, Text,
//           Ausgabestufe, Wünsche des Users
//   unten   Details des angeklickten Parts: Zeiten, Beschreibung wie in der Tabelle, Aufruf je Gerät, Rohangaben
// Oben wählt der Umschalter zwischen zwei Reitern: "Ablauf" (der Zeitstrahl, wie oben beschrieben) und "Bühne"
// (stage.rs: die LED-Matrix läuft mit dem echten Firmware-Code als DLL, sim.rs lädt sie; Bauen: python tools/build_sim.py).
//
// Die Daten kommen von tools/songgen.py --json (export.rs) - das Programm rechnet keine Zeiten selbst und schreibt
// keine Datei. Ändert sich show.yaml oder struktur.xlsx des geöffneten Songs, lädt es den Ablauf von selbst neu
// (jede Sekunde wird der Änderungszeitpunkt der beiden Dateien verglichen).
//
// Starten: im Ordner tools/showview `cargo run --release`, oder target/release/showview.exe direkt. Optional als
// Argument der Projektordner und/oder ein Song: `showview.exe [Projektordner] [Song] [--buehne]`
// (--buehne: gleich im Reiter "Bühne" und den Song sofort abspielen, praktisch zum Ausprobieren).
//
// Rust-Idiome:
//   mod x;               bindet die Datei x.rs als Modul ein
//   Option<T>            Wert oder None; `if let Some(v) = ...` packt ihn aus
//   std::sync::mpsc      Kanal zwischen Threads: der Lade-Thread schickt das Ergebnis, die Oberfläche holt es ab.
//                        So friert das Fenster nicht ein, während Python rechnet (ein Song dauert ~1 s).

mod export;
mod midi_in;
mod model;
mod shape;
mod sim;
mod stage;
mod timeline;

use eframe::egui;
use model::{SongData, SongEntry};
use std::path::PathBuf;
use std::sync::mpsc::{channel, Receiver};
use std::time::{Duration, Instant, SystemTime};

fn main() -> eframe::Result {
    // Argumente: ein vorhandener Ordner = Projektordner, alles andere = Song zum Öffnen
    let mut root_arg = None;
    let mut song_arg = None;
    let mut buehne = false; // --buehne: gleich im Reiter "Bühne" starten und den Song abspielen
    for a in std::env::args().skip(1) {
        if a == "--buehne" { buehne = true }
        else if PathBuf::from(&a).is_dir() { root_arg = Some(a) } else { song_arg = Some(a) }
    }
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([1400.0, 720.0])
            .with_maximized(true) // ein Zeitstrahl braucht Breite: gleich maximiert starten
            .with_title("showview - Song-Ablauf"),
        ..Default::default() // alle übrigen Einstellungen wie vorgegeben
    };
    eframe::run_native(
        "showview",
        options,
        // Die "App-Erzeugung" ist eine Closure, die eframe einmal beim Start aufruft
        Box::new(move |_cc| Ok(Box::new(App::new(export::find_root(root_arg), song_arg, buehne)))),
    )
}

/// Ergebnis eines Ladevorgangs aus dem Hintergrund-Thread.
enum Loaded {
    List(Result<Vec<SongEntry>, String>),
    Song(String, Result<SongData, String>), // Ordnername + Ablauf oder Fehlertext
}

/// Die zwei Reiter des Fensters.
#[derive(PartialEq, Clone, Copy)]
enum Tab {
    Ablauf, // der Zeitstrahl des Songs
    Buehne, // der Simulator: Matrix mit dem echten Firmware-Code
}

struct App {
    tab: Tab,
    stage: stage::Stage,
    root: Option<PathBuf>,
    songs: Vec<SongEntry>,
    filter: String,
    current: Option<String>, // geöffneter Song-Ordner
    data: Option<SongData>,
    error: Option<String>,
    loading: bool,
    rx: Option<Receiver<Loaded>>, // Empfänger des laufenden Ladevorgangs
    view: timeline::TimelineView,
    selected: Option<usize>, // angeklickter Part
    watched: Vec<PathBuf>,
    stamps: Vec<Option<SystemTime>>,
    last_check: Instant,
    want_song: Option<String>, // Song aus der Befehlszeile, wird nach dem Laden der Liste geöffnet
}

impl App {
    fn new(root: Option<PathBuf>, want_song: Option<String>, buehne: bool) -> Self {
        let mut app = Self {
            tab: if buehne { Tab::Buehne } else { Tab::Ablauf },
            stage: stage::Stage::new(buehne),
            root,
            songs: Vec::new(),
            filter: String::new(),
            current: None,
            data: None,
            error: None,
            loading: false,
            rx: None,
            view: Default::default(),
            selected: None,
            watched: Vec::new(),
            stamps: Vec::new(),
            last_check: Instant::now(),
            want_song,
        };
        match app.root.clone() {
            Some(root) => app.spawn(move || Loaded::List(export::load_list(&root).map(|l| l.songs))),
            None => app.error = Some("Projektordner nicht gefunden (kein tools/songgen.py) - als Argument angeben".into()),
        }
        app
    }

    /// Eine Ladeaufgabe im Hintergrund starten. `F: FnOnce() -> Loaded + Send + 'static` heißt: eine Funktion ohne
    /// Argumente, die einmal läuft, ein Loaded liefert und in einen anderen Thread verschoben werden darf.
    fn spawn<F: FnOnce() -> Loaded + Send + 'static>(&mut self, job: F) {
        let (tx, rx) = channel();
        std::thread::spawn(move || {
            let _ = tx.send(job()); // ist das Fenster inzwischen zu, geht das Ergebnis ins Leere - egal
        });
        self.rx = Some(rx);
        self.loading = true;
    }

    /// Einen Song (neu) laden. keep_view = true beim automatischen Neuladen: Zoom und Auswahl bleiben stehen.
    fn open(&mut self, dir: String, keep_view: bool) {
        let Some(root) = self.root.clone() else { return };
        self.watched = export::watched_files(&root, &dir);
        self.stamps = export::stamps(&self.watched);
        if !keep_view {
            self.selected = None;
            self.view.fit();
        }
        self.current = Some(dir.clone());
        self.spawn(move || {
            let r = export::load_song(&root, &dir);
            Loaded::Song(dir, r)
        });
    }

    /// Fertige Ergebnisse abholen (ohne zu warten) und die Dateien des Songs auf Änderungen prüfen.
    fn poll(&mut self) {
        if let Some(rx) = &self.rx {
            if let Ok(msg) = rx.try_recv() {
                self.loading = false;
                self.rx = None;
                match msg {
                    Loaded::List(Ok(list)) => {
                        self.songs = list;
                        if let Some(want) = self.want_song.take() {
                            let w = want.to_lowercase();
                            if let Some(s) = self.songs.iter().find(|s| s.dir.to_lowercase().starts_with(&w)) {
                                let d = s.dir.clone();
                                self.open(d, false);
                            }
                        }
                    }
                    Loaded::List(Err(e)) => self.error = Some(e),
                    Loaded::Song(dir, res) if Some(&dir) == self.current.as_ref() => match res {
                        Ok(d) => {
                            if self.selected.is_some_and(|i| i >= d.parts.len()) {
                                self.selected = None;
                            }
                            self.data = Some(d);
                            self.error = None;
                        }
                        // beim Neuladen nach einer Änderung bleibt der alte Ablauf sichtbar, darüber der Fehler
                        Err(e) => self.error = Some(e),
                    },
                    Loaded::Song(..) => {} // Ergebnis eines Songs, der inzwischen nicht mehr gewählt ist
                }
            }
        }
        if !self.loading && self.current.is_some() && self.last_check.elapsed() > Duration::from_secs(1) {
            self.last_check = Instant::now();
            let now = export::stamps(&self.watched);
            if now != self.stamps {
                let dir = self.current.clone().unwrap();
                self.open(dir, true);
            }
        }
    }

    /// Linke Spalte: Song-Liste mit Suchfeld.
    fn song_list(&mut self, ui: &mut egui::Ui) {
        ui.heading("Songs");
        ui.add(egui::TextEdit::singleline(&mut self.filter).hint_text("suchen ..."));
        ui.separator();
        let mut open = None;
        egui::ScrollArea::vertical().show(ui, |ui| {
            let f = self.filter.to_lowercase();
            for s in &self.songs {
                if !f.is_empty() && !s.dir.to_lowercase().contains(&f) && !s.name.clone().unwrap_or_default().to_lowercase().contains(&f) {
                    continue;
                }
                let label = match s.id {
                    Some(id) => format!("{:>3}  {}", id, s.dir),
                    None => format!("     {}", s.dir),
                };
                // Extend = nicht umbrechen (lange Ordnernamen laufen rechts aus dem Panel, statt zweizeilig zu werden)
                let text = egui::RichText::new(label).monospace();
                let is_cur = self.current.as_deref() == Some(s.dir.as_str());
                // add_enabled: ohne show.yaml gibt es keinen Ablauf, der Eintrag ist dann ausgegraut
                let r = ui.add_enabled(s.show, egui::Button::selectable(is_cur, text).wrap_mode(egui::TextWrapMode::Extend));
                if r.clicked() {
                    open = Some(s.dir.clone());
                }
            }
        });
        if let Some(d) = open {
            self.open(d, false);
        }
    }

    /// Untere Spalte: alles über den angeklickten Part.
    fn details(&self, ui: &mut egui::Ui) {
        let Some(data) = &self.data else { return };
        let Some(part) = self.selected.and_then(|i| data.parts.get(i)) else {
            ui.label("Einen Part im Zeitstrahl anklicken, um seine Details zu sehen.  (Mausrad = Zoom, Ziehen = verschieben, Doppelklick = ganzer Song)");
            return;
        };
        egui::ScrollArea::vertical().auto_shrink([false, false]).show(ui, |ui| {
            ui.heading(format!("{}  (case {} → {})", part.name, part.case, part.next));
            let bars = match (part.bar_from, part.bar_to) {
                (Some(a), Some(b)) => format!(", Takt {a} bis vor {b}"),
                _ => String::new(),
            };
            ui.label(format!(
                "{} – {}  ·  {:.0} ms  ·  {:.0} BPM{}",
                timeline::fmt_time(part.start),
                timeline::fmt_time(part.end()),
                part.dur,
                part.bpm,
                bars
            ));
            ui.separator();
            for l in &part.lines {
                ui.label(l);
            }
            for e in &part.errors {
                ui.colored_label(egui::Color32::LIGHT_RED, e);
            }
            ui.separator();
            egui::Grid::new("devs").striped(true).show(ui, |ui| {
                for d in &data.stage {
                    if let Some(c) = part.devices.get(d) {
                        ui.label(d);
                        let t = egui::RichText::new(&c.call).monospace();
                        ui.label(if c.own { t.color(egui::Color32::from_rgb(255, 230, 120)) } else { t });
                        ui.end_row();
                    }
                }
            });
            ui.collapsing("Rohangaben (show.yaml + Tabelle)", |ui| {
                let text = serde_json::to_string_pretty(&part.sec).unwrap_or_default();
                ui.label(egui::RichText::new(text).monospace());
            });
        });
    }
}

impl eframe::App for App {
    fn ui(&mut self, ui: &mut egui::Ui, _frame: &mut eframe::Frame) {
        self.poll();
        // regelmäßig neu zeichnen, damit poll() Ergebnisse und Dateiänderungen auch ohne Mausbewegung bemerkt
        ui.ctx().request_repaint_after(Duration::from_millis(if self.loading { 100 } else { 500 }));

        // Reihenfolge wichtig: jedes Panel nimmt sich seinen Rand vom noch freien Platz, die Mitte bekommt den Rest
        if self.tab == Tab::Ablauf {
            egui::Panel::bottom("details").resizable(true).default_size(240.0).min_size(160.0).show(ui, |ui| self.details(ui));
        }
        egui::Panel::left("songs").resizable(true).default_size(290.0).show(ui, |ui| self.song_list(ui));
        egui::Panel::top("head").show(ui, |ui| {
            // Umschalter der Reiter: selectable_value setzt self.tab auf den angeklickten Wert
            ui.horizontal(|ui| {
                ui.selectable_value(&mut self.tab, Tab::Ablauf, "Ablauf");
                ui.selectable_value(&mut self.tab, Tab::Buehne, "Bühne");
            });
            ui.horizontal(|ui| {
                if let Some(d) = &self.data {
                    let s = &d.song;
                    ui.strong(format!("#{} {}", s.id.unwrap_or(0), s.name.clone().unwrap_or_else(|| s.dir.clone())));
                    ui.label(format!(
                        "{} BPM · Start-MIDI {} · {} Parts · {}{}",
                        s.bpm,
                        s.midi_offset,
                        d.parts.len(),
                        s.scheme.clone().unwrap_or_default(),
                        s.next_song.as_ref().map(|n| format!(" · weiter in {n}")).unwrap_or_default()
                    ));
                    if d.schema > model::SCHEMA {
                        ui.colored_label(egui::Color32::YELLOW, "(neueres JSON-Format von songgen.py - showview neu bauen)");
                    }
                } else {
                    ui.label("Kein Song geöffnet");
                }
                if self.loading {
                    ui.spinner();
                }
            });
            if let Some(e) = &self.error {
                ui.colored_label(egui::Color32::LIGHT_RED, e);
            }
            if let Some(d) = &self.data {
                for e in &d.errors {
                    ui.colored_label(egui::Color32::LIGHT_RED, format!("songgen.py: {e}"));
                }
            }
        });
        egui::CentralPanel::default().show(ui, |ui| match self.tab {
            Tab::Ablauf => {
                if let Some(d) = &self.data {
                    let c = self.view.show(ui, d, self.selected);
                    if c.part.is_some() {
                        self.selected = c.part;
                    }
                }
            }
            Tab::Buehne => {
                // Der Song des Simulators = der links gewählte Song (Song-ID aus der Liste bzw. aus dem Ablauf)
                let song = self.current.as_ref().and_then(|dir| {
                    let from_list = self.songs.iter().find(|s| &s.dir == dir).and_then(|s| s.id);
                    let from_data = self.data.as_ref().and_then(|d| d.song.id);
                    from_list.or(from_data).map(|id| (id, dir.clone()))
                });
                self.stage.show(ui, self.root.as_deref(), song);
            }
        });
    }
}
