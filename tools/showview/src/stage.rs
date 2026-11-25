// stage.rs - der Reiter "Bühne": zeigt Bass, Matrix und Gitarre so, wie die ECHTE Firmware sie berechnet.
//
// Die Firmware-Quellen laufen als DLL, je Gerät eine (sim/bin/sim_rinasbass.dll, sim_scrollmatrix.dll,
// sim_andresgit.dll; Rust-Hülle in sim.rs, Bauen mit python tools/build_sim.py). Dieser Reiter
//   - hat die Knöpfe Start (alle DLLs frisch laden, Einschalten, MIDI "Kanal 10, CC 22 = Song") und Stop (CC 22 = 0),
//   - stellt bei jedem Bild des Fensters die Simulator-Uhr ALLER Geräte um dieselbe vergangene Echtzeit vor
//     (sim_advance) - alle laufen auf einer gemeinsamen Uhr und bekommen dieselben Befehle,
//   - holt von jedem Gerät das zuletzt gesendete Bild und malt es: die Matrix als Raster, Gitarre und Bass als LED-Kette
//     auf der SG-Kontur (Orte: shape.rs) samt Bund-Markern (Ausgang 1) und Gurt (Ausgang 2, ohne Marker).
// Fehlt die DLL eines Geräts, läuft der Rest weiter; oben steht dann, was zu bauen ist.
//
// ANORDNUNG (vorläufig, die endgültige kommt mit den Lampen in einem eigenen Issue): Bass links, Matrix Mitte,
// Gitarre rechts. Alle drei haben denselben Maßstab (ein Gerät ist so groß wie die Gitarre in Foto-Pixeln; die
// Matrix hat denselben LED-Abstand wie die Kontur - ANNAHME, die echte Rastergröße ist unbekannt). Die ganze
// Grafik liegt in einem eigenen Koordinatensystem und wird erst beim Zeichnen auf den freien Platz abgebildet: sie
// füllt das Fenster, das Seitenverhältnis bleibt, nichts wird verzerrt.
//
// FARBKORREKTUR: die Firmware stellt FastLED.setCorrection(TypicalLEDStrip) ein (LED_COLOR_CORRECTION in
// definitions.h): Rot 255, Grün 176, Blau 240 von 255 - erst beim Senden, also NICHT in den Puffern, die sim_frame()
// liefert. Die Anzeige rechnet sie darum für alle Geräte nach (Schalter "Farbkorrektur").
//
// Wie in egui gezeichnet wird, steht in timeline.rs: Immediate-Mode - jedes Bild wird komplett neu gemalt, gemerkt
// wird nur, was zwischen zwei Bildern bleiben muss (struct Stage).
//
// Rust-Idiome: Option<T> = Wert oder None; `if let Some(x) = ...` packt ihn aus; `&mut self` = die Funktion darf den
// Zustand der Struktur verändern; `Vec<T>` = wachsende Liste; `|a| ...` = kleine namenlose Funktion (Closure).

use crate::shape::{Pt, Shape};
use crate::sim::Sim;
use eframe::egui::{self, Color32, Painter, Pos2, Rect, Sense, Stroke, StrokeKind, Ui, Vec2};
use std::path::{Path, PathBuf};
use std::time::Instant;

/// Wie ein Gerät gezeichnet wird.
#[derive(Clone, Copy, PartialEq)]
enum Kind {
    Instrument, // Gitarre / Bass: LED-Kette auf der SG-Kontur + Gurt
    Matrix,     // Raster
}

/// Ein Gerät der Bühne: DLL-Name, Beschriftung, Zeichenart. Reihenfolge = von links nach rechts.
struct Spec {
    dll: &'static str,
    label: &'static str,
    kind: Kind,
}
const SPECS: [Spec; 3] = [
    Spec { dll: "sim_rinasbass.dll", label: "Bass", kind: Kind::Instrument },
    Spec { dll: "sim_scrollmatrix.dll", label: "Matrix", kind: Kind::Matrix },
    Spec { dll: "sim_andresgit.dll", label: "Gitarre", kind: Kind::Instrument },
];

/// Höchstens so viele ms werden pro Fensterbild vorgestellt (z. B. nach dem Ziehen des Fensters stand die Anzeige
/// still - dann nicht in einem Sprung nachholen, sondern gedeckelt weiterlaufen).
const MAX_STEP_MS: f64 = 250.0;
/// Lücke zwischen zwei Geräten in Foto-Pixeln der Grafik.
const GAP: f32 = 90.0;
/// Rand um jedes Gerät in Foto-Pixeln (Platz für den Leuchthof der äußeren LEDs).
const PAD: f32 = 40.0;
/// LED-Abstand der Matrix, falls kein Instrument geladen ist (sonst der der Kontur): so groß wie ein LED-Abstand der SG.
const DEFAULT_PITCH: f32 = 21.0;
/// Farbkorrektur TypicalLEDStrip (0xFFB0F0): Faktor je Farbanteil, wie er beim Senden wirkt.
const CORRECTION: [f32; 3] = [255.0 / 255.0, 176.0 / 255.0, 240.0 / 255.0];

/// Ein geladenes Gerät mit seinem letzten Bild.
struct Device {
    spec: &'static Spec,
    sim: Sim,
    rgb1: Vec<u8>,          // zuletzt geholtes Bild von Ausgang 1 (RGB je LED)
    rgb2: Vec<u8>,          // dasselbe von Ausgang 2 (Gurt bei Gitarre/Bass)
    brightness: i32,        // Helligkeit des Bildes (0..255)
    frame_no: i32,          // Bildnummer der DLL
    state: (i32, i32, u32), // (Song, Part, ms im Part)
    led_of_xy: Vec<usize>,  // nur Matrix: LED-Nummer je Koordinate (y * Breite + x), einmal beim Start aus sim_xy() geholt
    shape: Option<Shape>,   // nur Instrument: Kontur und LED-Orte
}

/// Was zwischen zwei Bildern erhalten bleibt.
#[derive(Default)]
pub struct Stage {
    devices: Vec<Device>,   // die geladenen Geräte (leer = noch nicht gestartet)
    running: bool,          // läuft die Uhr?
    last: Option<Instant>,  // Zeitpunkt des vorigen Fensterbildes (Echtzeit)
    rest_ms: f64,           // Bruchteile von Millisekunden, die noch nicht vorgestellt wurden
    error: Option<String>,  // Fehlertext (DLL nicht ladbar ...)
    bright_view: bool,      // true = aufgehellte Darstellung (dunkle Effekte bleiben sichtbar)
    auto_start: bool,       // true = den Song beim ersten Zeichnen sofort starten (Befehlszeile --buehne), danach wieder false
    inited: bool,           // wurden die Standardwerte der Schalter schon einmal gesetzt?
    correction: bool,       // true = Farbkorrektur der LED-Streifen nachrechnen
}

impl Stage {
    /// Neuer, leerer Reiter. auto_start = true: beim ersten Zeichnen den gewählten Song gleich starten (--buehne).
    pub fn new(auto_start: bool) -> Self {
        Self { auto_start, ..Default::default() } // alle übrigen Felder auf ihren Standardwert (0, None, leer, false)
    }

    /// Pfad der DLL eines Geräts im Projektordner.
    fn dll_path(root: &Path, spec: &Spec) -> PathBuf {
        root.join("sim").join("bin").join(spec.dll)
    }

    /// Start: alle vorhandenen DLLs frisch laden (leeres Gedächtnis wie nach dem Einschalten), einschalten, Song per MIDI wählen.
    fn start(&mut self, root: &Path, song_id: i64) {
        self.devices.clear(); // alte DLLs entladen (Drop)
        self.error = None;
        // Zufallsstartwert aus der Uhrzeit: wie ein echtes Einschalten, Zufallseffekte sehen jedes Mal anders aus.
        // Jedes Gerät bekommt einen eigenen (Startwert + Nummer): auf der Bühne würfeln die Geräte auch unabhängig.
        let seed = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis() as u32)
            .unwrap_or(1);
        for (n, spec) in SPECS.iter().enumerate() {
            let path = Self::dll_path(root, spec);
            if !path.is_file() {
                continue; // fehlende DLL: dieses Gerät fehlt, die anderen laufen
            }
            match Sim::load(&path) {
                Ok(mut sim) => {
                    sim.reset(seed.wrapping_add(n as u32));
                    let info = sim.info;
                    // Je nach Zeichenart die Geometrie bestimmen (vor dem Verschieben von `sim` ins Device)
                    let (mut led_of_xy, mut shape) = (Vec::new(), None);
                    match spec.kind {
                        Kind::Matrix => {
                            // Zuordnung Koordinate -> LED-Nummer einmal holen (die Verdrahtung ändert sich nicht)
                            let (w, h) = (info.matrix_w, info.matrix_h);
                            led_of_xy = (0..h).flat_map(|y| (0..w).map(move |x| (x, y))).map(|(x, y)| sim.xy(x, y).unwrap_or(0)).collect();
                        }
                        Kind::Instrument => {
                            shape = Some(Shape::new(
                                info.num_leds as usize,
                                info.head_tip_idx,
                                info.loop_dir,
                                info.strap_pin_pos,
                                info.strap_leds as usize,
                                info.zone_neck_up_start,
                                info.zone_head_up_start,
                                info.bund_min,
                                info.bund_max,
                            ));
                        }
                    }
                    sim.midi(10, 22, song_id as i32); // Kanal 10, CC 22 = Song wählen (wie von der Gitarre per Funk)
                    self.devices.push(Device {
                        spec,
                        sim,
                        rgb1: vec![0; info.leds_out as usize * 3],
                        rgb2: vec![0; info.leds_out as usize * 3],
                        brightness: 0,
                        frame_no: 0,
                        state: (0, 0, 0),
                        led_of_xy,
                        shape,
                    });
                }
                Err(e) => self.error = Some(format!("{}: {e}", spec.label)),
            }
        }
        self.running = !self.devices.is_empty();
        self.last = Some(Instant::now());
        self.rest_ms = 0.0;
    }

    /// Stop: wie am Gerät "Song 0" (Songpause) wählen. Die Uhr läuft weiter, die Songpause ist ja auch eine Show.
    fn stop(&mut self) {
        for d in self.devices.iter_mut() {
            d.sim.midi(10, 22, 0);
        }
    }

    /// Uhr vorstellen und die neuesten Bilder holen (einmal pro Fensterbild).
    fn tick(&mut self) {
        let now = Instant::now();
        let mut whole = 0.0;
        if self.running {
            if let Some(last) = self.last {
                // Echtzeit seit dem letzten Bild in Millisekunden, gedeckelt; ganze ms an die DLLs, den Rest merken
                let dt = (now - last).as_secs_f64() * 1000.0;
                self.rest_ms += dt.min(MAX_STEP_MS);
                whole = self.rest_ms.floor();
                self.rest_ms -= whole;
            }
        }
        self.last = Some(now);
        for d in self.devices.iter_mut() {
            if whole >= 1.0 {
                d.sim.advance(whole as u32); // alle Geräte um dieselbe Zeit: gemeinsame Uhr
            }
            let has_strap = d.sim.info.strap_leds > 0;
            let (n, b) = d.sim.frame(&mut d.rgb1, if has_strap { Some(&mut d.rgb2) } else { None });
            d.frame_no = n;
            d.brightness = b;
            d.state = d.sim.state();
        }
    }

    /// Farbe einer LED auf dem Bildschirm: Rohwert aus dem Puffer, Farbkorrektur, Helligkeit des Geräts.
    /// Rückgabe: (Farbe, größter Farbanteil nach Korrektur und Helligkeit als Maß für "leuchtet").
    fn led_color(&self, rgb: &[u8], gain: f32) -> (Color32, f32) {
        let k = if self.correction { CORRECTION } else { [1.0; 3] };
        let c = |i: usize| (rgb[i] as f32 * k[i] * gain).min(255.0);
        let (r, g, b) = (c(0), c(1), c(2));
        (Color32::from_rgb(r as u8, g as u8, b as u8), r.max(g).max(b))
    }

    /// Eine LED als Punkt mit Hof zeichnen. `dim_limit`: darunter gilt die LED als aus (dunkelgrauer Punkt).
    fn draw_led(&self, painter: &Painter, center: Pos2, radius: f32, rgb: &[u8], gain: f32, dim_limit: f32) {
        let (col, luma) = self.led_color(rgb, gain);
        if luma > 40.0 {
            // leuchtender Hof: größerer, durchsichtiger Kreis in der LED-Farbe
            painter.circle_filled(center, radius * 1.7, Color32::from_rgba_unmultiplied(col.r(), col.g(), col.b(), 26));
        }
        painter.circle_filled(center, radius, if luma < dim_limit { Color32::from_rgb(22, 22, 26) } else { col }); // aus = dunkelgrauer Punkt
    }

    /// Den Reiter zeichnen. `root` = Projektordner, `song` = (Song-ID, Name) des links gewählten Songs.
    pub fn show(&mut self, ui: &mut Ui, root: Option<&Path>, song: Option<(i64, String)>) {
        if !self.inited {
            self.bright_view = true; // Standard: aufgehellt (auf dem Bildschirm wirkt "echte" Helligkeit viel dunkler als auf der Bühne)
            self.correction = true; // Standard: Farbkorrektur wie auf dem Gerät
            self.inited = true;
        }
        let Some(root) = root else {
            ui.label("Projektordner nicht gefunden.");
            return;
        };
        // Welche DLLs gibt es? (Liste der Namen der fehlenden für die Hinweiszeile)
        let missing: Vec<&str> = SPECS.iter().filter(|s| !Self::dll_path(root, s).is_file()).map(|s| s.dll).collect();
        let any_dll = missing.len() < SPECS.len();

        // Befehlszeile --buehne: sobald ein Song bekannt und mindestens eine DLL da ist, einmal automatisch starten
        if self.auto_start && any_dll {
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
            if ui.add_enabled(any_dll && song.is_some(), egui::Button::new("▶ Start")).clicked() {
                if let Some((id, _)) = &song {
                    self.start(root, *id);
                }
            }
            if ui.add_enabled(!self.devices.is_empty(), egui::Button::new("■ Stop")).clicked() {
                self.stop();
            }
            ui.checkbox(&mut self.bright_view, "aufgehellt").on_hover_text(
                "aus = Farbe mal Helligkeit des Geräts (wie echt, am Bildschirm sehr dunkel); an = Bildschirm-freundlich aufgehellt",
            );
            ui.checkbox(&mut self.correction, "Farbkorrektur").on_hover_text(
                "an = die Farbkorrektur der LED-Streifen (TypicalLEDStrip: Rot 255, Grün 176, Blau 240) wie auf dem Gerät nachrechnen",
            );
        });
        if !missing.is_empty() {
            ui.colored_label(Color32::YELLOW, format!("{} fehlt - erst bauen: python tools/build_sim.py", missing.join(", ")));
        }
        if let Some(e) = &self.error {
            ui.colored_label(Color32::LIGHT_RED, e);
        }

        // --- Uhr stellen, Bilder holen ---
        self.tick();
        if !self.devices.is_empty() && self.running {
            ui.ctx().request_repaint(); // solange es läuft, sofort das nächste Bild (Zeit läuft in Echtzeit weiter)
        }

        // --- Bühne zeichnen: Platz = alles, was frei ist, abzüglich einer Zeile je Gerät für die Statusanzeige ---
        let status_h = 18.0 * self.devices.len().max(1) as f32 + 6.0;
        let avail = ui.available_size();
        let area = Vec2::new(avail.x.max(10.0), (avail.y - status_h).max(10.0));
        let (rect, _resp) = ui.allocate_exact_size(area, Sense::hover());
        let painter = ui.painter_at(rect);
        painter.rect_filled(rect, 0.0, Color32::from_rgb(8, 8, 10)); // dunkler Hintergrund (die "Bühne")

        if self.devices.is_empty() {
            if any_dll {
                painter.text(rect.center(), egui::Align2::CENTER_CENTER, "Song links wählen und ▶ Start drücken", egui::FontId::proportional(16.0), Color32::GRAY);
            }
            return;
        }
        self.draw_stage(&painter, rect);

        // --- Statusanzeige unter der Bühne: eine Zeile je Gerät ---
        for d in &self.devices {
            let (song_id, part, ms) = d.state;
            ui.label(format!(
                "{:<12} Song {song_id}   Part {part}   {:.1} s im Part   Bild {}   Helligkeit {}",
                d.spec.label,
                ms as f64 / 1000.0,
                d.frame_no,
                d.brightness
            ));
        }
    }

    /// Alle Geräte nebeneinander in `rect` zeichnen.
    fn draw_stage(&self, painter: &Painter, rect: Rect) {
        // --- 1. Größe jedes Geräts im Grafik-Koordinatensystem (Foto-Pixel) ---
        // LED-Abstand der Matrix = LED-Abstand der Kontur (ANNAHME, siehe Dateikopf)
        let pitch = self.devices.iter().find_map(|d| d.shape.as_ref().map(|s| s.spacing)).unwrap_or(DEFAULT_PITCH);
        // (Breite, Höhe, linke obere Ecke des Zeichenbereichs im Grafik-Koordinatensystem) je Gerät
        let boxes: Vec<(f32, f32, Pt)> = self
            .devices
            .iter()
            .map(|d| match &d.shape {
                Some(s) => (s.max[0] - s.min[0] + 2.0 * PAD, s.max[1] - s.min[1] + 2.0 * PAD, [s.min[0] - PAD, s.min[1] - PAD]),
                None => (d.sim.info.matrix_w as f32 * pitch + 2.0 * PAD, d.sim.info.matrix_h as f32 * pitch + 2.0 * PAD, [0.0, 0.0]),
            })
            .collect();
        let total_w: f32 = boxes.iter().map(|b| b.0).sum::<f32>() + GAP * (boxes.len() as f32 - 1.0);
        let total_h: f32 = boxes.iter().map(|b| b.1).fold(0.0, f32::max);

        // --- 2. ein gemeinsamer Maßstab, der in den freien Platz passt (unten/oben ein Streifen für die Beschriftung) ---
        let label_h = 22.0;
        let margin = 8.0;
        let scale = ((rect.width() - 2.0 * margin) / total_w).min((rect.height() - 2.0 * margin - label_h) / total_h).max(0.01);
        let drawn = Vec2::new(total_w * scale, total_h * scale);
        let top_left = Pos2::new(rect.center().x - drawn.x / 2.0, rect.center().y + label_h / 2.0 - drawn.y / 2.0);

        // Anzeige-Helligkeit: "echt" = brightness/255; "aufgehellt" = Wurzel davon (80/255 -> 0,56)
        let mut x_off = 0.0;
        for (d, (w, h, origin)) in self.devices.iter().zip(boxes.iter()) {
            let b = d.brightness.clamp(0, 255) as f32 / 255.0;
            let gain = if self.bright_view { b.sqrt() } else { b };
            // Umrechnung Grafik -> Bildschirm: Gerät in seinem Streifen von x_off an, senkrecht zentriert
            let dev_origin = Pos2::new(top_left.x + x_off * scale, top_left.y + (total_h - h) * scale / 2.0);
            let to_screen = |p: Pt| Pos2::new(dev_origin.x + (p[0] - origin[0]) * scale, dev_origin.y + (p[1] - origin[1]) * scale);
            // Beschriftung über dem Gerät
            painter.text(
                Pos2::new(dev_origin.x + w * scale / 2.0, top_left.y - 6.0),
                egui::Align2::CENTER_BOTTOM,
                format!("{} ({})", d.spec.label, d.sim.info.name()),
                egui::FontId::proportional(14.0),
                Color32::from_gray(150),
            );
            match &d.shape {
                Some(shape) => self.draw_instrument(painter, d, shape, gain, scale, &to_screen),
                None => self.draw_matrix(painter, d, gain, pitch * scale, &to_screen, PAD),
            }
            x_off += w + GAP;
        }
    }

    /// Gitarre / Bass: Kontur, LED-Kette (Ausgang 1 mit Bund-Markern) und Gurt (Ausgang 2, ohne Marker).
    fn draw_instrument(&self, painter: &Painter, d: &Device, shape: &Shape, gain: f32, scale: f32, to_screen: &dyn Fn(Pt) -> Pos2) {
        let spacing = shape.spacing * scale; // Abstand zweier LEDs auf dem Bildschirm in Pixeln
        let radius = (spacing * 0.36).max(1.0);
        // Umriss des Korpus und Gurtband: dunkle, dicke Linien unter den LEDs
        let outline: Vec<Pos2> = shape.outline.iter().map(|&p| to_screen(p)).collect();
        painter.add(egui::Shape::closed_line(outline, Stroke::new((spacing * 0.9).max(1.0), Color32::from_gray(34))));
        let strap_line: Vec<Pos2> = shape.strap_line.iter().map(|&p| to_screen(p)).collect();
        painter.add(egui::Shape::line(strap_line, Stroke::new((spacing * 0.8).max(1.0), Color32::from_gray(26))));
        // Beschriftung der Hals-Oberkante: dort leuchten die Bund-Marker (Ausgang 1)
        if d.sim.info.has_markers != 0 {
            let at = shape.marker_label_at;
            let off = 70.0; // Foto-Pixel nach außen
            let pos = to_screen([at[0] + shape.marker_label_dir[0] * off, at[1] + shape.marker_label_dir[1] * off]);
            // Schriftgröße wächst mit der Zeichnung (höchstens 12 pt); wird sie zu klein, entfällt die Beschriftung (kein Gedränge im kleinen Fenster)
            let font = (spacing * 0.55).clamp(6.0, 12.0);
            if spacing * 0.55 >= 7.0 {
                painter.text(pos, egui::Align2::CENTER_CENTER, "Bund-Marker", egui::FontId::proportional(font), Color32::from_gray(110));
            }
            // kleiner Strich von der Beschriftung zur Hals-Oberkante
            let near = to_screen([at[0] + shape.marker_label_dir[0] * 30.0, at[1] + shape.marker_label_dir[1] * 30.0]);
            let far = to_screen([at[0] + shape.marker_label_dir[0] * 52.0, at[1] + shape.marker_label_dir[1] * 52.0]);
            painter.line_segment([near, far], Stroke::new(1.0, Color32::from_gray(70)));
        }
        // Gurt (Ausgang 2): die ersten strap_leds LEDs von leds2, ohne Marker
        for (j, &p) in shape.strap_pos.iter().enumerate() {
            if j * 3 + 2 < d.rgb2.len() {
                // Marker (sehr dunkle Einzel-LEDs) sollen sichtbar bleiben: Schwelle für "aus" niedrig wie bei der Kette
                self.draw_led(painter, to_screen(p), radius * 0.9, &d.rgb2[j * 3..j * 3 + 3], gain, 2.0);
            }
        }
        // Instrument (Ausgang 1): Schwelle für "aus" niedrig (2), damit die dunklen Marker nicht als "aus" gelten
        for (i, &p) in shape.led_pos.iter().enumerate() {
            if i * 3 + 2 < d.rgb1.len() {
                let rgb = &d.rgb1[i * 3..i * 3 + 3];
                let c = to_screen(p);
                if shape.fretboard.get(i).copied().unwrap_or(false) {
                    // Griffbrett: ein Marker leuchtet nur mit Pegel ~7 von 255 - am Bildschirm unsichtbar. In der aufgehellten
                    // Darstellung wird eine leuchtende Griffbrett-LED deshalb auf feste Sichtbarkeit angehoben (gleicher Farbton)
                    // und bekommt einen Ring; jede Griffbrett-LED zeigt einen feinen grauen Ring (= dunkles Griffbrett).
                    painter.circle_stroke(c, radius * 1.25, Stroke::new(1.0, Color32::from_gray(52)));
                    let top = rgb[0].max(rgb[1]).max(rgb[2]);
                    if self.bright_view && top > 0 {
                        let boost = 200.0 / top as f32; // größter Farbanteil -> 200 (vor Farbkorrektur und Helligkeit)
                        let lifted: Vec<u8> = rgb.iter().map(|&v| (v as f32 * boost).min(255.0) as u8).collect();
                        self.draw_led(painter, c, radius * 1.15, &lifted, 1.0, 2.0);
                        painter.circle_stroke(c, radius * 1.6, Stroke::new(1.5, Color32::from_gray(190)));
                        continue;
                    }
                }
                self.draw_led(painter, c, radius, rgb, gain, 2.0);
            }
        }
    }

    /// Matrix: 54 x 10 LEDs über die echte Verdrahtung (sim_xy). `pitch` = LED-Abstand auf dem Bildschirm in Pixeln.
    fn draw_matrix(&self, painter: &Painter, d: &Device, gain: f32, pitch: f32, to_screen: &dyn Fn(Pt) -> Pos2, pad: f32) {
        let (w, h) = (d.sim.info.matrix_w as usize, d.sim.info.matrix_h as usize);
        let radius = pitch * 0.42;
        // linke obere Ecke des Rasters: der Rand (pad) liegt in Grafik-Koordinaten, to_screen rechnet ihn um
        let origin = to_screen([pad, pad]);
        for y in 0..h {
            for x in 0..w {
                let led = d.led_of_xy.get(y * w + x).copied().unwrap_or(0);
                let c = Pos2::new(origin.x + (x as f32 + 0.5) * pitch, origin.y + (y as f32 + 0.5) * pitch);
                self.draw_led(painter, c, radius, &d.rgb1[led * 3..led * 3 + 3], gain, 6.0);
            }
        }
        let grid = Vec2::new(pitch * w as f32, pitch * h as f32);
        painter.rect_stroke(Rect::from_min_size(origin, grid).expand(4.0), 4.0, Stroke::new(1.0, Color32::from_gray(50)), StrokeKind::Outside);
    }
}
