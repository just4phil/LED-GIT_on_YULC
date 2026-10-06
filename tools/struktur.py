#!/usr/bin/env python3
"""
struktur.py - liest und schreibt die Struktur-Tabelle eines Songs (songs/<Song>/quelle/struktur.xlsx)

Die Tabelle ist die einzige Datei, die der User pro Song pflegt. Bibliothek für songgen.py, kein eigenes Kommando.
Gelesen wird das Blatt "Struktur" (sonst das erste Blatt, das die Kopfzeile hat):

    Zeile 1          Titel, daneben Interpret           (oder beschriftet: Titel / Interpret)
    Kopf             Midi-StartNummer (= Song-ID), BPM, StartBit - der Wert steht rechts neben der Beschriftung
    Kopfzeile        von takt | Songpart | Effektidee | Energie 0-5 | BPM pro Part
                     optional: Beschreibung | Akkorde | bisher (alter Code); andere Spalten werden ignoriert
    eine Zeile pro Part, die erste ist die Pause am Anfang (schwarz), die letzte heißt "Ende"

'von takt' ist die Taktnummer, an der der Part beginnt - ab 0 gezählt oder als Taktnummer im DAW, es zählt nur der
Abstand zur ersten Zeile. Halbe Takte als Kommazahl (21,75). Ein Part endet, wo der nächste beginnt; die Zeile
"Ende" liefert nur den Schlusstakt (steht in ihrer Effektidee eine Zeit wie "10 sek.", ist das die Länge des
Schluss-Blacks). StartBit: so weit hinter dem Anfang der ersten Zeile kommt das Start-MIDI, als Bruchteil des Takts
(0,125 = 1/8, 0,25 = 1/4, 0,375 = 3/8). 'BPM pro Part' leer oder gleich dem BPM im Kopf = Songtempo.
Spalten und Kopf werden an ihrer Beschriftung erkannt, nicht an der Position.
"""
import hashlib
import json
import re

import openpyxl
from openpyxl.styles import Alignment, Font, PatternFill
from openpyxl.utils import get_column_letter

TABLE_FILE = "struktur.xlsx"
SHEET = "Struktur"
BEATS_PER_BAR = 4
HEAD_LABELS = {"midi-startnummer": "id", "song-id": "id", "bpm": "bpm", "startbit": "startbit", "titel": "name",
			   "interpret": "artist"}
COL_LABELS = {"von takt": "von", "songpart": "name", "part": "name", "effektidee": "idea", "energie 0-5": "energy",
			  "energie": "energy", "bpm pro part": "bpm", "bpm": "bpm", "beschreibung": "description", "akkorde": "chords",
			  "bisher (alter code)": "old", "bisher": "old"}
INFO_KEYS = ("idea", "description", "chords", "old")		# nur für Claude (Gestaltung), der Generator rechnet damit nicht
END_NAMES = ("ende", "black", "fini", "finito", "back to default")	# so heißt die letzte Zeile (Schlusstakt)
END_BLACK_RE = re.compile(r"(\d+(?:[.,]\d+)?)\s*(ms|sek|sec|s)\b", re.I)
COLS = [("von takt", 9), ("Songpart", 30), ("Effektidee", 40), ("Energie 0-5", 12), ("BPM pro Part", 13)]
EXTRA_COLS = [("description", "Beschreibung", 50), ("chords", "Akkorde", 28), ("old", "bisher (alter Code)", 70)]
HEADER_ROW = 5


class TableError(Exception):
	pass


def fmt(x):
	return f"{x:.4f}".rstrip("0").rstrip(".")


def text(v):
	return "" if v is None else " ".join(str(v).split())


def multiline(v):
	return "" if v is None else "\n".join(l.strip() for l in str(v).splitlines() if l.strip())


def label(v):
	return text(v).lower().rstrip(":").strip() if isinstance(v, str) else ""


def empty(v):
	return v is None or (isinstance(v, str) and not v.strip())


def number(v, what, allow_fraction=False):
	if isinstance(v, bool) or empty(v):
		raise TableError(f"{what} fehlt")
	if isinstance(v, (int, float)):
		return float(v)
	s = str(v).strip().replace(",", ".")
	if allow_fraction and "/" in s:
		n, d = s.split("/")
		return float(n) / float(d)
	try:
		return float(s)
	except ValueError:
		raise TableError(f"{what}: '{v}' ist keine Zahl")


def neat(x):
	"""126.0 -> 126, 110.5 bleibt."""
	return int(round(x)) if abs(x - round(x)) < 1e-9 else x


def offset_fields(startbit, first_bpm):
	"""StartBit (Bruchteil des Takts) -> midi_offset als Notenwert; krumme Werte als midi_offset_ms."""
	sixteenths = startbit * 16
	if abs(sixteenths - round(sixteenths)) > 1e-6:
		return {"midi_offset_ms": round(startbit * 240000.0 / first_bpm)}
	n, d = round(sixteenths), 16
	while n and n % 2 == 0:
		n, d = n // 2, d // 2
	return {"midi_offset": f"{n}/{d}" if n else 0}


def startbit_of(song):
	"""Umkehrung von offset_fields (für write_table): midi_offset "3/8" -> 0.375."""
	v = song.get("midi_offset", 0)
	if "midi_offset_ms" in song:
		first = song["sections"][0]
		return float(song["midi_offset_ms"]) * float(first.get("bpm", song["bpm"])) / 240000.0
	if isinstance(v, str) and "/" in v:
		n, d = v.split("/")
		return float(n) / float(d)
	return float(v)


#==================================================================
#=========== lesen ================================================
#==================================================================

def find_sheet(wb):
	"""Blatt mit der Kopfzeile ('von takt' + 'Songpart'): bevorzugt das Blatt 'Struktur'. Liefert (Blatt, Zeile, Spalten)."""
	sheets = ([wb[SHEET]] if SHEET in wb.sheetnames else []) + [ws for ws in wb.worksheets if ws.title != SHEET]
	for ws in sheets:
		for row in ws.iter_rows(max_row=40):
			found = {}
			for c in row:
				key = COL_LABELS.get(label(c.value))
				if key:
					found.setdefault(key, c.column)
			if "name" in found and "von" in found:
				return ws, row[0].row, found
	raise TableError("Kopfzeile der Parts nicht gefunden - es braucht die Spalten 'von takt' und 'Songpart' "
					 "(das alte Kalkulator-Format mit 'bis takt' wird nicht mehr gelesen)")


def read_table(path):
	"""Tabelle -> Song-Dict, wie es songgen.build_timeline() erwartet (id, name, artist, bpm, midi_offset, sections ...)."""
	try:
		wb = openpyxl.load_workbook(path, data_only=True)
		wbf = openpyxl.load_workbook(path, data_only=False)
	except PermissionError:
		raise TableError(f"{path.name} lässt sich nicht öffnen (in Excel geöffnet und gesperrt?)")
	except Exception as e:
		raise TableError(f"{path.name} ist keine lesbare Excel-Datei ({e})")
	ws, header_row, cols = find_sheet(wb)
	wsf = wbf[ws.title]

	head, used, where = {}, set(), {}
	for row in ws.iter_rows(max_row=header_row - 1):
		for c in row:
			key = HEAD_LABELS.get(label(c.value))
			if not key:
				continue
			used.add(c.coordinate)
			cand = ws.cell(c.row, c.column + 1)		# Wert rechts daneben
			if key not in head and not empty(cand.value) and label(cand.value) not in HEAD_LABELS:
				head[key], where[key] = cand.value, cand.coordinate
				used.add(cand.coordinate)
	if "name" not in head:		# ohne Beschriftung: erste freie Textzeile = Titel, daneben Interpret
		for row in ws.iter_rows(max_row=header_row - 1):
			free = [c for c in row if isinstance(c.value, str) and not empty(c.value) and c.coordinate not in used]
			if free and not any(c.coordinate in used for c in row):
				head["name"] = free[0].value
				if len(free) > 1:
					head.setdefault("artist", free[1].value)
				break
	if empty(head.get("name")):
		raise TableError("Titel fehlt (Zeile 1: Titel, daneben Interpret)")
	bpm = number(head.get("bpm"), "BPM im Kopf")
	startbit = number(head.get("startbit", 0), "StartBit", allow_fraction=True)
	if not 0 <= startbit < 1:
		raise TableError("StartBit ist der Bruchteil des Takts, nach dem das Start-MIDI kommt (0 bis unter 1, z. B. 0,375)")
	song_id = number(head.get("id"), "Midi-StartNummer (Song-ID)")
	if song_id != int(song_id):
		raise TableError(f"Midi-StartNummer {song_id} ist keine ganze Zahl")

	def value(r, key, what):
		"""Zellwert; bei einer Formel ohne gespeichertes Ergebnis: Verweis aufs Kopf-BPM = None, sonst Fehler."""
		if key not in cols:
			return None
		v = ws.cell(r, cols[key]).value
		f = wsf.cell(r, cols[key]).value
		if v is None and isinstance(f, str) and f.startswith("="):
			if key == "bpm" and f[1:].replace("$", "").strip().upper() == where.get("bpm"):
				return None
			raise TableError(f"Zeile {r}: {what} ist eine Formel ohne gespeichertes Ergebnis - die Datei einmal in Excel speichern")
		return v

	rows = []
	for r in range(header_row + 1, ws.max_row + 1):
		name, von = text(value(r, "name", "Songpart")), value(r, "von", "von takt")
		if not name and empty(von):
			continue
		if not name:
			raise TableError(f"Zeile {r}: Songpart fehlt")
		von = number(von, f"Zeile {r} '{name}': von takt")
		if rows and von <= rows[-1]["von"]:
			raise TableError(f"Zeile {r} '{name}': von takt {fmt(von)} liegt nicht nach {fmt(rows[-1]['von'])} (Zeile {rows[-1]['row']})")
		p = {"row": r, "name": name, "von": von}
		for k in INFO_KEYS:
			v = multiline(value(r, k, k)) if k == "old" else text(value(r, k, k))
			if v:
				p[k] = v
		e = value(r, "energy", "Energie")
		if not empty(e):
			p["energy"] = int(number(e, f"Zeile {r} '{name}': Energie"))
			if not 0 <= p["energy"] <= 5:
				raise TableError(f"Zeile {r} '{name}': Energie {p['energy']} liegt nicht in 0..5")
		b = value(r, "bpm", "BPM pro Part")
		if not empty(b):
			b = number(b, f"Zeile {r} '{name}': BPM pro Part")
			if b <= 0:
				raise TableError(f"Zeile {r} '{name}': BPM pro Part {fmt(b)}")
			if abs(b - bpm) > 0.001:
				p["bpm"] = neat(b)
		rows.append(p)
	if len(rows) < 2:
		raise TableError("keine Parts eingetragen (mindestens die Pause am Anfang und die Zeile 'Ende')")
	end = rows.pop()
	if not end["name"].lower().startswith(END_NAMES):
		raise TableError(f"Zeile {end['row']} '{end['name']}': die letzte Zeile muss 'Ende' heißen - ihr 'von takt' ist der "
						 f"Schlusstakt des Songs")

	song = {"id": int(song_id), "name": text(head["name"]), "artist": text(head.get("artist")), "bpm": neat(bpm),
			"beats_per_bar": BEATS_PER_BAR}
	song.update(offset_fields(startbit, rows[0].get("bpm", bpm)))
	m = END_BLACK_RE.search(end.get("idea", ""))
	if m:
		n = float(m.group(1).replace(",", "."))
		song["end_black_ms"] = int(round(n if m.group(2).lower() == "ms" else n * 1000))
	seen, sections = {}, []
	for p, nxt in zip(rows, rows[1:] + [end]):
		bars = nxt["von"] - p["von"]
		whole = int(bars + 1e-9)
		beats = neat(round((bars - whole) * BEATS_PER_BAR, 4))
		key = p["name"]		# Groß/Klein zählt: "strobe" und "STROBE" sind zwei Namen
		seen[key] = seen.get(key, 0) + 1
		sec = {"name": p["name"] if seen[key] == 1 else f"{p['name']} ({seen[key]})"}
		if whole or not beats:
			sec["bars"] = whole
		if beats:
			sec["beats"] = beats
		sec.update({k: p[k] for k in ("bpm", "energy") + INFO_KEYS if k in p})
		sections.append(sec)
	names = [s["name"] for s in sections]
	dup = sorted({n for n in names if names.count(n) > 1})
	if dup:
		raise TableError(f"Partnamen nicht eindeutig: {', '.join(dup)} (gleiche Namen werden mit (2), (3) nummeriert - "
						 f"einen davon umbenennen)")
	song["sections"] = sections
	return song


def content_sha(song):
	"""Hash über den gelesenen Inhalt der Tabelle (Excel ändert die Datei-Bytes bei jedem Speichern)."""
	data = {k: v for k, v in song.items() if not k.startswith("_")}
	return hashlib.sha256(json.dumps(data, sort_keys=True, ensure_ascii=False).encode("utf-8")).hexdigest()


def table_sha(path):
	"""content_sha einer Datei; '-' wenn sie fehlt oder nicht lesbar ist."""
	if not path.is_file():
		return "-"
	try:
		return content_sha(read_table(path))
	except TableError:
		return "?"


#==================================================================
#=========== schreiben (Vorlage, Umstellung alter Songs) ==========
#==================================================================

def write_table(path, song, notes=()):
	"""Song-Dict (Form wie read_table) als Tabelle im Format des Users schreiben. Überschreibt nie eine vorhandene Datei."""
	if path.exists():
		raise TableError(f"{path} gibt es schon - sie wird nicht überschrieben")
	wb = openpyxl.Workbook()
	ws = wb.active
	ws.title = SHEET
	bold, grey = Font(bold=True), Font(color="808080", italic=True)
	head_fill = PatternFill("solid", fgColor="BDD7EE")
	ws["B1"], ws["C1"] = song["name"], song.get("artist", "")
	ws["B1"].font = bold
	for r, (lab, val) in enumerate((("Midi-StartNummer", song["id"]), ("BPM", song["bpm"]), ("StartBit", neat(startbit_of(song)))), 2):
		ws.cell(r, 2, lab)
		ws.cell(r, 3, val).alignment = Alignment(horizontal="left")
	extras = [(k, title, w) for k, title, w in EXTRA_COLS if any(s.get(k) for s in song["sections"])]
	columns = COLS + [(title, w) for _k, title, w in extras]
	for c, (title, width) in enumerate(columns, 1):
		cell = ws.cell(HEADER_ROW, c, title)
		cell.font, cell.fill = bold, head_fill
		ws.column_dimensions[get_column_letter(c)].width = width
	wrap = Alignment(wrap_text=True, vertical="top")
	top = Alignment(vertical="top")
	von, r = 0.0, HEADER_ROW
	for sec in song["sections"]:
		r += 1
		ws.cell(r, 1, neat(von)).number_format = "0.00"
		ws.cell(r, 2, sec["name"])
		ws.cell(r, 3, sec.get("idea"))
		ws.cell(r, 4, sec.get("energy"))
		ws.cell(r, 5, sec["bpm"] if "bpm" in sec else "=$C$3")
		for i, (k, _t, _w) in enumerate(extras):
			ws.cell(r, len(COLS) + 1 + i, sec.get(k))
		for c in range(1, len(columns) + 1):
			ws.cell(r, c).alignment = wrap if c == 3 or c > len(COLS) else top
		von += sec.get("bars", 0) + sec.get("beats", 0) / BEATS_PER_BAR
	r += 1
	ws.cell(r, 1, neat(von)).number_format = "0.00"
	ws.cell(r, 2, "Ende")
	ws.cell(r, 3, f"{fmt(song.get('end_black_ms', 10000) / 1000).replace('.', ',')} sek. BLACK")
	ws.cell(r, 4, 0)
	ws.cell(r, 5, "=$C$3")
	for i, note in enumerate(notes):
		ws.cell(1 + i, len(columns) + 2, note).font = grey
	ws.freeze_panes = ws.cell(HEADER_ROW + 1, 3)
	path.parent.mkdir(parents=True, exist_ok=True)
	wb.save(path)
