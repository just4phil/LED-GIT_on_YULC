#!/usr/bin/env python3
"""
sheet2song.py - semantische Song-Datei aus Songbook-Sheet (XML) + Audio (MP3) erzeugen

    tools/.venv/Scripts/python tools/sheet2song.py <Song> --id 31 [--midi-offset 1/8]

<Song> = Ordner unter songs/ (z. B. SuchAShame_v1). Sheet (.txt/.xml) und MP3 liegen in songs/<Song>/quelle/
(oder direkt im Ordner); mit --sheet / --audio lassen sie sich auch einzeln angeben.

Das Sheet liefert Parts, Lyrics, Akkorde, Tempo (<myTempo>, sonst <tempo>) und <transpose>.
Das Audio liefert, wie viele Takte jeder Part wirklich dauert: Die Akkordfolge jedes Parts wird
im Audio gesucht (Chroma-Analyse pro halbem Takt), die Part-Grenzen liegen immer auf einem Taktanfang.

Schreibt songs/<Song>/song.yaml - aber NUR, wenn es die Datei noch nicht gibt. Eine vorhandene song.yaml
gehört dem User und wird nie überschrieben: das Ergebnis landet dann als Vorschlag in song.vorschlag.yaml
daneben (zum Vergleichen, der Generator beachtet sie nicht).
Bund-Marker werden nur vorgeschlagen, wenn es für die Song-ID noch keine gibt (markerLEDs.cpp oder
markers: in der vorhandenen song.yaml); ein vorhandener markers:-Block wird wörtlich übernommen.
"""
import argparse
import math
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
import markers as mk  # noqa: E402

from songgen import ROOT, SONG_FILE, VERSIONS_DIR, SongError, find_song_dir  # noqa: E402

PROPOSAL_FILE = "song.vorschlag.yaml"	# Ergebnis, wenn es schon eine song.yaml gibt (die wird nie überschrieben)
SR = 22050
HOP = 512
SILENT_BAR_DB = -35			# Takt gilt als still (Pause vor/nach dem Song)
LEFTOVER_COST = 0.08		# Kosten je halbem Takt, der am Ende keinem Part zugeordnet wird
PRIOR_WEIGHT = 1.0			# wie stark die aus dem Sheet geschätzte Taktzahl zählt (gegen den Audio-Abgleich)
JUMP_DB = 3.0				# Pegelsprung innerhalb eines Parts, ab dem ein Hinweis kommt

NOTE_IDX = {"C": 0, "C#": 1, "DB": 1, "D": 2, "D#": 3, "EB": 3, "E": 4, "F": 5, "F#": 6, "GB": 6,
			"G": 7, "G#": 8, "AB": 8, "A": 9, "A#": 10, "BB": 10, "B": 11, "H": 11}
NOTE_NAME = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"]


#==================================================================
#=========== Sheet ================================================
#==================================================================

def parse_chord(tok):
	"""'C#m7/G#' -> (1, 'min'); reduziert auf Dur/Moll-Dreiklang. None, wenn kein Akkord."""
	m = re.match(r"\s*([A-Ha-h])([#b]?)(.*)$", tok)
	if not m:
		return None
	root = NOTE_IDX.get((m.group(1).upper() + m.group(2).upper()).replace("HB", "B"))
	if root is None:
		return None
	rest = m.group(3).split("/")[0]
	minor = bool(re.match(r"(m(?!aj)|min|-|dim)", rest))
	return (root, "min" if minor else "maj")


def chord_label(ch, transpose=0):
	root, q = ch
	return NOTE_NAME[(root + transpose) % 12] + ("m" if q == "min" else "")


def parse_sheet(path):
	text = Path(path).read_text(encoding="utf-8-sig")
	root = ET.fromstring(text)
	g = lambda tag: (root.findtext(tag) or "").strip()
	tempo = g("myTempo") or g("tempo")
	sheet = {
		"title": g("title"), "artist": g("artist"),
		"bpm": float(tempo) if tempo else None,
		"transpose": int(g("transpose") or 0),
		"parts": [],
	}
	last = None
	for part in root.findall("part"):
		rows = []
		for row in part.findall("row"):
			raw = row.text or ""
			chords = [parse_chord(c) for c in re.findall(r"\[([^\]]+)\]", raw)]
			chords = [c for c in chords if c]
			lyric = re.sub(r"\s+", " ", re.sub(r"\[[^\]]*\]", "", raw)).strip()
			lines = max(1, len([l for l in raw.split("\n") if l.strip()]))
			if not chords and not lyric:
				continue
			if not chords:
				if last is None:
					continue
				chords = [last]
			last = chords[-1]
			# Takte der Zeile schätzen: Textzeile = 1 Takt (4 Akkorde = 2 Takte), mehrzeilig = je Zeile 1 Takt,
			# reine Akkordzeile (instrumental) = 1 Akkord pro Takt
			bars = len(chords) if not lyric else max(lines, math.ceil(len(chords) / 2))
			rows.append({"chords": chords, "lyric": lyric, "bars": bars})
		# "Verse 1, 16 Takte" im Part-Namen = feste Taktzahl vom User (zählt mehr als jede Schätzung);
		# damit zählen auch Parts ohne Akkorde (Synth-Intro, Drum-Solo)
		name = part.get("name", "part").strip()
		fixed = None
		m = re.match(r"(.*?)[\s,;:(-]*(\d+)\s*takte?\)?\s*$", name, re.I)
		if m and m.group(1).strip():
			name, fixed = m.group(1).strip(), int(m.group(2))
		if rows or fixed:
			sheet["parts"].append({"name": name, "rows": rows, "fixed_bars": fixed})
	if not sheet["parts"]:
		raise SystemExit("FEHLER: keine Parts mit Akkorden im Sheet gefunden")
	return sheet


def part_pattern(part):
	"""Akkordfolge des Parts in halben Takten (so wie im Sheet notiert) + geschätzte Takte."""
	seq = []
	for r in part["rows"]:
		halves, n = 2 * r["bars"], len(r["chords"])
		base, extra = divmod(halves, n)
		for i, c in enumerate(r["chords"]):
			seq += [c] * (base + (1 if i >= n - extra else 0))
	return seq, part.get("fixed_bars") or sum(r["bars"] for r in part["rows"])


#==================================================================
#=========== Audio ================================================
#==================================================================

def tmpl(ch):
	root, q = ch
	t = np.zeros(12)
	t[[root, (root + (3 if q == "min" else 4)) % 12, (root + 7) % 12]] = 1
	return t / np.linalg.norm(t)


def load_audio(path):
	import librosa
	y, sr = librosa.load(str(path), sr=SR, mono=True)
	S = np.abs(librosa.stft(y, hop_length=HOP))
	rms_db = librosa.amplitude_to_db(librosa.feature.rms(S=S)[0], ref=np.max)
	times = librosa.frames_to_time(np.arange(len(rms_db)), sr=sr, hop_length=HOP)
	chroma = librosa.feature.chroma_cqt(y=librosa.effects.harmonic(y), sr=sr, hop_length=HOP, bins_per_octave=36)
	onset = librosa.onset.onset_strength(y=librosa.effects.percussive(y), sr=sr, hop_length=HOP)
	freqs = librosa.fft_frequencies(sr=sr)
	kick = librosa.onset.onset_strength(S=librosa.amplitude_to_db(S[freqs < 120]), sr=sr, hop_length=HOP)
	return {"dur": len(y) / sr, "times": times, "rms_db": rms_db, "chroma": chroma[:, :len(times)],
			"onset": onset[:len(times)], "kick": kick[:len(times)]}


def beat_phase(a, bpm):
	"""Lage des Beat-Rasters (BPM ist bekannt): Versatz mit der stärksten Onset-Summe auf dem Raster."""
	beat = 60.0 / bpm
	t, env = a["times"], a["onset"]
	best, best_ph = -1, 0.0
	for ph in np.arange(0, beat, 0.005):
		idx = np.searchsorted(t, np.arange(ph, t[-1], beat)).clip(0, len(env) - 1)
		s = env[idx].mean()
		if s > best:
			best, best_ph = s, ph
	return best_ph


def bar_grid(a, first_bar, bar):
	starts = np.arange(first_bar, a["dur"] - bar * 0.25, bar)
	t = a["times"]
	halves, db = [], []
	for s in starts:
		for h in range(2):
			m = (t >= s + h * bar / 2) & (t < s + (h + 1) * bar / 2)
			c = a["chroma"][:, m].mean(axis=1) if m.any() else np.zeros(12)
			halves.append(c / (np.linalg.norm(c) + 1e-9))
		m = (t >= s) & (t < s + bar)
		db.append(float(a["rms_db"][m].mean()) if m.any() else -80.0)
	return starts, np.array(halves), np.array(db)


#==================================================================
#=========== Abgleich (dynamische Programmierung über Taktgrenzen) ==
#==================================================================

def align(parts, halves, s0, last, chord_list):
	"""Parts nacheinander auf die Takte s0..last verteilen, sodass die Akkorde am besten passen.
	Kosten je halbem Takt = (bester Akkord dort) - (erwarteter Akkord); Muster dürfen sich wiederholen
	(Sheet notiert Wiederholungen oft nur einmal) oder langsamer laufen (1 Akkord pro Takt bzw. pro 2 Takte).
	Parts mit fester Taktzahl ("... 16 Takte" im Namen) bekommen genau diese Länge."""
	T = np.array([tmpl(c) for c in chord_list])
	cidx = {c: i for i, c in enumerate(chord_list)}
	sim = halves @ T.T									# [halbe Takte, Akkorde]
	cost = sim.max(axis=1, keepdims=True) - sim			# 0 = erwarteter Akkord ist der passendste
	N = last - s0 + 1
	P = len(parts)
	INF = 1e18
	dp = np.full((P + 1, N + 1), INF)
	back = np.zeros((P + 1, N + 1), dtype=int)
	var = np.zeros((P + 1, N + 1), dtype=int)
	dp[0][0] = 0.0
	pats = []
	for p in parts:
		seq, exp_bars = part_pattern(p)
		idx = np.array([cidx[c] for c in seq], dtype=int)
		pats.append(([idx, np.repeat(idx, 2), np.repeat(idx, 4)] if len(idx) else [idx], exp_bars))

	for pi, (variants, exp_bars) in enumerate(pats):
		fixed = parts[pi].get("fixed_bars")
		max_len = min(N, max(4, exp_bars * 4))
		for b in range(N):
			if dp[pi][b] >= INF:
				continue
			h0 = 2 * (s0 + b)
			for vi, pat in enumerate(variants):
				L = min(2 * max_len, 2 * (N - b))
				if len(pat):
					c = np.cumsum(cost[h0 + np.arange(L), pat[np.arange(L) % len(pat)]])
				else:
					c = np.zeros(L)		# Part ohne Akkorde: nur die feste Länge zählt
				for n in range(1, L // 2 + 1):
					if fixed and n != fixed:
						continue
					prior = 0.0 if fixed else PRIOR_WEIGHT * abs(math.log(n / exp_bars))
					v = dp[pi][b] + c[2 * n - 1] + prior
					if v < dp[pi + 1][b + n]:
						dp[pi + 1][b + n] = v
						back[pi + 1][b + n] = b
						var[pi + 1][b + n] = vi
	ends = [dp[P][e] + LEFTOVER_COST * 2 * (N - e) for e in range(N + 1)]
	e = int(np.argmin(ends))
	total = ends[e]
	res, b = [], e
	for pi in range(P, 0, -1):
		a = back[pi][b]
		res.append((a, b, var[pi][b]))
		b = a
	res.reverse()
	# Güte je Part: Anteil halber Takte, in denen der erwartete Akkord der beste (oder fast beste) ist
	fits = []
	for pi, (a, b, vi) in enumerate(res):
		pat = pats[pi][0][vi]
		hs = 2 * (s0 + a) + np.arange(2 * (b - a))
		if not len(pat):
			fits.append(None)
			continue
		seq = pat[np.arange(len(hs)) % len(pat)]
		fits.append(float((cost[hs, seq] < 0.05).mean()) if len(hs) else 0.0)
	return res, e, total, fits


#==================================================================
#=========== Ausgabe ==============================================
#==================================================================

def fmt(t):
	return f"{int(t // 60)}:{t % 60:05.2f}"


def ystr(s):
	return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def find_source(song_dir, exts, what):
	"""Die eine Datei mit passender Endung im Song-Ordner bzw. in quelle/ (nicht in versionen/)."""
	found = [f for f in sorted(song_dir.rglob("*")) if f.is_file() and f.suffix.lower() in exts
			 and VERSIONS_DIR not in f.parts and f.parent.name != "audio-analyse"]
	if len(found) != 1:
		raise SystemExit(f"FEHLER: {what} in songs/{song_dir.name}: {len(found)} Kandidaten ({', '.join(f.name for f in found) or 'keine'})"
						 f" - nach quelle/ legen oder --{'sheet' if what == 'Sheet' else 'audio'} angeben")
	return found[0]


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("song", help="Ordnername unter songs/")
	ap.add_argument("--sheet", help="Sheet-Datei (Standard: die einzige .txt/.xml im Song-Ordner bzw. in quelle/)")
	ap.add_argument("--audio", help="Audiodatei (Standard: die einzige .mp3 im Song-Ordner bzw. in quelle/)")
	ap.add_argument("--id", type=int, required=True, help="Song-ID (MIDI CC 22), 0..127")
	ap.add_argument("--bpm", type=float, help="Tempo, falls nicht im Sheet (<myTempo>/<tempo>)")
	ap.add_argument("--midi-offset", default="1/8", help="Notenwert, z. B. 1/8 (Standard)")
	args = ap.parse_args()

	try:
		song_dir = find_song_dir(args.song)
	except SongError as e:
		raise SystemExit(f"FEHLER: {e}")
	args.sheet = args.sheet or find_source(song_dir, (".txt", ".xml"), "Sheet")
	args.audio = args.audio or find_source(song_dir, (".mp3",), "MP3")
	sheet = parse_sheet(args.sheet)
	bpm = args.bpm or sheet["bpm"]
	if not bpm:
		raise SystemExit("FEHLER: kein Tempo im Sheet - bitte --bpm angeben")
	audio = Path(args.audio).resolve()
	song_file = song_dir / SONG_FILE
	is_proposal = song_file.exists()	# vorhandene song.yaml gehört dem User -> nur einen Vorschlag daneben schreiben
	out = song_dir / PROPOSAL_FILE if is_proposal else song_file

	tr = sheet["transpose"]
	parts = sheet["parts"]
	# Akkorde im Audio = Sheet-Akkorde + transpose
	for p in parts:
		for r in p["rows"]:
			r["chords"] = [((c[0] + tr) % 12, c[1]) for c in r["chords"]]
	chord_list = sorted({c for p in parts for r in p["rows"] for c in r["chords"]})

	print(f"Sheet: {sheet['title']} - {sheet['artist']}, {bpm:g} BPM, transpose {tr:+d}, {len(parts)} Parts")
	print("Audio analysieren ...")
	a = load_audio(audio)
	beat = 60.0 / bpm
	bar = 4 * beat
	ph = beat_phase(a, bpm)

	# alle 4 möglichen Taktanfänge probieren, der mit dem besten Akkord-Abgleich gewinnt
	best = None
	for k in range(4):
		first_bar = (ph + k * beat) % bar
		starts, halves, db = bar_grid(a, first_bar, bar)
		sounding = np.where(db > SILENT_BAR_DB)[0]
		s0, last = int(sounding[0]), int(sounding[-1])
		res, e, total, fits = align(parts, halves, s0, last, chord_list)
		if best is None or total < best["total"]:
			best = dict(k=k, first_bar=first_bar, starts=starts, db=db, s0=s0, last=last, res=res, e=e, total=total, fits=fits)
	b = best
	s0, starts, db = b["s0"], b["starts"], b["db"]

	sections, report = [], []
	if s0 > 0:
		sections.append({"name": "pause", "bars": s0, "energy": 0,
						 "description": f"{s0} stille(r) Takt(e) am Anfang der Audiodatei"})
	used = set()
	for pi, (st, en, vi) in enumerate(b["res"]):
		p = parts[pi]
		name = p["name"].lower()
		n, i = name, 2
		while n in used:
			n, i = f"{name} {i}", i + 1
		used.add(n)
		nb = en - st
		exp = part_pattern(p)[1]
		gs = s0 + st
		lyric = next((r["lyric"] for r in p["rows"] if r["lyric"]), "")
		chords = " | ".join(" ".join(chord_label(((c[0] - tr) % 12, c[1])) for c in r["chords"]) for r in p["rows"])
		# Pegelsprünge innerhalb des Parts (Kandidaten zum Aufteilen)
		hints = []
		seg = db[gs:gs + nb]
		for j in range(1, nb):
			d = seg[j] - seg[j - 1]
			if abs(d) >= JUMP_DB:
				hints.append(f"Pegelsprung vor Takt {j + 1}: {seg[j - 1]:.1f} -> {seg[j]:.1f} dB")
		sec = {"name": n, "bars": nb, "lyrics": lyric, "chords": chords, "_hints": hints}
		if vi == 1:
			sec["_hints"].insert(0, "Akkorde laufen halb so schnell wie im Sheet notiert (1 Akkord pro Takt)")
		elif vi == 2:
			sec["_hints"].insert(0, "Akkorde laufen viermal so langsam wie im Sheet notiert (1 Akkord pro 2 Takte)")
		if p.get("fixed_bars"):
			sec["_hints"].insert(0, f"Taktzahl fest aus dem Sheet-Namen ({p['fixed_bars']} Takte)")
		sections.append(sec)
		report.append((n, nb, exp, starts[gs], b["fits"][pi], hints))
	tail = b["last"] - s0 + 1 - b["e"]
	if tail > 0:
		sections.append({"name": "ende", "bars": tail, "energy": 1,
						 "description": "Ausklang nach dem letzten Part (keinem Sheet-Part zugeordnet)"})

	beat1_ms = round(starts[0] * 1000)
	lines = [
		f"# Semantische Songbeschreibung, erzeugt von tools/sheet2song.py",
		f"#   Sheet: {Path(args.sheet).name}",
		f"#   Audio: {audio.name}",
		f"# Taktzahlen aus dem Abgleich der Sheet-Akkorde mit dem Audio. Bitte prüfen: Namen, Hinweise (# !),",
		f"# midi_offset; Parts mit Pegelsprung ggf. aufteilen (z. B. 'verse 2a'/'verse 2b').",
		"# Die Technik (Szenen/Farben) leitet Claude in show.yaml ab. Diese Datei hier gehört dir: kein Werkzeug",
		"# und kein Claude überschreibt sie. Pro Part kannst du ergänzen:",
		"#   energy: 0-5, description, mood, instruments, solo  -> Einschätzung, daraus wird die Show abgeleitet",
		"#   scene: SCENE_..., scheme: SCHEME_..., fx: \"prog...\"  -> feste Vorgabe, hat immer Vorrang vor show.yaml",
		"#   text: \"FUN\" (pulsiert im Beat; mehrere Wörter: eins pro Beat) oder text: {scroll: \"...\"}  -> Text auf der Matrix",
		"#   fade: complement  oder  fade: {to: triad|analog|rainbow|SCHEME_..., per: bar, hard: true}  -> Farben wandern im Takt",
		"",
		f"id: {args.id}",
		f"name: {ystr(sheet['title'] or audio.stem)}",
		f"artist: {ystr(sheet['artist'])}",
		f"bpm: {bpm:g}",
		"beats_per_bar: 4",
		f"midi_offset: {args.midi_offset}",
		f"audio: {audio.relative_to(song_dir).as_posix() if audio.is_relative_to(song_dir) else audio.as_posix()}",
		f"audio_beat1_ms: {beat1_ms}",
		"",
		"sections:",
	]
	for s in sections:
		lines.append(f"  - name: {ystr(s['name'])}")
		lines.append(f"    bars: {s['bars']}")
		for k in ("energy", "lyrics", "chords", "description"):
			if s.get(k) not in (None, ""):
				v = s[k]
				lines.append(f"    {k}: {v if isinstance(v, int) else ystr(v)}")
		for h in s.get("_hints", []):
			lines.append(f"    # ! {h}")
		lines.append("")
	# Bund-Marker: nie etwas Vorhandenes ändern
	old_block = None
	if song_file.exists():
		m = re.search(r"^(?:#[^\n]*\n)*markers:.*", song_file.read_text(encoding="utf-8"), re.S | re.M)
		old_block = m.group(0).rstrip("\n") if m else None
	counts = {}
	for pi, (st, en, vi) in enumerate(b["res"]):
		seq, exp = part_pattern(parts[pi])
		scale = (en - st) / max(1, exp)
		for c in seq:
			counts[c] = counts.get(c, 0) + scale
	chords_text = ", ".join(chord_label(c) for c, _n in sorted(counts.items(), key=lambda kv: -kv[1]))
	if args.id in mk.handwritten_ids():
		marker_msg = f"von Hand in markerLEDs.cpp (case {args.id}) - kein Vorschlag"
		lines += [f"# Bund-Marker: von Hand in src/markerLEDs.cpp gesetzt (case {args.id}), werden dort gepflegt.",
				  f"# Akkorde im Song (transponiert): {chords_text}"]
	elif old_block:
		marker_msg = "aus der vorhandenen Datei übernommen (unverändert)"
		lines.append(old_block)
	else:
		names, notes = mk.propose(counts)
		marker_msg = "VORSCHLAG: " + ", ".join(f"{n} ({mk.FRET[n]}. Bund)" for n in names)
		lines += mk.yaml_block(names, notes, chords_text)
	if is_proposal:
		lines[:0] = [f"# VORSCHLAG - {SONG_FILE} existiert schon und wurde NICHT angefasst. Nur zum Vergleichen,",
					 "# der Generator beachtet diese Datei nicht. Übernimm von Hand, was du brauchst.", ""]
	out.write_text("\n".join(lines) + "\n", encoding="utf-8")

	print(f"Takt 1 (Raster) bei {beat1_ms} ms, Takt = {bar * 1000:.1f} ms, erster klingender Takt: {s0 + 1}")
	print(f"\n  {'part':<24} {'takte':>5} {'sheet':>5}  {'start mp3':>9}  {'akkorde':>7}")
	for n, nb, exp, st, fit, hints in report:
		flag = "  <- prüfen" if fit is not None and fit < 0.5 else ""
		fit_txt = f"{fit * 100:6.0f}%" if fit is not None else "      -"
		print(f"  {n:<24} {nb:>5} {exp:>5}  {fmt(st):>9}  {fit_txt}{flag}")
		for h in hints:
			print(f"  {'':<24} ! {h}")
	if tail > 0:
		print(f"  {'ende':<24} {tail:>5}")
	print(f"\nBund-Marker: {marker_msg}")
	print(f"\n-> {out.relative_to(ROOT)}")
	if is_proposal:
		print(f"   {SONG_FILE} existiert schon und wurde nicht angefasst - das ist nur ein Vorschlag zum Vergleichen.")
	return 0


if __name__ == "__main__":
	sys.exit(main())
