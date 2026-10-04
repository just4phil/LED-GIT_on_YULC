#!/usr/bin/env python3
"""
songgen.py - erzeugt die Song-Funktion EINES Songs aus songs/<Song>/

    tools/.venv/Scripts/python tools/songgen.py                        # Songs und ihren Stand auflisten
    tools/.venv/Scripts/python tools/songgen.py <Song>                 # diesen Song generieren
    tools/.venv/Scripts/python tools/songgen.py <Song> --dry-run       # nur Timeline anzeigen, nichts schreiben
    tools/.venv/Scripts/python tools/songgen.py <Song> --versions      # gespeicherte Versionen
    tools/.venv/Scripts/python tools/songgen.py <Song> --restore <Version>
    tools/.venv/Scripts/python tools/songgen.py --assemble             # nur src/ aus den gespeicherten Songs neu bauen

<Song> = Ordnername unter songs/ (Anfang genügt, Groß/Klein egal). Pro Song-Ordner:
    song.yaml       gehört dem User: Tempo, Takte, midi_offset, Stimmungen, Effekt-Wünsche. WIRD NIE GESCHRIEBEN.
    show.yaml       technisch (von Claude abgeleitet): Szenen, Farbschemata, Overrides, Tails
    generated.cpp   erzeugter Code dieses Songs
    versionen/<Zeit>/   Kopie von song.yaml + show.yaml + generated.cpp bei jeder Generierung
Die Struktur steht nur in song.yaml, die Show ordnet per Abschnittsname zu. Gestaltung in song.yaml
(scene, fx, scheme, fade, devices, tail, text, dazu Übergang und Modifikatoren: transition, fade_in, fade_out,
pulse, gate, dim, tint, only, span) hat immer Vorrang vor show.yaml.

Schreibt:
    songs/<Song>/generated.cpp + versionen/   (nur für den angegebenen Song)
    src/songs_generated.cpp / .h              (zusammengesetzt aus den generated.cpp ALLER Songs, unverändert übernommen)
    src/main.cpp                              (nur zwischen den Markern "GENERATED SONGS")

Zeitbasis: 0 ms = Eintreffen des Start-MIDI-Signals. Das MIDI kommt midi_offset (Notenwert, z. B. 1/8)
NACH Takt 1 -> der erste Part wird um den Offset kürzer. Alle Grenzen werden aus absoluten Zeiten
gerundet, dadurch entsteht keine kumulative Rundungsdrift.
"""
import argparse
import datetime
import hashlib
import re
import shutil
import subprocess
import sys
from pathlib import Path

import yaml

import markers as mk

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
SONGS_DIR = ROOT / "songs"
OUT_CPP = SRC / "songs_generated.cpp"
OUT_H = SRC / "songs_generated.h"
MAIN_CPP = SRC / "main.cpp"

SONG_FILE = "song.yaml"			# gehört dem User, wird von keinem Werkzeug geschrieben
SHOW_FILE = "show.yaml"
GEN_FILE = "generated.cpp"
VERSIONS_DIR = "versionen"
VERSION_FILES = (SONG_FILE, SHOW_FILE, GEN_FILE)

MARK_BEGIN = "// >>> GENERATED SONGS (tools/songgen.py) >>>"	# dahinter kommen die cases neuer Song-IDs
TAG = "// <<< GENERATED SONGS <<<"							# hängt an jedem generierten Aufruf in main.cpp
SONGS_CPP = SRC / "songs.cpp"
CALL_RE = re.compile(r"^(\s*)(\w+)\(\);")
OLD_CALL_RE = re.compile(r"^\s*//\s*(\w+)\(\);")

SCROLL_DEVICES = ("SCROLLMATRIX", "GITBOARD")	# zeigen am Songanfang Titel + Interpret als Lauftext
SCROLL_MAX_WAIT_MS = 4000	# so lange darf die Matrix vor dem Lauftext schwarz bleiben, damit er genau an einer Grenze endet
BLACK = "progBlack(${dur}, ${next})"
MAX_PART_MS = 0xFFFFFFFF
ENERGY_SCENE = {1: "SCENE_CALM", 2: "SCENE_VERSE", 3: "SCENE_BUILDUP", 4: "SCENE_DROP", 5: "SCENE_DROP"}

# text: eines Parts - die Matrix-Geräte zeigen Text (progText/progTextScroll), alle anderen spielen ihre Szene weiter
MATRIX_KEYS = ("matrix", "SCROLLMATRIX", "GITBOARD")
TEXT_PER = {"beat": 1, "half": 2}	# Beats pro Wort; "bar" = ein Takt, eine Zahl = so viele Beats
TEXT_COLORS = {"weiss": "White", "weiß": "White", "white": "White", "rot": "Red", "red": "Red", "blau": "Blue", "blue": "Blue",
			   "gruen": "Green", "grün": "Green", "green": "Green", "gelb": "Yellow", "yellow": "Yellow", "orange": "Orange",
			   "pink": "DeepPink", "lila": "Purple", "purple": "Purple", "cyan": "Cyan"}

# fade: eines Parts - die Schemafarben wandern im Takt zu einem Ziel und zurück (setColorFade in colorSchemes.h)
FADE_TARGETS = {"complement": "FADE_COMPLEMENT", "komplement": "FADE_COMPLEMENT", "triad": "FADE_TRIAD",
				"analog": "FADE_ANALOG", "rainbow": "FADE_RAINBOW", "regenbogen": "FADE_RAINBOW"}

# Ausgabestufe (fxPipeline.h): Übergang in den Part und Modifikatoren auf das fertige Bild. Längen in Beats, Stärken in Prozent.
PIPELINE_KEYS = ("transition", "fade_in", "fade_out", "pulse", "gate", "dim", "tint", "only", "span")
TRANSITIONS = {"cut": None, "fade": "TRANS_FADE", "black": "TRANS_BLACK", "flash": "TRANS_FLASH", "wipe": "TRANS_WIPE",
			   "wipe_back": "TRANS_WIPE_BACK", "stage_lr": "TRANS_STAGE_LR", "stage_rl": "TRANS_STAGE_RL",
			   "stage_out": "TRANS_STAGE_OUT", "dissolve": "TRANS_DISSOLVE"}
DEVICE_MASKS = {	# only: Schlüssel wie bei devices -> Bühnen-Maske (DEV_... in definitions.h)
	"LAMPE1": "DEV_LAMPE1", "LAMPE2": "DEV_LAMPE2", "RINASBASS": "DEV_BASS", "ANDRESGIT": "DEV_GIT",
	"SCROLLMATRIX": "DEV_DRUMS", "GITBOARD": "DEV_GITBOARD",
	"guitar": "DEV_GIT | DEV_BASS", "lamp": "DEV_LAMPE1 | DEV_LAMPE2", "matrix": "DEV_DRUMS | DEV_GITBOARD",
}

# Geräte-Overrides: Schlüssel im YAML -> Präprozessor-Bedingung. Einzelgeräte vor Klassen.
DEVICE_KEYS = {
	"LAMPE1": "defined(LAMPE1)", "LAMPE2": "defined(LAMPE2)", "RINASBASS": "defined(RINASBASS)",
	"ANDRESGIT": "defined(ANDRESGIT)", "SCROLLMATRIX": "defined(SCROLLMATRIX)", "GITBOARD": "defined(GITBOARD)",
	"guitar": "DEVICE_CLASS == CLASS_GUITAR", "lamp": "DEVICE_CLASS == CLASS_LAMP", "matrix": "DEVICE_CLASS == CLASS_MATRIX",
}


class SongError(Exception):
	pass


#==================================================================
#=========== Timing (wird auch von songanalyze.py genutzt) =========
#==================================================================

def section_beats(sec, song):
	"""Anzahl Beats eines Abschnitts (bars oder beats, optional beats_per_bar-Override)."""
	bpb = sec.get("beats_per_bar", song.get("beats_per_bar", 4))
	if "bars" in sec and "beats" in sec:
		return sec["bars"] * bpb + sec["beats"]
	if "bars" in sec:
		return sec["bars"] * bpb
	if "beats" in sec:
		return sec["beats"]
	raise SongError(f"Abschnitt '{sec.get('name')}' braucht 'bars' oder 'beats'")


def section_bpm(sec, song):
	return float(sec.get("bpm", song["bpm"]))


def midi_offset_ms(song):
	"""midi_offset als Notenwert ("1/8", "3/16", "-1/16"; Viertel = 1 Beat), umgerechnet mit dem Tempo des
	ersten Abschnitts. midi_offset_ms überschreibt das, falls der Offset mal kein Notenwert ist."""
	if "midi_offset_ms" in song:
		return float(song["midi_offset_ms"])
	v = song.get("midi_offset", 0)
	if isinstance(v, str) and "/" in v:
		num, den = v.split("/")
		whole = float(num) / float(den)
	elif v == 0:
		whole = 0.0
	else:
		raise SongError(f"midi_offset '{v}' bitte als Notenwert angeben, z. B. 1/8")
	return whole * 4 * 60000.0 / section_bpm(song["sections"][0], song)


def offset_text(song):
	ms = midi_offset_ms(song)
	return f"{song['midi_offset']} = {ms:.0f} ms" if "midi_offset" in song and "midi_offset_ms" not in song else f"{ms:.0f} ms"


def section_times(song):
	"""Startzeiten ab Takt 1 in ms (float), plus Endzeit: Liste der Länge n+1."""
	t = 0.0
	times = [t]
	for sec in song["sections"]:
		t += section_beats(sec, song) * 60000.0 / section_bpm(sec, song)
		times.append(t)
	return times


#==================================================================
#=========== Parts bauen ==========================================
#==================================================================

def expand_parts(song):
	"""Abschnitte -> Parts (Abschnitte mit 'tail' werden in Haupt- und Schluss-Part geteilt)."""
	parts = []
	times = section_times(song)
	for i, sec in enumerate(song["sections"]):
		start, end = times[i], times[i + 1]
		bpm = section_bpm(sec, song)
		tail = sec.get("tail")
		if tail:
			tail_ms = tail["beats"] * 60000.0 / bpm
			if tail_ms >= end - start:
				raise SongError(f"tail von '{sec['name']}' ist länger als der Abschnitt")
			main = {k: v for k, v in sec.items() if k != "tail"}
			parts.append((start, main))
			tsec = dict(tail)
			tsec.setdefault("name", sec["name"] + " (tail)")
			tsec["_parent"] = sec["name"]
			tsec.setdefault("bpm", sec.get("bpm", song["bpm"]))
			tsec.setdefault("scheme", sec.get("scheme"))
			parts.append((end - tail_ms, tsec))
		else:
			parts.append((start, sec))
	return parts, times[-1]


def build_timeline(song):
	parts, song_end = expand_parts(song)
	offset = midi_offset_ms(song)

	# Zeitbasis MIDI: alle Zeiten um den Offset verschieben
	bounds = [p[0] - offset for p in parts] + [song_end - offset]
	secs = [p[1] for p in parts]

	if bounds[1] <= 0:
		raise SongError(f"midi_offset ({offset:.0f} ms) ist länger als der erste Part")
	if bounds[0] < 0:
		bounds[0] = 0.0	# erster Part beginnt mit dem MIDI-Signal, ist also um den Offset kürzer
	elif bounds[0] > 0:
		# MIDI kommt vor Takt 1 -> Vorlauf in Schwarz
		secs.insert(0, {"name": "vorlauf (MIDI vor Takt 1)", "fx": BLACK})
		bounds.insert(0, 0.0)

	end_black = int(song.get("end_black_ms", 10000))
	secs.append({"name": "BLACK (Ende)", "fx": BLACK, "why": "alle Geräte schwarz, dann Pausen-Loop"})
	bounds.append(bounds[-1] + end_black)

	ms = [round(b) for b in bounds]
	n = len(secs)
	step = 5
	while step > 3 and (n + 1) * step > 255:	# >= 3, damit case 1/2 für den Lauftext frei sind
		step -= 1
	if (n + 1) * step > 255:
		raise SongError(f"zu viele Parts ({n}) für prog (byte)")

	timeline = []
	for i, sec in enumerate(secs):
		dur = ms[i + 1] - ms[i]
		if dur <= 0:
			raise SongError(f"Part '{sec.get('name')}' hat Dauer {dur} ms")
		timeline.append({
			"case": i * step, "next": (i + 1) * step, "start": ms[i], "dur": dur, "sec": sec,
			"bpm": section_bpm(sec, song),
		})
	return timeline, n * step


#==================================================================
#=========== Code erzeugen ========================================
#==================================================================

def fill(expr, part, song):
	bpm = part["bpm"]
	beat = 60000.0 / bpm
	bpb = part["sec"].get("beats_per_bar", song.get("beats_per_bar", 4))
	vals = {
		"dur": part["dur"], "next": part["next"], "bpm": round(bpm),
		"beat": round(beat), "half": round(beat * 2), "bar": round(beat * bpb),
	}
	out = expr
	for k, v in vals.items():
		out = out.replace("${" + k + "}", str(v))
	if "${" in out:
		raise SongError(f"unbekannter Platzhalter in '{expr}'")
	return out.rstrip(";") + ";"


def default_call(part, song):
	sec = part["sec"]
	if "fx" in sec:
		return fill(sec["fx"], part, song)
	scene = sec.get("scene")
	if not scene:
		energy = sec.get("energy")
		if energy is None:
			raise SongError(f"Abschnitt '{sec.get('name')}' hat keine Gestaltung: scene/fx in {SHOW_FILE} oder energy setzen")
		if energy <= 0:
			return fill(BLACK, part, song)
		scene = ENERGY_SCENE[min(5, int(energy))]
	return f"scene({scene}, {part['dur']}, {part['next']}, {round(part['bpm'])});"


def per_beats(per, sec, song):
	"""per: beat | half | bar | <Zahl> -> Beats (None, wenn unbekannt)."""
	bpb = sec.get("beats_per_bar", song.get("beats_per_bar", 4))
	beats = bpb if per == "bar" else TEXT_PER.get(per, per)
	if isinstance(beats, bool) or not isinstance(beats, (int, float)) or beats <= 0:
		return None
	return beats


def fade_spec(sec):
	spec = sec.get("fade")
	return spec if isinstance(spec, dict) else {"to": spec}


def scheme_lines(part, song, offset=0):
	"""setColorScheme/setColorFade eines Parts (fade: complement | {to: ..., per: beat|half|bar|<Beats>, hard: true})."""
	sec = part["sec"]
	lines = []
	if sec.get("scheme"):
		lines.append(f"\t\tsetColorScheme({sec['scheme']});")
	if sec.get("fade"):
		spec = fade_spec(sec)
		to = str(spec.get("to"))
		ms = round((per_beats(spec.get("per", "bar"), sec, song) or 0) * 60000.0 / part["bpm"])	# ungültiges per meldet validate()
		lines.append(f"\t\tsetColorFade({FADE_TARGETS.get(to.lower(), to)}, {ms}" + (f", {'true' if spec.get('hard') else 'false'}, {offset}" if offset else ", true" if spec.get("hard") else "") + ");")
	return lines


def crgb(color):
	"""Farbname (rot, blau, ...) oder CRGB-Ausdruck -> C++-Ausdruck, None wenn unbekannt."""
	c = str(color).strip()
	if c.lower() in TEXT_COLORS:
		return "CRGB::" + TEXT_COLORS[c.lower()]
	return c if c.startswith("CRGB") else None


def percent(v):
	"""Prozent 0..100 -> 0..255, None wenn ungültig."""
	if isinstance(v, bool) or not isinstance(v, (int, float)) or not 0 <= v <= 100:
		return None
	return round(v * 255 / 100)


def pipeline_calls(part, song, offset=0):
	"""Übergang und Modifikatoren eines Parts (fxPipeline.h) -> (C++-Zeilen, Beschreibungen, Fehler).
	transition: fade | {type: wipe, beats: 2}     fade_in / fade_out: <Beats>     dim: <Prozent>
	pulse: <Prozent> | {depth: 50, per: beat|half|bar|<Beats>}     gate: <pro Beat> | {per_beat: 2, duty: 30}
	tint: rot | {color: rot, amount: 40}     only: [guitar, LAMPE1] | {devices: [...], others: 15}     span: [0, 50]
	offset > 0: Rest-Part der Matrix nach dem Lauftext - ohne Übergang, FadeIn/Pulse/Gate rechnen ab dem Part-Beginn."""
	sec = part["sec"]
	name = sec.get("name", "?")
	bpm = round(part["bpm"])
	calls, infos, errors = [], [], []

	def as_dict(key, short, allowed):
		spec = sec[key]
		spec = dict(spec) if isinstance(spec, dict) else {short: spec}
		unknown = [k for k in spec if k not in allowed]
		if unknown:
			errors.append(f"{name}: {key} kennt nur {', '.join(allowed)} - unbekannt: {', '.join(map(str, unknown))}")
		return spec

	def beats_ms(key, beats):
		if isinstance(beats, bool) or not isinstance(beats, (int, float)) or beats <= 0:
			errors.append(f"{name}: {key} ist eine Länge in Beats (Zahl > 0), nicht '{beats}'")
			return None
		ms = round(beats * 60000.0 / part["bpm"])
		if not offset and ms > part["dur"]:
			errors.append(f"{name}: {key} ({ms} ms) ist länger als der Part ({part['dur']} ms)")
			return None
		return ms

	def pct(key, v):
		p = percent(v)
		if p is None:
			errors.append(f"{name}: {key} ist eine Angabe in Prozent (0..100), nicht '{v}'")
		return p

	if "transition" in sec:
		spec = as_dict("transition", "type", ("type", "beats"))
		typ = str(spec.get("type", "")).lower()
		if typ not in TRANSITIONS:
			errors.append(f"{name}: transition '{spec.get('type')}' unbekannt - {', '.join(TRANSITIONS)}")
		elif TRANSITIONS[typ]:
			ms = beats_ms("transition beats", spec.get("beats", 1))
			if ms and not offset:
				calls.append(f"fxTransition({TRANSITIONS[typ]}, {ms});")
				infos.append(f"Übergang {typ} {ms} ms")

	timed = False	# FadeIn/Pulse/Gate hängen an der Zeit seit Part-Beginn
	if "fade_in" in sec:
		ms = beats_ms("fade_in", sec["fade_in"])
		if ms and offset < ms:
			calls.append(f"fxFadeIn({ms});")
			infos.append(f"blendet {ms} ms ein")
			timed = True
	if "fade_out" in sec:
		ms = beats_ms("fade_out", sec["fade_out"])
		if ms:
			calls.append(f"fxFadeOut({ms});")
			infos.append(f"blendet die letzten {ms} ms aus")

	if "pulse" in sec:
		spec = as_dict("pulse", "depth", ("depth", "per"))
		depth = pct("pulse depth", spec.get("depth", 50))
		beats = per_beats(spec.get("per", "beat"), sec, song)
		if beats is None or float(beats) != int(beats) or beats > 255:
			errors.append(f"{name}: pulse per '{spec.get('per')}' unbekannt - beat, half, bar oder eine ganze Zahl (Beats pro Puls)")
		elif depth:
			calls.append(f"fxPulse({bpm}, {depth}" + (f", {int(beats)}" if beats != 1 else "") + ");")
			infos.append(f"pulsiert alle {int(beats)} Beat(s), Tiefe {spec.get('depth', 50)} %")
			timed = True

	if "gate" in sec:
		spec = as_dict("gate", "per_beat", ("per_beat", "duty"))
		n, duty = spec.get("per_beat", 1), spec.get("duty", 50)
		if isinstance(n, bool) or not isinstance(n, int) or not 1 <= n <= 16:
			errors.append(f"{name}: gate per_beat ist eine ganze Zahl von 1 bis 16 (Blitze pro Beat), nicht '{n}'")
		elif isinstance(duty, bool) or not isinstance(duty, int) or not 1 <= duty <= 99:
			errors.append(f"{name}: gate duty ist der Hell-Anteil in Prozent (1..99), nicht '{duty}'")
		else:
			calls.append(f"fxGate({bpm}, {n}" + (f", {duty}" if duty != 50 else "") + ");")
			infos.append(f"Tor {n}x pro Beat, {duty} % hell")
			timed = True

	if "dim" in sec:
		v = pct("dim", sec["dim"])
		if v is not None:
			calls.append(f"fxDim({v});")
			infos.append(f"Helligkeit {sec['dim']} %")

	if "tint" in sec:
		spec = as_dict("tint", "color", ("color", "amount"))
		col = crgb(spec.get("color", ""))
		amount = pct("tint amount", spec.get("amount", 50))
		if col is None:
			errors.append(f"{name}: tint-Farbe '{spec.get('color')}' unbekannt - {', '.join(sorted(TEXT_COLORS))} oder ein CRGB-Ausdruck")
		elif amount:
			calls.append(f"fxTint({col}, {amount});")
			infos.append(f"Farbstich {spec.get('color')} {spec.get('amount', 50)} %")

	if "only" in sec:
		spec = sec["only"] if isinstance(sec["only"], dict) else {"devices": sec["only"]}
		unknown = [k for k in spec if k not in ("devices", "others")]
		devs = spec.get("devices")
		devs = [devs] if isinstance(devs, str) else list(devs or [])
		bad = [str(d) for d in devs if d not in DEVICE_MASKS]
		others = pct("only others", spec.get("others", 0))
		if unknown or bad or not devs:
			errors.append(f"{name}: only braucht Geräte aus {', '.join(DEVICE_MASKS)} (dazu others in Prozent)"
						  + (f" - unbekannt: {', '.join(map(str, unknown)) or ', '.join(bad)}" if unknown or bad else ""))
		elif others is not None:
			calls.append(f"fxMaskStage({' | '.join(DEVICE_MASKS[d] for d in devs)}" + (f", {others}" if others else "") + ");")
			infos.append(f"nur {', '.join(devs)}" + (f", Rest {spec['others']} %" if others else ", Rest aus"))

	if "span" in sec:
		span = sec["span"]
		ab = [percent(v) for v in span] if isinstance(span, (list, tuple)) and len(span) == 2 else [None]
		if None in ab or ab[0] >= ab[1]:
			errors.append(f"{name}: span ist [von, bis] in Prozent entlang des Geräts, z. B. [0, 50] - nicht '{span}'")
		else:
			calls.append(f"fxMaskSpan({ab[0]}, {ab[1]});")
			infos.append(f"nur Bereich {span[0]}-{span[1]} % des Geräts")

	if offset and timed:
		calls.append(f"fxTimeOffset({offset});")
	return ["\t\t" + c for c in calls], infos, errors


def fmt_time(ms):
	return f"{ms // 60000}:{(ms // 1000) % 60:02d}.{ms % 1000:03d}"


def section_length_text(sec, song):
	if "bars" in sec:
		return f"{sec['bars']} T" + (f"+{sec['beats']} B" if "beats" in sec else "")
	if "beats" in sec:
		return f"{sec['beats']} B"
	return ""


def matrix_widths():
	"""MATRIX_WIDTH je Scroll-Gerät aus definitions.h (#ifdef SCROLLMATRIX ... #else ...)."""
	text = (SRC / "definitions.h").read_text(encoding="utf-8", errors="ignore")
	m = re.search(r"#ifdef SCROLLMATRIX\s+#define MATRIX_WIDTH\s+(\d+).*?#else\s+#define MATRIX_WIDTH\s+(\d+)", text, re.S)
	if not m:
		raise SongError("MATRIX_WIDTH für SCROLLMATRIX/GITBOARD in definitions.h nicht gefunden")
	return {"SCROLLMATRIX": int(m.group(1)), "GITBOARD": int(m.group(2))}


def scroll_title(song):
	return song.get("scroll_title") or (song["name"] + (f" by {song['artist']}" if song.get("artist") else ""))


def plan_scroll(song, timeline, width):
	"""Lauftext am Songanfang für ein Scroll-Gerät planen (wie in den handgeschriebenen Songs):
	- wait: Matrix bleibt erst schwarz, damit der Text genau an einer Part-Grenze endet
	- fill: Text läuft sofort, danach der Rest des laufenden Parts verkürzt (ab einem Beat), dann Wiedereinstieg
	Dauer eines Durchlaufs wie in progScrollText(): (MATRIX_WIDTH - 2 + 6 * Zeichen) * delay."""
	text = scroll_title(song)
	delay = int(song.get("scroll_delay", 90))
	S = (width - 2 + 6 * len(text)) * delay
	inner = timeline[1:-1]	# Grenzen, an denen die Matrix wieder einsteigen kann (nicht das End-BLACK)
	if not inner:
		raise SongError("Song zu kurz für den Lauftext")
	base = {"text": text, "delay": delay, "natural": S}

	k = next((p for p in inner if p["start"] >= S), None)
	if k is not None and k["start"] - S <= SCROLL_MAX_WAIT_MS:
		wait = k["start"] - S
		if wait < 50:	# progBlack mit ~0 ms vermeiden -> Text minimal länger laufen lassen
			return dict(base, mode="wait", wait=0, scroll=k["start"], join=k)
		return dict(base, mode="wait", wait=wait, scroll=S, join=k)

	# Part, in dem der Text endet; Ende auf den nächsten Beat dieses Parts runden (Beat-Phase bleibt korrekt)
	idx = max(i for i, p in enumerate(timeline[:-1]) if p["start"] <= S)
	j = timeline[idx]
	beat = 60000.0 / j["bpm"]
	end_j = j["start"] + j["dur"]
	s2 = round(j["start"] + -(-(S - j["start"]) // beat) * beat)
	nxt = timeline[idx + 1]
	if end_j - s2 < beat:
		return dict(base, mode="wait", wait=0, scroll=end_j, join=nxt)
	return dict(base, mode="fill", scroll=s2, fill_part=j, fill_dur=end_j - s2, join=nxt)


def text_call(part, song, widths):
	"""text: eines Parts -> (Aufruf für die Matrix-Geräte mit ${dur}/${next}, Beschreibung für die Timeline).
	Formen: "FUN" | "THEY JUST WANNA" (ein Wort pro Beat) | {words: ..., per: beat|half|bar|<Beats>, color: ...}
	| {scroll: ..., color: ...} (Lauftext, endet genau am Part-Ende)."""
	sec = part["sec"]
	name = sec.get("name", "?")
	spec = sec["text"]
	if not isinstance(spec, dict):
		spec = {"words": spec}
	unknown = [k for k in spec if k not in ("words", "scroll", "per", "color")]
	if unknown or ("words" in spec) == ("scroll" in spec):
		raise SongError(f"{name}: text braucht genau eins von words/scroll (dazu per, color)"
						+ (f" - unbekannt: {', '.join(unknown)}" if unknown else ""))
	scroll = "scroll" in spec
	txt = " ".join(str(spec["scroll"] if scroll else spec["words"]).split())
	bad = sorted({c for c in txt if not 32 <= ord(c) < 127})
	if not txt or bad:
		raise SongError(f"{name}: text " + (f"enthält Zeichen, die die Matrix-Schrift nicht kann: {' '.join(bad)} "
						"(nur ASCII, also ae/oe/ue statt Umlaut)" if bad else "ist leer"))
	lit = '"' + txt.replace("\\", "\\\\").replace('"', '\\"') + '"'

	color = ""
	if spec.get("color"):
		c = crgb(spec["color"])
		if c is None:
			raise SongError(f"{name}: text-Farbe '{spec['color']}' unbekannt - {', '.join(sorted(TEXT_COLORS))} oder ein CRGB-Ausdruck")
		color = ", " + c

	if scroll:
		if "per" in spec:
			raise SongError(f"{name}: per gilt nur für words, nicht für scroll")
		return f"progTextScroll({lit}, ${{dur}}, ${{next}}{color})", f'Lauftext "{txt}", endet am Part-Ende'

	per = spec.get("per", "beat")
	beats = per_beats(per, sec, song)
	if beats is None:
		raise SongError(f"{name}: text per '{per}' unbekannt - beat, half, bar oder eine Zahl (Beats pro Wort)")
	ms = round(beats * 60000.0 / part["bpm"])
	words = txt.split()
	info = f'"{txt}": ' + ("pulsiert" if len(words) == 1 else f"{len(words)} Wörter, eins") + f" alle {ms} ms"
	longest = max(len(w) for w in words)
	wide = [d for d, w in widths.items() if longest * 6 - 1 > w]
	if wide:
		info += f" - ACHTUNG: '{max(words, key=len)}' passt nicht auf {', '.join(wide)}, läuft dort als Lauftext"
	return f"progText({lit}, ${{dur}}, ${{next}}, {ms}{color})", info


def apply_texts(song, timeline):
	"""text: der Parts als Override für die Matrix-Geräte eintragen (Tails haben ihr eigenes text: oder keins)."""
	widths = matrix_widths()
	for part in timeline:
		sec = part["sec"]
		if "text" not in sec:
			continue
		devices = dict(sec.get("devices") or {})
		clash = [k for k in MATRIX_KEYS if k in devices]
		if clash:
			raise SongError(f"{sec.get('name')}: text und devices.{clash[0]} zugleich - bitte nur eins für die Matrix")
		devices["matrix"], part["text_info"] = text_call(part, song, widths)
		part["sec"] = dict(sec, devices=devices)


def device_call(part, song, device):
	"""Aufruf für ein bestimmtes Gerät: Einzelgerät > Geräteklasse > Szene/fx."""
	devices = part["sec"].get("devices") or {}
	for key in (device, "matrix"):
		if key in devices:
			return fill(devices[key], part, song)
	return default_call(part, song)


def scroll_code(song, device, plan):
	"""(Zeilen für case 0 dieses Geräts, Zusatz-cases 1/2)"""
	title = plan["text"].replace('"', '\\"')

	def scroll(dur, nxt):
		return (f'\t\tprogScrollText("{title}", {dur}, {plan["delay"]}, getRandomColor(), {nxt});'
				f'\t// 1 Durchlauf = {plan["natural"]} ms')

	join = plan["join"]
	head, extra = [], []
	if plan["mode"] == "wait" and plan["wait"] == 0:
		head.append(scroll(plan["scroll"], join["case"]) + f", Einstieg case {join['case']}")
	elif plan["mode"] == "wait":
		head.append(f"\t\tprogBlack({plan['wait']}, 1);\t// Lauftext verzögern, damit er genau an case {join['case']} endet")
		extra += [f"\tcase 1:\t// Lauftext bis {fmt_time(join['start'])}, Einstieg case {join['case']}",
				  scroll(plan["scroll"], join["case"]), "\t\tbreak;"]
	else:
		fp = dict(plan["fill_part"], dur=plan["fill_dur"], next=join["case"])
		head.append(scroll(plan["scroll"], 2))
		extra.append(f"\tcase 2:\t// Rest von '{fp['sec'].get('name')}' ab {fmt_time(plan['scroll'])}, Einstieg case {join['case']}")
		extra += scheme_lines(fp, song, plan["scroll"] - plan["fill_part"]["start"])	# Farbwanderung synchron zu den anderen Geräten
		extra += pipeline_calls(fp, song, plan["scroll"] - plan["fill_part"]["start"])[0]
		extra +=["\t\t" + device_call(fp, song, device), "\t\tbreak;"]
	return head, extra


def gen_function(song, timeline, end_case):
	fn = song["function"]
	lines = []
	bpm = song["bpm"]
	lines.append(f"//#{song['id']} {song['name']}" + (f" - {song['artist']}" if song.get("artist") else "")
				 + f"  {bpm} BPM  midi_offset {offset_text(song)}  (generiert aus songs/{song['_dir']}: {SONG_FILE} + {song['_show'] or '-'})")
	lines.append(f"void {fn}() {{")
	lines.append("")
	if song.get("_marker_inline"):
		lines.append(f"\t// Marker einzelner Parts (markers.parts in {SONG_FILE}), läuft nach setMarkerLEDs()")
		lines += song["_marker_inline"] + [""]
	if song.get("scheme"):
		lines.append(f"\tsetColorScheme({song['scheme']});\t// Default für alle Parts")
		lines.append("")
	lines.append("\tswitch (prog) {")
	lines.append("")

	plans = song.get("_scroll_plans") or {}

	for i, part in enumerate(timeline):
		sec = part["sec"]
		comment = f"{sec.get('name', '')}  {section_length_text(sec, song)}  {part['dur']}ms  @{fmt_time(part['start'])}"
		if sec.get("why"):
			comment += f"  -- {sec['why']}"
		lines.append(f"\tcase {part['case']}:\t// {comment}")

		lines += scheme_lines(part, song)
		lines += pipeline_calls(part, song)[0]

		call = default_call(part, song)
		devices = sec.get("devices") or {}

		if i == 0 and plans:
			# Songanfang: Scroll-Geräte zeigen den Titel, alle anderen sind schwarz
			extras = []
			for j, (dev, plan) in enumerate(plans.items()):
				head, extra = scroll_code(song, dev, plan)
				lines.append(f"#{'if' if j == 0 else 'elif'} defined({dev})")
				lines += head
				extras.append((dev, extra))
			lines += ["#else", f"\t\t{call}", "#endif", "\t\tbreak;", ""]
			for dev, extra in extras:
				if extra:
					lines += [f"#if defined({dev})"] + extra + ["#endif", ""]
			continue

		if devices:
			ordered = sorted(devices.items(), key=lambda kv: list(DEVICE_KEYS).index(kv[0]))
			for j, (key, expr) in enumerate(ordered):
				lines.append(f"#{'if' if j == 0 else 'elif'} {DEVICE_KEYS[key]}")
				lines.append(f"\t\t{fill(expr, part, song)}")
			lines.append("#else")
			lines.append(f"\t\t{call}")
			lines.append("#endif")
		else:
			lines.append(f"\t\t{call}")
		lines.append("\t\tbreak;")
		lines.append("")

	lines.append(f"\tcase {end_case}:")
	lines.append("\t\tclearAll();")
	lines.append("\t\tswitchToSong(0);\t// SongID 0 == DEFAULT loop")
	lines.append("\t\tbreak;")
	lines.append("\t}")
	lines.append("}")
	return "\n".join(lines)


#==================================================================
#=========== Prüfen ===============================================
#==================================================================

def enum_names(path, prefix):
	return set(re.findall(r"\b(" + prefix + r"[A-Z0-9_]+)\b", path.read_text(encoding="utf-8", errors="ignore")))


def known_functions():
	names = set()
	for h in ["FXprograms.h", "guitarShapeFX.h", "scenes.h", "matrixFunctions.h", "functions.h", "colorSchemes.h"]:
		p = SRC / h
		if p.exists():
			names |= set(re.findall(r"^\s*[\w:<>]+\s*\*?\s+(\w+)\s*\(", p.read_text(encoding="utf-8", errors="ignore"), re.M))
	return names


def main_cases(lines):
	"""{Song-ID: (erste, letzte+1 Zeile)} der aktiven cases im switch (songID) von main.cpp."""
	s = next((i for i, l in enumerate(lines) if "switch (songID)" in l), None)
	if s is None:
		raise SongError("switch (songID) in main.cpp nicht gefunden")
	cases, cur = {}, None
	for i in range(s + 1, len(lines)):
		st = lines[i].strip()
		m = re.match(r"case (\d+):", st)
		if m or st.startswith("default:"):
			if cur is not None:
				cases[cur[0]] = (cur[1], i)
			if not m:
				break
			cur = (int(m.group(1)), i)
	return cases


def main_lines():
	return MAIN_CPP.read_text(encoding="utf-8").split("\n")


def handwritten_songs(lines=None):
	"""{Song-ID: Funktionsname} der handgeschriebenen Songs in main.cpp - egal ob noch aktiv oder schon durch
	einen generierten ersetzt (dann steht der alte Aufruf auskommentiert direkt über dem generierten)."""
	lines = lines or main_lines()
	out = {}
	for cid, (a, b) in main_cases(lines).items():
		for i in range(a + 1, b):
			m = CALL_RE.match(lines[i])
			if not m:
				continue
			if not m.group(2).startswith("gen_"):
				out[cid] = m.group(2)
			else:
				old = OLD_CALL_RE.match(lines[i - 1])
				if old:
					out[cid] = old.group(1)
			break
	return out


def bind_main(lines, frags):
	"""Generierte Songs in den switch (songID) einbinden (Zeilenliste von main.cpp, wird verändert):
	- gibt es den case schon (alter, handgeschriebener Song): alten Aufruf auskommentieren, generierten darunter
	- sonst: neuer case hinter dem Marker GENERATED SONGS
	- generierte Aufrufe ohne Song-Ordner wieder entfernen (alter Aufruf wird wieder aktiv)"""
	want = {f["id"]: f["function"] for f in frags}
	changed = True
	while changed:
		changed = False
		for cid, (a, b) in main_cases(lines).items():
			for i in range(a + 1, b):
				m = CALL_RE.match(lines[i])
				if not (m and m.group(2).startswith("gen_")) or want.get(cid) == m.group(2):
					continue
				if OLD_CALL_RE.match(lines[i - 1]):
					lines[i - 1] = re.sub(r"//\s*", "", lines[i - 1], count=1)
					del lines[i]
				else:
					end = next(k for k in range(i, b) if lines[k].strip().startswith("break;"))
					del lines[a:end + 1]
				changed = True
				break
			if changed:
				break
	for cid, fn in sorted(want.items(), reverse=True):
		cases = main_cases(lines)
		new = f"\t\t\t{fn}(); {TAG}"
		if cid in cases:
			a, b = cases[cid]
			calls = [i for i in range(a + 1, b) if CALL_RE.match(lines[i])]
			gen = [i for i in calls if CALL_RE.match(lines[i]).group(2).startswith("gen_")]
			if gen:
				lines[gen[0]] = new
			elif calls:
				for i in calls:
					ind = CALL_RE.match(lines[i]).group(1)
					lines[i] = ind + "//" + lines[i][len(ind):]
				lines.insert(calls[-1] + 1, new)
			else:
				raise SongError(f"main.cpp: case {cid} hat keinen Funktionsaufruf")
		else:
			k = next((i for i, l in enumerate(lines) if MARK_BEGIN in l), None)
			if k is None:
				raise SongError(f"Marker '{MARK_BEGIN}' fehlt in main.cpp")
			lines[k + 1:k + 1] = [f"\t\tcase {cid}:", new, "\t\t\tbreak;"]
	return lines


def old_function_body(name):
	"""[(Zeilennummer, Zeile)] der handgeschriebenen Song-Funktion in songs.cpp."""
	lines = SONGS_CPP.read_text(encoding="utf-8", errors="ignore").split("\n")
	s = next((i for i, l in enumerate(lines) if re.match(r"\s*void\s+" + re.escape(name) + r"\s*\(\s*\)", l)), None)
	if s is None:
		return []
	depth, out, started = 0, [], False
	for i in range(s, len(lines)):
		code = lines[i].split("//")[0]
		depth += code.count("{") - code.count("}")
		started = started or "{" in code
		out.append((i + 1, lines[i]))
		if started and depth <= 0:
			break
	return out


def part_constants(song, timeline):
	"""[(Konstante, case)]: Part-Nummern für handgeschriebenen Code, der in den Song springt (Trailer)."""
	base = "GEN_" + re.sub(r"\W+", "_", song["function"][4:] if song["function"].startswith("gen_") else song["function"]).upper()
	out, seen = [], set()
	for p in timeline[:-1]:
		c = base + "_" + re.sub(r"\W+", "_", str(p["sec"].get("name", ""))).strip("_").upper()
		if c not in seen:
			seen.add(c)
			out.append((c, p["case"]))
	return out


def trailer_jumps(song_id):
	"""[(Zeilennummer, Ziel)]: Stellen in songs.cpp, die mit 'songID = N; switchToPart(x);' in diesen Song springen."""
	lines = SONGS_CPP.read_text(encoding="utf-8", errors="ignore").split("\n")
	out = []
	for i, l in enumerate(lines[:-1]):
		if l.strip().startswith("//") or not re.search(r"\bsongID\s*=\s*" + str(song_id) + r"\s*;", l.split("//")[0]):
			continue
		for k in (i, i + 1, i + 2):
			m = re.search(r"switchToPart\(\s*(\w+)\s*\)", lines[k].split("//")[0]) if k < len(lines) else None
			if m:
				out.append((k + 1, m.group(1)))
				break
	return out


def validate(song, timeline):
	scenes = enum_names(SRC / "scenes.h", "SCENE_")
	schemes = enum_names(SRC / "colorSchemes.h", "SCHEME_")
	funcs = known_functions()
	errors = []

	if song.get("scheme") and song["scheme"] not in schemes:
		errors.append(f"unbekanntes Farbschema {song['scheme']}")
	for part in timeline:
		sec = part["sec"]
		name = sec.get("name", "?")
		if sec.get("scene") and sec["scene"] not in scenes:
			errors.append(f"{name}: unbekannte Szene {sec['scene']}")
		if sec.get("scheme") and sec["scheme"] not in schemes:
			errors.append(f"{name}: unbekanntes Farbschema {sec['scheme']}")
		if sec.get("fade"):
			spec = fade_spec(sec)
			to = str(spec.get("to"))
			unknown = [k for k in spec if k not in ("to", "per", "hard")]
			if unknown:
				errors.append(f"{name}: fade kennt nur to, per, hard - unbekannt: {', '.join(unknown)}")
			if to.lower() not in FADE_TARGETS and (to not in schemes or to in ("SCHEME_RANDOM", "SCHEME_COUNT")):
				errors.append(f"{name}: fade-Ziel '{to}' unbekannt - complement, triad, analog, rainbow oder ein SCHEME_...")
			if per_beats(spec.get("per", "bar"), sec, song) is None:
				errors.append(f"{name}: fade per '{spec.get('per')}' unbekannt - beat, half, bar oder eine Zahl (Beats pro Weg)")
			if not isinstance(spec.get("hard", False), bool):
				errors.append(f"{name}: fade hard ist true oder false")
			if (sec.get("scheme") or song.get("scheme") or "SCHEME_RANDOM") == "SCHEME_RANDOM":
				errors.append(f"{name}: fade braucht ein Farbschema (scheme), mit SCHEME_RANDOM gibt es nichts zu überblenden")
		errors += pipeline_calls(part, song)[2]
		for key in (sec.get("devices") or {}):
			if key not in DEVICE_KEYS:
				errors.append(f"{name}: unbekannter Geräteschlüssel '{key}' (erlaubt: {', '.join(DEVICE_KEYS)})")
		exprs = [sec.get("fx")] + list((sec.get("devices") or {}).values())
		for e in filter(None, exprs):
			fn = re.match(r"\s*(\w+)\s*\(", e)
			if not fn:
				errors.append(f"{name}: fx '{e}' ist kein Funktionsaufruf")
			elif fn.group(1) not in funcs:
				errors.append(f"{name}: Funktion {fn.group(1)} nicht in den Headern gefunden")
		if round(part["bpm"]) > 255:
			errors.append(f"{name}: bpm > 255 passt nicht in scene()")
	return errors


#==================================================================
#=========== Dateien schreiben ====================================
#==================================================================

def pascal(name):
	return "".join(w[:1].upper() + w[1:] for w in re.split(r"[^A-Za-z0-9]+", name) if w)


SONG_DESIGN_KEYS = ("function", "scheme", "scroll_text", "scroll_title", "scroll_delay", "end_black_ms")
SECTION_DESIGN_KEYS = ("scene", "fx", "devices", "tail", "scheme", "fade", "text") + PIPELINE_KEYS
STRUCTURE_KEYS = ("name", "bars", "beats", "bpm", "beats_per_bar")


def merge_show(song, show):
	"""Technische Gestaltung (show.yaml) per Abschnittsname in die Struktur einsetzen.
	Gestaltung, die der User in song.yaml gesetzt hat, hat immer Vorrang vor der Show."""
	if "markers" in show:
		raise SongError(f"{SHOW_FILE}: markers gehören in {SONG_FILE}, nicht in die Show")
	notes = song.setdefault("_notes", [])
	for k in SONG_DESIGN_KEYS:
		if k not in show:
			continue
		if k not in song:
			song[k] = show[k]
		elif song[k] != show[k]:
			notes.append(f"{k}: {song[k]} aus {SONG_FILE} hat Vorrang vor {SHOW_FILE}")
	design = show.get("sections") or {}
	names = [s["name"] for s in song["sections"]]
	for name in design:
		if name not in names:
			raise SongError(f"{SHOW_FILE}: Abschnitt '{name}' gibt es in {SONG_FILE} nicht")
	for sec in song["sections"]:
		d = dict(design.get(sec["name"]) or {})
		clash = [k for k in d if k in STRUCTURE_KEYS]
		if clash:
			raise SongError(f"{SHOW_FILE}: '{sec['name']}' darf {', '.join(clash)} nicht setzen - Struktur gehört in {SONG_FILE}")
		user = {k: sec[k] for k in SECTION_DESIGN_KEYS if k in sec}
		if "scene" in user and "fx" in user:
			raise SongError(f"{SONG_FILE}: '{sec['name']}' hat scene UND fx - bitte nur eins")
		if "scene" in user or "fx" in user:
			# der User legt den Effekt fest -> Effekt und Geräte-Overrides der Show gelten für diesen Part nicht
			for k in ("scene", "fx", "devices", "text", "why"):
				d.pop(k, None)
			d["why"] = f"Vorgabe aus {SONG_FILE}"
		elif "devices" in user:
			user["devices"] = {**(d.get("devices") or {}), **user["devices"]}
		if "why" in sec:
			d.pop("why", None)
		sec.update(d)
		sec.update(user)
		if user:
			notes.append(f"'{sec['name']}': {', '.join(user)} aus {SONG_FILE} (Vorrang vor {SHOW_FILE})")


def force_black_start(song):
	"""Mit dem Start-MIDI sind immer erst alle Geräte schwarz: der erste Abschnitt ist immer progBlack."""
	first = song["sections"][0]
	design = [k for k in SECTION_DESIGN_KEYS if k in first and first[k] != BLACK]
	if design:
		song.setdefault("_notes", []).append(
			f"erster Abschnitt '{first['name']}' ist immer BLACK - ignoriert: {', '.join(design)}")
	for k in SECTION_DESIGN_KEYS:
		if k != "fx":
			first.pop(k, None)
	first["fx"] = BLACK
	first.setdefault("why", "Start-MIDI: alle Geräte schwarz")


#==================================================================
#=========== Song-Ordner ==========================================
#==================================================================

def song_dirs():
	return sorted(d for d in SONGS_DIR.iterdir() if d.is_dir())


def find_song_dir(name):
	"""Song-Ordner unter songs/: exakter Name oder eindeutiger Anfang (Groß/Klein egal)."""
	key = Path(name).name.lower()
	dirs = song_dirs()
	hit = [d for d in dirs if d.name.lower() == key] or [d for d in dirs if d.name.lower().startswith(key)]
	if len(hit) != 1:
		raise SongError(f"Song '{name}' {'nicht gefunden' if not hit else 'ist nicht eindeutig'} - Ordner unter songs/: "
						+ ", ".join(d.name for d in dirs))
	return hit[0]


def find_audio(song, song_dir):
	"""Audiodatei eines Songs: 'audio:' relativ zum Song-Ordner (auch nur der Dateiname), sonst die einzige MP3 im Ordner."""
	if song.get("audio"):
		a = Path(song["audio"])
		for c in (song_dir / a, song_dir / "quelle" / a.name, song_dir / a.name, ROOT / a):
			if c.is_file():
				return c.resolve()
	found = [p for p in song_dir.rglob("*.mp3") if VERSIONS_DIR not in p.parts]
	return found[0].resolve() if len(found) == 1 else None


NOTE_AFTER_QUOTE = re.compile(r'^(\s*(?:-\s+)?[\w ]+:\s*)"([^"]*)"[ \t]*([^\s#].*?)\s*$')


def read_song_yaml(path):
	"""song.yaml lesen. Der User schreibt Anmerkungen gern hinter den Wert (idea: "ruhig" -> mehr Bewegung);
	das ist kein gültiges YAML, deshalb wird der Rest der Zeile vorher in den Text hineingezogen."""
	lines = []
	for line in path.read_text(encoding="utf-8").splitlines():
		m = NOTE_AFTER_QUOTE.match(line)
		if m:
			rest = m.group(3).replace("\\", "/").replace('"', "'")
			line = f'{m.group(1)}"{m.group(2)} {rest}"'
		lines.append(line)
	try:
		return yaml.safe_load("\n".join(lines))
	except yaml.YAMLError as e:
		mark = getattr(e, "problem_mark", None)
		where = f" Zeile {mark.line + 1}" if mark else ""
		raise SongError(f"{path.parent.name}/{path.name}{where}: kein gültiges YAML ({getattr(e, 'problem', e)})")


def load_song(song_dir, song_path=None, show_path=None):
	"""song.yaml + show.yaml eines Ordners laden und zusammenführen (beide Dateien werden nur gelesen)."""
	song_path = song_path or song_dir / SONG_FILE
	show_path = show_path or song_dir / SHOW_FILE
	label = f"{song_dir.name}/{SONG_FILE}"
	if not song_path.exists():
		raise SongError(f"{label} fehlt")
	song = read_song_yaml(song_path)
	song["_dir"] = song_dir.name
	for req in ("id", "name", "bpm", "sections"):
		if req not in song:
			raise SongError(f"{label}: Feld '{req}' fehlt")
	names = [s.get("name") for s in song["sections"]]
	dup = sorted({n for n in names if names.count(n) > 1})
	if None in names or dup:
		raise SongError(f"{label}: jeder Abschnitt braucht einen eindeutigen Namen (doppelt: {', '.join(map(str, dup))})")

	song["_show"] = SHOW_FILE if show_path.exists() else None
	show = (yaml.safe_load(show_path.read_text(encoding="utf-8")) or {}) if show_path.exists() else {}
	merge_show(song, show)
	force_black_start(song)
	song.setdefault("function", "gen_" + pascal(song["name"]))
	return song


def generate(song, others):
	"""Einen geladenen Song prüfen und Code erzeugen. others: Fragmente der anderen Songs (ID-/Namenskollisionen).
	Liefert (timeline, Funktions-Code, Marker-case-Zeilen, Fehler)."""
	errors = []
	notes = song.setdefault("_notes", [])
	if not 1 <= song["id"] <= 127:
		errors.append("Song-ID muss 1..127 sein (MIDI; 0 ist der Pausen-Loop)")
	old = handwritten_songs().get(song["id"])
	if old:
		notes.append(f"ersetzt den handgeschriebenen Song {old}() in main.cpp case {song['id']}: der alte Aufruf wird "
					 f"auskommentiert, der alte Code bleibt in songs.cpp")
	for o in others:
		if o["id"] == song["id"]:
			errors.append(f"Song-ID {song['id']} hat auch songs/{o['dir']}")
		if o["function"] == song["function"]:
			errors.append(f"Funktionsname {song['function']} hat auch songs/{o['dir']}")

	timeline, end_case = build_timeline(song)
	apply_texts(song, timeline)
	if song.get("scroll_text", True):
		widths = matrix_widths()
		song["_scroll_plans"] = {d: plan_scroll(song, timeline, widths[d]) for d in SCROLL_DEVICES}
	errors += validate(song, timeline)

	# Bund-Marker: Handarbeit in markerLEDs.cpp hat immer Vorrang. Slot-Angaben einzelner Parts
	# (markers.parts.<abschnitt>.<all|guitar|bass>: {slot: Name}) laufen zusätzlich inline in der Song-Funktion.
	marker_lines = []
	markers = song.get("markers") or {}
	hand = song["id"] in mk.handwritten_ids()
	slots = mk.slot_parts(markers)
	part_cases = {}
	for p in timeline:
		part_cases.setdefault(p["sec"].get("_parent") or p["sec"].get("name"), []).append(p["case"])
	errs = mk.validate(markers, [x["name"] for x in song["sections"]]) if markers else []
	errors += errs
	if hand:
		lists = any(k != "parts" for k in markers) or len(slots) < sum(len(s or {}) for s in (markers.get("parts") or {}).values())
		song["_marker_note"] = "Grund-Marker von Hand in markerLEDs.cpp" + (
			f" - Marker-Listen in {SONG_FILE} werden ignoriert" if lists else "")
	elif markers:
		if not errs:
			marker_lines = mk.gen_case(song["id"], markers, part_cases)
		song["_marker_note"] = f"generiert aus markers: in {SONG_FILE}"
	else:
		song["_marker_note"] = "keine"
	if slots and not errs:
		song["_marker_inline"] = mk.inline_code(markers, part_cases, mk.handwritten_slots(song["id"]) if hand else set(range(1, 8)))
		song["_marker_note"] += f" + {len(slots)} Part-Marker inline"
	# Der alte Code setzt Marker inline -> sie MÜSSEN in den generierten Code übernommen werden
	inline_old = [(n, l.strip()) for n, l in (old_function_body(old) if old else [])
				  if re.search(r"markerLED\d\s*=", l.split("//")[0])]
	if inline_old and not slots:
		errors.append(f"der alte Code {old}() setzt Marker-LEDs in einzelnen Parts, sie müssen übernommen werden: in {SONG_FILE} "
					  f"unter markers.parts eintragen, z. B. 'bridge 1: {{bass: {{5: ASaite_E}}}}'. Stellen in songs.cpp:\n"
					  + "\n".join(f"        Zeile {n}: {l}" for n, l in inline_old))
	elif inline_old:
		notes.append(f"{old}() setzt Marker inline (songs.cpp Zeilen {', '.join(str(n) for n, _l in inline_old)}) - "
					 f"bitte mit markers.parts in {SONG_FILE} vergleichen")

	# Trailer: handgeschriebener Code, der in diesen Song springt, braucht die neuen Part-Nummern
	song["_parts"] = part_constants(song, timeline)
	consts = dict(song["_parts"])
	for n, target in trailer_jumps(song["id"]):
		if target.isdigit():
			errors.append(f"songs.cpp Zeile {n} springt mit fester Part-Nummer switchToPart({target}) in diesen Song. Die "
						  f"Nummern des generierten Codes sind andere: dort eine Konstante aus der Liste 'Part-Konstanten' einsetzen")
		elif target not in consts:
			errors.append(f"songs.cpp Zeile {n}: switchToPart({target}) - diese Konstante gibt es für den Song nicht")
		else:
			notes.append(f"Trailer-Einsprung songs.cpp Zeile {n}: {target} = case {consts[target]}")
	return timeline, gen_function(song, timeline, end_case), marker_lines, errors


#==================================================================
#=========== Fragment (generated.cpp je Song) =====================
#==================================================================

FRAG_HEAD = "// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern (Quelle: song.yaml + show.yaml)"


def norm_text(path):
	"""Dateiinhalt mit einheitlichen Zeilenenden (None, wenn die Datei fehlt)."""
	return path.read_bytes().replace(b"\r\n", b"\n") if path.exists() else None


def sha(path):
	data = norm_text(path)
	return hashlib.sha256(data).hexdigest() if data is not None else "-"


def write_lf(path, text):
	with open(path, "w", encoding="utf-8", newline="\n") as f:
		f.write(text)


def fragment_text(song, code, marker_lines, song_dir):
	head = [FRAG_HEAD, f"//@id {song['id']}", f"//@function {song['function']}", f"//@name {song['name']}",
			f"//@song_sha {sha(song_dir / SONG_FILE)}", f"//@show_sha {sha(song_dir / SHOW_FILE)}"]
	head += [f"//@part {c} {n}" for c, n in song.get("_parts", [])] + ["//@code"]
	return "\n".join(head) + "\n" + code + "\n//@markers\n" + "".join(l + "\n" for l in marker_lines)


def read_fragment(path):
	text = path.read_text(encoding="utf-8")
	m = re.match(r"(.*?)\n//@code\n(.*)\n//@markers\n(.*)\Z", text, re.S)
	if not m:
		raise SongError(f"{path.relative_to(ROOT)} ist kein gültiges Fragment - Song neu generieren")
	meta = dict(re.findall(r"^//@(\w+) (.*)$", m.group(1), re.M))
	return {"id": int(meta["id"]), "function": meta["function"], "name": meta["name"],
			"song_sha": meta.get("song_sha"), "show_sha": meta.get("show_sha"),
			"parts": re.findall(r"^//@part (\w+) (\d+)$", m.group(1), re.M),
			"code": m.group(2), "markers": [l for l in m.group(3).splitlines() if l.strip()],
			"dir": path.parent.name}


def fragments(skip=None):
	return [read_fragment(d / GEN_FILE) for d in song_dirs() if d != skip and (d / GEN_FILE).exists()]


def is_current(song_dir, frag):
	"""Passt generated.cpp noch zu song.yaml + show.yaml?"""
	return frag["song_sha"] == sha(song_dir / SONG_FILE) and frag["show_sha"] == sha(song_dir / SHOW_FILE)


#==================================================================
#=========== Versionen ============================================
#==================================================================

def versions(song_dir):
	v = song_dir / VERSIONS_DIR
	return sorted(d for d in v.iterdir() if d.is_dir()) if v.exists() else []


def git_state():
	try:
		run = lambda *a: subprocess.run(["git", *a], cwd=ROOT, capture_output=True, text=True, timeout=10).stdout.strip()
		head = run("rev-parse", "--short", "HEAD")
		return head + (" + lokale Änderungen in tools/ oder src/" if run("status", "--porcelain", "--", "tools", "src") else "")
	except Exception:
		return "unbekannt"


def save_version(song_dir, note=""):
	"""Aktuellen Stand (song.yaml, show.yaml, generated.cpp) nach versionen/<Zeit>/ kopieren.
	Nichts zu tun, wenn es genau diesen Stand schon als Version gibt. Liefert den Ordner oder None."""
	if not (song_dir / GEN_FILE).exists():
		return None
	for v in versions(song_dir):
		if all(norm_text(song_dir / f) == norm_text(v / f) for f in VERSION_FILES):
			return None
	now = datetime.datetime.now()
	stamp = now.strftime("%Y-%m-%d_%H%M")
	dst, n = song_dir / VERSIONS_DIR / stamp, 1
	while dst.exists():
		n += 1
		dst = song_dir / VERSIONS_DIR / f"{stamp}_{n}"
	dst.mkdir(parents=True)
	for f in VERSION_FILES:
		if (song_dir / f).exists():
			shutil.copy2(song_dir / f, dst / f)
	info = {
		"zeit": now.strftime("%Y-%m-%d %H:%M:%S"),
		"notiz": note or "",
		"werkzeug_git": git_state(),
		"code_passt_zu_yaml": is_current(song_dir, read_fragment(song_dir / GEN_FILE)),
		"sha256": {f: sha(song_dir / f) for f in VERSION_FILES},
	}
	write_lf(dst / "info.yaml", yaml.safe_dump(info, allow_unicode=True, sort_keys=False))
	return dst


#==================================================================
#=========== src/ zusammensetzen ==================================
#==================================================================

CPP_HEADER = """//==================================================================
// AUTOMATISCH GENERIERT von tools/songgen.py aus songs/<Song>/generated.cpp
// NICHT von Hand ändern -> song.yaml / show.yaml anpassen und den Song neu generieren
//==================================================================
#include <Arduino.h>
#include <FastLED.h>
#include "definitions.h"
#include "functions.h"
#include "FXprograms.h"
#include "guitarShapeFX.h"
#include "colorSchemes.h"
#include "scenes.h"
#include "matrixFunctions.h"
#include "songs_generated.h"

extern volatile byte prog;
extern byte markerLED1, markerLED2, markerLED3, markerLED4, markerLED5, markerLED6, markerLED7;
"""


def gen_markers(marker_cases):
	"""setGeneratedMarkerLEDs(): wird aus setMarkerLEDs() (markerLEDs.cpp) im default-Fall aufgerufen,
	also nur für Songs ohne handgeschriebene Marker."""
	lines = ["//==================================================================",
			 "// Bund-Marker der generierten Songs (aus markers: in songs/<Song>/song.yaml)",
			 "//==================================================================",
			 "void setGeneratedMarkerLEDs(byte songID, byte partID) {",
			 "#if !defined(NOMARKER)",
			 "\tswitch (songID) {"]
	for case_lines in marker_cases:
		lines += case_lines
	lines += ["\tdefault:", "\t\tbreak;\t// keine Marker", "\t}", "#endif", "}"]
	return "\n".join(lines)


def write_if_changed(path, text):
	if not path.exists() or path.read_text(encoding="utf-8") != text:
		path.write_text(text, encoding="utf-8")


def gen_is_generated(frags):
	"""isGeneratedSong(): bei diesen Songs nimmt main.cpp die Verspätung eines Part-Wechsels in den nächsten Part mit."""
	cases = ["\tcase " + ": case ".join(str(f["id"]) for f in frags) + ":", "\t\treturn true;"] if frags else []
	return "\n".join(["//==================================================================",
					  "// Generierte Songs haben eine exakte Timeline (kein von Hand verkürzter Part): main.cpp gleicht bei ihnen",
					  "// die Verspätung jedes Part-Wechsels aus, damit sich über den Song kein Versatz zum Klick aufsummiert",
					  "//==================================================================",
					  "bool isGeneratedSong(byte songID) {", "\tswitch (songID) {"] + cases + ["\t}", "\treturn false;", "}"])


def assemble():
	"""src/songs_generated.cpp/.h + Block in main.cpp aus den generated.cpp aller Song-Ordner bauen.
	Der Code jedes Songs wird unverändert übernommen, hier wird nichts neu generiert."""
	frags = sorted(fragments(), key=lambda f: f["id"])
	seen = {}
	for f in frags:
		for key in (f["id"], f["function"]):
			if key in seen:
				raise SongError(f"songs/{f['dir']}: {key} ist auch in songs/{seen[key]} vergeben")
			seen[key] = f["dir"]

	write_if_changed(OUT_CPP, CPP_HEADER + "\n" + "\n\n".join(f["code"] for f in frags) + "\n\n"
					 + gen_markers([f["markers"] for f in frags if f["markers"]]) + "\n\n" + gen_is_generated(frags) + "\n")

	h = ["// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern", "#pragma once", "",
		 "#include <Arduino.h>", "",
		 "void setGeneratedMarkerLEDs(byte songID, byte partID);\t// Marker der generierten Songs",
		 "bool isGeneratedSong(byte songID);\t// exakte Timeline -> main.cpp gleicht die Verspätung der Part-Wechsel aus", ""]
	for f in frags:
		h.append(f"void {f['function']}();\t// #{f['id']} {f['name']}")
	if any(f["parts"] for f in frags):
		h += ["", "// Part-Nummern (case) der generierten Songs - für handgeschriebenen Code, der in einen Song springt (Trailer)"]
		for f in frags:
			h += [f"#define {c} {n}" for c, n in f["parts"]]
	write_if_changed(OUT_H, "\n".join(h) + "\n")

	text = MAIN_CPP.read_text(encoding="utf-8")
	new = "\n".join(bind_main(text.split("\n"), frags))
	if new != text:
		MAIN_CPP.write_text(new, encoding="utf-8")
	return frags


def print_assembled(frags):
	print(f"\nsrc/songs_generated.cpp/.h + main.cpp: {len(frags)} Song(s) - "
		  + (", ".join(f"#{f['id']} {f['name']}" for f in frags) or "keine"))


def print_timeline(song, timeline):
	print(f"\n#{song['id']} {song['name']}  ({song['function']}, {song['bpm']} BPM, midi_offset {offset_text(song)})")
	print(f"  {'case':>4}  {'start':>9}  {'dauer':>6}  abschnitt")
	for p in timeline:
		print(f"  {p['case']:>4}  {fmt_time(p['start']):>9}  {p['dur']:>6}  {p['sec'].get('name', '')}")
	for dev, pl in (song.get("_scroll_plans") or {}).items():
		if pl["mode"] == "wait":
			how = (f"{pl['wait']} ms schwarz, dann " if pl["wait"] else "") + f"Lauftext {pl['scroll']} ms"
		else:
			how = f"Lauftext {pl['scroll']} ms, dann Rest von '{pl['fill_part']['sec'].get('name')}' ({pl['fill_dur']} ms)"
		print(f"  {dev:<12} \"{pl['text']}\": {how} -> Einstieg case {pl['join']['case']} @{fmt_time(pl['join']['start'])}")
	for p in timeline:
		if p.get("text_info"):
			print(f"  Text (Matrix) case {p['case']} '{p['sec'].get('name', '')}': {p['text_info']}")
	for p in timeline:
		infos = pipeline_calls(p, song)[1]
		if infos:
			print(f"  Ausgabe case {p['case']} '{p['sec'].get('name', '')}': {', '.join(infos)}")
	for n in song.get("_notes", []):
		print(f"  Hinweis: {n}")
	print(f"  Marker: {song.get('_marker_note')}")
	for l in song.get("_marker_inline", []):
		print("    " + l.replace("\t", "  "))


#==================================================================
#=========== Kommandos ============================================
#==================================================================

def cmd_list():
	print("Songs unter songs/ (generieren: songgen.py <Song>):\n")
	for d in song_dirs():
		if (d / GEN_FILE).exists():
			f = read_fragment(d / GEN_FILE)
			sid = f"#{f['id']}"
			if not (d / SONG_FILE).exists():
				state = f"Code eingefroren (keine {SONG_FILE}) - bleibt in der Firmware, wie er ist"
			else:
				state = "aktuell" if is_current(d, f) else f"{SONG_FILE}/{SHOW_FILE} seit der Generierung geändert"
		elif (d / SONG_FILE).exists():
			state, sid = "noch nicht generiert", ""
		else:
			state, sid = f"leer (keine {SONG_FILE})", ""
		print(f"  {d.name:<28} {sid:>4}  {state}  ({len(versions(d))} Versionen)")
	return 0


def cmd_generate(song_dir, dry_run, note):
	song = load_song(song_dir)
	timeline, code, marker_lines, errors = generate(song, fragments(skip=song_dir))
	print_timeline(song, timeline)
	if trailer_jumps(song["id"]) or errors:
		print("  Part-Konstanten (songs_generated.h): " + ", ".join(f"{c}={n}" for c, n in song["_parts"]))
	if errors:
		print("\nFEHLER:", file=sys.stderr)
		for e in errors:
			print("  - " + e, file=sys.stderr)
		return 1
	if dry_run:
		print("\n(dry-run, nichts geschrieben)")
		return 0

	gen = song_dir / GEN_FILE
	if gen.exists():
		old = read_fragment(gen)
		if old["song_sha"] != sha(song_dir / SONG_FILE):
			print(f"\n{SONG_FILE} wurde seit der letzten Generierung geändert (von dir) - wird übernommen.")
		if old["code"] == code and old["markers"] == marker_lines:
			print("Der erzeugte Code ist identisch mit dem bisherigen.")
	write_lf(gen, fragment_text(song, code, marker_lines, song_dir))
	v = save_version(song_dir, note)
	print(f"\n-> songs/{song_dir.name}/{GEN_FILE}")
	print(f"-> Version {v.name} gespeichert" if v else "-> keine neue Version (genau dieser Stand ist schon gespeichert)")
	print_assembled(assemble())
	return 0


def cmd_versions(song_dir):
	vs = versions(song_dir)
	if not vs:
		print(f"songs/{song_dir.name}: noch keine Versionen")
		return 0
	print(f"Versionen von songs/{song_dir.name} (zurückholen: songgen.py {song_dir.name} --restore <Version>):\n")
	for v in vs:
		info = yaml.safe_load((v / "info.yaml").read_text(encoding="utf-8")) if (v / "info.yaml").exists() else {}
		active = all(norm_text(song_dir / f) == norm_text(v / f) for f in (SHOW_FILE, GEN_FILE))
		print(f"  {v.name}{'  <- aktiv' if active else ''}  {info.get('notiz') or ''}"
			  + ("" if info.get("code_passt_zu_yaml", True) else "  (Code älter als die YAML-Dateien)"))
	return 0


def cmd_restore(song_dir, version):
	hit = [v for v in versions(song_dir) if v.name == version] or [v for v in versions(song_dir) if v.name.startswith(version)]
	if len(hit) != 1:
		raise SongError(f"Version '{version}' {'nicht gefunden' if not hit else 'ist nicht eindeutig'} - siehe --versions")
	vdir = hit[0]
	frag = read_fragment(vdir / GEN_FILE)

	saved = save_version(song_dir, f"automatisch vor Restore von {vdir.name}")
	if saved:
		print(f"Bisheriger Stand gesichert als Version {saved.name}")

	# zurück kommen nur show.yaml + generated.cpp. song.yaml gehört dem User und wird nie geschrieben.
	if (vdir / SHOW_FILE).exists():
		shutil.copy2(vdir / SHOW_FILE, song_dir / SHOW_FILE)
	elif (song_dir / SHOW_FILE).exists():
		(song_dir / SHOW_FILE).unlink()
	shutil.copy2(vdir / GEN_FILE, song_dir / GEN_FILE)
	print(f"Version {vdir.name} ist wieder aktiv: {SHOW_FILE} + {GEN_FILE} (Code 1:1 wie gespeichert)")

	if norm_text(song_dir / SONG_FILE) != norm_text(vdir / SONG_FILE):
		print(f"\nACHTUNG: deine {SONG_FILE} ist heute anders als in dieser Version. Sie wird NICHT zurückkopiert.\n"
			  f"  Der wiederhergestellte Code gehört zu songs/{song_dir.name}/{VERSIONS_DIR}/{vdir.name}/{SONG_FILE}.\n"
			  f"  Beim nächsten Generieren gilt wieder deine aktuelle {SONG_FILE}. Willst du die alte zurück, kopiere sie selbst.")
	try:
		song = load_song(song_dir, vdir / SONG_FILE, vdir / SHOW_FILE)
		_tl, code, marker_lines, _e = generate(song, [])
		same = code == frag["code"] and marker_lines == frag["markers"]
	except SongError:
		same = False
	print("\nKontrolle: der heutige Generator erzeugt aus dieser Version exakt denselben Code." if same else
		  "\nHinweis: der heutige Generator würde aus dieser Version anderen Code erzeugen (Werkzeug oder Header geändert).\n"
		  "  Verwendet wird der gespeicherte Code, unverändert.")
	print_assembled(assemble())
	return 0


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("song", nargs="?", help="Ordnername unter songs/ (Anfang genügt)")
	ap.add_argument("--dry-run", action="store_true", help="nur Timeline anzeigen, nichts schreiben")
	ap.add_argument("--note", default="", help="Notiz zur gespeicherten Version")
	ap.add_argument("--versions", action="store_true", help="gespeicherte Versionen des Songs auflisten")
	ap.add_argument("--restore", metavar="VERSION", help="Version wieder aktiv machen (show.yaml + Code, nie song.yaml)")
	ap.add_argument("--assemble", action="store_true", help="nur src/ aus den generated.cpp aller Songs neu zusammensetzen")
	args = ap.parse_args()

	try:
		if args.assemble:
			print_assembled(assemble())
			return 0
		if not args.song:
			return cmd_list()
		song_dir = find_song_dir(args.song)
		if args.versions:
			return cmd_versions(song_dir)
		if args.restore:
			return cmd_restore(song_dir, args.restore)
		return cmd_generate(song_dir, args.dry_run, args.note)
	except SongError as e:
		print(f"FEHLER: {e}", file=sys.stderr)
		return 1


if __name__ == "__main__":
	sys.exit(main())
