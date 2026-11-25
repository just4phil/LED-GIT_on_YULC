// timeline.rs - zeichnet den Ablauf eines Songs als Zeitstrahl mit mehreren Spuren (Lanes), untereinander:
//
//   Zeit / Takt   Lineal: oben Minuten:Sekunden, darunter die Taktnummern der Tabelle (wie im DAW) und Beat-Striche
//   Parts         ein Block je Part (case), Farbe nach Szene/Effekt, unten ein Balken für die Energie 0-5 der
//                 Tabelle; ein weißer Keil am Anfang = Übergang (transition) mit seiner Länge
//   LAMPE1 ...    eine Spur je Bühnengerät (links nach rechts wie auf der Bühne): was das Gerät spielt. Eigene
//                 Angaben (devices: in show.yaml) sind hell umrandet. Warmweiße Hüllkurven darüber = Blinder
//                 (Einblenden, voll, Ausklingen; Höhe = Stärke). Auf der Matrix zusätzlich der Titel-Lauftext.
//   Overlay       die zweite Effekt-Ebene über dem Part-Effekt (overlay:, auch Text mit over: true), mit Zeitfenstern
//   Text          Wörter / Lauftext auf der Matrix (text:)
//   Ausgabe       Modifier der Ausgabestufe (fade_in/out als Rampen, gate, pulse, dim, tint, soft, smooth, fade ...)
//   Wünsche       Fähnchen: Änderungswunsch des Users (gelb) und die Antworten in cues: (grün) auf ihrem Takt
//
// Bedienung: Mausrad = Zoom um den Mauszeiger, Shift + Mausrad oder Ziehen = verschieben, Doppelklick = ganzer Song,
// Klick auf einen Part = Details unten (main.rs). Überfahren zeigt einen Hinweis-Text (Tooltip).
//
// Wie in egui gezeichnet wird: egui ist ein "immediate mode"-GUI - die Oberfläche wird in jedem Bild (Frame) komplett
// neu beschrieben, es gibt keine gespeicherten Widgets. Diese Datei holt sich mit allocate_exact_size() eine
// Fläche, liest die Maus darüber aus (Response) und malt mit dem Painter Rechtecke, Linien, Polygone und Text
// hinein. Gespeichert wird nur, was zwischen zwei Bildern bleiben muss: Zoom und Position (struct TimelineView).

use crate::model::{call_name, value_text, Part, ScrollPlan, SongData};
use eframe::egui::{
    self, Align2, Color32, FontId, Pos2, Rect, Sense, Shape, Stroke, StrokeKind, Ui, Vec2,
};
use serde_json::Value;

const LABEL_W: f32 = 96.0; // Breite der Spaltenbeschriftung links in Pixeln
const RULER_H: f32 = 34.0; // Höhe des Lineals
const PART_H: f32 = 40.0; // Höhe der Spur "Parts"
const DEV_H: f32 = 34.0; // Höhe einer Geräte-Spur
const LANE_H: f32 = 26.0; // Höhe der übrigen Spuren
const GAP: f32 = 3.0; // Abstand zwischen den Spuren

/// Was zwischen zwei Bildern erhalten bleibt: Zoom (Pixel pro Millisekunde) und die Zeit am linken Rand.
pub struct TimelineView {
    px_per_ms: f64,
    view_start: f64,
    fit_pending: bool, // true = beim nächsten Zeichnen den ganzen Song einpassen (nach dem Laden eines Songs)
}

/// Was der Benutzer in diesem Bild angeklickt hat.
pub struct Clicked {
    pub part: Option<usize>, // Index in SongData::parts
}

impl Default for TimelineView {
    fn default() -> Self {
        Self { px_per_ms: 0.05, view_start: 0.0, fit_pending: true }
    }
}

/// Eine Spur: Beschriftung, Höhe, und was in ihr gezeigt wird.
enum Lane {
    Parts,
    Device(String),
    Overlay,
    Text,
    Output,
    Wishes,
}

impl TimelineView {
    /// Nach dem Laden eines neuen Songs aufrufen: beim nächsten Zeichnen wird der ganze Song eingepasst.
    pub fn fit(&mut self) {
        self.fit_pending = true;
    }

    /// Den Zeitstrahl in die verfügbare Fläche zeichnen. selected = gerade ausgewählter Part (wird hervorgehoben).
    pub fn show(&mut self, ui: &mut Ui, data: &SongData, selected: Option<usize>) -> Clicked {
        let size = ui.available_size();
        // Sense::click_and_drag: diese Fläche will Klicks und Ziehen mitbekommen
        let (rect, resp) = ui.allocate_exact_size(size, Sense::click_and_drag());
        let painter = ui.painter_at(rect);
        painter.rect_filled(rect, 0.0, Color32::from_gray(18));

        let track = Rect::from_min_max(Pos2::new(rect.left() + LABEL_W, rect.top()), rect.max);
        let total = data.parts.last().map(|p| p.end()).unwrap_or(1000.0).max(1000.0);

        // ---- Zoom und Verschieben ----
        if self.fit_pending && track.width() > 10.0 {
            self.px_per_ms = (track.width() as f64 - 20.0) / total;
            self.view_start = 0.0;
            self.fit_pending = false;
        }
        if resp.double_clicked() {
            self.fit_pending = true;
        }
        if resp.hovered() {
            // ui.input(|i| ...) liest den Eingabezustand dieses Bildes (Mausrad, Tasten) - als Closure (|i| ...)
            let (scroll, zoom, shift) = ui.input(|i| (i.smooth_scroll_delta, i.zoom_delta(), i.modifiers.shift));
            if let Some(pos) = resp.hover_pos() {
                let at_ms = self.ms_at(pos.x, track);
                let mut factor = zoom as f64;
                if !shift {
                    factor *= (scroll.y as f64 * 0.003).exp(); // Mausrad hoch = hineinzoomen
                }
                if (factor - 1.0).abs() > 1e-6 {
                    let min = (track.width() as f64 - 20.0) / total * 0.5; // nicht weiter heraus als der halbe Song
                    self.px_per_ms = (self.px_per_ms * factor).clamp(min.min(0.5), 2.0);
                    // die Zeit unter dem Mauszeiger soll beim Zoomen an derselben Stelle bleiben
                    self.view_start = at_ms - (pos.x - track.left()) as f64 / self.px_per_ms;
                }
                let pan = if shift { scroll.y + scroll.x } else { scroll.x };
                self.view_start -= pan as f64 / self.px_per_ms;
            }
        }
        if resp.dragged() {
            self.view_start -= resp.drag_delta().x as f64 / self.px_per_ms;
        }
        let max_start = (total - (track.width() as f64 - 20.0) / self.px_per_ms).max(0.0);
        self.view_start = self.view_start.clamp(-2000.0, max_start + 2000.0);

        // ---- Spuren anlegen ----
        let mut lanes: Vec<(String, f32, Lane)> = vec![("Parts".into(), PART_H, Lane::Parts)];
        for d in &data.stage {
            lanes.push((short_device(d).into(), DEV_H, Lane::Device(d.clone())));
        }
        lanes.push(("Overlay".into(), LANE_H, Lane::Overlay));
        lanes.push(("Text".into(), LANE_H, Lane::Text));
        lanes.push(("Ausgabe".into(), LANE_H, Lane::Output));
        lanes.push(("Wünsche".into(), LANE_H, Lane::Wishes));

        // Spurhöhen an die Fensterhöhe anpassen: ist mehr Platz da als nötig, werden alle Spuren gleichmäßig höher
        // (höchstens 3,5-fach), damit der Zeitstrahl die Fläche füllt und Beschriftungen lesbar bleiben
        let need: f32 = RULER_H + lanes.iter().map(|(_, h, _)| *h + GAP).sum::<f32>() + 20.0;
        let k = ((rect.height() - RULER_H - 20.0) / (need - RULER_H - 20.0)).clamp(1.0, 3.5);
        for lane in lanes.iter_mut() {
            lane.1 *= k;
        }

        let hover = resp.hover_pos();
        let mut tip: Option<String> = None; // Hinweis-Text unter dem Mauszeiger (wird am Ende angezeigt)
        let mut hover_part: Option<usize> = None;

        self.draw_ruler(&painter, data, Rect::from_min_size(track.min, Vec2::new(track.width(), RULER_H)));
        label(&painter, Pos2::new(rect.left() + 6.0, rect.top() + RULER_H / 2.0), "Zeit / Takt");

        let mut y = rect.top() + RULER_H + GAP;
        for (name, h, lane) in &lanes {
            let band = Rect::from_min_max(Pos2::new(track.left(), y), Pos2::new(track.right(), y + *h));
            let lbl = Rect::from_min_max(Pos2::new(rect.left(), y), Pos2::new(track.left() - 4.0, y + *h));
            painter.rect_filled(lbl, 3.0, Color32::from_gray(30));
            label(&painter, Pos2::new(lbl.left() + 6.0, lbl.center().y), name);
            let clip = painter.with_clip_rect(band.intersect(track)); // nichts über den Spurrand hinaus malen
            for (i, part) in data.parts.iter().enumerate() {
                let x0 = self.x_of(part.start, track);
                let x1 = self.x_of(part.end(), track);
                if x1 < track.left() || x0 > track.right() {
                    continue; // Part liegt außerhalb des sichtbaren Bereichs
                }
                let prect = Rect::from_min_max(Pos2::new(x0, band.top()), Pos2::new(x1, band.bottom()));
                let t = match lane {
                    Lane::Parts => self.draw_part(&clip, part, prect, selected == Some(i)),
                    Lane::Device(d) => self.draw_device(&clip, data, part, d, prect, track),
                    Lane::Overlay => self.draw_overlay(&clip, part, prect, track),
                    Lane::Text => self.draw_text(&clip, part, prect, track),
                    Lane::Output => self.draw_output(&clip, part, prect, track),
                    Lane::Wishes => self.draw_wishes(&clip, part, prect, track, hover),
                };
                // Tooltip: nur für den Part, über dem die Maus gerade steht
                if let Some(p) = hover {
                    if band.contains(p) && p.x >= x0 && p.x < x1 {
                        hover_part = Some(i);
                        if let Some(t) = t {
                            tip = Some(t);
                        }
                    }
                }
            }
            if let Lane::Device(d) = lane {
                // Titel-Lauftext auf den Matrix-Geräten (am Songanfang und nach dem Einstieg aus einem Vorspann)
                let plans = [data.scroll.get(d), data.entry_scroll.as_ref().and_then(|m| m.get(d))];
                for plan in plans.into_iter().flatten().flatten() {
                    if let Some(t) = self.draw_scroll(&clip, plan, band, track, hover) {
                        tip = Some(t);
                    }
                }
            }
            y += *h + GAP;
        }

        // Linie und Zeitangabe am Mauszeiger
        if let Some(p) = hover {
            if track.contains(p) {
                let ms = self.ms_at(p.x, track);
                painter.line_segment([Pos2::new(p.x, track.top()), Pos2::new(p.x, y)], Stroke::new(1.0, Color32::from_white_alpha(70)));
                let bar = data.parts.iter().find(|q| ms >= q.start && ms < q.end()).and_then(|q| bar_at(q, ms));
                let txt = match bar {
                    Some(b) => format!("{}  Takt {:.2}", fmt_time(ms), b),
                    None => fmt_time(ms),
                };
                painter.text(Pos2::new(p.x + 4.0, y + 2.0), Align2::LEFT_TOP, txt, FontId::proportional(12.0), Color32::LIGHT_GRAY);
            }
        }

        let clicked = if resp.clicked() { hover_part } else { None };
        if let Some(t) = tip {
            resp.on_hover_text_at_pointer(t);
        }
        Clicked { part: clicked }
    }

    // ---- Umrechnung Zeit <-> Pixel ----
    fn x_of(&self, ms: f64, track: Rect) -> f32 {
        track.left() + ((ms - self.view_start) * self.px_per_ms) as f32
    }
    fn ms_at(&self, x: f32, track: Rect) -> f64 {
        self.view_start + (x - track.left()) as f64 / self.px_per_ms
    }

    /// Lineal: oben Sekunden, unten Taktnummern (aus Tempo und Taktnummer der Tabelle je Part) und Beat-Striche.
    fn draw_ruler(&self, painter: &egui::Painter, data: &SongData, r: Rect) {
        let clip = painter.with_clip_rect(r);
        clip.rect_filled(r, 0.0, Color32::from_gray(26));
        // Sekunden: Abstand so wählen, dass die Beschriftungen mindestens 70 Pixel auseinander liegen
        let step = [1.0, 2.0, 5.0, 10.0, 15.0, 30.0, 60.0, 120.0]
            .into_iter()
            .find(|s| s * 1000.0 * self.px_per_ms >= 70.0)
            .unwrap_or(300.0);
        let first = (self.view_start / 1000.0 / step).floor() as i64;
        let last = ((self.view_start + r.width() as f64 / self.px_per_ms) / 1000.0 / step).ceil() as i64;
        for k in first.max(0)..=last {
            let ms = k as f64 * step * 1000.0;
            let x = self.x_of(ms, r);
            clip.line_segment([Pos2::new(x, r.top()), Pos2::new(x, r.top() + 6.0)], Stroke::new(1.0, Color32::GRAY));
            clip.text(Pos2::new(x + 2.0, r.top()), Align2::LEFT_TOP, fmt_time(ms), FontId::proportional(11.0), Color32::GRAY);
        }
        // Takte je Part
        for part in &data.parts {
            let (Some(from), Some(to)) = (part.bar_from, part.bar_to) else { continue };
            let bar_px = part.beat_ms() * part.beats_per_bar.max(1.0) * self.px_per_ms;
            let every = [1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0].into_iter().find(|k| k * bar_px >= 34.0).unwrap_or(128.0);
            let beat_px = part.beat_ms() * self.px_per_ms;
            let mut n = from.ceil();
            while n < to {
                if let Some(t) = part.bar_time(n) {
                    let x = self.x_of(t, r);
                    clip.line_segment([Pos2::new(x, r.bottom() - 12.0), Pos2::new(x, r.bottom())], Stroke::new(1.0, Color32::from_gray(150)));
                    if ((n - 1.0) % every).abs() < 1e-6 {
                        clip.text(Pos2::new(x + 2.0, r.bottom() - 1.0), Align2::LEFT_BOTTOM, format!("{n}"), FontId::proportional(11.0), Color32::from_gray(200));
                    }
                    if beat_px >= 7.0 {
                        for b in 1..part.beats_per_bar.max(1.0) as i64 {
                            let xb = self.x_of(t + b as f64 * part.beat_ms(), r);
                            clip.line_segment([Pos2::new(xb, r.bottom() - 5.0), Pos2::new(xb, r.bottom())], Stroke::new(1.0, Color32::from_gray(90)));
                        }
                    }
                }
                n += 1.0;
            }
        }
    }

    /// Spur "Parts": Block mit Name, Energie-Balken und Übergangs-Keil.
    fn draw_part(&self, p: &egui::Painter, part: &Part, r: Rect, selected: bool) -> Option<String> {
        let effect = part.effect_short();
        let fill = effect_color(&effect);
        p.rect_filled(r.shrink2(Vec2::new(0.5, 0.0)), 3.0, fill);
        if part.parent.is_some() {
            // tail (Schluss-Stück eines Abschnitts): schräg schraffiert, damit man es vom Haupt-Part unterscheidet
            let mut x = r.left() - r.height();
            while x < r.right() {
                p.line_segment([Pos2::new(x, r.bottom()), Pos2::new(x + r.height(), r.top())], Stroke::new(1.0, Color32::from_black_alpha(60)));
                x += 8.0;
            }
        }
        // Energie 0-5 der Tabelle als Balken am unteren Rand (Breite des Parts, Höhe nach Energie)
        if let Some(e) = part.key("energy").and_then(|v| v.as_f64()) {
            let h = (e.clamp(0.0, 5.0) / 5.0) as f32 * 8.0;
            p.rect_filled(Rect::from_min_max(Pos2::new(r.left() + 1.0, r.bottom() - h), Pos2::new(r.right() - 1.0, r.bottom())), 0.0, Color32::from_white_alpha(90));
        }
        // Übergang: weißer Keil vom Part-Anfang über seine Länge (beats, Standard 1)
        if let Some(tr) = part.key("transition") {
            let (typ, beats) = match tr {
                Value::Object(m) => (m.get("type").map(value_text).unwrap_or_default(), m.get("beats").and_then(|b| b.as_f64()).unwrap_or(1.0)),
                other => (value_text(other), 1.0),
            };
            let w = ((beats * part.beat_ms()) * self.px_per_ms).max(4.0) as f32;
            p.add(Shape::convex_polygon(
                vec![Pos2::new(r.left(), r.top()), Pos2::new(r.left() + w, r.top()), Pos2::new(r.left(), r.bottom())],
                Color32::from_white_alpha(140),
                Stroke::NONE,
            ));
            p.text(Pos2::new(r.left() + 2.0, r.bottom() - 10.0), Align2::LEFT_CENTER, typ, FontId::proportional(10.0), Color32::BLACK);
        }
        let border = if selected { Stroke::new(2.0, Color32::WHITE) } else { Stroke::new(1.0, Color32::from_black_alpha(160)) };
        p.rect_stroke(r, 3.0, border, StrokeKind::Inside);
        if r.width() > 18.0 {
            p.with_clip_rect(r.shrink(2.0).intersect(p.clip_rect())).text(
                Pos2::new(r.left() + 4.0, r.top() + 3.0),
                Align2::LEFT_TOP,
                format!("{}\n{}", part.name, effect),
                FontId::proportional(11.0),
                text_on(fill),
            );
        }
        let mut tip = format!(
            "{}  (case {})\n{} - {}  ({:.0} ms, {:.0} BPM)",
            part.name, part.case, fmt_time(part.start), fmt_time(part.end()), part.dur, part.bpm
        );
        if let (Some(a), Some(b)) = (part.bar_from, part.bar_to) {
            tip += &format!("\nTakt {a} bis vor {b}");
        }
        for l in &part.lines {
            tip += &format!("\n{l}");
        }
        Some(tip)
    }

    /// Geräte-Spur: Effekt des Geräts, eigene Angaben umrandet, Blinder als Hüllkurven.
    fn draw_device(&self, p: &egui::Painter, data: &SongData, part: &Part, dev: &str, r: Rect, track: Rect) -> Option<String> {
        let call = part.devices.get(dev).cloned().unwrap_or_default();
        let name = call_name(&call.call);
        let fill = effect_color(&name);
        let inner = r.shrink2(Vec2::new(0.5, 1.0));
        p.rect_filled(inner, 2.0, fill.gamma_multiply(if call.own { 1.0 } else { 0.55 }));
        if call.own {
            p.rect_stroke(inner, 2.0, Stroke::new(1.5, Color32::from_rgb(255, 230, 120)), StrokeKind::Inside);
        }
        if r.width() > 30.0 {
            p.with_clip_rect(inner.shrink(2.0).intersect(p.clip_rect())).text(
                Pos2::new(inner.left() + 4.0, inner.top() + 2.0),
                Align2::LEFT_TOP,
                &name,
                FontId::proportional(10.5),
                text_on(fill),
            );
        }
        let mut tip = format!("{dev} in '{}':\n{}{}", part.name, call.call, if call.own { "\n(eigene Angabe unter devices:)" } else { "" });
        // Blinder dieses Geräts
        let beat = part.beat_ms();
        for b in &part.blinders {
            if !blinder_hits(&b.devices, dev) {
                continue;
            }
            let casc = b.cascade.as_ref().map(|c| c.steps.get(dev).copied().unwrap_or(0.0) * c.step_ms).unwrap_or(0.0);
            let mut full = part.start + b.at_ms;
            loop {
                let begin = full - b.attack_ms - casc;
                let end = full - b.attack_ms + b.len_ms;
                let top = r.bottom() - (b.amount / 255.0).clamp(0.05, 1.0) as f32 * (r.height() - 2.0);
                let pts = vec![
                    Pos2::new(self.x_of(begin, track), r.bottom()),
                    Pos2::new(self.x_of(full, track), top),
                    Pos2::new(self.x_of(full + b.hold_ms, track), top),
                    Pos2::new(self.x_of(end, track), r.bottom()),
                ];
                p.add(Shape::convex_polygon(pts, Color32::from_rgba_unmultiplied(255, 214, 150, 170), Stroke::new(1.0, Color32::from_rgb(255, 240, 200))));
                if b.every <= 0.0 {
                    break;
                }
                full += b.every * beat; // Blinder-Raster: alle `every` Beats bis zum Part-Ende
                if full >= part.end() {
                    break;
                }
            }
            tip += &format!(
                "\nBlinder bei {} (voll), {:.0} ms lang, Stärke {:.0} %{}{}{}",
                fmt_time(part.start + b.at_ms),
                b.len_ms,
                b.amount / 255.0 * 100.0,
                if b.every > 0.0 { format!(", alle {} Beats", b.every) } else { String::new() },
                if b.carry { ", klingt im nächsten Part aus" } else { "" },
                if b.color != "FX_BLINDER_WARM" { format!(", Farbe {}", b.color) } else { String::new() }
            );
        }
        let _ = data;
        Some(tip)
    }

    /// Titel-Lauftext eines Matrix-Geräts als Band über der Geräte-Spur.
    fn draw_scroll(&self, p: &egui::Painter, plan: &ScrollPlan, band: Rect, track: Rect, hover: Option<Pos2>) -> Option<String> {
        let a = plan.start + plan.wait;
        let b = a + plan.scroll;
        let r = Rect::from_min_max(Pos2::new(self.x_of(a, track), band.top() + 1.0), Pos2::new(self.x_of(b, track), band.top() + 13.0));
        p.rect_filled(r, 2.0, Color32::from_rgb(70, 140, 220));
        p.with_clip_rect(r.intersect(p.clip_rect())).text(
            Pos2::new(r.left() + 3.0, r.center().y),
            Align2::LEFT_CENTER,
            format!("Lauftext: {}", plan.text),
            FontId::proportional(10.0),
            Color32::WHITE,
        );
        if let (Some(fs), Some(fd)) = (plan.fill_start, plan.fill_dur) {
            let fr = Rect::from_min_max(Pos2::new(self.x_of(fs, track), r.top()), Pos2::new(self.x_of(fs + fd, track), r.bottom()));
            p.rect_stroke(fr, 2.0, Stroke::new(1.0, Color32::from_rgb(70, 140, 220)), StrokeKind::Inside);
        }
        match hover {
            Some(h) if r.contains(h) => Some(format!(
                "Titel-Lauftext \"{}\"\n{} - {}{}\ndanach wieder normaler Ablauf ab {}",
                plan.text,
                fmt_time(a),
                fmt_time(b),
                if plan.wait > 0.0 { format!(" (vorher {:.0} ms schwarz)", plan.wait) } else { String::new() },
                fmt_time(plan.join_start)
            )),
            _ => None,
        }
    }

    /// Spur "Overlay": die zweite Ebene des Parts mit ihren Zeitfenstern.
    fn draw_overlay(&self, p: &egui::Painter, part: &Part, r: Rect, track: Rect) -> Option<String> {
        let ov = part.key("overlay")?; // kein overlay -> nichts zeichnen, kein Tooltip
        let (name, mods) = match ov {
            Value::Object(m) => {
                let n = m.get("scene").or(m.get("fx")).map(|v| call_name(&value_text(v)));
                // ohne scene/fx läuft die Ebene nur auf den Geräten unter devices (z. B. Text über der Szene:
                // devices: {matrix: "progText(...)"}) -> "progText (matrix)"
                let n = n.or_else(|| match m.get("devices") {
                    Some(Value::Object(d)) => d.values().next().map(|v| {
                        let keys: Vec<&str> = d.keys().map(|k| k.as_str()).collect();
                        format!("{} ({})", call_name(&value_text(v)), keys.join(", "))
                    }),
                    _ => None,
                });
                (n.unwrap_or_else(|| "Ebene".into()), m.clone())
            }
            other => (call_name(&value_text(other)), serde_json::Map::new()),
        };
        let mode = mods.get("mode").map(value_text).unwrap_or_else(|| "add".into());
        let color = Color32::from_rgb(150, 90, 200);
        for (a, b) in self.windows(part, &mods) {
            let wr = Rect::from_min_max(Pos2::new(self.x_of(a, track), r.top() + 2.0), Pos2::new(self.x_of(b, track), r.bottom() - 2.0));
            p.rect_filled(wr, 3.0, color);
            ramps(p, self, part, &mods, wr, track);
            p.with_clip_rect(wr.intersect(p.clip_rect())).text(
                Pos2::new(wr.left() + 3.0, wr.center().y),
                Align2::LEFT_CENTER,
                format!("{name} ({mode})"),
                FontId::proportional(10.5),
                Color32::WHITE,
            );
        }
        Some(format!("Overlay in '{}': {}", part.name, value_text(ov)))
    }

    /// Spur "Text": Wörter oder Lauftext auf der Matrix.
    fn draw_text(&self, p: &egui::Painter, part: &Part, r: Rect, track: Rect) -> Option<String> {
        let text = part.key("text")?;
        let (words, mods) = match text {
            Value::Object(m) => (m.get("words").or(m.get("scroll")).map(value_text).unwrap_or_default(), m.clone()),
            other => (value_text(other), serde_json::Map::new()),
        };
        for (a, b) in self.windows(part, &mods) {
            let wr = Rect::from_min_max(Pos2::new(self.x_of(a, track), r.top() + 2.0), Pos2::new(self.x_of(b, track), r.bottom() - 2.0));
            p.rect_filled(wr, 3.0, Color32::from_rgb(40, 150, 140));
            p.with_clip_rect(wr.intersect(p.clip_rect())).text(
                Pos2::new(wr.left() + 3.0, wr.center().y),
                Align2::LEFT_CENTER,
                format!("\u{201E}{words}\u{201C}"),
                FontId::proportional(10.5),
                Color32::WHITE,
            );
        }
        Some(format!("Text in '{}':\n{}", part.name, part.text_info.clone().unwrap_or_else(|| value_text(text))))
    }

    /// Spur "Ausgabe": Modifier der Ausgabestufe als Band mit Beschriftung, fade_in / fade_out als Rampen.
    fn draw_output(&self, p: &egui::Painter, part: &Part, r: Rect, track: Rect) -> Option<String> {
        const KEYS: [&str; 12] = ["transition", "fade_in", "fade_out", "pulse", "gate", "dim", "tint", "only", "span", "soft", "smooth", "fade"];
        let Some(Value::Object(sec)) = Some(&part.sec) else { return None };
        let found: Vec<String> = KEYS.iter().filter_map(|k| sec.get(*k).map(|v| format!("{k} {}", value_text(v)))).collect();
        if found.is_empty() {
            return None;
        }
        let inner = r.shrink2(Vec2::new(0.5, 2.0));
        p.rect_filled(inner, 2.0, Color32::from_rgb(60, 70, 90));
        ramps(p, self, part, sec, inner, track);
        p.with_clip_rect(inner.intersect(p.clip_rect())).text(
            Pos2::new(inner.left() + 3.0, inner.center().y),
            Align2::LEFT_CENTER,
            found.join(" · "),
            FontId::proportional(10.5),
            Color32::from_gray(230),
        );
        Some(format!("Ausgabestufe in '{}':\n{}", part.name, found.join("\n")))
    }

    /// Spur "Wünsche": Änderungswunsch des Parts (am Part-Anfang), Wünsche der Zwischenzeilen und cues: auf ihrem Takt.
    fn draw_wishes(&self, p: &egui::Painter, part: &Part, r: Rect, track: Rect, hover: Option<Pos2>) -> Option<String> {
        let mut flags: Vec<(f64, Color32, String)> = Vec::new();
        if let Some(idea) = part.key("idea") {
            flags.push((part.start, Color32::from_rgb(240, 200, 60), format!("Änderungswunsch '{}': {}", part.name, value_text(idea))));
        }
        if let Some(Value::Array(ws)) = part.key("wishes") {
            for w in ws {
                let Some(von) = w.get("von").and_then(|v| v.as_f64()) else { continue };
                let Some(t) = part.bar_time(von) else { continue };
                let mut txt = format!("Takt {von}:");
                if let Some(i) = w.get("idea") {
                    txt += &format!(" Wunsch: {}", value_text(i));
                }
                if let Some(e) = w.get("energy") {
                    txt += &format!(" (Energie {})", value_text(e));
                }
                flags.push((t, Color32::from_rgb(240, 200, 60), txt));
            }
        }
        if let Some(Value::Object(cues)) = part.key("cues") {
            for (bar, txt) in cues {
                // die Taktnummer ist im JSON ein Schlüssel und damit Text ("5.25") - erst in eine Zahl umwandeln
                let Ok(b) = bar.parse::<f64>() else { continue };
                let Some(t) = part.bar_time(b) else { continue };
                flags.push((t, Color32::from_rgb(110, 210, 120), format!("Takt {bar}, umgesetzt: {}", value_text(txt))));
            }
        }
        let mut tip = None;
        for (t, col, txt) in flags {
            let x = self.x_of(t, track);
            p.line_segment([Pos2::new(x, r.top()), Pos2::new(x, r.bottom())], Stroke::new(1.5, col));
            p.add(Shape::convex_polygon(
                vec![Pos2::new(x, r.top()), Pos2::new(x + 8.0, r.top() + 4.0), Pos2::new(x, r.top() + 8.0)],
                col,
                Stroke::NONE,
            ));
            if let Some(h) = hover {
                if (h.x - x).abs() < 6.0 && h.y >= r.top() && h.y <= r.bottom() {
                    tip = Some(txt);
                }
            }
        }
        tip
    }

    /// Zeitfenster einer Ebene (overlay / text) in ms ab Song-Beginn: windows: [[von, bis], ...] oder from / to in
    /// Beats ab Part-Beginn; ohne Angabe der ganze Part.
    fn windows(&self, part: &Part, m: &serde_json::Map<String, Value>) -> Vec<(f64, f64)> {
        let beat = part.beat_ms();
        let at = |v: Option<&Value>, def: f64| v.and_then(|x| x.as_f64()).map(|b| part.start + b * beat).unwrap_or(def);
        if let Some(Value::Array(ws)) = m.get("windows") {
            let mut out = Vec::new();
            for w in ws {
                match w {
                    Value::Array(a) if a.len() >= 2 => out.push((at(a.first(), part.start), at(a.get(1), part.end()))),
                    Value::Object(o) => out.push((at(o.get("from"), part.start), at(o.get("to"), part.end()))),
                    _ => {}
                }
            }
            if !out.is_empty() {
                return out;
            }
        }
        vec![(at(m.get("from"), part.start), at(m.get("to"), part.end()))]
    }
}

/// fade_in / fade_out (in Beats) als dunkle Keile in ein Band malen: am Anfang wird es heller, am Ende dunkler.
fn ramps(p: &egui::Painter, v: &TimelineView, part: &Part, m: &serde_json::Map<String, Value>, r: Rect, track: Rect) {
    let beat = part.beat_ms();
    let shade = Color32::from_black_alpha(150);
    if let Some(b) = m.get("fade_in").and_then(|x| x.as_f64()) {
        let x1 = r.left().max(v.x_of(part.start, track)) + (b * beat * v.px_per_ms) as f32;
        p.add(Shape::convex_polygon(vec![r.left_top(), Pos2::new(x1.min(r.right()), r.top()), r.left_bottom()], shade, Stroke::NONE));
    }
    if let Some(b) = m.get("fade_out").and_then(|x| x.as_f64()) {
        let x0 = r.right() - (b * beat * v.px_per_ms) as f32;
        p.add(Shape::convex_polygon(vec![r.right_top(), r.right_bottom(), Pos2::new(x0.max(r.left()), r.bottom())], shade, Stroke::NONE));
    }
}

/// Trifft ein Blinder mit dieser Geräteliste das Gerät? Leer = alle; Klassen wie in DEVICE_MASKS von songgen.py.
fn blinder_hits(devices: &[String], dev: &str) -> bool {
    devices.is_empty()
        || devices.iter().any(|d| {
            d == dev
                || (d == "guitar" && (dev == "ANDRESGIT" || dev == "RINASBASS"))
                || (d == "lamp" && dev.starts_with("LAMPE"))
                || (d == "matrix" && (dev == "SCROLLMATRIX" || dev == "GITBOARD"))
        })
}

/// Taktnummer (Kommazahl) zu einer Zeit in einem Part - Umkehrung von Part::bar_time.
fn bar_at(part: &Part, ms: f64) -> Option<f64> {
    let from = part.bar_from?;
    let bpb = if part.beats_per_bar > 0.0 { part.beats_per_bar } else { 4.0 };
    Some(from + (ms - part.start + part.lead) / (bpb * part.beat_ms()))
}

/// Kurzer Gerätename für die Spaltenbeschriftung.
fn short_device(d: &str) -> &str {
    match d {
        "ANDRESGIT" => "Gitarre",
        "RINASBASS" => "Bass",
        "SCROLLMATRIX" => "Matrix",
        "LAMPE1" => "Lampe 1",
        "LAMPE2" => "Lampe 2",
        other => other,
    }
}

/// Beschriftung in der linken Spalte.
fn label(p: &egui::Painter, pos: Pos2, text: &str) {
    p.text(pos, Align2::LEFT_CENTER, text, FontId::proportional(12.0), Color32::from_gray(210));
}

/// Zeit in ms als "m:ss.s".
pub fn fmt_time(ms: f64) -> String {
    let s = ms.max(0.0) / 1000.0;
    format!("{}:{:04.1}", (s / 60.0).floor() as i64, s % 60.0)
}

/// Feste Farbe je Effektname: aus dem Namen wird ein Farbton berechnet (gleicher Name = gleiche Farbe, in jedem Song).
/// Schwarz-Effekte (progBlack) bleiben dunkelgrau, damit Pausen sofort auffallen.
fn effect_color(name: &str) -> Color32 {
    if name.is_empty() || name.contains("Black") || name == "SCENE_BLACK" {
        return Color32::from_gray(45);
    }
    // FNV-1a: ein einfacher Hash - macht aus dem Text eine Zahl, kleine Änderungen am Namen ergeben ganz andere Zahlen
    let mut h: u32 = 0x811c_9dc5;
    for b in name.bytes() {
        h = (h ^ b as u32).wrapping_mul(0x0100_0193);
    }
    hsv((h % 360) as f32, 0.55, 0.78)
}

/// Farbe aus Farbton (0-360°), Sättigung und Helligkeit (0-1) - die übliche HSV-Umrechnung.
fn hsv(h: f32, s: f32, v: f32) -> Color32 {
    let c = v * s;
    let x = c * (1.0 - ((h / 60.0) % 2.0 - 1.0).abs());
    let (r, g, b) = match (h / 60.0) as i32 {
        0 => (c, x, 0.0),
        1 => (x, c, 0.0),
        2 => (0.0, c, x),
        3 => (0.0, x, c),
        4 => (x, 0.0, c),
        _ => (c, 0.0, x),
    };
    let m = v - c;
    Color32::from_rgb(((r + m) * 255.0) as u8, ((g + m) * 255.0) as u8, ((b + m) * 255.0) as u8)
}

/// Schwarze oder weiße Schrift, je nachdem was auf der Hintergrundfarbe besser lesbar ist.
fn text_on(bg: Color32) -> Color32 {
    let l = 0.299 * bg.r() as f32 + 0.587 * bg.g() as f32 + 0.114 * bg.b() as f32;
    if l > 140.0 { Color32::BLACK } else { Color32::WHITE }
}
