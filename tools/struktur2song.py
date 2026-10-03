#!/usr/bin/env python3
"""
struktur2song.py - song.yaml aus der Struktur-Tabelle des Users (Excel) erzeugen

    tools/.venv/Scripts/python tools/struktur2song.py <Song> --neu     # songs/<Song>/quelle/struktur.xlsx anlegen
                                                                       # (Kopie von songs/struktur-vorlage.xlsx)
    tools/.venv/Scripts/python tools/struktur2song.py <Song>           # Tabelle -> song.yaml

Die Tabelle hat das Format des Excel-Songkalkulators (ein Song pro Datei): oben Midi-StartNummer (= Song-ID),
Interpret (A2), Titel (A3), BPM, StartTakt und StartBit (wo das Start-MIDI im DAW-Projekt liegt), darunter die
Kopfzeile mit "Songpart" und "bis takt" und pro Zeile ein Part. Gelesen werden nur Songpart, bis takt, Energie 0-5,
Effektidee, Beschreibung, BPM (Tempowechsel) und - falls vorhanden - Akkorde; die Millisekunden-Spalten sind nur
für den User, Millisekunden rechnet songgen.py. Kopf und Spalten werden an ihrer Beschriftung erkannt, nicht an
der Position. Ein Schluss-Black in der letzten Zeile wird nicht als Part übernommen (der Generator hängt ihn an).

Schreibt songs/<Song>/song.yaml - aber NUR, wenn es die Datei noch nicht gibt. Eine vorhandene song.yaml
gehört dem User und wird nie überschrieben: das Ergebnis landet dann als Vorschlag in song.vorschlag.yaml
(der Generator ignoriert sie). Die Vorlage wird ebenfalls nie überschrieben.
"""
import argparse
import re
import shutil
import sys
from pathlib import Path

import openpyxl
from openpyxl.styles import Alignment, Font, PatternFill
from openpyxl.utils import get_column_letter

sys.path.insert(0, str(Path(__file__).resolve().parent))
import markers as mk  # noqa: E402
from songgen import SONGS_DIR, SONG_FILE, VERSIONS_DIR, SongError, build_timeline, find_song_dir  # noqa: E402

TABLE_FILE = "struktur.xlsx"
TEMPLATE = SONGS_DIR / "struktur-vorlage.xlsx"	# Vorlage im Repo; --neu kopiert sie (der User darf sie anpassen)
PROPOSAL_FILE = "song.vorschlag.yaml"	# Ergebnis, wenn es schon eine song.yaml gibt (die wird nie überschrieben)
SHEET = "Struktur"
# Kopf: Beschriftung (klein) -> Schlüssel. Der Wert steht rechts daneben oder, wenn dort nichts steht, darunter.
# Interpret und Titel brauchen keine Beschriftung: die ersten beiden freien Texte in Spalte A.
HEAD_LABELS = {"midi-startnummer": "id", "song-id": "id", "bpm": "bpm", "starttakt": "starttakt", "startbit": "startbit",
			   "audio": "audio", "titel": "name", "interpret": "artist"}
# Part-Zeilen: Überschrift (klein) -> Schlüssel; alle anderen Spalten (ms, sekunden, millis akkum ...) werden ignoriert
COL_LABELS = {"songpart": "name", "part": "name", "bis takt": "bis", "von takt": "von", "energie 0-5": "energy",
			  "energie": "energy", "effektidee": "idea", "beschreibung": "description", "akkorde": "chords", "bpm": "bpm",
			  "ms gerundet!!": "ms_rounded", "ms gerundet": "ms_rounded"}
END_NAMES = ("black", "fini", "finito", "ende", "back to default")	# so heißt der Schluss-Black in der letzten Zeile
END_BLACK_MS = 10000	# Standard des Generators
COLS = [	# Vorlage: Kopfzeile wie im Songkalkulator (Überschrift, Breite)
	("Songpart", 30), ("ms gerundet!!", 13), ("Effektidee", 28), ("ms", 10), ("Energie 0-5", 12), (" (+ zu schnell)", 13),
	("BPM", 8), ("von takt", 10), ("bis takt", 10), ("takte", 8), ("sek/takt", 9), ("sekunden", 10), ("millis", 10),
	("millis akkum", 13), ("millis akkum", 13), ("Beschreibung", 44),
]
HEADER_ROW = 7
FIRST_ROW = HEADER_ROW + 2
ROWS = 80		# so viele Part-Zeilen bekommt die Vorlage (mit Formeln)
NOTES = [
	"Parts so schneiden, wie du sie für die Show brauchst. 'bis takt' = Taktnummer im DAW, an der der Part endet",
	"(= Anfang des nächsten). Halbe Takte als Komma-Zahl: 2269,5. Der erste Part beginnt bei StartTakt + StartBit.",
	"Das Werkzeug liest nur Songpart, bis takt, Energie, Effektidee, Beschreibung und BPM (bei Tempowechsel überschreiben).",
	"Alle ms-Spalten rechnen nur zur Kontrolle. Ein Schluss-Black in der letzten Zeile wird nicht als Part übernommen:",
	"der Generator hängt 10 s Schwarz an (steht in 'ms gerundet!!' etwas anderes, gilt dieser Wert).",
	"Optional: eine Spalte 'Akkorde' (z. B. Am F C G) für den Bund-Marker-Vorschlag, ein Feld 'Audio' im Kopf für die MP3.",
]

NOTE_IDX = {"C": 0, "C#": 1, "DB": 1, "D": 2, "D#": 3, "EB": 3, "E": 4, "F": 5, "F#": 6, "GB": 6,
			"G": 7, "G#": 8, "AB": 8, "A": 9, "A#": 10, "BB": 10, "B": 10, "H": 11}
NOTE_NAME = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"]


#==================================================================
#=========== Vorlage ==============================================
#==================================================================

def write_template(path):
	wb = openpyxl.Workbook()
	ws = wb.active
	ws.title = SHEET
	bold, grey = Font(bold=True), Font(color="808080", italic=True)
	fill = PatternFill("solid", fgColor="DDEBF7")
	for ref, label_text in (("A1", "Midi-StartNummer"), ("A4", "BPM"), ("C4", "StartTakt"), ("B6", "StartBit")):
		ws[ref].value, ws[ref].font = label_text, bold
	for ref in ("B1", "A2", "A3", "A5", "C5", "C6"):
		ws[ref].fill = fill
	for ref, hint in (("C1", "Song-ID: MIDI CC#0, 1..127 - freie Nummer oder die des alten Songs, den diese Fassung ersetzt"),
					  ("C2", "<- A2: Interpret"), ("C3", "<- A3: Titel"),
					  ("D5", "Taktnummer im DAW-Projekt, in der das Start-MIDI liegt"),
					  ("D6", "wie weit das MIDI nach dem Taktanfang kommt, als Bruchteil des Takts: 0,125 = 1/8 · 0,25 = 1/4 · 0,375 = 3/8")):
		ws[ref].value, ws[ref].font = hint, grey
	for c, (title, width) in enumerate(COLS, 1):
		cell = ws.cell(HEADER_ROW, c, title)
		cell.font = bold
		cell.fill = PatternFill("solid", fgColor="BDD7EE")
		ws.column_dimensions[get_column_letter(c)].width = width
	for r in range(FIRST_ROW, FIRST_ROW + ROWS):
		von = "$C$5+$C$6" if r == FIRST_ROW else f"I{r - 1}"
		akkum = f"M{r}" if r == FIRST_ROW else f"N{r - 1}+M{r}"
		for col, formula in (("D", f"M{r}"), ("G", "$A$5"), ("H", von), ("J", f"I{r}-H{r}"), ("K", f"240/G{r}"),
							 ("L", f"J{r}*K{r}"), ("M", f"L{r}*1000"), ("N", akkum)):
			ws[f"{col}{r}"].value = f'=IF(I{r}="","",{formula})'
			ws[f"{col}{r}"].font = Font() if col == "G" else grey
		for col in ("C", "P"):
			ws[f"{col}{r}"].alignment = Alignment(wrap_text=True, vertical="top")
	ws.cell(FIRST_ROW, 1, "pause")
	for i, note in enumerate(NOTES):
		ws.cell(FIRST_ROW + i, len(COLS) + 2, note).font = grey
	ws.freeze_panes = ws.cell(FIRST_ROW, 2)
	path.parent.mkdir(parents=True, exist_ok=True)
	wb.save(path)


#==================================================================
#=========== Tabelle lesen ========================================
#==================================================================

def number(v, what, allow_fraction=False):
	if isinstance(v, bool) or v is None or v == "":
		raise SongError(f"{what} fehlt")
	if isinstance(v, (int, float)):
		return float(v)
	s = str(v).strip().replace(",", ".")
	if allow_fraction and "/" in s:
		n, d = s.split("/")
		return float(n) / float(d)
	try:
		return float(s)
	except ValueError:
		raise SongError(f"{what}: '{v}' ist keine Zahl")


def text(v):
	return "" if v is None else " ".join(str(v).split())


def label(v):
	return text(v).lower().rstrip(":").strip() if isinstance(v, str) else ""


def empty(v):
	return v is None or (isinstance(v, str) and not v.strip())


def read_table(path):
	wb = openpyxl.load_workbook(path, data_only=True)
	ws = wb[SHEET] if SHEET in wb.sheetnames else wb.active
	header_row, cols = None, {}
	for row in ws.iter_rows():
		found = {}
		for c in row:
			found.setdefault(COL_LABELS.get(label(c.value)), c.column)
		if "name" in found and "bis" in found:
			header_row, cols = row[0].row, found
			break
	if header_row is None:
		raise SongError("Kopfzeile der Parts nicht gefunden - es braucht die Spalten 'Songpart' und 'bis takt'")

	head, used = {}, set()
	for row in ws.iter_rows(max_row=header_row - 1):
		for c in row:
			key = HEAD_LABELS.get(label(c.value))
			if not key:
				continue
			used.add(c.coordinate)
			for cand in (ws.cell(c.row, c.column + 1), ws.cell(c.row + 1, c.column)):	# Wert rechts daneben, sonst darunter
				if key not in head and cand.row < header_row and not empty(cand.value) and label(cand.value) not in HEAD_LABELS:
					head[key] = cand.value
					used.add(cand.coordinate)
	free = [text(ws.cell(r, 1).value) for r in range(1, header_row)
			if isinstance(ws.cell(r, 1).value, str) and not empty(ws.cell(r, 1).value) and ws.cell(r, 1).coordinate not in used]
	if "name" not in head and len(free) >= 2:	# ohne Beschriftung: erst Interpret, dann Titel
		head.setdefault("artist", free[0])
		head["name"] = free[1]
	elif "name" not in head and free:
		head["name"] = free[0]
	song = {"name": text(head.get("name")), "artist": text(head.get("artist")), "audio": text(head.get("audio")), "warnings": []}
	if not song["name"]:
		raise SongError("Titel fehlt (Spalte A über 'BPM': erst Interpret, darunter Titel)")
	song["id"] = int(number(head.get("id"), "Midi-StartNummer (Song-ID)"))
	song["bpm"] = number(head.get("bpm"), "BPM")
	start = number(head.get("starttakt"), "StartTakt")
	startbit = number(head.get("startbit", 0), "StartBit", allow_fraction=True)
	if start != int(start) or not 0 <= startbit < 1:
		raise SongError("StartTakt muss eine ganze Taktnummer sein, StartBit der Bruchteil dahinter (0 bis unter 1)")
	song["starttakt"], song["startbit"] = int(start), startbit

	parts, prev, seen = [], float(start), {}
	for r in range(header_row + 1, ws.max_row + 1):
		get = lambda k: ws.cell(r, cols[k]).value if k in cols else None  # noqa: E731
		name, bis = text(get("name")), get("bis")
		if not name and empty(bis):
			continue
		if not name:
			raise SongError(f"Zeile {r}: Partname fehlt")
		if empty(bis):
			if not parts and name == "pause":
				continue	# unausgefüllte Vorlage
			raise SongError(f"Zeile {r} '{name}': 'bis takt' fehlt (bei Formeln: Datei einmal in Excel speichern)")
		bis = number(bis, f"Zeile {r} '{name}': bis takt")
		if bis <= prev:
			raise SongError(f"Zeile {r} '{name}': bis takt {fmt(bis)} liegt nicht nach {fmt(prev)}")
		von, expect = get("von"), prev + (startbit if not parts else 0)
		if isinstance(von, (int, float)) and abs(von - expect) > 1e-6:
			song["warnings"].append(f"Zeile {r} '{name}': 'von takt' {fmt(von)} passt nicht zum Ende davor ({fmt(expect)}) - "
									f"gerechnet wird mit 'bis takt'")
		p = {"name": name, "row": r, "von": prev, "bis": bis, "bars": round(bis - prev, 4), "ms_rounded": get("ms_rounded"),
			 "idea": text(get("idea")), "description": text(get("description")), "chords": text(get("chords"))}
		if not empty(get("energy")):
			p["energy"] = int(number(get("energy"), f"Zeile {r} '{name}': Energie"))
			if not 0 <= p["energy"] <= 5:
				raise SongError(f"Zeile {r} '{name}': Energie {p['energy']} liegt nicht in 0..5")
		if not empty(get("bpm")):
			p["bpm"] = number(get("bpm"), f"Zeile {r} '{name}': BPM")
		parts.append(p)
		prev = bis
	if len(parts) > 1 and parts[-1]["name"].lower().startswith(END_NAMES):	# Schluss-Black: hängt der Generator an
		black = parts.pop()
		ms = black["ms_rounded"]
		song["end_black_ms"] = int(round(ms)) if isinstance(ms, (int, float)) and ms > 0 else END_BLACK_MS
		song["end_black_row"] = f"Zeile {black['row']} '{black['name']}'"
	if not parts:
		raise SongError("keine Parts eingetragen")
	for p in parts:		# eindeutige Namen
		key = p["name"].lower()
		seen[key] = seen.get(key, 0) + 1
		if seen[key] > 1:
			p["name"] = f"{p['name']} ({seen[key]})"
	song["parts"] = parts
	return song


#==================================================================
#=========== song.yaml schreiben ==================================
#==================================================================

def fmt(x):
	return f"{x:.4f}".rstrip("0").rstrip(".")


def ystr(s):
	return '"' + str(s).replace("\\", "\\\\").replace('"', '\\"') + '"'


def offset_lines(song):
	"""StartBit (Bruchteil des Takts) -> midi_offset als Notenwert; krumme Werte als midi_offset_ms."""
	sixteenths = song["startbit"] * 16
	if abs(sixteenths - round(sixteenths)) > 1e-6:
		bpm = song["parts"][0].get("bpm", song["bpm"])
		return [f"midi_offset_ms: {round(song['startbit'] * 240000.0 / bpm)}        # StartBit {fmt(song['startbit'])} ist kein Notenwert"]
	n, d = round(sixteenths), 16
	while n and n % 2 == 0:
		n, d = n // 2, d // 2
	return [f"midi_offset: {f'{n}/{d}' if n else 0}"]


def length_lines(bars, bpb=4):
	whole = int(bars + 1e-9)
	beats = round((bars - whole) * bpb, 3)
	return ([f"    bars: {whole}"] if whole or not beats else []) + ([f"    beats: {fmt(beats)}"] if beats else [])


def parse_chord(tok):
	"""'C#m7/G#' -> (1, 'min'); None, wenn kein Akkord."""
	m = re.match(r"\s*([A-Ha-h])([#b]?)(.*)$", tok)
	root = NOTE_IDX.get((m.group(1).upper() + m.group(2).upper()).replace("HB", "B")) if m else None
	if root is None:
		return None
	return (root, "min" if re.match(r"(m(?!aj)|min|-|dim)", m.group(3).split("/")[0]) else "maj")


def marker_lines(song, song_file):
	"""Bund-Marker: nie etwas Vorhandenes ändern (wie sheet2song.py)."""
	counts = {}
	for p in song["parts"]:
		for tok in re.split(r"[\s|,]+", p["chords"]):
			ch = parse_chord(tok) if tok else None
			if ch:
				counts[ch] = counts.get(ch, 0) + p["bars"]
	chords_text = ", ".join(NOTE_NAME[c[0]] + ("m" if c[1] == "min" else "") for c, _n in sorted(counts.items(), key=lambda kv: -kv[1]))
	if song["id"] in mk.handwritten_ids():
		return [f"# Bund-Marker: von Hand in src/markerLEDs.cpp gesetzt (case {song['id']}), werden dort gepflegt."] \
			+ ([f"# Akkorde im Song: {chords_text}"] if chords_text else []), "von Hand in markerLEDs.cpp - kein Vorschlag"
	if song_file.exists():
		m = re.search(r"^(?:#[^\n]*\n)*markers:.*", song_file.read_text(encoding="utf-8"), re.S | re.M)
		if m:
			return [m.group(0).rstrip("\n")], "aus der vorhandenen song.yaml übernommen (unverändert)"
	if not counts:
		return ["# Bund-Marker: noch keine. Akkorde in der Tabelle eintragen (Spalte Akkorde) oder hier markers: ergänzen."], \
			"keine (Spalte Akkorde fehlt oder ist leer)"
	names, notes = mk.propose(counts)
	return mk.yaml_block(names, notes, chords_text), "VORSCHLAG: " + ", ".join(f"{n} ({mk.FRET[n]}. Bund)" for n in names)


def find_mp3(song, song_dir):
	if song["audio"]:
		for c in (song_dir / "quelle" / song["audio"], song_dir / song["audio"]):
			if c.is_file():
				return c
		raise SongError(f"Audio '{song['audio']}' nicht gefunden (gesucht in quelle/ und im Song-Ordner)")
	found = [p for p in song_dir.rglob("*.mp3") if VERSIONS_DIR not in p.parts]
	return found[0] if len(found) == 1 else None


def render(song, song_dir, table, is_proposal):
	song_file = song_dir / SONG_FILE
	audio = find_mp3(song, song_dir)
	lines = []
	if is_proposal:
		lines += [f"# VORSCHLAG - {SONG_FILE} existiert schon und wurde NICHT angefasst. Nur zum Vergleichen,",
				  "# der Generator beachtet diese Datei nicht. Übernimm von Hand, was du brauchst.", ""]
	lines += [
		f"# Semantische Songbeschreibung, erzeugt von tools/struktur2song.py aus quelle/{table.name}",
		f"# (StartTakt {song['starttakt']}, StartBit {fmt(song['startbit'])}; Parts bis Takt {fmt(song['parts'][-1]['bis'])}).",
		"# Die Technik (Szenen/Farben) leitet Claude in show.yaml ab. Diese Datei hier gehört dir: kein Werkzeug",
		"# und kein Claude überschreibt sie. Pro Part kannst du ergänzen:",
		"#   energy: 0-5, description, idea, mood, instruments, solo, lyrics  -> Einschätzung, daraus wird die Show abgeleitet",
		"#   scene: SCENE_..., scheme: SCHEME_..., fx: \"prog...\"  -> feste Vorgabe, hat immer Vorrang vor show.yaml",
		"",
		f"id: {song['id']}", f"name: {ystr(song['name'])}", f"artist: {ystr(song['artist'])}", f"bpm: {fmt(song['bpm'])}",
		"beats_per_bar: 4",
	] + offset_lines(song)
	if song.get("end_black_ms", END_BLACK_MS) != END_BLACK_MS:
		lines.append(f"end_black_ms: {song['end_black_ms']}")
	if audio:
		lines += [f"audio: {audio.relative_to(song_dir).as_posix()}",
				  "# audio_beat1_ms: 0          # wo Takt 1 in der MP3 liegt - songanalyze.py schätzt es, wenn die Zeile fehlt"]
	lines += ["", "sections:"]
	for p in song["parts"]:
		lines.append(f"  - name: {ystr(p['name'])}")
		lines += length_lines(p["bars"])
		if "bpm" in p and abs(p["bpm"] - song["bpm"]) > 0.001:
			lines.append(f"    bpm: {fmt(p['bpm'])}")
		if "energy" in p:
			lines.append(f"    energy: {p['energy']}")
		for k in ("idea", "description", "chords"):
			if p[k]:
				lines.append(f"    {k}: {ystr(p[k])}")
		lines.append("")
	block, msg = marker_lines(song, song_file)
	return "\n".join(lines + block) + "\n", msg


def timeline_table(song):
	"""Kontrollausgabe wie im alten Kalkulator: Takte und Millisekunden ab dem Start-MIDI (so rechnet songgen.py)."""
	sixteenths = song["startbit"] * 16
	gen = {"bpm": song["bpm"], "end_black_ms": song.get("end_black_ms", END_BLACK_MS),
		   "sections": [dict({"name": p["name"], "bars": p["bars"]}, **({"bpm": p["bpm"]} if "bpm" in p else {})) for p in song["parts"]]}
	if abs(sixteenths - round(sixteenths)) > 1e-6:
		gen["midi_offset_ms"] = song["startbit"] * 240000.0 / song["parts"][0].get("bpm", song["bpm"])
	else:
		gen["midi_offset"] = f"{round(sixteenths)}/16" if round(sixteenths) else 0
	timeline, _end = build_timeline(gen)
	print(f"\n  {'Part':<32} {'von':>8} {'bis':>8} {'Takte':>7} {'Start ms':>9} {'Dauer ms':>9}")
	for p, t in zip(song["parts"], timeline):
		von = p["von"] + (song["startbit"] if p is song["parts"][0] else 0)
		print(f"  {p['name'][:32]:<32} {fmt(von):>8} {fmt(p['bis']):>8} {fmt(p['bis'] - von):>7} {t['start']:>9} {t['dur']:>9}")
	total = sum(p["bars"] for p in song["parts"])
	print(f"  {len(song['parts'])} Parts, {fmt(total)} Takte, {timeline[len(song['parts']) - 1]['start'] + timeline[len(song['parts']) - 1]['dur']} ms "
		  f"(+ {fmt(gen['end_black_ms'] / 1000)} s Schwarz am Ende)")
	if "end_black_row" in song:
		print(f"  {song['end_black_row']} ist der Schluss-Black: kein eigener Part, der Generator hängt ihn an")
	for w in song["warnings"]:
		print(f"  ! {w}")


def main():
	sys.stdout.reconfigure(encoding="utf-8")
	ap = argparse.ArgumentParser(description="song.yaml aus der Struktur-Tabelle (Excel) des Users erzeugen")
	ap.add_argument("song", help="Ordnername unter songs/ (mit --neu: genau so wird er angelegt, z. B. Vogue_v1)")
	ap.add_argument("--neu", action="store_true", help=f"leere Vorlage quelle/{TABLE_FILE} anlegen")
	args = ap.parse_args()
	try:
		if args.neu:
			path = SONGS_DIR / args.song / "quelle" / TABLE_FILE
			if path.exists():
				raise SongError(f"{path.relative_to(SONGS_DIR.parent)} gibt es schon - sie wird nicht überschrieben")
			if TEMPLATE.is_file():
				path.parent.mkdir(parents=True, exist_ok=True)
				shutil.copyfile(TEMPLATE, path)
			else:
				write_template(path)
			print(f"Vorlage angelegt: {path.relative_to(SONGS_DIR.parent)}\nAusfüllen, speichern, dann: struktur2song.py {args.song}")
			return
		song_dir = find_song_dir(args.song)
		table = next((p for p in (song_dir / "quelle" / TABLE_FILE, song_dir / TABLE_FILE) if p.is_file()), None)
		if table is None:
			raise SongError(f"songs/{song_dir.name}/quelle/{TABLE_FILE} fehlt - Vorlage anlegen mit --neu")
		song = read_table(table)
		is_proposal = (song_dir / SONG_FILE).exists()
		out = song_dir / (PROPOSAL_FILE if is_proposal else SONG_FILE)
		content, marker_msg = render(song, song_dir, table, is_proposal)
		print(f"#{song['id']} {song['name']} - {song['artist']}, {fmt(song['bpm'])} BPM, {offset_lines(song)[0].split('#')[0].strip()}")
		timeline_table(song)
		out.write_text(content, encoding="utf-8")
		print(f"\nBund-Marker: {marker_msg}")
		if is_proposal:
			print(f"{SONG_FILE} existiert schon und bleibt unangetastet -> Vorschlag: {out.relative_to(SONGS_DIR.parent)}")
		else:
			print(f"geschrieben: {out.relative_to(SONGS_DIR.parent)}")
	except SongError as e:
		raise SystemExit(f"FEHLER: {e}")


if __name__ == "__main__":
	main()
