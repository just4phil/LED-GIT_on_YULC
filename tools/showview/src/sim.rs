// sim.rs - lädt eine Simulator-DLL (sim/bin/sim_<gerät>.dll) und ruft ihre C-Funktionen auf.
//
// Die DLL enthält die ECHTEN Firmware-Quellen des Geräts (Effekte, Songs, Ausgabestufe ...), übersetzt für
// Windows (gebaut von tools/build_sim.py, Schnittstelle: sim/simApi.h). Hier steckt nur die Rust-Seite:
// DLL laden, Funktionen heraussuchen und eine "sichere Hülle" (struct Sim) darum legen, damit der Rest des
// Programms keinen `unsafe`-Code braucht.
//
// Rust-Idiome in dieser Datei:
//   unsafe              Rust prüft Zeiger und Aufrufe in fremden Bibliotheken nicht. Wer `unsafe { ... }` schreibt,
//                       verspricht dem Compiler, dass er selbst aufpasst. Hier ist es nur an den DLL-Aufrufen.
//   extern "C" fn       eine Funktion mit der C-Aufrufkonvention - die einzige Sprache, die Rust (MSVC) und die
//                       DLL (MinGW-g++) sicher gemeinsam sprechen.
//   #[repr(C)]          die Struktur im Speicher genau so anordnen wie in C (Reihenfolge und Größe der Felder),
//                       sonst würden die Bytes der DLL falsch gelesen.
//   Drop                Trait (Schnittstelle) mit der Funktion drop(), die Rust automatisch aufruft, wenn ein
//                       Wert nicht mehr gebraucht wird - hier: DLL entladen und die Kopie löschen.

use libloading::Library;
use std::path::{Path, PathBuf};
use std::sync::atomic::{AtomicU32, Ordering};

/// Gerätebeschreibung der DLL; genau wie `SimInfo` in sim/simApi.h (gleiche Felder in gleicher Reihenfolge).
#[repr(C)]
#[derive(Clone, Copy)]
pub struct SimInfo {
    name: [u8; 16],      // Gerätename als C-Text (Ende = Byte 0)
    pub device_class: i32, // 1 = Gitarre/Bass, 2 = Lampe, 3 = Matrix
    pub num_leds: i32,
    pub leds_out: i32, // so viele LEDs liefert sim_frame() je Ausgang
    pub matrix_w: i32,
    pub matrix_h: i32,
    pub has_markers: i32,
    pub strap_leds: i32,
}

impl SimInfo {
    /// Der Gerätename als normaler Rust-Text.
    pub fn name(&self) -> String {
        let end = self.name.iter().position(|&b| b == 0).unwrap_or(self.name.len());
        String::from_utf8_lossy(&self.name[..end]).into_owned()
    }
}

// Die Funktionstypen der DLL (Gegenstücke zu simApi.h). `unsafe extern "C" fn(...)` = Zeiger auf eine C-Funktion.
type FnVersion = unsafe extern "C" fn() -> i32;
type FnInfo = unsafe extern "C" fn(*mut SimInfo);
type FnReset = unsafe extern "C" fn(u32);
type FnMidi = unsafe extern "C" fn(i32, i32, i32);
type FnAdvance = unsafe extern "C" fn(u32);
type FnFrame = unsafe extern "C" fn(*mut u8, *mut u8, *mut i32) -> i32;
type FnState = unsafe extern "C" fn(*mut i32, *mut i32, *mut u32);
type FnXY = unsafe extern "C" fn(i32, i32) -> i32;

/// Eine geladene Simulator-DLL mit ihren Funktionen.
pub struct Sim {
    // Die Bibliothek muss leben, solange die Funktionszeiger unten benutzt werden. Option, damit drop() sie
    // vor dem Löschen der Datei gezielt entladen kann (take()).
    lib: Option<Library>,
    temp: PathBuf, // die Kopie der DLL, aus der geladen wurde (wird beim Entladen gelöscht)
    reset: FnReset,
    midi: FnMidi,
    advance: FnAdvance,
    frame: FnFrame,
    state: FnState,
    xy: FnXY,
    pub info: SimInfo,
}

/// Zähler für eindeutige Namen der DLL-Kopien (mehrere Starts im selben Programm).
static COPY_COUNTER: AtomicU32 = AtomicU32::new(0);

impl Sim {
    /// DLL laden. Die Datei wird zuerst in den Temp-Ordner kopiert und von dort geladen: Windows sperrt eine
    /// geladene DLL, mit der Kopie lässt sich `python tools/build_sim.py` auch bei offenem Fenster ausführen.
    /// Jedes Laden gibt der DLL außerdem ein frisches Gedächtnis (alle Variablen wie nach dem Einschalten).
    pub fn load(path: &Path) -> Result<Sim, String> {
        let n = COPY_COUNTER.fetch_add(1, Ordering::Relaxed);
        let temp = std::env::temp_dir().join(format!("showview_sim_{}_{}.dll", std::process::id(), n));
        std::fs::copy(path, &temp).map_err(|e| format!("DLL konnte nicht kopiert werden: {e}"))?;
        // unsafe: Laden einer fremden DLL führt deren Startcode aus; get() liefert die Funktion unter ihrem Namen.
        unsafe {
            let lib = Library::new(&temp).map_err(|e| format!("DLL nicht ladbar: {e}"))?;
            // Kleines Makro (Textbaustein): eine Funktion unter ihrem Namen in der DLL suchen und als einfachen
            // Funktionszeiger herausholen (der Zeiger ist kopierbar und unabhängig von der Ausleihe an `lib`)
            macro_rules! sym {
                ($name:literal, $t:ty) => {
                    *lib.get::<$t>($name).map_err(|e| format!("Funktion {} fehlt in der DLL: {e}", String::from_utf8_lossy($name)))?
                };
            }
            let version: FnVersion = sym!(b"sim_api_version", FnVersion);
            if version() != 1 {
                return Err(format!("DLL hat Schnittstellen-Version {}, erwartet 1 - neu bauen: python tools/build_sim.py", version()));
            }
            let info_fn: FnInfo = sym!(b"sim_info", FnInfo);
            let mut info = SimInfo { name: [0; 16], device_class: 0, num_leds: 0, leds_out: 0, matrix_w: 0, matrix_h: 0, has_markers: 0, strap_leds: 0 };
            info_fn(&mut info);
            Ok(Sim {
                reset: sym!(b"sim_reset", FnReset),
                midi: sym!(b"sim_midi", FnMidi),
                advance: sym!(b"sim_advance", FnAdvance),
                frame: sym!(b"sim_frame", FnFrame),
                state: sym!(b"sim_state", FnState),
                xy: sym!(b"sim_xy", FnXY),
                info,
                lib: Some(lib),
                temp,
            })
        }
    }

    /// Einschalt-Zustand herstellen (Uhr 0, Song 0).
    pub fn reset(&mut self, seed: u32) {
        unsafe { (self.reset)(seed) }
    }

    /// Ein MIDI-Controller-Befehl (Kanal 1-16, CC-Nummer, Wert), z. B. (10, 22, 30) = Song 30 starten.
    pub fn midi(&mut self, channel: i32, cc: i32, value: i32) {
        unsafe { (self.midi)(channel, cc, value) }
    }

    /// Simulierte Zeit um `ms` Millisekunden vorstellen (die Show rechnet dabei ihre Bilder).
    pub fn advance(&mut self, ms: u32) {
        unsafe { (self.advance)(ms) }
    }

    /// Das zuletzt gesendete Bild holen: füllt `out1` (RGB je LED, `leds_out` x 3 Byte) und liefert
    /// (Bildnummer, Helligkeit 0..255). `out1` muss mindestens `leds_out * 3` Bytes lang sein.
    pub fn frame(&self, out1: &mut [u8]) -> (i32, i32) {
        assert!(out1.len() >= self.info.leds_out as usize * 3, "Puffer zu klein");
        let mut brightness = 255;
        // *mut u8 = veränderbarer Zeiger auf das erste Byte; null = "diesen Ausgang nicht liefern"
        let n = unsafe { (self.frame)(out1.as_mut_ptr(), std::ptr::null_mut(), &mut brightness) };
        (n, brightness)
    }

    /// (Song-ID, Part, Millisekunden seit Part-Beginn)
    pub fn state(&self) -> (i32, i32, u32) {
        let (mut song, mut part, mut ms) = (0, 0, 0u32);
        unsafe { (self.state)(&mut song, &mut part, &mut ms) };
        (song, part, ms)
    }

    /// LED-Nummer zur Matrix-Koordinate (x = Spalte, y = Zeile, 0/0 oben links), None außerhalb.
    pub fn xy(&self, x: i32, y: i32) -> Option<usize> {
        let i = unsafe { (self.xy)(x, y) };
        if i < 0 { None } else { Some(i as usize) }
    }
}

impl Drop for Sim {
    /// Beim Wegwerfen: DLL entladen, dann die Temp-Kopie löschen (geht erst nach dem Entladen).
    fn drop(&mut self) {
        drop(self.lib.take());
        let _ = std::fs::remove_file(&self.temp);
    }
}
