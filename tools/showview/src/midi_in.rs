// midi_in.rs - MIDI-Eingang für den Reiter "Bühne" (Issue #25).
//
// Der Simulator soll sich wie die Gitarre mit dem WIDI verhalten (siehe midiApplyCC() in der Firmware):
//   NUR Kanal 10 (Statusbyte 0xB9 = Control Change, Kanal 10), CC 22 = Song wählen, CC 23 = Part wählen.
// Alles andere wird ignoriert. Wer die Befehle schickt, ist egal - z. B. das LP16-Tool über einen virtuellen
// MIDI-Port (Windows MIDI Services Loopback oder loopMIDI), oder das Beispielprogramm examples/send_cc.rs.
//
// Die Datei hat drei Teile:
//   1. REINE LOGIK (ohne Port, ohne DLL, mit `cargo test` geprüft):
//        parse_cc()   - aus den MIDI-Bytes den CC herausholen (oder nichts)
//        ClockMap     - den Zeitstempel des MIDI-Treibers auf die Uhr des Programms umrechnen
//        run_to()     - Geräte bis zu einem Zeitpunkt vorstellen und dabei Befehle auf die Millisekunde genau einfügen
//   2. MidiIn        - die echte Verbindung zu einem Port (midir). Der Callback läuft in einem eigenen Thread des
//                      Treibers und schickt jedes passende Ereignis über einen Kanal (std::sync::mpsc) an das Fenster.
//   3. Einstellungen - der gewählte Port wird in einer kleinen JSON-Datei gemerkt (settings_load/settings_save).
//
// WARUM ZEITSTEMPEL? Das Fenster zeichnet nur etwa alle 16 ms ein Bild. Würde man einen Befehl erst beim nächsten Bild
// anwenden, käme er bis zu 16 ms (bei hängendem Fenster mehr) zu spät, und das hinge von der Bildrate ab. Der MIDI-Treiber
// stempelt jedes Ereignis aber selbst mit der Ankunftszeit (Mikrosekunden). Damit stellt run_to() die Geräte erst bis
// genau zu diesem Zeitpunkt vor, gibt dann den Befehl und rechnet danach weiter - unabhängig von der Bildrate.
//
// ANNAHME (bewusst): Die Funk-Verzögerung der Clients (ESP-NOW, wenige ms) wird NICHT nachgebildet - alle Geräte
// bekommen den Befehl im selben Moment.

use midir::{Ignore, MidiInput, MidiInputConnection};
use std::path::PathBuf;
use std::sync::mpsc::{Receiver, Sender, channel};
use std::sync::OnceLock;
use std::time::Instant;

/// MIDI-Kanal der Firmware (1-basiert, wie in sim_midi(); im Statusbyte steht er 0-basiert: 0xB0 + 9 = 0xB9).
pub const MIDI_CHANNEL: i32 = 10;
/// CC 22 = Song wählen, CC 23 = Part wählen (wie midiApplyCC()).
pub const CC_SONG: u8 = 22;
pub const CC_PART: u8 = 23;

/// Nullpunkt der Programm-Uhr: der Zeitpunkt des ersten Aufrufs. Alle Zeiten in Mikrosekunden (arrival_us, t0 der
/// Bühne) zählen von hier. `OnceLock` = "wird genau einmal gefüllt, danach nur noch gelesen" (ohne `static mut`).
pub fn epoch() -> Instant {
    static EPOCH: OnceLock<Instant> = OnceLock::new();
    *EPOCH.get_or_init(Instant::now)
}

/// Ein ausgewertetes Ereignis, so wie der Callback es weitergibt.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct MidiEvent {
    pub stamp_us: u64,   // Zeitstempel des MIDI-Treibers (Mikrosekunden, Nullpunkt beliebig)
    pub arrival_us: u64, // Zeitpunkt, an dem der Callback lief (Mikrosekunden seit dem Programm-Epoch, siehe MidiIn)
    pub cc: u8,          // 22 oder 23
    pub value: u8,       // 0..127
}

/// Aus den rohen MIDI-Bytes (Statusbyte, Daten 1, Daten 2) den CC holen. Nur 0xB9 + CC 22/23 zählen, alles andere
/// (andere Kanäle, Noten, andere CCs, kaputte Längen) gibt None zurück. Rückgabe: (cc, wert).
pub fn parse_cc(bytes: &[u8]) -> Option<(u8, u8)> {
    // `[a, b, c]` ist ein Slice-Muster: passt nur, wenn genau drei Bytes da sind.
    if let [status, cc, value] = *bytes {
        if status == 0xB0 + (MIDI_CHANNEL as u8 - 1) && (cc == CC_SONG || cc == CC_PART) && value < 128 {
            return Some((cc, value));
        }
    }
    None
}

/// Rechnet Zeitstempel des MIDI-Treibers auf die Uhr des Programms um.
///
/// Der Treiber-Zeitstempel hat einen unbekannten Nullpunkt. Die Uhr des Programms (Instant, hier in µs seit einem
/// Epoch) kennt dafür die Ankunft im Callback - aber mit einer Verzögerung (Thread-Wechsel, Treiber-Puffer), die mal
/// größer, mal kleiner ist. Der Zeitstempel selbst hat dagegen kaum Zittern. Darum gilt: die Beziehung
/// "Ankunft - Zeitstempel" wird als kleinster bisher gesehener Wert gemerkt (= das Ereignis mit der geringsten
/// Verzögerung). Danach ist  geschätzte Zeit = Zeitstempel + dieser Versatz; die Abstände zwischen Ereignissen stammen
/// so vom exakten Treiber-Zeitstempel, und nur der Gesamtversatz ist auf die kleinste Verzögerung (meist < 1 ms) genau.
#[derive(Default)]
pub struct ClockMap {
    offset_us: Option<i64>, // kleinster Wert von (Ankunft - Zeitstempel) in µs
}

impl ClockMap {
    /// Ereignis einrechnen. Rückgabe: geschätzter Zeitpunkt des Ereignisses auf der Programm-Uhr (µs seit Epoch).
    pub fn map(&mut self, stamp_us: u64, arrival_us: u64) -> i64 {
        let off = arrival_us as i64 - stamp_us as i64;
        let best = match self.offset_us {
            Some(o) => o.min(off), // kleiner = weniger Verzögerung = besser
            None => off,
        };
        self.offset_us = Some(best);
        stamp_us as i64 + best
    }
}

/// Was run_to() von den Geräten braucht. Die echte Umsetzung ist `Sim` (DLL), im Test eine Attrappe.
pub trait Devices {
    /// Die Zeit aller Geräte um `ms` Millisekunden vorstellen.
    fn advance(&mut self, ms: u32);
    /// Einen MIDI-CC an alle Geräte geben (Kanal 1-16).
    fn midi(&mut self, channel: i32, cc: i32, value: i32);
}

/// Alle Geräte bis zur Zeit `target_ms` vorstellen und dabei die Ereignisse `events` zum richtigen Zeitpunkt einfügen.
///
/// - `done_ms`: bis wohin die Geräte schon gerechnet sind (wird fortgeschrieben; die DLLs rechnen nur in ganzen ms).
/// - `events`: Paare (Zeitpunkt in ms auf derselben Uhr, Ereignis), in Ankunftsreihenfolge. Ein Ereignis, dessen
///   Zeitpunkt schon vorbei ist (oder vor dem Start liegt), wird sofort angewendet.
/// - Ein Ereignis wird auf die nächste ganze Millisekunde gerundet, also +-0,5 ms genau, egal wie groß der Sprung ist.
/// Der Rest bis `target_ms` (unter 1 ms) bleibt für den nächsten Aufruf, damit keine Zeit verloren geht.
pub fn run_to<D: Devices>(devs: &mut D, done_ms: &mut f64, events: &[(f64, MidiEvent)], target_ms: f64) {
    for (t, ev) in events {
        // Bis zum Ereignis vorstellen (nie über `target_ms` hinaus: ein Ereignis aus der Zukunft kommt nicht vor, aber
        // zur Sicherheit begrenzen wir es)
        let t = t.min(target_ms);
        let n = (t - *done_ms).round();
        if n >= 1.0 {
            devs.advance(n as u32);
            *done_ms += n;
        }
        devs.midi(MIDI_CHANNEL, ev.cc as i32, ev.value as i32);
    }
    let n = (target_ms - *done_ms).floor();
    if n >= 1.0 {
        devs.advance(n as u32);
        *done_ms += n;
    }
}

/// Eine gefundene Port-Liste (Namen) neu lesen. Fehler (kein MIDI-System) = leere Liste.
pub fn list_ports() -> Vec<String> {
    let Ok(input) = MidiInput::new("showview-liste") else { return Vec::new() };
    input.ports().iter().filter_map(|p| input.port_name(p).ok()).collect()
}

/// Eine offene Verbindung zu einem Eingangs-Port. Solange das Objekt lebt, kommen Ereignisse; beim Wegwerfen
/// (Drop) schließt sich der Port. Der Callback läuft im Thread des Treibers und tut nur das Nötigste.
pub struct MidiIn {
    _conn: MidiInputConnection<()>, // Unterstrich: wird nur festgehalten, nicht benutzt
    rx: Receiver<MidiEvent>,
    pub port_name: String,
}

impl MidiIn {
    /// Port mit diesem Namen öffnen. `wake` wird bei jedem passenden Ereignis aufgerufen (weckt das Fenster, damit
    /// es auch bei ruhendem Bild gleich reagiert). `epoch` = Nullpunkt der Programm-Uhr für `arrival_us`.
    pub fn open(name: &str, epoch: Instant, wake: impl Fn() + Send + 'static) -> Result<MidiIn, String> {
        let mut input = MidiInput::new("showview").map_err(|e| e.to_string())?;
        input.ignore(Ignore::All); // SysEx, Timing-Takt, Active Sensing usw. gar nicht erst liefern
        let port = input
            .ports()
            .into_iter()
            .find(|p| input.port_name(p).map(|n| n == name).unwrap_or(false))
            .ok_or_else(|| "nicht gefunden".to_string())?;
        let (tx, rx): (Sender<MidiEvent>, Receiver<MidiEvent>) = channel();
        let conn = input
            .connect(
                &port,
                "showview-eingang",
                move |stamp_us, bytes, _| {
                    // Ankunftszeit sofort festhalten, bevor irgendetwas anderes passiert
                    let arrival_us = epoch.elapsed().as_micros() as u64;
                    if let Some((cc, value)) = parse_cc(bytes) {
                        let _ = tx.send(MidiEvent { stamp_us, arrival_us, cc, value }); // Fenster weg = Fehler egal
                        wake();
                    }
                },
                (),
            )
            .map_err(|e| e.to_string())?;
        Ok(MidiIn { _conn: conn, rx, port_name: name.to_string() })
    }

    /// Alles abholen, was seit dem letzten Aufruf angekommen ist (in Ankunftsreihenfolge).
    pub fn drain(&self) -> Vec<MidiEvent> {
        self.rx.try_iter().collect()
    }
}

// ---------------------------------------------------------------- Einstellungen

/// Wo die Einstellungen liegen: %APPDATA%\showview\settings.json (Windows), sonst neben dem Arbeitsordner.
fn settings_path() -> PathBuf {
    match std::env::var_os("APPDATA") {
        Some(dir) => PathBuf::from(dir).join("showview").join("settings.json"),
        None => PathBuf::from("showview-settings.json"),
    }
}

/// Den gemerkten Port-Namen lesen (None = aus oder keine Datei).
pub fn settings_load() -> Option<String> {
    let text = std::fs::read_to_string(settings_path()).ok()?;
    let v: serde_json::Value = serde_json::from_str(&text).ok()?;
    v.get("midi_port")?.as_str().map(|s| s.to_string())
}

/// Den Port-Namen merken (None = "aus"). Fehler werden ignoriert: die Einstellung ist nur eine Bequemlichkeit.
pub fn settings_save(port: Option<&str>) {
    let path = settings_path();
    if let Some(dir) = path.parent() {
        let _ = std::fs::create_dir_all(dir);
    }
    let v = serde_json::json!({ "midi_port": port });
    let _ = std::fs::write(path, v.to_string());
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Attrappe statt DLL: schreibt auf, was in welcher Simulator-Zeit passiert.
    #[derive(Default)]
    struct Fake {
        now_ms: u32,
        log: Vec<(u32, i32, i32, i32)>, // (Zeit, Kanal, CC, Wert)
    }
    impl Devices for Fake {
        fn advance(&mut self, ms: u32) {
            self.now_ms += ms;
        }
        fn midi(&mut self, channel: i32, cc: i32, value: i32) {
            self.log.push((self.now_ms, channel, cc, value));
        }
    }

    fn ev(cc: u8, value: u8) -> MidiEvent {
        MidiEvent { stamp_us: 0, arrival_us: 0, cc, value }
    }

    #[test]
    fn parse_nur_kanal_10_cc_22_23() {
        assert_eq!(parse_cc(&[0xB9, 22, 30]), Some((22, 30)));
        assert_eq!(parse_cc(&[0xB9, 23, 7]), Some((23, 7)));
        assert_eq!(parse_cc(&[0xB8, 22, 30]), None); // Kanal 9
        assert_eq!(parse_cc(&[0xBA, 22, 30]), None); // Kanal 11
        assert_eq!(parse_cc(&[0xB9, 21, 30]), None); // anderer CC
        assert_eq!(parse_cc(&[0x99, 22, 30]), None); // Note On statt CC
        assert_eq!(parse_cc(&[0xB9, 22]), None); // zu kurz
        assert_eq!(parse_cc(&[0xB9, 22, 30, 1]), None); // zu lang
    }

    #[test]
    fn befehle_in_reihenfolge_zum_richtigen_zeitpunkt() {
        let mut f = Fake::default();
        let mut done = 0.0;
        let events = [(1000.0, ev(22, 30)), (1500.4, ev(23, 2)), (4000.0, ev(22, 0))];
        // ein einziger grober Sprung von 5 s (= Fenster hat lange nicht gezeichnet)
        run_to(&mut f, &mut done, &events, 5000.0);
        assert_eq!(f.log, vec![(1000, 10, 22, 30), (1500, 10, 23, 2), (4000, 10, 22, 0)]);
        assert_eq!(f.now_ms, 5000);
    }

    #[test]
    fn zeitpunkt_unabhaengig_von_der_bildrate() {
        let events = [(333.0, ev(22, 30)), (777.0, ev(23, 5))];
        // gleiche Ereignisse, einmal mit 16-ms-Bildern, einmal mit 100-ms-Bildern, einmal in einem Stück
        for step in [1.0_f64, 16.0, 100.0, 5000.0] {
            let mut f = Fake::default();
            let mut done = 0.0;
            let mut t = 0.0;
            let mut sent = 0;
            while t < 1000.0 {
                t += step;
                // nur die Ereignisse übergeben, die bis `t` fällig sind (wie das Fenster es mit seiner Warteschlange täte)
                let due: Vec<_> = events[sent..].iter().filter(|(et, _)| *et <= t).cloned().collect();
                sent += due.len();
                run_to(&mut f, &mut done, &due, t.min(1000.0));
            }
            assert_eq!(f.log.len(), 2, "Bildschritt {step}");
            assert!((f.log[0].0 as i64 - 333).abs() <= 1, "Bildschritt {step}: {:?}", f.log);
            assert!((f.log[1].0 as i64 - 777).abs() <= 1, "Bildschritt {step}: {:?}", f.log);
            assert_eq!(f.log[0].2, 22);
            assert_eq!(f.log[1].2, 23);
        }
    }

    #[test]
    fn verspaetetes_ereignis_wird_sofort_angewendet() {
        let mut f = Fake::default();
        let mut done = 0.0;
        run_to(&mut f, &mut done, &[], 100.0);
        // Ereignis lag vor dem Start/vor der schon gerechneten Zeit: kein Zurückspulen, sofort anwenden
        run_to(&mut f, &mut done, &[(-50.0, ev(22, 1))], 120.0);
        assert_eq!(f.log, vec![(100, 10, 22, 1)]);
        assert_eq!(f.now_ms, 120);
    }

    #[test]
    fn bruchteile_gehen_nicht_verloren() {
        let mut f = Fake::default();
        let mut done = 0.0;
        for i in 1..=100 {
            run_to(&mut f, &mut done, &[], i as f64 * 0.7); // 70 ms in 0,7-ms-Schritten
        }
        assert_eq!(f.now_ms, 70);
    }

    #[test]
    fn uhr_umrechnung_nimmt_geringste_verzoegerung() {
        let mut m = ClockMap::default();
        // erstes Ereignis: Treiberzeit 1 000 000 µs, Ankunft 5 000 µs später (Callback langsam)
        let t1 = m.map(1_000_000, 10_005_000 + 0);
        assert_eq!(t1, 10_005_000);
        // zweites Ereignis 1 s später, diesmal nur 200 µs Verzögerung: der Versatz wird kleiner
        let t2 = m.map(2_000_000, 11_000_200);
        assert_eq!(t2, 11_000_200);
        // drittes Ereignis mit großer Verzögerung (3 ms): Zeit kommt aus dem gemerkten kleinsten Versatz, nicht aus der Ankunft
        let t3 = m.map(3_000_000, 12_003_000);
        assert_eq!(t3, 12_000_200);
    }
}
