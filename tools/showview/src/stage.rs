// stage.rs - der Reiter "Bühne": zeigt die LED-Matrix (SCROLLMATRIX) so, wie die ECHTE Firmware sie berechnet.
//
// Die Firmware-Quellen laufen als DLL (sim/bin/sim_scrollmatrix.dll, Rust-Hülle in sim.rs). Dieser Reiter
//   - hat die Knöpfe Start (DLL neu laden, Einschalten, MIDI "Kanal 10, CC 22 = Song") und Stop (CC 22 = 0),
//   - stellt bei jedem Bild des Fensters die Simulator-Uhr um die vergangene Echtzeit vor (sim_advance),
//   - holt das zuletzt gesendete Bild und malt die 54 x 10 LEDs als runde, leuchtende Punkte.
// Die Grafik füllt den freien Platz und skaliert mit dem Fenster (Seitenverhältnis bleibt, zentriert).
//
// Wie in egui gezeichnet wird, steht in timeline.rs: Immediate-Mode - jedes Bild wird komplett neu gemalt, gemerkt
// wird nur, was zwischen zwei Bildern bleiben muss (struct Stage).
//
// Rust-Idiome: Option<T> = Wert oder None; `if let Some(x) = ...` packt ihn aus; `&mut self` = die Funktion darf den
// Zustand der Struktur verändern.

use crate::sim::Sim;
use eframe::egui::{self, Color32, Pos2, Rect, Sense, Stroke, StrokeKind, Ui, Vec2};
use std::path::{Path, PathBuf};
use std::time::Instant;

/// Name der DLL der Matrix (je Gerät eine, siehe tools/build_sim.py).
const MATRIX_DLL: &str = "sim_scrollmatrix.dll";
/// Höchstens so viele ms werden pro Fensterbild vorgestellt (z. B. nach dem Ziehen des Fensters stand die Anzeige
/// still - dann nicht in einem Sprung nachholen, sondern gedeckelt weiterlaufen).
const MAX_STEP_MS: f64 = 250.0;

/// Was zwischen zwei Bildern erhalten bleibt.
#[derive(Default)]
pub struct Stage {
    sim: Option<Sim>,           // die geladene DLL (None = noch nicht gestartet)
    running: bool,              // läuft die Uhr?
    last: Option<Instant>,      // Zeitpunkt des vorigen Fensterbildes (Echtzeit)
    rest_ms: f64,               // Bruchteile von Millisekunden, die noch nicht vorgestellt wurden
    error: Option<String>,      // Fehlertext (DLL fehlt, nicht ladbar ...)
    led_of_xy: Vec<usize>,      // LED-Nummer je Matrix-Koordinate (y * Breite + x), einmal beim Start aus sim_xy() geholt
    rgb: Vec<u8>,               // zuletzt geholtes Bild (RGB je LED)
    brightness: i32,            // Helligkeit des Bildes (0..255)
    frame_no: i32,              // Bildnummer der DLL
    state: (i32, i32, u32),     // (Song, Part, ms im Part)
    bright_view: bool,          // true = aufgehellte Darstellung (dunkle Effekte bleiben sichtbar)
    auto_start: bool,       // true = den Song beim ersten Zeichnen sofort starten (Befehlszeile --buehne), danach wieder false
    bright_init: bool,          // wurde bright_view schon einmal auf den Standard gesetzt?
}

impl Stage {
    /// Neuer, leerer Reiter. auto_start = true: beim ersten Zeichnen den gewählten Song gleich starten (--buehne).
    pub fn new(auto_start: bool) -> Self {
        Self { auto_start, ..Default::default() } // alle übrigen Felder auf ihren Standardwert (0, None, leer, false)
    }

    /// Pfad der DLL im Projektordner.
    fn dll_path(root: &Path) -> PathBuf {
        root.join("sim").join("bin").join(MATRIX_DLL)
    }

    /// Start: DLL frisch laden (leeres Gedächtnis wie nach dem Einschalten), einschalten, Song per MIDI wählen.
    fn start(&mut self, root: &Path, song_id: i64) {
        self.sim = None; // alte DLL entladen (Drop)
        match Sim::load(&Self::dll_path(root)) {
            Ok(mut sim) => {
                // Zufallsstartwert aus der Uhrzeit: wie ein echtes Einschalten, Zufallseffekte sehen jedes Mal anders aus
                let seed = std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .map(|d| d.as_millis() as u32)
                    .unwrap_or(1);
                sim.reset(seed);
                // Zuordnung Koordinate -> LED-Nummer einmal holen (die Verdrahtung ändert sich nicht)
                let (w, h) = (sim.info.matrix_w, sim.info.matrix_h);
                self.led_of_xy = (0..h).flat_map(|y| (0..w).map(move |x| (x, y))).map(|(x, y)| sim.xy(x, y).unwrap_or(0)).collect();
                self.rgb = vec![0; sim.info.leds_out as usize * 3];
                sim.midi(10, 22, song_id as i32); // Kanal 10, CC 22 = Song wählen (wie von der Gitarre per Funk)
                self.sim = Some(sim);
                self.running = true;
                self.last = Some(Instant::now());
                self.rest_ms = 0.0;
                self.error = None;
            }
            Err(e) => self.error = Some(e),
        }
    }

    /// Stop: wie am Gerät "Song 0" (Songpause) wählen. Die Uhr läuft weiter, die Songpause ist ja auch eine Show.
    fn stop(&mut self) {
        if let Some(sim) = self.sim.as_mut() {
            sim.midi(10, 22, 0);
        }
    }

    /// Uhr vorstellen und das neueste Bild holen (einmal pro Fensterbild).
    fn tick(&mut self) {
        let now = Instant::now();
        let Some(sim) = self.sim.as_mut() else { return };
        if self.running {
            if let Some(last) = self.last {
                // Echtzeit seit dem letzten Bild in Millisekunden, gedeckelt; ganze ms an die DLL, den Rest merken
                let dt = (now - last).as_secs_f64() * 1000.0;
                self.rest_ms += dt.min(MAX_STEP_MS);
                let whole = self.rest_ms.floor();
                self.rest_ms -= whole;
                if whole >= 1.0 {
                    sim.advance(whole as u32);
                }
            }
        }
        self.last = Some(now);
        let (n, b) = sim.frame(&mut self.rgb);
        self.frame_no = n;
        self.brightness = b;
        self.state = sim.state();
    }

    /// Den Reiter zeichnen. `root` = Projektordner, `song` = (Song-ID, Name) des links gewählten Songs.
    pub fn show(&mut self, ui: &mut Ui, root: Option<&Path>, song: Option<(i64, String)>) {
        if !self.bright_init {
            self.bright_view = true; // Standard: aufgehellt (auf dem Bildschirm wirkt "echte" Helligkeit viel dunkler als auf der Bühne)
            self.bright_init = true;
        }
        let Some(root) = root else {
            ui.label("Projektordner nicht gefunden.");
            return;
        };
        let dll_ok = Self::dll_path(root).is_file();

        // Befehlszeile --buehne: sobald ein Song bekannt und die DLL da ist, einmal automatisch starten
        if self.auto_start && dll_ok {
            if let Some((id, _)) = &song {
                self.auto_start = false;
                self.start(root, *id);
            }
        }

        // --- Kopfzeile: Song, Start/Stop, Darstellung ---
        ui.horizontal(|ui| {
            match &song {
                Some((id, name)) => ui.strong(format!("Song {id}  {name}")),
                None => ui.label("Links einen Song mit Song-ID wählen (generierte Songs)."),
            };
            if ui.add_enabled(dll_ok && song.is_some(), egui::Button::new("▶ Start")).clicked() {
                if let Some((id, _)) = &song {
                    self.start(root, *id);
                }
            }
            if ui.add_enabled(self.sim.is_some(), egui::Button::new("■ Stop")).clicked() {
                self.stop();
            }
            ui.checkbox(&mut self.bright_view, "aufgehellt").on_hover_text(
                "aus = Farbe mal Helligkeit des Geräts (wie echt, am Bildschirm sehr dunkel); an = Bildschirm-freundlich aufgehellt",
            );
        });
        if !dll_ok {
            ui.colored_label(Color32::YELLOW, format!("{MATRIX_DLL} fehlt - erst bauen: python tools/build_sim.py"));
        }
        if let Some(e) = &self.error {
            ui.colored_label(Color32::LIGHT_RED, e);
        }

        // --- Uhr stellen, Bild holen ---
        self.tick();
        if self.sim.is_some() && self.running {
            ui.ctx().request_repaint(); // solange es läuft, sofort das nächste Bild (Zeit läuft in Echtzeit weiter)
        }

        // --- Matrix zeichnen: Platz = alles, was frei ist, abzüglich einer Zeile für die Statusanzeige ---
        let status_h = 28.0;
        let avail = ui.available_size();
        let area = Vec2::new(avail.x.max(10.0), (avail.y - status_h).max(10.0));
        let (rect, _resp) = ui.allocate_exact_size(area, Sense::hover());
        let painter = ui.painter_at(rect);
        painter.rect_filled(rect, 0.0, Color32::from_rgb(8, 8, 10)); // dunkler Hintergrund (die "Bühne")

        if let Some(sim) = &self.sim {
            let (w, h) = (sim.info.matrix_w as usize, sim.info.matrix_h as usize);
            // Rasterweite so, dass alle Spalten und Zeilen hineinpassen: das kleinere von Breite/Spalten und Höhe/Zeilen.
            // Der Rand von 8 px verhindert, dass die äußeren Punkte am Fensterrand kleben.
            let margin = 8.0;
            let pitch = ((rect.width() - 2.0 * margin) / w as f32).min((rect.height() - 2.0 * margin) / h as f32).max(1.0);
            let grid = Vec2::new(pitch * w as f32, pitch * h as f32);
            let origin = rect.center() - grid / 2.0; // zentriert
            let radius = pitch * 0.42;
            // Anzeige-Helligkeit: "echt" = brightness/255; "aufgehellt" = Wurzel davon (80/255 -> 0,56)
            let b = self.brightness.clamp(0, 255) as f32 / 255.0;
            let gain = if self.bright_view { b.sqrt() } else { b };
            for y in 0..h {
                for x in 0..w {
                    let led = self.led_of_xy.get(y * w + x).copied().unwrap_or(0);
                    let (r, g, bl) = (self.rgb[led * 3], self.rgb[led * 3 + 1], self.rgb[led * 3 + 2]);
                    let c = Pos2::new(origin.x + (x as f32 + 0.5) * pitch, origin.y + (y as f32 + 0.5) * pitch);
                    let col = Color32::from_rgb((r as f32 * gain) as u8, (g as f32 * gain) as u8, (bl as f32 * gain) as u8);
                    let luma = r.max(g).max(bl) as f32 * gain;
                    if luma > 40.0 {
                        // leuchtender Hof: größerer, durchsichtiger Kreis in der LED-Farbe
                        painter.circle_filled(c, radius * 1.7, Color32::from_rgba_unmultiplied(col.r(), col.g(), col.b(), 26));
                    }
                    painter.circle_filled(c, radius, if luma < 6.0 { Color32::from_rgb(22, 22, 26) } else { col }); // aus = dunkelgrauer Punkt
                }
            }
            painter.rect_stroke(Rect::from_min_size(origin, grid).expand(4.0), 4.0, Stroke::new(1.0, Color32::from_gray(50)), StrokeKind::Outside);
        } else if dll_ok {
            painter.text(rect.center(), egui::Align2::CENTER_CENTER, "Song links wählen und ▶ Start drücken", egui::FontId::proportional(16.0), Color32::GRAY);
        }

        // --- Statusanzeige unter der Matrix ---
        if let Some(sim) = &self.sim {
            let (song_id, part, ms) = self.state;
            ui.label(format!(
                "{}   Song {song_id}   Part {part}   {:.1} s im Part   Bild {}   Helligkeit {}",
                sim.info.name(),
                ms as f64 / 1000.0,
                self.frame_no,
                self.brightness
            ));
        }
    }
}
