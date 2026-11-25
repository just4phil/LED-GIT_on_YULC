// stage.rs - der Reiter "Bühne": zeigt Lampe 1, Bass, Matrix, Gitarre und Lampe 2 so, wie die ECHTE Firmware sie berechnet.
//
// Die Firmware-Quellen laufen als DLL, je Gerät eine (sim/bin/sim_rinasbass.dll, sim_scrollmatrix.dll,
// sim_andresgit.dll, sim_lampe1.dll, sim_lampe2.dll; Rust-Hülle in sim.rs, Bauen mit python tools/build_sim.py). Dieser Reiter
//   - hat die Knöpfe Start (alle DLLs frisch laden, Einschalten, MIDI "Kanal 10, CC 22 = Song") und Stop (CC 22 = 0),
//   - stellt bei jedem Bild des Fensters die Simulator-Uhr ALLER Geräte um dieselbe vergangene Echtzeit vor
//     (sim_advance) - alle laufen auf einer gemeinsamen Uhr und bekommen dieselben Befehle,
//   - holt von jedem Gerät das zuletzt gesendete Bild und malt es: die Lampen als zwei senkrechte LED-Reihen, die Matrix als Raster, Gitarre und Bass als LED-Kette
//     auf der SG-Kontur (Orte: shape.rs) samt Bund-Markern (Ausgang 1) und Gurt (Ausgang 2, ohne Marker).
// Fehlt die DLL eines Geräts, läuft der Rest weiter; oben steht dann, was zu bauen ist.
//
// ANORDNUNG (final seit Issue #24): von links nach rechts wie JSON_STAGE in tools/songgen.py und CASCADE_STAGE_POS:
// Lampe 1 · Bass · Matrix · Gitarre · Lampe 2. Alle Geräte liegen in EINEM Bühnen-Koordinatensystem ("Foto-Pixel" der
// Gitarren-Zeichnung); erst beim Zeichnen wird es als Ganzes auf die freie Fläche abgebildet (ein gemeinsamer Maßstab,
// Seitenverhältnis bleibt, zentriert): das Fenster ist frei skalierbar, nichts wird verzerrt, es gibt keine festen
// Pixelgrößen außer der Schriftgröße. Größenverhältnisse (ANNAHMEN, echte Maße unbekannt - sie ändern nur die
// Zeichnung, nie die Show):
//   - Instrumente: ein LED-Abstand = Umfang der SG-Kontur / LED-Zahl (Gitarre 163 LEDs, Bass 155).
//   - Matrix: 1,5 x LED-Abstand der Instrumente (MATRIX_PITCH_FACTOR) - die Matrix soll auf der Bühne als breite
//     Fläche gegenüber den Instrumenten lesbar sein (54 x 10 LEDs ergeben so ~1700 x 315 Foto-Pixel).
//   - Lampen: 0,75 x LED-Abstand (LAMP_PITCH_FACTOR), damit sie hoch und schmal bleiben, aber nicht die ganze Fläche
//     nach oben aufspannen (Lampe 1 mit 94 LEDs ist dann ~1480 hoch, Lampe 2 mit 78 LEDs ~1230).
// Fehlt die DLL eines Geräts, bleibt sein Platz auf der Bühne als graues Feld "nicht gebaut" stehen, die anderen laufen.
// Beschriftungen über den Geräten werden an die Spaltenbreite angepasst (langer Name -> kurzer Name -> Kürzel ->
// ausgeblendet), so überlappen sie bei keiner Fenstergröße; die Statusliste unter der Bühne nennt immer alle Namen.
//
// BEDIENUNG (Issue #24, ohne LP16-Tool): Start (alle DLLs frisch laden, CC 22 = Song), Stop (CC 22 = 0), Pause/Weiter
// (die Simulator-Uhr steht), Tempo x1 / x0,5 (die Simulator-Uhr läuft halb so schnell: Effekte, Part-Längen, Timer
// laufen langsamer, intern bleibt alles konsistent, weil die DLLs nur die vorgestellten Millisekunden sehen) und
// "Sprung zu Part": CC 23 = case an ALLE Geräte, genau wie echte Geräte ihn per Funk bekommen - der Part beginnt an
// seinem Anfang. Läuft der Song noch nicht, wird er vorher gestartet (CC 22). Während ein MIDI-Port offen ist, läuft die
// Uhr immer in Echtzeit (sonst passte sie nicht zur Musik): Pause und Tempo sind dann ausgegraut. Der Reiter "Ablauf"
// kann über request_jump() einen Sprung bestellen ("auf der Bühne ab hier abspielen").
// STATUS je Gerät unter der Bühne: Name, Song-ID, Part (prog) + Part-Name (aus songgen.py --json, wenn bekannt), Zeit im
// Part. Weichen Geräte im Part voneinander ab (länger als 0,3 s), sind sie orange markiert. Ausnahme: die Matrix
// im Titel-Lauftext - dafür gibt es Zusatz-cases, die nicht in parts[] stehen; sie zeigt dann "Lauftext" und gilt nicht
// als abweichend.
//
// FARBKORREKTUR: die Firmware stellt FastLED.setCorrection(TypicalLEDStrip) ein (LED_COLOR_CORRECTION in
// definitions.h): Rot 255, Grün 176, Blau 240 von 255 - erst beim Senden, also NICHT in den Puffern, die sim_frame()
// liefert. Die Anzeige rechnet sie darum für alle Geräte nach (Schalter "Farbkorrektur").
//
// Wie in egui gezeichnet wird, steht in timeline.rs: Immediate-Mode - jedes Bild wird komplett neu gemalt, gemerkt
// wird nur, was zwischen zwei Bildern bleiben muss (struct Stage).
//
// LAMPEN (Issue #23, Angabe des Users): eine Lampe ist eine senkrechte Säule aus ZWEI parallelen LED-Streifen, beide von
// unten nach oben, nichts gewickelt; beide hängen am SELBEN Datensignal und zeigen dasselbe Bild. Jeder Streifen hat
// anz_LEDs LEDs (Lampe 1: 94, Lampe 2: 78 - bei gleichem LED-Abstand ist Lampe 1 also höher). Gezeichnet wird für BEIDE
// Reihen derselbe Puffer: Ausgang 1 (leds1 / DATA_PIN_1). Begründung: bei den Lampen (NOMARKER) kopiert
// gitBlindingLEDs_OFF_MarkerLEDs_ON() dasselbe Bild nach leds1 und leds2, und auch die Warn-LEDs schreiben in beide - die
// Wahl ändert das Bild nicht; Ausgang 1 ist der Hauptausgang (wie bei Gitarre/Bass). Welches Ende LED 0 ist, kommt aus der DLL
// (SimInfo.lamp_idx0_at_bottom = LAMP_IDX0_AT_BOTTOM aus definitions.h), es wird nicht geraten. Die Helligkeit vom Knopf
// der Gitarre (REMOTE_BRIGHTNESS, SYNC_BRIGHT) wird nicht nachgebildet: Standardhelligkeit.
//
// Rust-Idiome: Option<T> = Wert oder None; `if let Some(x) = ...` packt ihn aus; `&mut self` = die Funktion darf den
// Zustand der Struktur verändern; `Vec<T>` = wachsende Liste; `|a| ...` = kleine namenlose Funktion (Closure).

use crate::shape::{Pt, Shape};
use crate::model::SongData;
use crate::settings::Settings;
use crate::sim::Sim;
use crate::timeline;
use eframe::egui::{self, Color32, Painter, Pos2, Rect, Sense, Stroke, StrokeKind, Ui, Vec2};
use std::path::{Path, PathBuf};
use crate::midi_in::{self, ClockMap, Devices, MidiEvent, MidiIn};
use std::time::Instant;

/// Wie ein Gerät gezeichnet wird.
#[derive(Clone, Copy, PartialEq)]
enum Kind {
    Instrument, // Gitarre / Bass: LED-Kette auf der SG-Kontur + Gurt
    Matrix,     // Raster
    Lamp,       // Lampe: zwei parallele senkrechte LED-Reihen mit demselben Bild (Ausgang 1)
}

/// Ein Gerät der Bühne: DLL-Name, Beschriftung, Zeichenart. Reihenfolge = von links nach rechts.
struct Spec {
    dll: &'static str,
    label: &'static str,
    short: &'static str, // Kürzel für sehr enge Fenster
    kind: Kind,
    /// Größe des Platzhalters (Breite, Höhe in Foto-Pixeln), wenn die DLL fehlt: ungefähr wie das echte Gerät
    fallback: (f32, f32),
}
const SPECS: [Spec; 5] = [
    Spec { dll: "sim_lampe1.dll", label: "Lampe 1", short: "L1", kind: Kind::Lamp, fallback: (170.0, 1560.0) },
    Spec { dll: "sim_rinasbass.dll", label: "Bass", short: "B", kind: Kind::Instrument, fallback: (1100.0, 950.0) },
    Spec { dll: "sim_scrollmatrix.dll", label: "Matrix", short: "M", kind: Kind::Matrix, fallback: (1780.0, 400.0) },
    Spec { dll: "sim_andresgit.dll", label: "Gitarre", short: "G", kind: Kind::Instrument, fallback: (1100.0, 950.0) },
    Spec { dll: "sim_lampe2.dll", label: "Lampe 2", short: "L2", kind: Kind::Lamp, fallback: (170.0, 1310.0) },
];

/// Höchstens so viele ms werden pro Fensterbild vorgestellt (z. B. nach dem Ziehen des Fensters stand die Anzeige
/// still - dann nicht in einem Sprung nachholen, sondern gedeckelt weiterlaufen).
const MAX_STEP_MS: f64 = 250.0;
/// Lücke zwischen zwei Geräten in Foto-Pixeln der Grafik.
const GAP: f32 = 70.0;
/// Rand um jedes Gerät in Foto-Pixeln (Platz für den Leuchthof der äußeren LEDs).
const PAD: f32 = 40.0;
/// LED-Abstand der Instrumente, falls keines geladen ist (sonst der der Kontur): so groß wie ein LED-Abstand der SG.
const DEFAULT_PITCH: f32 = 21.0;
/// Matrix: LED-Abstand = dieser Faktor x LED-Abstand der Instrumente (ANNAHME, siehe Dateikopf).
const MATRIX_PITCH_FACTOR: f32 = 1.5;
/// Lampe: LED-Abstand = dieser Faktor x LED-Abstand der Instrumente (ANNAHME, siehe Dateikopf).
const LAMP_PITCH_FACTOR: f32 = 0.75;
/// So lange muss ein Part-Unterschied zwischen den Geräten bestehen, bevor er orange markiert wird (ein Wechsel
/// wird nicht auf allen Geräten im selben 2-ms-Takt bemerkt).
const MISMATCH_MS: u128 = 300;
/// Lampe: Abstand der beiden senkrechten LED-Reihen (Mitte zu Mitte) in LED-Abständen. Ein Streifen ist etwa so
/// breit wie ein LED-Abstand; ANNAHME 1,6 (echtes Maß unbekannt) - hoch und schmal.
const LAMP_ROW_GAP: f32 = 1.6;
/// Lampe: kleinste Breite des Zeichenbereichs in Foto-Pixeln (Platz für die Beschriftung "Lampe 1").
const LAMP_MIN_W: f32 = 120.0;
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
    t0_us: i64,             // Simulator-Zeit 0 auf der Programm-Uhr (µs seit midi_in::epoch()): Zeitpunkt des Starts
    done_ms: f64,           // bis wohin (Simulator-ms seit Start) die Geräte schon gerechnet sind (ganze ms)
    midi: Option<MidiIn>,   // offene MIDI-Verbindung (None = MIDI-Eingang aus oder Port fehlt)
    midi_choice: Option<String>, // gewählter Port (auch wenn er gerade fehlt; None = "aus")
    midi_ports: Vec<String>, // zuletzt gelesene Port-Liste
    midi_note: Option<String>, // Hinweis zum Eingang ("nicht gefunden", Fehlertext)
    midi_list_open: bool,   // war die Aufklappliste im vorigen Bild offen? (zum Erkennen des Öffnens)
    clock_map: ClockMap,    // Treiber-Zeitstempel -> Programm-Uhr
    last_cc: Option<(u8, u8, Instant)>, // zuletzt empfangener Befehl (CC, Wert, Zeitpunkt) für die Anzeige
    error: Option<String>,  // Fehlertext (DLL nicht ladbar ...)
    auto_start: bool,       // true = den Song beim ersten Zeichnen sofort starten (Befehlszeile --buehne), danach wieder false
    inited: bool,           // wurden die gemerkten Einstellungen schon einmal geladen?
    settings: Settings,     // gemerkte Schalter (aufgehellt, Farbkorrektur, MIDI-Port; settings.rs)
    paused: bool,           // Pause: die Simulator-Uhr steht
    half_speed: bool,       // Tempo x0,5: die Simulator-Uhr läuft halb so schnell
    clock_ms: f64,          // Simulator-Uhr in ms seit Start (fließt exakt, auch mit Bruchteilen; done_ms zählt ganze ms)
    last_us: i64,           // Programm-Uhr (µs) beim vorigen Bild: daraus ergibt sich die vergangene Echtzeit
    was_midi: bool,         // war im vorigen Bild ein MIDI-Port offen? (zum Erkennen des Wechsels)
    pending_jump: Option<i64>, // bestellter Sprung (case) aus dem Reiter "Ablauf", wird im nächsten Bild ausgeführt
    jump_choice: Option<i64>, // in der Auswahl "Sprung zu Part" gewählter case
    mismatch_since: Option<Instant>, // seit wann weichen Geräte im Part voneinander ab?
    scroll_join_ms: Option<f64>, // Song-Zeit (ms), bis zu der die Matrix den Titel-Lauftext spielt (aus songgen.py --json), None = keiner
    scroll_parts: Vec<(i64, f64)>, // (case, Beginn ms) aller Parts: daraus ergibt sich die Song-Zeit
}

/// Alle Geräte als Ziel für midi_in::run_to(): jeder Aufruf geht an jede DLL (gemeinsame Uhr, gleiche Befehle).
impl Devices for Vec<Device> {
    fn advance(&mut self, ms: u32) {
        for d in self.iter_mut() {
            d.sim.advance(ms);
        }
    }
    fn midi(&mut self, channel: i32, cc: i32, value: i32) {
        for d in self.iter_mut() {
            d.sim.midi(channel, cc, value);
        }
    }
}

impl Stage {
    /// Neuer, leerer Reiter. auto_start = true: beim ersten Zeichnen den gewählten Song gleich starten (--buehne);
    /// jump = Part (case), zu dem danach gesprungen wird (--sprung).
    pub fn new(auto_start: bool, jump: Option<i64>) -> Self {
        Self { auto_start, pending_jump: jump, ..Default::default() } // alle übrigen Felder auf ihren Standardwert (0, None, leer, false)
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
                        Kind::Lamp => {} // braucht nur die LED-Zahl und die Richtung aus SimInfo
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
        self.t0_us = midi_in::epoch().elapsed().as_micros() as i64; // Simulator-Zeit 0 = jetzt
        self.last_us = self.t0_us;
        self.done_ms = 0.0;
        self.clock_ms = 0.0;
        self.paused = false; // ein neuer Start läuft immer
        self.mismatch_since = None;
    }

    /// Bestellt einen Sprung zu einem Part (CC 23 = case) für das nächste Bild. Dient dem Reiter "Ablauf"
    /// ("auf der Bühne ab hier abspielen"): der Sprung braucht Song, Projektordner und DLLs, die erst in show() bekannt sind.
    pub fn request_jump(&mut self, case: i64) {
        self.pending_jump = Some(case);
    }

    /// Sprung zu einem Part: wie ein Part-Befehl der Gitarre (CC 23 = case an alle Geräte, der Part beginnt an seinem
    /// Anfang). Läuft der Song noch nicht (nichts geladen oder ein anderer Song), wird er vorher gestartet (frisch laden,
    /// CC 22). Ein Sprung hebt eine Pause auf, sonst sähe man das Ergebnis nicht.
    fn jump(&mut self, root: &Path, song_id: i64, case: i64) {
        let running = self.devices.first().map(|d| d.state.0 as i64);
        if running != Some(song_id) {
            self.start(root, song_id);
        }
        for d in self.devices.iter_mut() {
            d.sim.midi(10, 23, case as i32); // Kanal 10, CC 23 = Part wählen
        }
        self.jump_choice = Some(case); // die Auswahl zeigt den Part, bei dem man gelandet ist
        self.paused = false;
        self.mismatch_since = None; // die Geräte bemerken den Wechsel gleich, eine alte Abweichung zählt nicht mehr
    }

    /// Stop: wie am Gerät "Song 0" (Songpause) wählen. Die Uhr läuft weiter, die Songpause ist ja auch eine Show.
    fn stop(&mut self) {
        for d in self.devices.iter_mut() {
            d.sim.midi(10, 22, 0);
        }
    }

    /// Uhr vorstellen und die neuesten Bilder holen (einmal pro Fensterbild). `raw` = die seit dem letzten Bild
    /// angekommenen MIDI-Befehle (Ankunftsreihenfolge): sie werden auf der Simulator-Uhr zeitgenau eingefügt.
    fn tick(&mut self, raw: &[MidiEvent]) {
        let now_us = midi_in::epoch().elapsed().as_micros() as i64;
        let midi_mode = self.midi.is_some();
        if self.running {
            let target;
            if midi_mode {
                // Mit MIDI-Eingang läuft die Uhr immer in Echtzeit (Pause und Tempo gibt es dann nicht): sonst passt sie
                // nicht zur Musik, und die 250-ms-Obergrenze für Hänger gilt auch nicht. Beim Umschalten in diesen
                // Modus die Echtzeit-Uhr so verankern, dass sie bei der aktuellen Simulator-Zeit weiterläuft.
                if !self.was_midi {
                    self.t0_us = now_us - (self.clock_ms * 1000.0) as i64;
                    self.paused = false;
                    self.half_speed = false;
                }
                target = (now_us - self.t0_us) as f64 / 1000.0;
            } else {
                // Ohne MIDI: die Simulator-Uhr läuft um die vergangene Echtzeit weiter - bei Pause gar nicht, bei Tempo
                // x0,5 halb so schnell. Nach einem Hänger (Fenster verschoben ...) höchstens MAX_STEP_MS nachholen.
                let dt_ms = ((now_us - self.last_us) as f64 / 1000.0).clamp(0.0, MAX_STEP_MS);
                let speed = if self.half_speed { 0.5 } else { 1.0 };
                target = if self.paused { self.clock_ms } else { self.clock_ms + dt_ms * speed };
            }
            self.clock_ms = target;
            // Treiber-Zeitstempel -> Simulator-ms; Befehle vor dem Start (negativ) werden sofort angewendet
            let events: Vec<(f64, MidiEvent)> = raw
                .iter()
                .map(|e| (((self.clock_map.map(e.stamp_us, e.arrival_us) - self.t0_us) as f64) / 1000.0, *e))
                .collect();
            midi_in::run_to(&mut self.devices, &mut self.done_ms, &events, target);
        }
        self.last_us = now_us;
        self.was_midi = midi_mode;
        for d in self.devices.iter_mut() {
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
        let k = if self.settings.correction { CORRECTION } else { [1.0; 3] };
        let c = |i: usize| (rgb[i] as f32 * k[i] * gain).min(255.0);
        let (r, g, b) = (c(0), c(1), c(2));
        (Color32::from_rgb(r as u8, g as u8, b as u8), r.max(g).max(b))
    }

    /// Eine LED als Punkt mit Hof zeichnen. `dim_limit`: darunter gilt die LED als aus (dunkelgrauer Punkt).
    fn draw_led(&self, painter: &Painter, center: Pos2, radius: f32, rgb: &[u8], gain: f32, dim_limit: f32) {
        let (col, luma) = self.led_color(rgb, gain);
        if luma > 40.0 {
            // leuchtender Hof: größerer, durchsichtiger Kreis in der LED-Farbe; je heller die LED, desto kräftiger
            // (Durchsichtigkeit 18..44 von 255) - ein leichter Schein, der die Punkte nicht verschmieren lässt
            let alpha = (18.0 + luma * 0.1).min(44.0) as u8;
            painter.circle_filled(center, radius * 1.7, Color32::from_rgba_unmultiplied(col.r(), col.g(), col.b(), alpha));
        }
        painter.circle_filled(center, radius, if luma < dim_limit { Color32::from_rgb(22, 22, 26) } else { col }); // aus = dunkelgrauer Punkt
    }

    /// Name des Parts zu einem `case` für die Statusanzeige, "-" ohne bekannten Ablauf, "?" für einen unbekannten case.
    fn part_name(parts: &[(i64, String, f64)], case: i32) -> String {
        match parts.iter().find(|p| p.0 == case as i64) {
            Some((_, name, _)) => name.clone(),
            None if parts.is_empty() => "-".to_string(),
            None => "?".to_string(),
        }
    }

    /// Den Reiter zeichnen. `root` = Projektordner, `song` = (Song-ID, Ordnername) des links gewählten Songs,
    /// `data` = der geladene Ablauf (für die Part-Liste und Part-Namen; gilt nur, wenn er zu diesem Song gehört).
    pub fn show(&mut self, ui: &mut Ui, root: Option<&Path>, song: Option<(i64, String)>, data: Option<&SongData>) {
        if !self.inited {
            self.settings = Settings::load(); // gemerkt: aufgehellt, Farbkorrektur, MIDI-Port
            self.inited = true;
            // Gemerkten MIDI-Port wiederherstellen und versuchen zu öffnen
            self.midi_choice = self.settings.midi_port.clone();
            self.midi_refresh(ui.ctx(), true);
        }
        let Some(root) = root else {
            ui.label("Projektordner nicht gefunden.");
            return;
        };
        // Welche DLLs gibt es? (Liste der Namen der fehlenden für die Hinweiszeile)
        let missing: Vec<&str> = SPECS.iter().filter(|s| !Self::dll_path(root, s).is_file()).map(|s| s.dll).collect();
        let any_dll = missing.len() < SPECS.len();
        // Parts des Songs: (case, Name, Beginn in ms) - nur wenn der geladene Ablauf zum gewählten Song gehört
        let parts: Vec<(i64, String, f64)> = match (&song, data) {
            (Some((_, dir)), Some(d)) if &d.song.dir == dir => d.parts.iter().map(|p| (p.case, p.name.clone(), p.start)).collect(),
            _ => Vec::new(),
        };

        // Befehlszeile --buehne: sobald ein Song bekannt und mindestens eine DLL da ist, einmal automatisch starten
        if self.auto_start && any_dll {
            if let Some((id, _)) = &song {
                self.auto_start = false;
                self.start(root, *id);
            }
        }
        // Bestellter Sprung (Reiter "Ablauf" oder Befehlszeile --sprung): ausführen, sobald Song und DLLs da sind
        if let (Some(case), Some((id, _))) = (self.pending_jump, &song) {
            if any_dll {
                self.pending_jump = None;
                self.jump(root, *id, case);
            }
        }

        // Titel-Lauftext der Matrix: bis wann spielt sie ihn (join_start des spätesten Plans)? Während dieser Zeit laufen ihre
        // cases (die Nummern der Part-Liste oder Zusatz-cases dahinter) NICHT mit den anderen Geräten gleich - normal.
        self.scroll_join_ms = data
            .filter(|d| song.as_ref().is_some_and(|(_, dir)| &d.song.dir == dir))
            .and_then(|d| d.scroll.values().flatten().map(|p| p.join_start).fold(None, |m: Option<f64>, v| Some(m.map_or(v, |x| x.max(v)))));
        self.scroll_parts = parts.iter().map(|p| (p.0, p.2)).collect();
        let before = (self.settings.bright_view, self.settings.correction);
        self.controls(ui, root, &song, &parts, any_dll);
        if before != (self.settings.bright_view, self.settings.correction) {
            self.settings.save(); // Schalter merken
        }
        self.midi_row(ui);
        if !missing.is_empty() {
            ui.colored_label(Color32::YELLOW, format!("{} fehlt - erst bauen: python tools/build_sim.py", missing.join(", ")));
        }
        if let Some(e) = &self.error {
            ui.colored_label(Color32::LIGHT_RED, e);
        }

        // --- MIDI-Befehle abholen; kommt der erste, bevor etwas läuft, erst die Geräte laden (Song 0 = Songpause) ---
        let raw = self.midi.as_ref().map(|m| m.drain()).unwrap_or_default();
        if let Some(last) = raw.last() {
            self.last_cc = Some((last.cc, last.value, Instant::now()));
            if self.devices.is_empty() && any_dll {
                self.start(root, 0);
            }
        }

        // --- Uhr stellen, Bilder holen ---
        self.tick(&raw);
        if !self.devices.is_empty() && self.running && !self.paused {
            ui.ctx().request_repaint(); // solange es läuft, sofort das nächste Bild (Zeit läuft in Echtzeit weiter)
        }

        // --- Bühne zeichnen: Platz = alles, was frei ist, abzüglich der Statusliste (Titelzeile + eine Zeile je Gerät) ---
        let avail = ui.available_size();
        // Die Statusliste bekommt ihre volle Höhe, in einem kleinen Fenster aber höchstens ein Drittel (sie scrollt dann): die Bühne
        // soll nie auf einen Streifen schrumpfen.
        let status_h = (19.0 * (SPECS.len() as f32 + 1.0) + 14.0).min(avail.y * 0.33);
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
        egui::ScrollArea::vertical().max_height(status_h).auto_shrink([false, false]).show(ui, |ui| self.status(ui, &parts));
    }

    /// Bedienzeilen: Start / Stop / Pause / Tempo, Sprung zu Part, Darstellungsschalter. `horizontal_wrapped` bricht die
    /// Zeile um, wenn das Fenster schmal ist.
    fn controls(&mut self, ui: &mut Ui, root: &Path, song: &Option<(i64, String)>, parts: &[(i64, String, f64)], any_dll: bool) {
        let midi_active = self.midi.is_some();
        // Knopftexte nie mitten im Wort umbrechen: lieber springt der ganze Knopf in die nächste Zeile (horizontal_wrapped)
        ui.style_mut().wrap_mode = Some(egui::TextWrapMode::Extend);
        ui.horizontal_wrapped(|ui| {
            match song {
                Some((id, name)) => ui.strong(format!("Song {id}  {name}")),
                None => ui.label("Links einen Song mit Song-ID wählen (generierte Songs)."),
            };
            if ui.add_enabled(any_dll && song.is_some(), egui::Button::new("▶ Start")).clicked() {
                if let Some((id, _)) = song {
                    self.start(root, *id);
                }
            }
            if ui.add_enabled(!self.devices.is_empty(), egui::Button::new("■ Stop")).clicked() {
                self.stop();
            }
            // Pause und Tempo: ausgegraut, solange ein MIDI-Port offen ist (die Uhr läuft dann in Echtzeit)
            let clock_ui = ui.add_enabled_ui(!midi_active && !self.devices.is_empty(), |ui| {
                if ui.button(if self.paused { "▶ Weiter" } else { "⏸ Pause" }).clicked() {
                    self.paused = !self.paused;
                }
                ui.label("Tempo");
                ui.selectable_value(&mut self.half_speed, false, "×1");
                ui.selectable_value(&mut self.half_speed, true, "×0,5").on_hover_text("halbe Geschwindigkeit: auch die Simulator-Zeit läuft nur halb so schnell");
            });
            if midi_active {
                clock_ui.response.on_hover_text("Pause und Tempo gehen nicht, solange ein MIDI-Eingang offen ist: die Uhr läuft dann in Echtzeit, sonst passt sie nicht zur Musik.");
                ui.label(egui::RichText::new("Pause/Tempo aus: Uhr in Echtzeit (MIDI)").color(Color32::YELLOW));
            }
        });
        ui.horizontal_wrapped(|ui| {
            // Auswahl des Parts: nur mit bekanntem Ablauf; die Auswahl springt auf den ersten Part, wenn sie nicht mehr passt
            if !parts.iter().any(|p| Some(p.0) == self.jump_choice) {
                self.jump_choice = parts.first().map(|p| p.0);
            }
            let label = |p: &(i64, String, f64)| format!("{:>3}  {}  ({})", p.0, p.1, timeline::fmt_time(p.2));
            let shown = parts.iter().find(|p| Some(p.0) == self.jump_choice).map(label).unwrap_or_else(|| "(Ablauf nicht geladen)".to_string());
            ui.label("Sprung zu Part:");
            egui::ComboBox::from_id_salt("jump_part").selected_text(shown).width(ui.available_width().clamp(120.0, 260.0)).show_ui(ui, |ui| {
                for p in parts {
                    if ui.selectable_label(Some(p.0) == self.jump_choice, label(p)).clicked() {
                        self.jump_choice = Some(p.0);
                    }
                }
            });
            let go = ui
                .add_enabled(any_dll && song.is_some() && self.jump_choice.is_some(), egui::Button::new("Springen"))
                .on_hover_text("CC 23 = Part an alle Geräte; der Part beginnt an seinem Anfang. Läuft der Song noch nicht, wird er vorher gestartet.");
            if go.clicked() {
                if let (Some((id, _)), Some(case)) = (song, self.jump_choice) {
                    self.jump(root, *id, case);
                }
            }
            ui.separator();
            ui.checkbox(&mut self.settings.bright_view, "aufgehellt").on_hover_text(
                "aus = Farbe mal Helligkeit des Geräts (wie echt, am Bildschirm sehr dunkel); an = Bildschirm-freundlich aufgehellt",
            );
            ui.checkbox(&mut self.settings.correction, "Farbkorrektur").on_hover_text(
                "an = die Farbkorrektur der LED-Streifen (TypicalLEDStrip: Rot 255, Grün 176, Blau 240) wie auf dem Gerät nachrechnen",
            );
        });
    }

    /// Statusliste unter der Bühne: eine Zeile je Gerät (Name, Song, Part + Name, Zeit im Part, Bild, Helligkeit). Weichen
    /// die Geräte im Part voneinander ab (länger als MISMATCH_MS), sind die abweichenden orange. Ein fehlendes Gerät steht
    /// grau da ("nicht gebaut"). Die Matrix im Titel-Lauftext (eigene cases, Zusatz-cases stehen nicht in parts[]) wird
    /// nicht verglichen und blau als "Lauftext" gezeigt.
    fn status(&mut self, ui: &mut Ui, parts: &[(i64, String, f64)]) {
        // Vergleichsbasis: der häufigste (Song, Part) unter den Geräten OHNE Matrix (Gleichstand: das erste). Die Matrix
        // wird mitverglichen, außer sie spielt gerade den Titel-Lauftext (Song-Zeit des Vergleichsparts < join_start).
        let others: Vec<(usize, i32, i32)> =
            self.devices.iter().enumerate().filter(|(_, d)| d.spec.kind != Kind::Matrix).map(|(i, d)| (i, d.state.0, d.state.1)).collect();
        let reference = others.iter().map(|c| (c.1, c.2)).max_by_key(|r| others.iter().filter(|c| (c.1, c.2) == *r).count());
        // Song-Zeit grob = Beginn des Vergleichsparts + Zeit darin (vom ersten Gerät dieses Parts)
        let song_ms = reference.and_then(|r| {
            let start = self.scroll_parts.iter().find(|p| p.0 == r.1 as i64)?.1;
            let ms = self.devices.iter().find(|d| d.state.1 == r.1 && d.spec.kind != Kind::Matrix)?.state.2;
            Some(start + ms as f64)
        });
        let scroll_active = matches!((self.scroll_join_ms, song_ms), (Some(j), Some(t)) if t < j + 300.0);
        let deviating: Vec<usize> = match reference {
            Some(r) => self
                .devices
                .iter()
                .enumerate()
                .filter(|(_, d)| (d.state.0, d.state.1) != r && !(d.spec.kind == Kind::Matrix && scroll_active))
                .map(|(i, _)| i)
                .collect(),
            None => Vec::new(),
        };
        if deviating.is_empty() {
            self.mismatch_since = None;
        } else if self.mismatch_since.is_none() {
            self.mismatch_since = Some(Instant::now());
        }
        let flagged = self.mismatch_since.is_some_and(|t| t.elapsed().as_millis() >= MISMATCH_MS);
        let orange = Color32::from_rgb(255, 160, 40);
        let small = |t: String, c: Color32| egui::RichText::new(t).monospace().size(12.0).color(c);
        let normal = ui.visuals().text_color();
        egui::Grid::new("status").num_columns(7).spacing([14.0, 1.0]).show(ui, |ui| {
            for h in ["Gerät", "Song", "Part", "Part-Name", "Zeit im Part", "Bild", "Hell."] {
                ui.label(small(h.to_string(), Color32::GRAY));
            }
            ui.end_row();
            for spec in SPECS.iter() {
                let Some((i, d)) = self.devices.iter().enumerate().find(|(_, d)| d.spec.dll == spec.dll) else {
                    ui.label(small(spec.label.to_string(), Color32::DARK_GRAY));
                    ui.label(small("nicht gebaut".to_string(), Color32::DARK_GRAY));
                    ui.end_row();
                    continue;
                };
                let (song_id, part, ms) = d.state;
                // Matrix im Titel-Lauftext: eigener cases, zeigt "Lauftext" (blau), kein Fehler
                let scrolling = spec.kind == Kind::Matrix && scroll_active && reference.is_some_and(|r| r.1 != part);
                let pname = if scrolling { "Lauftext".to_string() } else { Self::part_name(parts, part) };
                let col = if flagged && deviating.contains(&i) {
                    orange
                } else if scrolling {
                    Color32::LIGHT_BLUE
                } else {
                    normal
                };
                ui.label(small(spec.label.to_string(), col));
                ui.label(small(song_id.to_string(), col));
                ui.label(small(part.to_string(), col));
                ui.label(small(pname, col));
                ui.label(small(format!("{:.1} s", ms as f64 / 1000.0), col));
                ui.label(small(d.frame_no.to_string(), col));
                ui.label(small(d.brightness.to_string(), col));
                ui.end_row();
            }
        });
    }

    /// MIDI-Port-Liste neu lesen. Ist der gemerkte Port jetzt da und noch nicht offen, wird er geöffnet
    /// (`first` = erster Aufruf beim Programmstart: dann wird auch ein fehlender Port gemeldet).
    fn midi_refresh(&mut self, ctx: &egui::Context, first: bool) {
        self.midi_ports = midi_in::list_ports();
        if self.midi.is_none() {
            if let Some(name) = self.midi_choice.clone() {
                if self.midi_ports.contains(&name) {
                    self.midi_open(ctx, &name);
                } else if first {
                    self.midi_note = Some("nicht gefunden".to_string());
                }
            }
        }
    }

    /// Port öffnen (die alte Verbindung wird dabei geschlossen). Fehler landen in `midi_note`.
    fn midi_open(&mut self, ctx: &egui::Context, name: &str) {
        self.midi = None; // alte Verbindung schließen
        self.clock_map = ClockMap::default(); // neue Verbindung = neuer Treiber-Nullpunkt
        let ctx = ctx.clone(); // Kopie des Fenster-Zugangs für den Callback-Thread
        match MidiIn::open(name, midi_in::epoch(), move || ctx.request_repaint()) {
            Ok(m) => {
                self.midi = Some(m);
                self.midi_note = None;
            }
            Err(e) => self.midi_note = Some(e),
        }
    }

    /// Zeile "MIDI-Eingang": Aufklappliste (aus / Ports), Neu-lesen-Knopf, Status und zuletzt empfangener Befehl.
    fn midi_row(&mut self, ui: &mut Ui) {
        ui.horizontal_wrapped(|ui| {
            ui.label("MIDI-Eingang:");
            let shown = match (&self.midi_choice, &self.midi) {
                (None, _) => "aus".to_string(),
                (Some(n), Some(_)) => n.clone(),
                (Some(n), None) => format!("{n} (nicht gefunden)"),
            };
            let mut pick: Option<Option<String>> = None; // Some(None) = "aus" gewählt
            let combo = egui::ComboBox::from_id_salt("midi_port").selected_text(shown).width(ui.available_width().clamp(100.0, 240.0)).show_ui(ui, |ui| {
                if ui.selectable_label(self.midi_choice.is_none(), "aus").clicked() {
                    pick = Some(None);
                }
                for p in &self.midi_ports {
                    if ui.selectable_label(self.midi_choice.as_deref() == Some(p.as_str()), p).clicked() {
                        pick = Some(Some(p.clone()));
                    }
                }
            });
            // Beim Öffnen der Liste neu suchen (der Port kann seit dem Start entstanden sein)
            let open_now = combo.inner.is_some();
            let opened = open_now && !self.midi_list_open;
            self.midi_list_open = open_now;
            let refresh_click = ui.button("↻").on_hover_text("Port-Liste neu lesen").clicked();
            let ctx = ui.ctx().clone();
            if let Some(choice) = pick {
                self.midi_choice = choice.clone();
                self.settings.midi_port = choice.clone();
                self.settings.save();
                self.midi_note = None;
                match choice {
                    Some(name) => self.midi_open(&ctx, &name),
                    None => self.midi = None, // aus: Verbindung schließen
                }
            } else if opened || refresh_click {
                self.midi_refresh(&ctx, false);
            }
            if let Some(note) = &self.midi_note {
                ui.colored_label(Color32::YELLOW, note);
            }
            if let Some(m) = &self.midi {
                let last = match self.last_cc {
                    Some((cc, v, t)) => format!("zuletzt CC {cc} = {v} vor {:.0} s", t.elapsed().as_secs_f32()),
                    None => "noch nichts empfangen".to_string(),
                };
                ui.label(format!("MIDI: {} - {last}", m.port_name));
                ui.ctx().request_repaint_after(std::time::Duration::from_millis(500)); // "vor x s" aktuell halten
            }
        });
    }

    /// Einen Text in der ersten Schreibweise aus `candidates` zeichnen, die in `max_w` Pixel Breite passt (die Liste geht
    /// von lang nach kurz). Passt keine, wird nichts gezeichnet - so überlappen sich Beschriftungen nie.
    /// `bottom_center` = Mitte der Unterkante des Textes.
    fn fit_label(painter: &Painter, candidates: &[String], bottom_center: Pos2, max_w: f32, color: Color32) {
        let font = egui::FontId::proportional(13.0);
        for c in candidates {
            let galley = painter.layout_no_wrap(c.clone(), font.clone(), color); // Text vermessen, ohne ihn schon zu malen
            let size = galley.size();
            if size.x <= max_w {
                painter.galley(Pos2::new(bottom_center.x - size.x / 2.0, bottom_center.y - size.y), galley, color);
                return;
            }
        }
    }

    /// Alle fünf Geräte nebeneinander in `rect` zeichnen (Reihenfolge von SPECS). Ein Gerät ohne DLL bekommt ein graues
    /// Feld "nicht gebaut" in seiner ungefähren Größe.
    fn draw_stage(&self, painter: &Painter, rect: Rect) {
        // --- 1. Größe jedes Geräts im Bühnen-Koordinatensystem (Foto-Pixel) ---
        // LED-Abstand der Instrumente (ANNAHME, siehe Dateikopf); Matrix und Lampen leiten ihren davon ab
        let pitch = self.devices.iter().find_map(|d| d.shape.as_ref().map(|s| s.spacing)).unwrap_or(DEFAULT_PITCH);
        let lamp_pitch = pitch * LAMP_PITCH_FACTOR;
        let matrix_pitch = pitch * MATRIX_PITCH_FACTOR;
        // Ein Platz auf der Bühne: das Gerät (None = DLL fehlt), Breite, Höhe und linke obere Ecke seines Zeichenbereichs
        struct Slot<'a> {
            spec: &'static Spec,
            dev: Option<&'a Device>,
            w: f32,
            h: f32,
            origin: Pt,
        }
        let slots: Vec<Slot> = SPECS
            .iter()
            .map(|spec| {
                let dev = self.devices.iter().find(|d| d.spec.dll == spec.dll);
                let (w, h, origin) = match dev {
                    Some(d) => match &d.shape {
                        Some(s) => (s.max[0] - s.min[0] + 2.0 * PAD, s.max[1] - s.min[1] + 2.0 * PAD, [s.min[0] - PAD, s.min[1] - PAD]),
                        // Lampe: Breite mindestens LAMP_MIN_W; die Säule steht mittig darin, darum beginnt der Zeichenbereich
                        // (origin) um den halben Überschuss links von der Säule
                        None if spec.kind == Kind::Lamp => {
                            let inner = LAMP_ROW_GAP * lamp_pitch + lamp_pitch + 2.0 * PAD;
                            let w = inner.max(LAMP_MIN_W);
                            (w, d.sim.info.num_leds as f32 * lamp_pitch + 2.0 * PAD, [-(w - inner) / 2.0, 0.0])
                        }
                        None => (
                            d.sim.info.matrix_w as f32 * matrix_pitch + 2.0 * PAD,
                            d.sim.info.matrix_h as f32 * matrix_pitch + 2.0 * PAD,
                            [0.0, 0.0],
                        ),
                    },
                    None => (spec.fallback.0, spec.fallback.1, [0.0, 0.0]),
                };
                Slot { spec, dev, w, h, origin }
            })
            .collect();
        let total_w: f32 = slots.iter().map(|s| s.w).sum::<f32>() + GAP * (slots.len() as f32 - 1.0);
        let total_h: f32 = slots.iter().map(|s| s.h).fold(0.0, f32::max);

        // --- 2. ein gemeinsamer Maßstab, der in den freien Platz passt (oben ein Streifen für die Beschriftung) ---
        let label_h = 20.0;
        let margin = 8.0;
        let scale = ((rect.width() - 2.0 * margin) / total_w).min((rect.height() - 2.0 * margin - label_h) / total_h).max(0.01);
        let drawn = Vec2::new(total_w * scale, total_h * scale);
        let top_left = Pos2::new(rect.center().x - drawn.x / 2.0, rect.center().y + label_h / 2.0 - drawn.y / 2.0);

        // Anzeige-Helligkeit: "echt" = brightness/255; "aufgehellt" = Wurzel davon (80/255 -> 0,56)
        let mut x_off = 0.0;
        for slot in &slots {
            // Umrechnung Grafik -> Bildschirm: Gerät in seinem Streifen von x_off an, senkrecht zentriert
            let dev_origin = Pos2::new(top_left.x + x_off * scale, top_left.y + (total_h - slot.h) * scale / 2.0);
            let to_screen = |p: Pt| Pos2::new(dev_origin.x + (p[0] - slot.origin[0]) * scale, dev_origin.y + (p[1] - slot.origin[1]) * scale);
            // Beschriftung über dem Gerät: so viel Platz wie die Spalte des Geräts plus die halbe Lücke zu beiden Nachbarn
            // (die Lücke wird nur zu 80 % genutzt, damit zwei Beschriftungen nie aneinanderstoßen)
            let col_w = (slot.w + GAP * 0.8) * scale;
            let mut names = Vec::new();
            if let Some(d) = slot.dev {
                if slot.spec.kind != Kind::Lamp {
                    names.push(format!("{} ({})", slot.spec.label, d.sim.info.name()));
                }
            }
            names.push(slot.spec.label.to_string());
            names.push(slot.spec.short.to_string());
            let label_pos = Pos2::new(dev_origin.x + slot.w * scale / 2.0, top_left.y - 4.0);
            Self::fit_label(painter, &names, label_pos, col_w, Color32::from_gray(150));

            let Some(d) = slot.dev else {
                // DLL fehlt (oder war nicht ladbar): graues Feld mit Hinweis, der Rest der Bühne läuft weiter
                let r = Rect::from_min_size(dev_origin, Vec2::new(slot.w * scale, slot.h * scale));
                painter.rect_filled(r, 6.0, Color32::from_gray(20));
                painter.rect_stroke(r, 6.0, Stroke::new(1.0, Color32::from_gray(45)), StrokeKind::Inside);
                Self::fit_label(
                    painter,
                    &["nicht gebaut".to_string(), "n. gebaut".to_string(), "-".to_string()],
                    Pos2::new(r.center().x, r.center().y + 8.0),
                    r.width() - 6.0,
                    Color32::from_gray(90),
                );
                x_off += slot.w + GAP;
                continue;
            };
            let b = d.brightness.clamp(0, 255) as f32 / 255.0;
            let gain = if self.settings.bright_view { b.sqrt() } else { b };
            match &d.shape {
                Some(shape) => self.draw_instrument(painter, d, shape, gain, scale, &to_screen),
                None if slot.spec.kind == Kind::Lamp => self.draw_lamp(painter, d, gain, lamp_pitch * scale, &to_screen, PAD),
                None => self.draw_matrix(painter, d, gain, matrix_pitch * scale, &to_screen, PAD),
            }
            x_off += slot.w + GAP;
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
                    if self.settings.bright_view && top > 0 {
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

    /// Lampe: zwei senkrechte LED-Reihen nebeneinander, beide mit DEMSELBEN Bild (Ausgang 1), LED 0 unten oder oben je nach
    /// LAMP_IDX0_AT_BOTTOM (aus der DLL). `pitch` = LED-Abstand auf dem Bildschirm in Pixeln.
    fn draw_lamp(&self, painter: &Painter, d: &Device, gain: f32, pitch: f32, to_screen: &dyn Fn(Pt) -> Pos2, pad: f32) {
        let n = d.sim.info.num_leds as usize;
        let radius = pitch * 0.42;
        let at_bottom = d.sim.info.lamp_idx0_at_bottom != 0;
        let origin = to_screen([pad, pad]); // linke obere Ecke der Säule (to_screen kennt die Verschiebung des Zeichenbereichs)
        let row_gap = LAMP_ROW_GAP * pitch;
        // Gehäuse: dunkles Rechteck hinter beiden Reihen (Breite = Reihenabstand + ein LED-Abstand)
        let body = Rect::from_min_size(origin, Vec2::new(row_gap + pitch, pitch * n as f32)).expand(pitch * 0.35);
        painter.rect_filled(body, pitch * 0.6, Color32::from_gray(26));
        painter.rect_stroke(body, pitch * 0.6, Stroke::new(1.0, Color32::from_gray(50)), StrokeKind::Outside);
        for i in 0..n {
            if i * 3 + 2 >= d.rgb1.len() {
                break;
            }
            // Höhe über dem Boden in LED-Abständen: LED 0 unten (Standard) oder oben; Bildschirm-y wächst nach unten
            let from_bottom = if at_bottom { i } else { n - 1 - i };
            let y = origin.y + (n - 1 - from_bottom) as f32 * pitch + pitch * 0.5;
            let rgb = &d.rgb1[i * 3..i * 3 + 3];
            for row in 0..2 {
                // zwei Reihen, dasselbe Bild: beide hängen am selben Datensignal
                let x = origin.x + pitch * 0.5 + row as f32 * row_gap;
                self.draw_led(painter, Pos2::new(x, y), radius, rgb, gain, 6.0);
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
