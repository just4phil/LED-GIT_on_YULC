#!/usr/bin/env python3
"""
songgen.py - erzeugt Song-Funktionen aus songs/*.yaml

    tools/.venv/Scripts/python tools/songgen.py            # alle Songs generieren
    tools/.venv/Scripts/python tools/songgen.py --dry-run  # nur Timeline anzeigen, nichts schreiben

Pro Song zwei Dateien:
    songs/<name>.yaml        semantisch (vom User): Tempo, Takte, midi_offset, was musikalisch passiert
    songs/<name>.show.yaml   technisch (von Claude abgeleitet): Szenen, Farbschemata, Overrides, Tails
Die Struktur steht nur in der semantischen Datei, die Show-Datei ordnet per Abschnittsname zu.

Schreibt:
    src/songs_generated.cpp / .h    (komplett neu, eine Funktion pro YAML)
    src/main.cpp                    (nur zwischen den Markern "GENERATED SONGS")

Zeitbasis: 0 ms = Eintreffen des Start-MIDI-Signals. Das MIDI kommt midi_offset (Notenwert, z. B. 1/8)
NACH Takt 1 -> der erste Part wird um den Offset kürzer. Alle Grenzen werden aus absoluten Zeiten
gerundet, dadurch entsteht keine kumulative Rundungsdrift.
"""
import argparse
import re
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

MARK_BEGIN = "// >>> GENERATED SONGS (tools/songgen.py) >>>"
MARK_END = "// <<< GENERATED SONGS <<<"

SCROLL_DEVICES = ("SCROLLMATRIX", "GITBOARD")	# zeigen am Songanfang Titel + Interpret als Lauftext
SCROLL_MAX_WAIT_MS = 4000	# so lange darf die Matrix vor dem Lauftext schwarz bleiben, damit er genau an einer Grenze endet
BLACK = "progBlack(${dur}, ${next})"
MAX_PART_MS = 0xFFFFFFFF
ENERGY_SCENE = {1: "SCENE_CALM", 2: "SCENE_VERSE", 3: "SCENE_BUILDUP", 4: "SCENE_DROP", 5: "SCENE_DROP"}

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
			raise SongError(f"Abschnitt '{sec.get('name')}' hat keine Gestaltung: scene/fx in der .show.yaml oder energy setzen")
		if energy <= 0:
			return fill(BLACK, part, song)
		scene = ENERGY_SCENE[min(5, int(energy))]
	return f"scene({scene}, {part['dur']}, {part['next']}, {round(part['bpm'])});"


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
		if fp["sec"].get("scheme"):
			extra.append(f"\t\tsetColorScheme({fp['sec']['scheme']});")
		extra += ["\t\t" + device_call(fp, song, device), "\t\tbreak;"]
	return head, extra


def gen_function(song, timeline, end_case):
	fn = song["function"]
	lines = []
	bpm = song["bpm"]
	lines.append(f"//#{song['id']} {song['name']}" + (f" - {song['artist']}" if song.get("artist") else "")
				 + f"  {bpm} BPM  midi_offset {offset_text(song)}  (generiert aus songs/{song['_file']} + {song['_show'] or '-'})")
	lines.append(f"void {fn}() {{")
	lines.append("")
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

		if sec.get("scheme"):
			lines.append(f"\t\tsetColorScheme({sec['scheme']});")

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


def main_song_ids():
	"""case-IDs im switch(songID) von main.cpp, ohne den generierten Block."""
	text = MAIN_CPP.read_text(encoding="utf-8")
	text = re.sub(re.escape(MARK_BEGIN) + ".*?" + re.escape(MARK_END), "", text, flags=re.S)
	m = re.search(r"switch \(songID\) \{(.*?)default:", text, re.S)
	if not m:
		raise SongError("switch (songID) in main.cpp nicht gefunden")
	body = "\n".join(l for l in m.group(1).splitlines() if not l.strip().startswith("//"))
	return set(int(x) for x in re.findall(r"case (\d+):", body))


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


SONG_DESIGN_KEYS = ("function", "scheme", "scroll_text", "end_black_ms")
STRUCTURE_KEYS = ("name", "bars", "beats", "bpm", "beats_per_bar")


def merge_show(song, show, show_name):
	"""Technische Gestaltung (show.yaml) per Abschnittsname in die semantische Struktur einsetzen."""
	if "markers" in show:
		raise SongError(f"{show_name}: markers gehören in {song['_file']} (Song-Datei), nicht in die Show")
	for k in SONG_DESIGN_KEYS:
		if k in show:
			song[k] = show[k]
	design = show.get("sections") or {}
	names = [s["name"] for s in song["sections"]]
	for name in design:
		if name not in names:
			raise SongError(f"{show_name}: Abschnitt '{name}' gibt es in {song['_file']} nicht")
	for sec in song["sections"]:
		d = design.get(sec["name"]) or {}
		clash = [k for k in d if k in STRUCTURE_KEYS]
		if clash:
			raise SongError(f"{show_name}: '{sec['name']}' darf {', '.join(clash)} nicht setzen - Struktur gehört in {song['_file']}")
		sec.update(d)


def force_black_start(song):
	"""Mit dem Start-MIDI sind immer erst alle Geräte schwarz: der erste Abschnitt ist immer progBlack."""
	first = song["sections"][0]
	design = [k for k in ("scene", "fx", "devices", "tail", "scheme") if k in first and first[k] != BLACK]
	if design:
		song.setdefault("_notes", []).append(
			f"erster Abschnitt '{first['name']}' ist immer BLACK - ignoriert: {', '.join(design)}")
	for k in ("scene", "devices", "tail", "scheme"):
		first.pop(k, None)
	first["fx"] = BLACK
	first.setdefault("why", "Start-MIDI: alle Geräte schwarz")


def load_songs():
	songs = []
	for f in sorted(SONGS_DIR.glob("*.yaml")):
		if f.name.endswith((".analysis.yaml", ".show.yaml")):
			continue
		song = yaml.safe_load(f.read_text(encoding="utf-8"))
		song["_file"] = f.name
		for req in ("id", "name", "bpm", "sections"):
			if req not in song:
				raise SongError(f"{f.name}: Feld '{req}' fehlt")
		names = [s.get("name") for s in song["sections"]]
		dup = sorted({n for n in names if names.count(n) > 1})
		if None in names or dup:
			raise SongError(f"{f.name}: jeder Abschnitt braucht einen eindeutigen Namen (doppelt: {', '.join(map(str, dup))})")

		show_file = f.with_name(f.stem + ".show.yaml")
		song["_show"] = show_file.name if show_file.exists() else None
		if show_file.exists():
			merge_show(song, yaml.safe_load(show_file.read_text(encoding="utf-8")) or {}, show_file.name)
		force_black_start(song)
		song.setdefault("function", "gen_" + pascal(song["name"]))
		songs.append(song)
	return songs


CPP_HEADER = """//==================================================================
// AUTOMATISCH GENERIERT von tools/songgen.py aus songs/*.yaml
// NICHT von Hand ändern -> YAML anpassen und neu generieren
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
			 "// Bund-Marker der generierten Songs (aus markers: in songs/*.yaml)",
			 "//==================================================================",
			 "void setGeneratedMarkerLEDs(byte songID, byte partID) {",
			 "#if !defined(NOMARKER)",
			 "\tswitch (songID) {"]
	for case_lines in marker_cases:
		lines += case_lines
	lines += ["\tdefault:", "\t\tbreak;\t// keine Marker", "\t}", "#endif", "}"]
	return "\n".join(lines)


def write_outputs(songs, funcs_code, marker_cases):
	OUT_CPP.write_text(CPP_HEADER + "\n" + "\n\n".join(funcs_code) + "\n\n" + gen_markers(marker_cases) + "\n",
					   encoding="utf-8")

	h = ["// AUTOMATISCH GENERIERT von tools/songgen.py - nicht von Hand ändern", "#pragma once", "",
		 "#include <Arduino.h>", "",
		 "void setGeneratedMarkerLEDs(byte songID, byte partID);\t// Marker der generierten Songs", ""]
	for s in songs:
		h.append(f"void {s['function']}();\t// #{s['id']} {s['name']}")
	OUT_H.write_text("\n".join(h) + "\n", encoding="utf-8")

	text = MAIN_CPP.read_text(encoding="utf-8")
	block = [MARK_BEGIN]
	for s in songs:
		block += [f"\t\tcase {s['id']}:", f"\t\t\t{s['function']}();", "\t\t\tbreak;"]
	block.append("\t\t" + MARK_END)
	new = re.sub(re.escape(MARK_BEGIN) + ".*?" + re.escape(MARK_END), lambda m: "\n".join(block), text, flags=re.S)
	if new == text and MARK_BEGIN not in text:
		raise SongError("Marker 'GENERATED SONGS' fehlen in main.cpp")
	if new != text:
		MAIN_CPP.write_text(new, encoding="utf-8")


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
	for n in song.get("_notes", []):
		print(f"  Hinweis: {n}")


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("--dry-run", action="store_true", help="nur Timeline anzeigen, nichts schreiben")
	args = ap.parse_args()

	try:
		songs = load_songs()
		taken = main_song_ids()
		seen_ids, seen_fns = {}, {}
		code, all_errors, marker_cases = [], [], []
		hand_markers = mk.handwritten_ids()
		for s in songs:
			if s["id"] in taken:
				all_errors.append(f"{s['_file']}: Song-ID {s['id']} ist in main.cpp schon vergeben")
			if not 0 <= s["id"] <= 127:
				all_errors.append(f"{s['_file']}: Song-ID muss 0..127 sein (MIDI)")
			if s["id"] in seen_ids:
				all_errors.append(f"{s['_file']}: Song-ID {s['id']} auch in {seen_ids[s['id']]}")
			if s["function"] in seen_fns:
				all_errors.append(f"{s['_file']}: Funktionsname {s['function']} auch in {seen_fns[s['function']]}")
			seen_ids[s["id"]] = seen_fns[s["function"]] = s["_file"]

			timeline, end_case = build_timeline(s)
			if s.get("scroll_text", True):
				widths = matrix_widths()
				s["_scroll_plans"] = {d: plan_scroll(s, timeline, widths[d]) for d in SCROLL_DEVICES}
			all_errors += [f"{s['_file']}: {e}" for e in validate(s, timeline)]

			# Bund-Marker: Handarbeit in markerLEDs.cpp hat immer Vorrang
			if s["id"] in hand_markers:
				s["_marker_note"] = "von Hand in markerLEDs.cpp" + (" - markers: in der YAML wird ignoriert" if s.get("markers") else "")
			elif s.get("markers"):
				errs = mk.validate(s["markers"], [x["name"] for x in s["sections"]])
				all_errors += [f"{s['_file']}: {e}" for e in errs]
				part_cases = {}
				for p in timeline:
					part_cases.setdefault(p["sec"].get("_parent") or p["sec"].get("name"), []).append(p["case"])
				if not errs:
					marker_cases.append(mk.gen_case(s["id"], s["markers"], part_cases))
				s["_marker_note"] = "generiert aus markers: der Song-Datei"
			else:
				s["_marker_note"] = "keine"
			print_timeline(s, timeline)
			print(f"  Marker: {s.get('_marker_note')}")
			code.append(gen_function(s, timeline, end_case))
	except SongError as e:
		print(f"FEHLER: {e}", file=sys.stderr)
		return 1

	if all_errors:
		print("\nFEHLER:", file=sys.stderr)
		for e in all_errors:
			print("  - " + e, file=sys.stderr)
		return 1

	if args.dry_run:
		print("\n(dry-run, nichts geschrieben)")
		return 0
	write_outputs(songs, code, marker_cases)
	print(f"\n{len(songs)} Song(s) -> {OUT_CPP.relative_to(ROOT)}, {OUT_H.relative_to(ROOT)}, main.cpp")
	return 0


if __name__ == "__main__":
	sys.exit(main())
