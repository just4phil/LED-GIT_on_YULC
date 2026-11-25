// send_cc.rs - Testsender für den MIDI-Eingang des Simulators (Issue #25).
//
// Aufruf:   cargo run --release --example send_cc                       (listet die MIDI-Ausgänge)
//           cargo run --release --example send_cc -- <port> <cc> <wert>  (schickt einen Control Change auf Kanal 10)
// <port> = die Nummer aus der Liste oder ein Teil des Namens (Groß-/Kleinschreibung egal), z. B. "loopMIDI".
// Beispiel: ... -- loopMIDI 22 30   startet Song 30 (Maniac) wie die Gitarre mit dem WIDI.
// Optional als vierter Wert der Kanal (1-16, Standard 10) - praktisch, um zu prüfen, dass andere Kanäle ignoriert werden.

use midir::{MidiOutput, MidiOutputPort};

fn main() {
    let out = MidiOutput::new("showview-send_cc").expect("MIDI nicht verfügbar");
    let ports = out.ports();
    let args: Vec<String> = std::env::args().skip(1).collect();
    if args.len() < 3 {
        println!("MIDI-Ausgänge:");
        for (i, p) in ports.iter().enumerate() {
            println!("  {i}: {}", out.port_name(p).unwrap_or_default());
        }
        println!("\nAufruf: send_cc <port|nummer> <cc> <wert> [kanal 1-16, Standard 10]");
        return;
    }
    // Port suchen: erst als Nummer, sonst als Namensteil
    let port: &MidiOutputPort = match args[0].parse::<usize>().ok().and_then(|i| ports.get(i)) {
        Some(p) => p,
        None => ports
            .iter()
            .find(|p| out.port_name(p).map(|n| n.to_lowercase().contains(&args[0].to_lowercase())).unwrap_or(false))
            .expect("Port nicht gefunden (ohne Argumente gibt es die Liste)"),
    };
    let name = out.port_name(port).unwrap_or_default();
    let cc: u8 = args[1].parse().expect("cc = Zahl 0-127");
    let value: u8 = args[2].parse().expect("wert = Zahl 0-127");
    let channel: u8 = args.get(3).map(|c| c.parse().expect("kanal = 1-16")).unwrap_or(10);
    let mut conn = out.connect(port, "send_cc").expect("Port lässt sich nicht öffnen");
    // Statusbyte 0xB0 + (Kanal-1) = Control Change; danach CC-Nummer und Wert
    conn.send(&[0xB0 + (channel - 1), cc, value]).expect("Senden fehlgeschlagen");
    std::thread::sleep(std::time::Duration::from_millis(100)); // dem Treiber Zeit geben, bevor die Verbindung schließt
    println!("gesendet: Kanal {channel}, CC {cc} = {value} an {name}");
}
