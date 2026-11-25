// settings.rs - die kleinen Einstellungen von showview, die zwischen zwei Programmstarts erhalten bleiben.
//
// Sie liegen in einer JSON-Datei: %APPDATA%\showview\settings.json (Windows), sonst `showview-settings.json` im
// Arbeitsordner. Gemerkt werden (alles Bequemlichkeiten - fehlt die Datei oder ist sie kaputt, gelten die Standardwerte):
//   midi_port   Name des gewählten MIDI-Eingangs (null = aus)                       (seit Issue #25)
//   bright_view Schalter "aufgehellt" im Reiter Bühne (Standard an)                  (seit Issue #24)
//   correction  Schalter "Farbkorrektur" im Reiter Bühne (Standard an)               (seit Issue #24)
//
// Rust-Idiome: `#[derive(Serialize, Deserialize)]` erzeugt den Code, der die Struktur in JSON schreibt und aus JSON
// liest (Feldname = Schlüssel). `#[serde(default)]` über der Struktur: fehlende Schlüssel (z. B. in einer alten Datei,
// die nur midi_port kennt) bekommen den Wert aus `Default`.

use serde::{Deserialize, Serialize};
use std::path::PathBuf;

/// Alle gemerkten Einstellungen.
#[derive(Serialize, Deserialize, Clone, PartialEq, Debug)]
#[serde(default)]
pub struct Settings {
    pub midi_port: Option<String>,
    pub bright_view: bool,
    pub correction: bool,
}

impl Default for Settings {
    /// Standardwerte: kein MIDI-Port, aufgehellte Darstellung und Farbkorrektur an.
    fn default() -> Self {
        Self { midi_port: None, bright_view: true, correction: true }
    }
}

/// Wo die Einstellungen liegen.
fn path() -> PathBuf {
    match std::env::var_os("APPDATA") {
        Some(dir) => PathBuf::from(dir).join("showview").join("settings.json"),
        None => PathBuf::from("showview-settings.json"),
    }
}

impl Settings {
    /// Einstellungen lesen; ohne (lesbare) Datei die Standardwerte.
    pub fn load() -> Settings {
        std::fs::read_to_string(path()).ok().and_then(|t| serde_json::from_str(&t).ok()).unwrap_or_default()
    }

    /// Einstellungen schreiben. Fehler werden ignoriert: die Einstellung ist nur eine Bequemlichkeit.
    pub fn save(&self) {
        let p = path();
        if let Some(dir) = p.parent() {
            let _ = std::fs::create_dir_all(dir);
        }
        if let Ok(text) = serde_json::to_string_pretty(self) {
            let _ = std::fs::write(p, text);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Eine alte Datei aus Issue #25 kennt nur den Port: die neuen Schalter bekommen ihren Standardwert (an).
    #[test]
    fn old_file_gets_defaults() {
        let s: Settings = serde_json::from_str(r#"{"midi_port":"Loop"}"#).unwrap();
        assert_eq!(s, Settings { midi_port: Some("Loop".into()), bright_view: true, correction: true });
    }

    /// Schreiben und wieder lesen ergibt dasselbe.
    #[test]
    fn roundtrip() {
        let s = Settings { midi_port: None, bright_view: false, correction: false };
        let t = serde_json::to_string(&s).unwrap();
        assert_eq!(serde_json::from_str::<Settings>(&t).unwrap(), s);
    }
}
