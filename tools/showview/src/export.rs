// export.rs - holt die Daten von tools/songgen.py (Aufruf mit --json) und liest sie in die Strukturen aus model.rs.
//
// Warum über Python? Die ganze Zeitrechnung eines Songs (Tails, Vorlauf vor dem Start-MIDI, Schluss-Black, Tempo
// pro Part, Blinder in Takten) steckt in songgen.py. Würde die Ansicht das in Rust nachbauen, liefen zwei
// Rechnungen irgendwann auseinander. So zeigt die Ansicht genau die Zeiten, die auch im erzeugten C++-Code stehen.
// songgen.py --json schreibt dabei keine einzige Datei (nur lesen).
//
// Rust-Idiome hier:
//   Result<T, String>  = entweder Ok(Wert) oder Err(Fehlertext); `?` hinter einem Aufruf gibt einen Fehler sofort
//                        an den Aufrufer weiter.
//   PathBuf / &Path    = Dateipfad (besitzend / geliehen), `join` hängt Teile an.

use crate::model::{SongData, SongList};
use std::path::{Path, PathBuf};
use std::process::Command;

/// Sucht den Projektordner (den mit tools/songgen.py): erst das Argument der Befehlszeile, dann vom aktuellen Ordner
/// und vom Ordner der .exe aus jeweils aufwärts. So läuft das Programm per Doppelklick aus target/release/ genauso
/// wie per `cargo run` aus tools/showview/.
pub fn find_root(arg: Option<String>) -> Option<PathBuf> {
    let mut starts: Vec<PathBuf> = Vec::new();
    if let Some(a) = arg {
        starts.push(PathBuf::from(a));
    }
    if let Ok(cwd) = std::env::current_dir() {
        starts.push(cwd);
    }
    if let Ok(exe) = std::env::current_exe() {
        starts.push(exe);
    }
    for start in starts {
        // ancestors() liefert den Pfad selbst und dann jeden Elternordner bis zum Laufwerk
        for dir in start.ancestors() {
            if dir.join("tools").join("songgen.py").is_file() {
                return Some(dir.to_path_buf());
            }
        }
    }
    None
}

/// Der Python-Interpreter des Projekts (tools/.venv, dort sind yaml und openpyxl installiert), sonst "python".
fn python(root: &Path) -> PathBuf {
    let venv = root.join("tools").join(".venv").join("Scripts").join("python.exe");
    if venv.is_file() { venv } else { PathBuf::from("python") }
}

/// songgen.py mit den gegebenen Argumenten starten und die Ausgabe (JSON-Text) zurückgeben.
fn run_songgen(root: &Path, args: &[&str]) -> Result<String, String> {
    let mut cmd = Command::new(python(root));
    cmd.arg(root.join("tools").join("songgen.py")).args(args).current_dir(root);
    // Unter Windows würde sonst bei jedem Aufruf kurz ein schwarzes Konsolenfenster aufblitzen.
    // CREATE_NO_WINDOW = 0x08000000 (Windows-Konstante); `#[cfg(windows)]` übersetzt den Block nur unter Windows.
    #[cfg(windows)]
    {
        use std::os::windows::process::CommandExt;
        cmd.creation_flags(0x0800_0000);
    }
    let out = cmd.output().map_err(|e| format!("songgen.py konnte nicht gestartet werden: {e}"))?;
    if !out.status.success() {
        // songgen.py meldet Fehler (z. B. Partname aus show.yaml fehlt in der Tabelle) auf stderr
        let err = String::from_utf8_lossy(&out.stderr);
        return Err(if err.trim().is_empty() { "songgen.py ist mit einem Fehler beendet".into() } else { err.trim().to_string() });
    }
    String::from_utf8(out.stdout).map_err(|e| format!("Ausgabe von songgen.py ist kein UTF-8: {e}"))
}

/// Liste aller Song-Ordner.
pub fn load_list(root: &Path) -> Result<SongList, String> {
    let text = run_songgen(root, &["--json"])?;
    serde_json::from_str(&text).map_err(|e| format!("Song-Liste nicht lesbar: {e}"))
}

/// Ablauf eines Songs (dir = Ordnername unter songs/).
pub fn load_song(root: &Path, dir: &str) -> Result<SongData, String> {
    let text = run_songgen(root, &[dir, "--json"])?;
    serde_json::from_str(&text).map_err(|e| format!("Ablauf von {dir} nicht lesbar: {e}"))
}

/// Die Dateien, von denen der Ablauf eines Songs abhängt - ändert sich eine davon, wird neu geladen.
pub fn watched_files(root: &Path, dir: &str) -> Vec<PathBuf> {
    let song = root.join("songs").join(dir);
    vec![song.join("show.yaml"), song.join("quelle").join("struktur.xlsx")]
}

/// Zeitstempel der letzten Änderung jeder Datei (None, wenn es sie nicht gibt) - zum Vergleichen beim Überwachen.
pub fn stamps(files: &[PathBuf]) -> Vec<Option<std::time::SystemTime>> {
    files.iter().map(|f| std::fs::metadata(f).and_then(|m| m.modified()).ok()).collect()
}
