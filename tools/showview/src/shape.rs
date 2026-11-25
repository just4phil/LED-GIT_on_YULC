// shape.rs - die Form von Gitarre und Bass für den Reiter "Bühne": SG-Kontur, Orte der LEDs, Gurt.
//
// Die LEDs von Gitarre und Bass sitzen auf EINEM Streifen, der einmal rund um die Kontur einer SG läuft (Ausgang 1
// der Firmware, `leds1`). Der Gurt ist ein zweiter, eigener Streifen mit 60 LEDs (Ausgang 2, `leds2`). Hier steht,
// WO jede LED in der Zeichnung liegt. Die Rechnung ist dieselbe wie in `initGuitarShape()` (src/guitarShapeFX.cpp)
// und in der JavaScript-Vorschau von docs/LED-Effekte-und-Szenen.html: die Eckpunkte der Kontur werden
// abgeschritten und alle (Gesamtlänge / LED-Zahl) eine LED gesetzt.
//
// Koordinaten: "Foto-Pixel" wie in der Firmware (x nach rechts, y nach unten). Die Zeichnung (stage.rs) rechnet sie
// erst beim Malen auf den freien Platz um - so skaliert alles mit dem Fenster, ohne zu verzerren.
//
// Ausrichtung (Entscheidung zu Issue #22): wie im Foto und in der Vorschau-Seite - Kopfplatte oben RECHTS, Korpus
// unten links, Hals schräg dazwischen (rund 36 Grad geneigt, so wird eine Gitarre getragen). Das entspricht einem
// Rechtshänder, den man von vorn (vom Publikum aus) sieht. Dieselbe Form dient für den Bass: gleiche SG-Kontur,
// nur mit seiner eigenen LED-Zahl (155) und den daraus umgerechneten Zonen.
//
// Rust-Idiome: `const X: [[f32; 2]; 31]` = unveränderliche Tabelle fester Länge aus 31 Zahlenpaaren;
// `.iter().map(..).collect()` baut aus einer Folge eine neue Liste; `f32` = Kommazahl mit 32 Bit.

/// Ein Punkt der Zeichnung (x, y) in Foto-Pixeln.
pub type Pt = [f32; 2];

/// Eckpunkte der SG-Kontur, Reihenfolge = Umlauf ab der Spitze der Kopfplatte (identisch zu `outlinePx` in
/// src/guitarShapeFX.cpp). Reihenfolge: Kopfplatte, Hals-Unterkante hinunter zum Korpus, um den Korpus herum
/// (unteres Horn, Rundung, oberes Horn) und die Hals-Oberkante wieder hinauf zur Kopfplatte.
const OUTLINE: [Pt; 31] = [
    [912.0, 648.0], [880.0, 720.0], [840.0, 745.0], [790.0, 760.0], [470.0, 990.0], [430.0, 1010.0], [400.0, 1045.0],
    [410.0, 1075.0], [455.0, 1080.0], [440.0, 1110.0], [400.0, 1120.0], [330.0, 1170.0], [300.0, 1220.0], [270.0, 1290.0],
    [200.0, 1330.0], [120.0, 1310.0], [60.0, 1250.0], [20.0, 1150.0], [10.0, 1070.0], [40.0, 1020.0], [120.0, 1000.0],
    [200.0, 960.0], [270.0, 900.0], [320.0, 890.0], [345.0, 905.0], [335.0, 945.0], [380.0, 960.0], [760.0, 720.0],
    [790.0, 680.0], [840.0, 640.0], [880.0, 625.0],
];

/// Der untere Gurtpin (am Korpusende, Mitte zwischen zwei Eckpunkten der Kontur: der Punkt zwischen
/// (200, 1330) und (120, 1310)).
const LOWER_STRAP_PIN: Pt = [160.0, 1320.0];

/// Anzahl der Teilstücke, in die die Gurtkurve zum gleichmäßigen Verteilen der LEDs zerlegt wird.
const STRAP_SAMPLES: usize = 400;

/// Alles, was zum Zeichnen eines Instruments (Gitarre oder Bass) gebraucht wird.
pub struct Shape {
    /// Eckpunkte der Kontur (zum Zeichnen des Umrisses).
    pub outline: Vec<Pt>,
    /// Ort jeder LED des Instruments, Index = LED-Nummer (`leds1[i]`).
    pub led_pos: Vec<Pt>,
    /// Ort jeder LED des Gurts, Index = LED-Nummer im Gurt (`leds2[j]`, j = 0 ist am oberen Gurtpin).
    pub strap_pos: Vec<Pt>,
    /// Die Gurtlinie als dichte Punktfolge (zum Zeichnen des Gurtbandes).
    pub strap_line: Vec<Pt>,
    /// Abstand zweier benachbarter LEDs auf der Kontur in Foto-Pixeln (bestimmt die Punktgröße).
    pub spacing: f32,
    /// Umschließendes Rechteck der ganzen Zeichnung (Kontur + Gurt): kleinste und größte Koordinate.
    pub min: Pt,
    pub max: Pt,
    /// Ort der Hals-Oberkante mit den Markern: Mitte und Richtung "nach außen" (Einheitsvektor), für die Beschriftung.
    pub marker_label_at: Pt,
    pub marker_label_dir: Pt,
    /// `true` für jede LED des Instruments, die zum dunklen Griffbrett gehört (Bund_min .. Bund_max - 1): dort leuchten
    /// nur die Marker. Die Anzeige hebt sie hervor, weil Marker mit ihrem Pegel von 7 von 255 am Bildschirm kaum zu sehen sind.
    pub fretboard: Vec<bool>,
}

/// Punkt auf der Kontur im Abstand `s` vom Start (Kopfspitze), gemessen entlang der Kontur.
/// `seg` = Länge jedes Teilstücks, damit nicht jedes Mal neu gerechnet werden muss.
fn point_at(s: f32, seg: &[f32]) -> Pt {
    let n = OUTLINE.len();
    let (mut p, mut start) = (0usize, 0.0f32);
    // Teilstück suchen, in dem der Weg s liegt (das letzte Teilstück schließt die Schleife zur Kopfspitze)
    while p < n - 1 && s > start + seg[p] {
        start += seg[p];
        p += 1;
    }
    let f = if seg[p] > 0.0 { (s - start) / seg[p] } else { 0.0 };
    let (a, b) = (OUTLINE[p], OUTLINE[(p + 1) % n]);
    [a[0] + (b[0] - a[0]) * f, a[1] + (b[1] - a[1]) * f]
}

/// Punkt auf einer Kurve dritten Grades (Bezier) für t = 0..1: p0 = Anfang, c1/c2 = Steuerpunkte (ziehen die Kurve
/// zu sich hin), p3 = Ende.
fn bezier(p0: Pt, c1: Pt, c2: Pt, p3: Pt, t: f32) -> Pt {
    let u = 1.0 - t;
    let (a, b, c, d) = (u * u * u, 3.0 * u * u * t, 3.0 * u * t * t, t * t * t);
    [a * p0[0] + b * c1[0] + c * c2[0] + d * p3[0], a * p0[1] + b * c1[1] + c * c2[1] + d * p3[1]]
}

impl Shape {
    /// Form für ein Instrument mit `n` LEDs berechnen.
    /// `head_tip` = LED-Nummer an der Kopfspitze (GUITAR_HEAD_TIP_IDX), `dir` = +1/-1 (GUITAR_LOOP_DIR),
    /// `strap_pin` = Konturposition des Gurtansatzes (GUITAR_STRAP_PIN_POS), `strap_n` = LEDs im Gurt,
    /// `neck_up` / `head_up` = Konturpositionen von Anfang und Ende der Hals-Oberkante (ZONE_NECK_UP_START / ZONE_HEAD_UP_START).
    /// `bund_min` / `bund_max` = Griffbrett-LEDs (Bund_min bis Bund_max - 1).
    /// Alles kommt aus der DLL (sim_info), die es wiederum aus definitions.h bzw. guitarShapeFX.h hat.
    pub fn new(n: usize, head_tip: i32, dir: i32, strap_pin: i32, strap_n: usize, neck_up: i32, head_up: i32, bund_min: i32, bund_max: i32) -> Shape {
        let count = OUTLINE.len();
        // Teilstück-Längen (Satz des Pythagoras) und Gesamtlänge der geschlossenen Kontur
        let seg: Vec<f32> = (0..count)
            .map(|p| {
                let (a, b) = (OUTLINE[p], OUTLINE[(p + 1) % count]);
                ((b[0] - a[0]).powi(2) + (b[1] - a[1]).powi(2)).sqrt()
            })
            .collect();
        let total: f32 = seg.iter().sum();
        let nn = n.max(1) as i32;

        // Ort der LED mit Konturposition k: gleichmäßig verteilt. Die LED-Nummer dazu ist wie loopToLed() in der Firmware
        // (Kopfspitze + Richtung * k, auf die Schleife gewickelt: rem_euclid ist ein Rest, der nie negativ wird).
        let mut led_pos = vec![[0.0f32; 2]; n];
        for k in 0..n as i32 {
            let i = (head_tip + dir * k).rem_euclid(nn) as usize;
            led_pos[i] = point_at(total * k as f32 / nn as f32, &seg);
        }
        let on_contour = |k: i32| point_at(total * k.rem_euclid(nn) as f32 / nn as f32, &seg);

        // Gurt: eine Kurve vom oberen Gurtpin (am oberen Horn) hinauf über die Schulter, links am Korpus vorbei zum
        // unteren Gurtpin am Korpusende. ANNAHME (es gibt keine Messung): die Nummern laufen vom oberen Pin zum
        // unteren, LED 0 sitzt am oberen Gurtpin. Der Gurt hinter dem Rücken ist nicht zu sehen; die Kurve ist
        // eine Andeutung, wie die LEDs ungefähr hängen.
        let top_pin = on_contour(strap_pin);
        let c1 = [top_pin[0] + 10.0, top_pin[1] - 450.0];
        let c2 = [-350.0, 950.0];
        let dense: Vec<Pt> = (0..=STRAP_SAMPLES).map(|i| bezier(top_pin, c1, c2, LOWER_STRAP_PIN, i as f32 / STRAP_SAMPLES as f32)).collect();
        // Weglänge entlang der Kurve, damit die LEDs gleiche Abstände bekommen (nicht gleiche Zeitschritte t)
        let mut cum = vec![0.0f32; dense.len()];
        for i in 1..dense.len() {
            cum[i] = cum[i - 1] + ((dense[i][0] - dense[i - 1][0]).powi(2) + (dense[i][1] - dense[i - 1][1]).powi(2)).sqrt();
        }
        let strap_len = *cum.last().unwrap_or(&0.0);
        let mut strap_pos = Vec::with_capacity(strap_n);
        let mut idx = 0usize;
        for j in 0..strap_n {
            // j = 0 an den Anfang, j = strap_n - 1 ans Ende der Kurve
            let target = if strap_n > 1 { strap_len * j as f32 / (strap_n - 1) as f32 } else { 0.0 };
            while idx + 1 < dense.len() - 1 && cum[idx + 1] < target {
                idx += 1;
            }
            let span = cum[idx + 1] - cum[idx];
            let f = if span > 0.0 { ((target - cum[idx]) / span).clamp(0.0, 1.0) } else { 0.0 };
            strap_pos.push([dense[idx][0] + (dense[idx + 1][0] - dense[idx][0]) * f, dense[idx][1] + (dense[idx + 1][1] - dense[idx][1]) * f]);
        }

        // Umschließendes Rechteck (Kontur + Gurt)
        let mut min = [f32::MAX, f32::MAX];
        let mut max = [f32::MIN, f32::MIN];
        for p in OUTLINE.iter().chain(dense.iter()) {
            min = [min[0].min(p[0]), min[1].min(p[1])];
            max = [max[0].max(p[0]), max[1].max(p[1])];
        }

        // Beschriftung "Bund-Marker": mitten auf der Hals-Oberkante, nach außen (von der Kontur weg, nach oben links)
        let mid = (neck_up + head_up) / 2;
        let marker_label_at = on_contour(mid);
        let (a, b) = (on_contour(mid - 3), on_contour(mid + 3)); // die Richtung des Halses an dieser Stelle
        let (dx, dy) = (b[0] - a[0], b[1] - a[1]);
        let len = (dx * dx + dy * dy).sqrt().max(1e-3);
        // Senkrechte zur Halsrichtung; wir wollen die, die nach oben (kleineres y) zeigt
        let mut nrm = [dy / len, -dx / len];
        if nrm[1] > 0.0 {
            nrm = [-nrm[0], -nrm[1]];
        }

        Shape {
            outline: OUTLINE.to_vec(),
            led_pos,
            strap_pos,
            strap_line: dense,
            spacing: total / nn as f32,
            min,
            max,
            marker_label_at,
            marker_label_dir: nrm,
            fretboard: (0..n as i32).map(|i| i >= bund_min && i < bund_max).collect(),
        }
    }
}
