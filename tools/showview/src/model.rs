// model.rs - die Daten, die tools/songgen.py mit --json ausgibt, als Rust-Strukturen.
//
// Wie das funktioniert: serde ("serialize/deserialize") ist die Standard-Bibliothek in Rust, um Daten aus Formaten
// wie JSON in eigene Strukturen zu lesen. `#[derive(Deserialize)]` über einer struct erzeugt beim Übersetzen den
// Code, der ein JSON-Objekt Feld für Feld in diese struct füllt - der Feldname in Rust muss dazu genau so heißen wie
// der Schlüssel im JSON. `#[serde(default)]` heißt: fehlt der Schlüssel im JSON, nimm den Standardwert (0, leer,
// None). So bricht die Ansicht nicht, wenn songgen.py später neue Felder dazubekommt oder alte weglässt.
//
// Alle Zeiten sind Millisekunden ab dem Start-MIDI des Songs (wie im erzeugten Code). Sie werden als f64
// (Kommazahl) gelesen, weil sich damit auf dem Bildschirm am einfachsten rechnen lässt.
//
// Zusammenspiel: export.rs ruft songgen.py auf und liest dessen Ausgabe in diese Strukturen, timeline.rs zeichnet
// sie, main.rs zeigt die Song-Liste und die Details des angeklickten Parts.

use serde::Deserialize;
use serde_json::Value;
use std::collections::BTreeMap;

/// Diese Fassung des JSON-Aufbaus versteht die Ansicht (JSON_SCHEMA in songgen.py). Ist die Zahl dort größer, wird
/// eine Warnung angezeigt - vieles klappt dann trotzdem, weil unbekannte Felder einfach übergangen werden.
pub const SCHEMA: i64 = 1;

/// Ausgabe von `songgen.py --json` ohne Song: alle Song-Ordner unter songs/.
#[derive(Deserialize, Default)]
#[serde(default)]
pub struct SongList {
    pub schema: i64,
    pub songs: Vec<SongEntry>,
}

/// Ein Song-Ordner in der Liste links.
#[derive(Deserialize, Default, Clone)]
#[serde(default)]
pub struct SongEntry {
    pub dir: String,       // Ordnername, z. B. "Maniac_v1"
    pub table: bool,       // quelle/struktur.xlsx vorhanden
    pub show: bool,        // show.yaml vorhanden (nur dann gibt es einen Ablauf zu zeigen)
    pub generated: bool,   // generated.cpp vorhanden
    pub id: Option<i64>,   // Song-ID (nur bei generierten Songs bekannt); Option = "kann fehlen"
    pub name: Option<String>,
}

/// Ausgabe von `songgen.py <Song> --json`: der ganze Ablauf eines Songs.
#[derive(Deserialize, Default)]
#[serde(default)]
pub struct SongData {
    pub schema: i64,
    pub song: SongMeta,
    pub stage: Vec<String>,                          // Bühnengeräte von links nach rechts
    pub parts: Vec<Part>,                            // alle Parts in zeitlicher Reihenfolge (auch Vorlauf und Schluss-Black)
    pub scroll: BTreeMap<String, Option<ScrollPlan>>, // Titel-Lauftext am Songanfang je Matrix-Gerät
    pub entry_scroll: Option<BTreeMap<String, Option<ScrollPlan>>>, // Lauftext nach dem Einstieg aus einem Vorspann
    pub notes: Vec<String>,                          // Hinweise von songgen.py
    pub errors: Vec<String>,                         // Fehler, die das Generieren verhindern würden
}

/// Angaben zum ganzen Song.
#[derive(Deserialize, Default)]
#[serde(default)]
pub struct SongMeta {
    pub id: Option<i64>,
    pub name: Option<String>,
    pub dir: String,
    pub bpm: f64,
    pub function: Option<String>,
    pub scheme: Option<String>,
    pub next_song: Option<String>,     // gesetzt bei einem Vorspann: in diesen Song geht es danach weiter
    pub trailer_entry: Option<String>, // Part, an dem ein Vorspann in diesen Song springt
    pub end_black_ms: Option<f64>,
    pub midi_offset: String,
    pub scroll_text: bool,
}

/// Ein Part der Timeline (ein `case` im erzeugten Code).
#[derive(Deserialize, Default, Clone)]
#[serde(default)]
pub struct Part {
    pub case: i64,
    pub next: i64,
    pub name: String,
    pub start: f64,         // ms ab Start-MIDI
    pub dur: f64,           // Länge in ms
    pub bpm: f64,
    pub beats_per_bar: f64,
    pub lead: f64,          // nur im ersten Part: so viele ms der Tabellenzeile liegen vor dem Start-MIDI
    pub bar_from: Option<f64>, // Taktnummer der Tabelle, an der der Part beginnt (fehlt bei tail/Vorlauf/Ende)
    pub bar_to: Option<f64>,   // ... und vor der er endet
    pub parent: Option<String>, // gesetzt bei einem tail (Schluss-Stück eines Abschnitts)
    pub devices: BTreeMap<String, DeviceCall>, // Aufruf je Bühnengerät
    pub blinders: Vec<Blinder>,
    pub text_info: Option<String>,
    pub lines: Vec<String>,  // der Part in Worten, wie in der Spalte "Effekt (füllt KI)" der Tabelle
    pub errors: Vec<String>,
    pub sec: Value,          // alle Rohangaben aus show.yaml + Tabelle (beliebiges JSON, deshalb Value)
}

/// Was ein Gerät in einem Part abspielt.
#[derive(Deserialize, Default, Clone)]
#[serde(default)]
pub struct DeviceCall {
    pub call: String, // z. B. "scene(SCENE_STAR, 12229, 8, 157)" oder "progFire(10701, 8, 30)"
    pub own: bool,    // true = eigene Angabe unter devices: in show.yaml (weicht von den anderen ab)
}

/// Ein Blinder (warmweißer Blitz), schon in ms umgerechnet.
#[derive(Deserialize, Default, Clone)]
#[serde(default)]
pub struct Blinder {
    pub at_ms: f64,      // Moment der VOLLEN Helligkeit, ms ab Part-Beginn
    pub len_ms: f64,     // ganze Länge inkl. Einblenden
    pub every: f64,      // 0 = einmalig, sonst Wiederholung alle so viele Beats
    pub amount: f64,     // Stärke 0..255
    pub color: String,   // "FX_BLINDER_WARM", "scheme" oder ein CRGB-Ausdruck
    pub devices: Vec<String>, // leer = alle Geräte; sonst Geräte oder Klassen (guitar, lamp, matrix)
    pub attack_ms: f64,  // Einblenden VOR at_ms
    pub hold_ms: f64,    // so lange bleibt er nach at_ms voll
    pub carry: bool,     // klingt im nächsten Part weiter aus
    pub title: bool,     // leuchtet auf der Matrix hinter dem Titel-Lauftext
    pub cascade: Option<Cascade>,
}

/// Kaskade eines Blinders: die Geräte gehen nacheinander an, gemeinsamer Vollpunkt.
#[derive(Deserialize, Default, Clone)]
#[serde(default)]
pub struct Cascade {
    pub step_ms: f64,
    pub steps: BTreeMap<String, f64>, // je Gerät: so viele Stufen vor dem Vollpunkt geht es an
}

/// Plan des Titel-Lauftexts auf einem Matrix-Gerät.
#[derive(Deserialize, Default, Clone)]
#[serde(default)]
pub struct ScrollPlan {
    pub text: String,
    pub mode: String,  // "wait" = erst schwarz warten, dann Text; "fill" = Text sofort, danach Rest-Part
    pub start: f64,    // ms ab Song-Beginn, an dem der Plan beginnt
    pub scroll: f64,   // Länge des Lauftexts in ms
    pub wait: f64,     // nur "wait": so lange schwarz vor dem Text
    pub join_start: f64, // ab hier spielt die Matrix wieder den normalen Ablauf
    pub fill_start: Option<f64>,
    pub fill_dur: Option<f64>,
}

impl Part {
    /// Ein Schlüssel aus den Rohangaben (show.yaml) - None, wenn es ihn nicht gibt.
    pub fn key(&self, k: &str) -> Option<&Value> {
        self.sec.get(k)
    }

    /// Länge eines Beats in ms bei diesem Part-Tempo.
    pub fn beat_ms(&self) -> f64 {
        if self.bpm > 0.0 { 60000.0 / self.bpm } else { 500.0 }
    }

    /// Zeit (ms ab Start-MIDI) einer Taktnummer der Tabelle in diesem Part, z. B. 5.375 -> Moment von Takt 5 + 3/8.
    /// Der erste Part beginnt erst mit dem Start-MIDI, deshalb wird `lead` abgezogen.
    pub fn bar_time(&self, bar: f64) -> Option<f64> {
        let from = self.bar_from?; // `?` bei Option: ist bar_from None, gibt die Funktion sofort None zurück
        let bpb = if self.beats_per_bar > 0.0 { self.beats_per_bar } else { 4.0 };
        Some(self.start - self.lead + (bar - from) * bpb * self.beat_ms())
    }

    /// Ende des Parts in ms.
    pub fn end(&self) -> f64 {
        self.start + self.dur
    }

    /// Szene oder Effekt-Funktion des Parts in Kurzform, z. B. "SCENE_STAR" oder "progFire".
    pub fn effect_short(&self) -> String {
        if let Some(s) = self.key("scene").and_then(|v| v.as_str()) {
            return s.to_string();
        }
        if let Some(f) = self.key("fx").and_then(|v| v.as_str()) {
            return call_name(f);
        }
        // weder scene noch fx: der Aufruf, den alle Geräte ohne eigene Angabe bekommen (Szene aus der Energie)
        self.devices.values().find(|d| !d.own).map(|d| call_name(&d.call)).unwrap_or_default()
    }
}

/// "scene(SCENE_STAR, 1, 2)" -> "SCENE_STAR", "progFire(1, 2)" -> "progFire": der Name, an dem man den Effekt erkennt.
pub fn call_name(call: &str) -> String {
    let head = call.split('(').next().unwrap_or(call).trim();
    if head == "scene" {
        // bei scene(...) ist das erste Argument der eigentliche Name
        if let Some(arg) = call.split('(').nth(1) {
            return arg.split(',').next().unwrap_or("").trim().to_string();
        }
    }
    head.to_string()
}

/// Ein JSON-Wert als kurzer, lesbarer Text (für Beschriftungen): Text ohne Anführungszeichen, alles andere kompakt.
pub fn value_text(v: &Value) -> String {
    match v {
        Value::String(s) => s.clone(),
        Value::Object(m) => m
            .iter()
            .map(|(k, v)| format!("{k} {}", value_text(v)))
            .collect::<Vec<_>>()
            .join(", "),
        Value::Array(a) => a.iter().map(value_text).collect::<Vec<_>>().join(" / "),
        other => other.to_string(),
    }
}
