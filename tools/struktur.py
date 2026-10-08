#!/usr/bin/env python3
"""
struktur.py - liest und schreibt die Struktur-Tabelle eines Songs (songs/<Song>/quelle/struktur.xlsx)

Die Tabelle ist die einzige Datei, die der User pro Song pflegt. Bibliothek für songgen.py, kein eigenes Kommando.
Gelesen wird das Blatt "Struktur" (sonst das erste Blatt, das die Kopfzeile hat):

    Zeile 1          Titel, daneben Interpret           (oder beschriftet: Titel / Interpret)
    Kopf             Midi-StartNummer (= Song-ID), BPM, StartBit - der Wert steht rechts neben der Beschriftung
    Kopfzeile        von takt | Songpart | Effekt (füllt KI) | Änderungswunsch | Energie 0-5 | BPM pro Part
                     optional: Beschreibung | Akkorde | bisher (alter Code); andere Spalten werden ignoriert
    eine Zeile pro Part, die erste ist die Pause am Anfang (schwarz), die letzte heißt "Ende"
    dazwischen beliebig viele Zwischenzeilen: 'von takt' ohne Songpart (siehe "Viertel-Raster" unten)

Wer schreibt was (seit 06.10.2026, Wunsch des Users):
    Änderungswunsch  schreibt der USER: was er sich für den Part wünscht ("zu statisch", "Text einblenden"). Er löscht
                     seine Wünsche selbst wieder, wenn sie erledigt sind. Kein Werkzeug ändert diese Spalte.
    Effekt (füllt KI) schreibt das SKRIPT (songgen.py über write_effects() unten): der Effekt, der für den Part gerade
                     umgesetzt ist, mit Beschreibung. Wird bei jeder Generierung überschrieben - dort nichts eintragen.
                     Erkannt wird jede Überschrift, die mit "Effekt" beginnt (außer dem alten "Effektidee").
    alle anderen     gehören dem User (Takte, Partnamen, Energie, BPM ...), das Skript rührt sie nicht an.
Altes Format (Tabellen vor dem 06.10.2026): eine Spalte "Effektidee" mit den Wünschen des Users, keine Spalte "Effekt".
Es wird weiter gelesen; write_effects() stellt eine solche Tabelle beim ersten Schreiben um (die Wünsche wandern in die
neue Spalte "Änderungswunsch" rechts daneben).

'von takt' ist die Taktnummer, an der der Part beginnt - ab 0 gezählt oder als Taktnummer im DAW, es zählt nur der
Abstand zur ersten Zeile. Halbe Takte als Kommazahl (21,75). Ein Part endet, wo der nächste beginnt; die Zeile
"Ende" liefert nur den Schlusstakt (steht in ihrem Änderungswunsch oder ihrem Effekt eine Zeit wie "10 sek.", ist das
die Länge des Schluss-Blacks). StartBit: so weit hinter dem Anfang der ersten Zeile kommt das Start-MIDI, als Bruchteil des Takts
(0,125 = 1/8, 0,25 = 1/4, 0,375 = 3/8). 'BPM pro Part' leer oder gleich dem BPM im Kopf = Songtempo.
Spalten und Kopf werden an ihrer Beschriftung erkannt, nicht an der Position.

Viertel-Raster (seit 07.10.2026, Wunsch des Users): damit er seine Wünsche auf den Vierteltakt genau angeben kann, darf
die Tabelle zwischen den Parts ZWISCHENZEILEN haben - Zeilen mit 'von takt', aber ohne Songpart (z. B. eine Zeile je
Vierteltakt: 0 / 0,25 / 0,5 / 0,75 / 1 ...). Eine Zwischenzeile gehört zum Part darüber:
    ganz leer                 wird überlesen: dort läuft das Programm des Parts einfach weiter. Sie zählt auch nicht
                              zum Fingerabdruck - eine nur gerasterte Tabelle gilt als unverändert.
    mit Änderungswunsch       ein Wunsch genau an dieser Stelle des Parts ("hier Blinder")
    mit Energie               wird mitgelesen und geprüft (0..5), hat aber noch keine Wirkung
    mit Songpart              ist keine Zwischenzeile, sondern ein neuer Part, der hier beginnt
In "Effekt (füllt KI)" einer Zwischenzeile schreibt das Skript, was an dieser Stelle umgesetzt ist (write_effects).
Ein Tempowechsel braucht einen Part: 'BPM pro Part' einer Zwischenzeile bleibt leer (oder gleich dem Tempo ihres Parts).
Beide Formen gelten: das volle Raster genauso wie eine kompakte Tabelle mit einzelnen Zwischenzeilen nur dort, wo ein
Wunsch steht. write_raster() legt von einer kompakten Tabelle eine gerasterte Kopie an (songgen.py <Song> --raster),
die Zwischenzeilen sind darin je Part als Excel-Gliederung zusammengefasst und lassen sich ein- und ausklappen.
"""
import copy
import hashlib
import json
import os
import re

import openpyxl
from openpyxl.styles import Alignment, Font, PatternFill
from openpyxl.utils import column_index_from_string, get_column_letter

# --- Feste Einstellungen ---
TABLE_FILE = "struktur.xlsx"	# Dateiname der Tabelle in songs/<Song>/quelle/
SHEET = "Struktur"				# Name des Tabellenblatts
BEATS_PER_BAR = 4				# Schläge je Takt (alle Songs stehen im 4/4-Takt)
# Wörterbücher "Beschriftung in der Tabelle (klein geschrieben) -> interner Name". So werden Kopf-Felder und
# Spalten an ihrem Text erkannt, egal wo sie stehen; mehrere Schreibweisen führen zum selben internen Namen.
HEAD_LABELS = {"midi-startnummer": "id", "song-id": "id", "bpm": "bpm", "startbit": "startbit", "titel": "name",
			   "interpret": "artist"}
# "idea" = der Wunsch des Users: Spalte "Änderungswunsch" (neu) oder "Effektidee" (altes Format).
# "effect" = die Spalte "Effekt", die das Skript schreibt (write_effects).
COL_LABELS = {"von takt": "von", "songpart": "name", "part": "name", "effektidee": "idea", "änderungswunsch": "idea",
			  "änderungswünsche": "idea", "aenderungswunsch": "idea", "wunsch": "idea", "effekt": "effect",
			  "energie 0-5": "energy",
			  "energie": "energy", "bpm pro part": "bpm", "bpm": "bpm", "beschreibung": "description", "akkorde": "chords",
			  "bisher (alter code)": "old", "bisher": "old"}
INFO_KEYS = ("idea", "description", "chords", "old")		# nur für Claude (Gestaltung), der Generator rechnet damit nicht
OLD_IDEA_LABEL = "effektidee"			# an dieser Überschrift erkennt write_effects() das alte Format
EFFECT_TITLE, WISH_TITLE = "Effekt (füllt KI)", "Änderungswunsch"	# Überschriften der beiden Spalten (so hat der User sie genannt)
EFFECT_WIDTH, WISH_WIDTH = 70, 40		# Spaltenbreiten, die write_effects() beim Umstellen setzt
TMP_FILE = "struktur.tmp.xlsx"			# write_effects() schreibt erst hierhin, prüft und ersetzt dann die Tabelle
RASTER_FILE = "struktur-raster.xlsx"	# die gerasterte Kopie von write_raster() - der User tauscht sie selbst gegen seine Tabelle
RASTER_STEP = 0.25						# Abstand der Rasterzeilen in Takten (0,25 = ein Vierteltakt = ein Schlag im 4/4-Takt)
END_NAMES = ("ende", "black", "fini", "finito", "back to default")	# so heißt die letzte Zeile (Schlusstakt)
# Suchmuster ("regulärer Ausdruck") für eine Zeitangabe wie "10 sek." oder "2,5 s" oder "800 ms" in der
# Ende-Zeile (Änderungswunsch oder Effekt): eine Zahl (mit Punkt oder Komma), dahinter die Einheit.
END_BLACK_RE = re.compile(r"(\d+(?:[.,]\d+)?)\s*(ms|sek|sec|s)\b", re.I)
# Spalten, die write_table() anlegt: (Überschrift, Breite)
COLS = [("von takt", 9), ("Songpart", 30), (EFFECT_TITLE, EFFECT_WIDTH), (WISH_TITLE, WISH_WIDTH), ("Energie 0-5", 12),
		("BPM pro Part", 13)]
EXTRA_COLS = [("description", "Beschreibung", 50), ("chords", "Akkorde", 28), ("old", "bisher (alter Code)", 70)]
HEADER_ROW = 5		# in dieser Zeile schreibt write_table() die Spaltenüberschriften


# Eigene Fehlerart für alles, was an einer Tabelle nicht stimmt. songgen.py fängt sie ab und zeigt dem User
# die Meldung im Klartext statt eines Programmabsturzes.
class TableError(Exception):
	pass


# --- Kleine Helfer zum Aufbereiten von Zellinhalten ---
# Zahl ohne überflüssige Nullen als Text: 21.7500 -> "21.75", 8.0000 -> "8"
def fmt(x):
	return f"{x:.4f}".rstrip("0").rstrip(".")


# Zellinhalt als Text in einer Zeile: leere Zelle -> "", mehrfache Leerzeichen und Umbrüche -> ein Leerzeichen
def text(v):
	return "" if v is None else " ".join(str(v).split())


# wie text(), aber Zeilenumbrüche bleiben erhalten (für die Spalte "bisher (alter Code)")
def multiline(v):
	return "" if v is None else "\n".join(l.strip() for l in str(v).splitlines() if l.strip())


# Beschriftung zum Vergleichen: klein geschrieben, ohne Doppelpunkt am Ende ("BPM:" -> "bpm")
def label(v):
	return text(v).lower().rstrip(":").strip() if isinstance(v, str) else ""


# Ist die Zelle leer (gar kein Inhalt oder nur Leerzeichen)?
def empty(v):
	return v is None or (isinstance(v, str) and not v.strip())


# Zellinhalt als Zahl. Akzeptiert Zahlen, Text mit Komma oder Punkt ("21,75") und - wenn erlaubt - Brüche ("3/8").
# what = Beschreibung der Stelle für die Fehlermeldung.
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

# Überschrift einer Spalte -> interner Name (None = unbekannte Spalte, wird ignoriert). Die Spalte des Skripts heißt
# beim User "Effekt (füllt KI)": jede Überschrift, die mit "Effekt" beginnt, gilt - nur "Effektidee" ist das alte
# Format mit seinen Wünschen.
def col_key(v):
	lab = label(v)
	if lab in COL_LABELS:
		return COL_LABELS[lab]
	return "effect" if lab.startswith("effekt") else None


# Durchsucht die ersten 40 Zeilen jedes Blatts nach der Zeile mit den Spaltenüberschriften.
def find_sheet(wb):
	"""Blatt mit der Kopfzeile ('von takt' + 'Songpart'): bevorzugt das Blatt 'Struktur'. Liefert (Blatt, Zeile, Spalten)."""
	sheets = ([wb[SHEET]] if SHEET in wb.sheetnames else []) + [ws for ws in wb.worksheets if ws.title != SHEET]
	for ws in sheets:
		for row in ws.iter_rows(max_row=40):
			found = {}
			for c in row:
				key = col_key(c.value)
				if key:
					found.setdefault(key, c.column)
			if "name" in found and "von" in found:
				return ws, row[0].row, found
	raise TableError("Kopfzeile der Parts nicht gefunden - es braucht die Spalten 'von takt' und 'Songpart' "
					 "(das alte Kalkulator-Format mit 'bis takt' wird nicht mehr gelesen)")


# Die Hauptfunktion: liest die Tabelle und liefert alles als "Dictionary" (Nachschlagetabelle Name -> Wert).
# Ablauf: 1. Blatt und Kopfzeile finden, 2. Kopf-Felder lesen (Titel, Song-ID, BPM, StartBit), 3. die Zeilen
# der Parts lesen und prüfen, 4. aus den "von takt"-Werten die Länge jedes Parts berechnen.
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

	def energy_of(r, what):
		"""Energie einer Zeile als ganze Zahl 0..5; None, wenn die Zelle leer ist."""
		e = value(r, "energy", "Energie")
		if empty(e):
			return None
		e = int(number(e, f"{what}: Energie"))
		if not 0 <= e <= 5:
			raise TableError(f"{what}: Energie {e} liegt nicht in 0..5")
		return e

	rows = []
	last = None		# (von takt, Zeilennummer) der Zeile davor - Parts und Zwischenzeilen müssen gemeinsam aufsteigen
	for r in range(header_row + 1, ws.max_row + 1):
		name, von = text(value(r, "name", "Songpart")), value(r, "von", "von takt")
		if not name and empty(von):
			continue
		what = f"Zeile {r} '{name}'" if name else f"Zeile {r}"
		von = number(von, f"{what}: von takt")
		if last and von <= last[0]:
			raise TableError(f"{what}: von takt {fmt(von)} liegt nicht nach {fmt(last[0])} (Zeile {last[1]})")
		last = (von, r)
		if not name:
			# Zwischenzeile (Viertel-Raster): gehört zum Part darüber. Gemerkt wird jede, auch die leere - in ihre
			# Spalte "Effekt" kann das Skript später schreiben. Ein Wunsch ("idea") oder eine Energie macht sie "gefüllt".
			if not rows:
				raise TableError(f"Zeile {r}: 'von takt' ohne Songpart vor dem ersten Part - die erste Zeile braucht einen Namen "
								 f"(die Pause am Anfang)")
			sub = {"row": r, "von": von}
			idea = text(value(r, "idea", "Änderungswunsch"))
			if idea:
				sub["idea"] = idea
			e = energy_of(r, what)
			if e is not None:
				sub["energy"] = e
			b = value(r, "bpm", "BPM pro Part")
			if not empty(b) and abs(number(b, f"{what}: BPM pro Part") - rows[-1].get("bpm", bpm)) > 0.001:
				raise TableError(f"{what}: BPM pro Part steht in einer Zeile ohne Songpart - ein Tempowechsel braucht einen "
								 f"eigenen Part (Namen in die Spalte Songpart eintragen)")
			rows[-1]["subs"].append(sub)
			continue
		p = {"row": r, "name": name, "von": von, "subs": []}
		for k in INFO_KEYS:
			v = multiline(value(r, k, k)) if k == "old" else text(value(r, k, k))
			if v:
				p[k] = v
		e = energy_of(r, what)
		if e is not None:
			p["energy"] = e
		b = value(r, "bpm", "BPM pro Part")
		if not empty(b):
			b = number(b, f"Zeile {r} '{name}': BPM pro Part")
			if b <= 0:
				raise TableError(f"Zeile {r} '{name}': BPM pro Part {fmt(b)}")
			if abs(b - bpm) > 0.001:
				p["bpm"] = neat(b)
		eff = text(value(r, "effect", "Effekt"))	# nur für die Ende-Zeile gebraucht (Länge des Schluss-Blacks)
		if eff:
			p["_effect"] = eff
		rows.append(p)
	if len(rows) < 2:
		raise TableError("keine Parts eingetragen (mindestens die Pause am Anfang und die Zeile 'Ende')")
	end = rows.pop()
	if not end["name"].lower().startswith(END_NAMES):
		raise TableError(f"Zeile {end['row']} '{end['name']}': die letzte Zeile mit Songpart muss 'Ende' heißen - ihr 'von takt' "
						 f"ist der Schlusstakt des Songs")
	late = [s for s in end["subs"] if "idea" in s or "energy" in s]	# leere Rasterzeilen hinter "Ende" stören nicht
	if late:
		raise TableError(f"Zeile {late[0]['row']}: Eintrag hinter der Zeile 'Ende' (Zeile {end['row']}) - dort ist der Song zu Ende")

	song = {"id": int(song_id), "name": text(head["name"]), "artist": text(head.get("artist")), "bpm": neat(bpm),
			"beats_per_bar": BEATS_PER_BAR}
	song.update(offset_fields(startbit, rows[0].get("bpm", bpm)))
	# Länge des Schluss-Blacks: der Wunsch des Users geht vor, sonst gilt, was in "Effekt" steht
	m = END_BLACK_RE.search(end.get("idea", "")) or END_BLACK_RE.search(end.get("_effect", ""))
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
		# Wünsche aus den Zwischenzeilen des Parts: "von" = Taktnummer wie in der Tabelle, "at" = Beats ab Part-Beginn
		wishes = [{"von": neat(s["von"]), "at": neat(round((s["von"] - p["von"]) * BEATS_PER_BAR, 4)),
				   **{k: s[k] for k in ("idea", "energy") if k in s}} for s in p["subs"] if "idea" in s or "energy" in s]
		if wishes:
			sec["wishes"] = wishes
		sections.append(sec)
	names = [s["name"] for s in sections]
	dup = sorted({n for n in names if names.count(n) > 1})
	if dup:
		raise TableError(f"Partnamen nicht eindeutig: {', '.join(dup)} (gleiche Namen werden mit (2), (3) nummeriert - "
						 f"einen davon umbenennen)")
	song["sections"] = sections
	# Wo die Parts in der Datei stehen (für write_effects). Schlüssel mit "_" zählen nicht zum Fingerabdruck.
	#   rows / end_row   Zeilennummer je Part und der Zeile "Ende"
	#   von / end_von    'von takt' je Part und der Zeile "Ende" (für Angaben in Taktnummern der Tabelle, z. B. blinder bar:)
	#   subs             je Part seine Zwischenzeilen als {von takt: Zeilennummer}, auch die leeren
	song["_table"] = {"rows": [p["row"] for p in rows], "end_row": end["row"], "von": [p["von"] for p in rows],
					  "end_von": end["von"], "subs": [{s["von"]: s["row"] for s in p["subs"]} for p in rows]}
	return song


# Ein "Hash" ist ein Fingerabdruck: gleiche Inhalte ergeben dieselbe Zeichenfolge, die kleinste Änderung eine
# völlig andere. songgen.py merkt sich ihn je Version und erkennt daran, ob die Tabelle seitdem geändert wurde.
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

# Legt eine NEUE Tabelle an (so entsteht die Vorlage songs/struktur-vorlage.xlsx).
# Eine vorhandene Tabelle wird nie überschrieben - sie gehört dem User.
#   raster   True = zwischen den Parts leere Zwischenzeilen im Viertel-Raster anlegen, eingeklappt (raster_sheet)
# Geschrieben werden die Parts; Wünsche aus Zwischenzeilen (sec["wishes"]) gehören nicht dazu. 'BPM pro Part' bleibt
# leer, wenn der Part im Songtempo läuft (leer = Songtempo).
def write_table(path, song, notes=(), raster=False):
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
		ws.cell(r, 4, sec.get("idea"))		# Spalte 3 ("Effekt") bleibt leer: sie füllt songgen.py bei der Generierung
		ws.cell(r, 5, sec.get("energy"))
		ws.cell(r, 6, sec.get("bpm"))
		for i, (k, _t, _w) in enumerate(extras):
			ws.cell(r, len(COLS) + 1 + i, sec.get(k))
		for c in range(1, len(columns) + 1):
			ws.cell(r, c).alignment = wrap if c in (3, 4) or c > len(COLS) else top
		von += sec.get("bars", 0) + sec.get("beats", 0) / BEATS_PER_BAR
	r += 1
	ws.cell(r, 1, neat(von)).number_format = "0.00"
	ws.cell(r, 2, "Ende")
	ws.cell(r, 3, f"{fmt(song.get('end_black_ms', 10000) / 1000).replace('.', ',')} sek. BLACK")
	ws.cell(r, 5, 0)
	for i, note in enumerate(notes):
		ws.cell(1 + i, len(columns) + 2, note).font = grey
	ws.freeze_panes = ws.cell(HEADER_ROW + 1, 3)
	if raster:
		raster_sheet(ws, [(HEADER_ROW + 1 + i, ws.cell(HEADER_ROW + 1 + i, 1).value) for i in range(len(song["sections"]) + 1)], 1)
	path.parent.mkdir(parents=True, exist_ok=True)
	wb.save(path)


#==================================================================
#=========== Viertel-Raster anlegen ===============================
#==================================================================

# Ist das eine ganze Taktnummer (20 oder 20.0, nicht 20.25)? Leere Zellen und Texte: nein.
def _whole_bar(von):
	return isinstance(von, (int, float)) and not isinstance(von, bool) and abs(von - round(von)) < 1e-6


# Fügt in ein Tabellenblatt die fehlenden Zwischenzeilen des Viertel-Rasters ein.
#   named     die Zeilen mit Songpart, von oben nach unten: [(Zeilennummer, von takt), ...] - die letzte ist "Ende"
#   von_col   Nummer der Spalte 'von takt'
#   existing  schon vorhandene Zwischenzeilen: [(Zeilennummer, von takt), ...] (bleiben, wie sie sind)
# Zwischen zwei benachbarten Zeilen kommen die Rasterwerte, die dort noch fehlen (0,25 / 0,5 / 0,75 ...). Gearbeitet
# wird von unten nach oben: so bleiben die Zeilennummern weiter oben gültig, während unten eingefügt wird.
# Jede neue Zeile bekommt das Aussehen der Zeile über der Lücke (Schrift, Rahmen, Zahlenformat).
# Die Zwischenzeilen werden zu einer Excel-Gliederung in drei Ebenen zusammengefasst (Wunsch des Users, 08.10.2026):
#   Ebene 1   nur die Part-Zeilen
#   Ebene 2   dazu die vollen Takte (Zwischenzeilen mit ganzer Taktnummer: 1 / 2 / 3 ...)
#   Ebene 3   dazu die Viertel (0,25 / 0,5 / 0,75)
# Die Knöpfe "1", "2", "3" oben links schalten die ganze Tabelle auf eine Ebene; am linken Rand steht an jeder
# Part-Zeile ein Plus für ihre Takte und an jeder Takt-Zeile ein Plus für ihre Viertel. Angelegt wird alles eingeklappt.
# openpyxl verschiebt beim Einfügen nur die Zellen: von Hand eingestellte Zeilenhöhen werden hier mitgenommen;
# Formeln und verbundene Zellen passt es nicht an - der Aufrufer prüft vorher, dass es unterhalb keine gibt.
# Rückgabe: Zahl der eingefügten Zeilen.
def raster_sheet(ws, named, von_col, existing=()):
	lines = sorted(list(named) + list(existing))		# alle Zeilen mit 'von takt', von oben nach unten
	names = {r for r, _von in named}
	heights = {r: d.height for r, d in ws.row_dimensions.items() if d.height}
	inserts = []		# (vor dieser Zeile, so viele Zeilen) - für die neuen Zeilennummern der alten Zeilen
	for (row_a, von_a), (row_b, von_b) in reversed(list(zip(lines, lines[1:]))):
		k = int(von_a / RASTER_STEP + 1e-6) + 1		# Nummer des ersten Rasterpunkts hinter von_a
		grid = []
		while k * RASTER_STEP < von_b - 1e-6:
			grid.append(k * RASTER_STEP)
			k += 1
		if not grid:
			continue
		ws.insert_rows(row_b, len(grid))
		inserts.append((row_b, len(grid)))
		for i, von in enumerate(grid):
			for col in range(1, ws.max_column + 1):
				ws.cell(row_b + i, col)._style = copy.copy(ws.cell(row_a, col)._style)
			ws.cell(row_b + i, von_col, neat(von))

	def moved(row):		# wo eine Zeile von vorher jetzt steht
		return row + sum(n for at, n in inserts if at <= row)
	for r in list(ws.row_dimensions):
		ws.row_dimensions[r].height = None
	for r, h in heights.items():
		ws.row_dimensions[moved(r)].height = h
	first, last = moved(lines[0][0]), moved(lines[-1][0])
	parts = {moved(r) for r in names}
	ws.sheet_properties.outlinePr.summaryBelow = False		# das Plus steht an der Part-Zeile ÜBER ihren Zwischenzeilen
	bounds = sorted(parts)
	for start, stop in zip(bounds, bounds[1:]):		# je Part: seine Zeile (start) und die Zwischenzeilen bis vor den nächsten Part
		# Steht in einer Zwischenzeile schon etwas (ein Wunsch, eine Energie, ein Effekt-Text), bleibt der Part aufgeklappt -
		# sonst wäre der Eintrag nach dem Rastern versteckt
		filled = any(not empty(ws.cell(r, col).value) for r in range(start + 1, stop)
					 for col in range(1, ws.max_column + 1) if col != von_col)
		ws.row_dimensions[start].collapsed = stop > start + 1 and not filled
		for r in range(start + 1, stop):
			von = ws.cell(r, von_col).value
			whole = _whole_bar(von)		# voller Takt oder Viertel?
			d = ws.row_dimensions[r]
			d.outlineLevel = 1 if whole else 2		# Excel zählt ab 0: 1 = zweite Ebene (Takte), 2 = dritte Ebene (Viertel)
			d.hidden = not filled
			# eine Takt-Zeile ist selbst die Kopfzeile ihrer Viertel: eingeklappt, wenn gleich darunter ein Viertel folgt
			d.collapsed = whole and not filled and r + 1 < stop and not _whole_bar(ws.cell(r + 1, von_col).value)
	return sum(n for _at, n in inserts)


# Legt von der Tabelle eines Songs eine gerasterte KOPIE an (songgen.py <Song> --raster): dieselbe Tabelle, zwischen
# den Parts aber eine Zeile je Vierteltakt für die Wünsche des Users. Die Tabelle selbst (src) wird nur gelesen -
# der User prüft die Kopie und tauscht sie selbst gegen seine struktur.xlsx.
# Abgelehnt wird eine Tabelle mit eigenen Formeln oder verbundenen Zellen im Bereich der Parts (das Einfügen von
# Zeilen würde sie zerstören, siehe raster_sheet) - dort fügt der User die Zeilen in Excel selbst ein.
# Kontrolle wie bei write_effects(): die Kopie wird wieder gelesen und muss denselben Fingerabdruck haben.
# Rückgabe: Zahl der eingefügten Zeilen.
def write_raster(src, dst):
	if dst.exists():
		raise TableError(f"{dst.name} gibt es schon - sie wird nicht überschrieben (erst löschen oder umbenennen)")
	before = read_table(src)
	wb = openpyxl.load_workbook(src)		# ohne data_only: Formeln bleiben Formeln
	ws, header_row, cols = find_sheet(wb)
	hint = "die Zwischenzeilen dort bitte in Excel selbst einfügen (Excel zieht Formeln richtig mit)"
	for other in wb.worksheets:
		for row in other.iter_rows():
			for c in row:
				if not (isinstance(c.value, str) and c.value.startswith("=")):
					continue
				if other is not ws:
					if ws.title.lower() in c.value.lower():
						raise TableError(f"das Blatt '{other.title}' rechnet mit dem Blatt '{ws.title}' (z. B. {c.coordinate}) - {hint}")
					continue
				m = SIMPLE_REF_RE.fullmatch(c.value.strip())		# erlaubt: schlichter Verweis auf den Kopf, z. B. =$C$3
				if not m or int(m.group(2)) > header_row:
					raise TableError(f"die Tabelle enthält eigene Formeln (z. B. {c.coordinate}) - {hint}")
	if any(rng.max_row > header_row for rng in ws.merged_cells.ranges):
		raise TableError(f"die Tabelle hat verbundene Zellen unter der Kopfzeile - {hint}")
	t = before["_table"]
	named = list(zip(t["rows"] + [t["end_row"]], t["von"] + [t["end_von"]]))
	existing = [(r, von) for part in t["subs"] for von, r in part.items()]
	count = raster_sheet(ws, named, cols["von"], existing)
	try:
		wb.save(dst)
		if content_sha(read_table(dst)) != content_sha(before):
			raise TableError("Kontrolle nach dem Schreiben: die Kopie hätte nicht denselben Inhalt wie die Tabelle - nichts angelegt")
	except TableError:
		if dst.exists():
			dst.unlink()
		raise
	except PermissionError:
		raise TableError(f"{dst.name} lässt sich nicht schreiben (in Excel geöffnet?)")
	return count


#==================================================================
#=========== Spalte "Effekt" schreiben ============================
#==================================================================

# Suchmuster für die einzige Art Formel, die das Einfügen einer Spalte übersteht: ein schlichter Verweis auf eine
# Zelle, z. B. "=$C$3" (BPM pro Part = BPM im Kopf). Gruppe 1 = Spaltenbuchstaben, Gruppe 2 = Zeilennummer.
SIMPLE_REF_RE = re.compile(r"=\$?([A-Z]{1,3})\$?(\d+)")


# Fügt links von Spalte idx eine leere Spalte ein. openpyxl verschiebt dabei nur die Zellen - die Spaltenbreiten
# werden hier von Hand mitgenommen. Formeln passt openpyxl NICHT an (Excel würde es tun): eine Formel, die auf eine
# Spalte rechts der neuen zeigt, rechnete danach mit der falschen Zelle. Deshalb vorher prüfen: erlaubt sind nur
# schlichte Verweise auf Zellen links der neuen Spalte (=$C$3). Hat der User eigene Rechnungen im Blatt (z. B. die
# ms-Spalten in der Tabelle von "All The Things She Said"), wird nichts eingefügt - er legt die Spalten dann selbst
# in Excel an, das seine Formeln richtig mitzieht.
def insert_column(ws, idx, width):
	for other in ws.parent.worksheets:		# rechnet ein anderes Blatt mit Zellen dieses Blatts, verrutschte auch das
		if other is not ws:
			for row in other.iter_rows():
				for c in row:
					if isinstance(c.value, str) and c.value.startswith("=") and ws.title.lower() in c.value.lower():
						raise TableError(
							f"das Blatt '{other.title}' rechnet mit dem Blatt '{ws.title}' (z. B. {c.coordinate}) - das Skript fügt dort "
							f"keine Spalte ein. Bitte in Excel selbst anlegen: die Spalte 'Effektidee' in '{WISH_TITLE}' umbenennen "
							f"und links davon eine leere Spalte '{EFFECT_TITLE}' einfügen")
	for row in ws.iter_rows():
		for c in row:
			if isinstance(c.value, str) and c.value.startswith("="):
				m = SIMPLE_REF_RE.fullmatch(c.value.strip())
				if not m or column_index_from_string(m.group(1)) >= idx:
					raise TableError(
						f"die Tabelle hat noch das alte Format und enthält eigene Formeln (z. B. {c.coordinate}) - das Skript fügt dort "
						f"keine Spalte ein. Bitte in Excel selbst anlegen: die Spalte 'Effektidee' in '{WISH_TITLE}' umbenennen und "
						f"links davon eine leere Spalte '{EFFECT_TITLE}' einfügen")
	old = {column_index_from_string(l): d.width for l, d in ws.column_dimensions.items() if d.width}
	ws.insert_cols(idx)
	for col in sorted(old, reverse=True):		# von rechts nach links, damit keine Breite überschrieben wird, bevor sie gelesen ist
		if col >= idx:
			ws.column_dimensions[get_column_letter(col + 1)].width = old[col]
	ws.column_dimensions[get_column_letter(idx)].width = width
	if idx > 1:
		for row in ws.iter_rows(min_col=idx, max_col=idx):		# Aussehen (Schrift, Farbe, Rahmen) vom linken Nachbarn übernehmen
			for c in row:
				c._style = copy.copy(ws.cell(c.row, idx - 1)._style)


# Sorgt dafür, dass es die Spalten "Effekt" und "Änderungswunsch" gibt, und liefert die Nummer der Effekt-Spalte.
#   altes Format ("Effektidee")   -> die Spalte heißt jetzt "Effekt", rechts daneben entsteht "Änderungswunsch" und
#                                    bekommt die bisherigen Texte des Users (die Ende-Zeile behält ihre Zeitangabe)
#   nur "Änderungswunsch"         -> "Effekt" entsteht links davon
#   keins von beiden              -> beide entstehen rechts neben "Songpart"
# Rückgabe: (Spaltennummer von "Effekt", Text was umgestellt wurde oder "")
def ensure_effect_column(ws, header_row, cols, part_rows):
	if "effect" in cols:
		return cols["effect"], ""
	wrap = Alignment(wrap_text=True, vertical="top")
	if "idea" in cols and label(ws.cell(header_row, cols["idea"]).value) == OLD_IDEA_LABEL:
		c = cols["idea"]
		insert_column(ws, c + 1, WISH_WIDTH)
		ws.cell(header_row, c, EFFECT_TITLE)
		ws.cell(header_row, c + 1, WISH_TITLE)
		for r in part_rows:		# die Wünsche des Users wandern nach rechts, "Effekt" wird frei für das Skript
			ws.cell(r, c + 1).value = ws.cell(r, c).value
			ws.cell(r, c + 1).alignment = wrap
			ws.cell(r, c).value = None
		ws.column_dimensions[get_column_letter(c)].width = EFFECT_WIDTH
		return c, (f"Tabelle auf das neue Format umgestellt: Spalte '{EFFECT_TITLE}' (schreibt das Skript) und neue Spalte "
				   f"'{WISH_TITLE}' rechts daneben - deine bisherigen Texte aus 'Effektidee' stehen jetzt dort")
	if "idea" in cols:
		c = cols["idea"]
		insert_column(ws, c, EFFECT_WIDTH)
		ws.cell(header_row, c, EFFECT_TITLE)
		return c, f"Spalte '{EFFECT_TITLE}' links von '{WISH_TITLE}' angelegt"
	c = cols["name"] + 1
	insert_column(ws, c, WISH_WIDTH)
	insert_column(ws, c, EFFECT_WIDTH)
	ws.cell(header_row, c, EFFECT_TITLE)
	ws.cell(header_row, c + 1, WISH_TITLE)
	return c, f"Spalten '{EFFECT_TITLE}' und '{WISH_TITLE}' angelegt"


# Schreibt in die Spalte "Effekt" jedes Parts, was gerade umgesetzt ist. Das ist die EINZIGE Stelle, an der ein
# Werkzeug eine vorhandene Tabelle des Users ändert (sein Wunsch vom 06.10.2026), deshalb mit Sicherungen:
#   1. Geschrieben wird erst in eine zweite Datei (struktur.tmp.xlsx) daneben.
#   2. Diese wird wieder gelesen: Titel, Song-ID, Tempo, Takte, Partnamen, Energie und die Wünsche des Users müssen
#      exakt dieselben sein wie vorher (gleicher Fingerabdruck). Sonst wird sie verworfen, die Tabelle bleibt.
#   3. Erst dann ersetzt sie die Tabelle - in einem Schritt (os.replace), es gibt nie eine halb geschriebene Datei.
# Ist die Tabelle in Excel geöffnet, wird nichts geschrieben (Excel sperrt sie; ungespeicherte Eingaben gingen verloren).
# Hinweis: openpyxl speichert Formeln ohne ihr zuletzt berechnetes Ergebnis. Für "=$C$3" (BPM pro Part) macht das
# nichts, read_table() kennt den Verweis. Andere Formeln (z. B. in 'von takt') fallen bei Schritt 2 auf -> kein Schreiben.
#   texts         je Part der Text für "Effekt", in der Reihenfolge der Tabelle (ohne die Zeile "Ende")
#   end_black_ms  Länge des Schluss-Blacks: kommt in die Ende-Zeile, wenn dort in "Effekt" noch nichts steht
#   cue_texts     {von takt: Text} für die Zwischenzeilen (Viertel-Raster): was an dieser Stelle umgesetzt ist.
#                 Zwischenzeilen, die hier nicht genannt sind, werden in "Effekt" geleert - die Spalte gehört dem Skript.
# Rückgabe: Text, was geschehen ist (für die Ausgabe von songgen.py). Fehler: TableError, die Tabelle ist dann unverändert.
def write_effects(path, texts, end_black_ms, cue_texts=None):
	if (path.parent / ("~$" + path.name)).exists():
		raise TableError(f"{path.name} ist in Excel geöffnet - bitte schließen")
	before = read_table(path)
	rows, end_row = before["_table"]["rows"], before["_table"]["end_row"]
	if len(texts) != len(rows):
		raise TableError(f"{len(texts)} Effekt-Texte für {len(rows)} Parts")
	sub_rows = {von: r for part in before["_table"]["subs"] for von, r in part.items()}	# alle Zwischenzeilen: von takt -> Zeile
	cue_texts = {float(von): t for von, t in (cue_texts or {}).items()}
	missing = sorted(von for von in cue_texts if von not in sub_rows)
	if missing:
		raise TableError("keine Zwischenzeile mit von takt " + ", ".join(fmt(v).replace(".", ",") for v in missing))
	try:
		wb = openpyxl.load_workbook(path)		# ohne data_only: Formeln bleiben Formeln
	except PermissionError:
		raise TableError(f"{path.name} ist gesperrt (in Excel geöffnet?) - bitte schließen")
	ws, header_row, cols = find_sheet(wb)
	col, migrated = ensure_effect_column(ws, header_row, cols, rows)
	# Breiten: die Effekt-Texte sind mehrzeilig und brauchen Platz; eine Wunsch-Spalte ohne eigene Breite wäre zu schmal
	width = ws.column_dimensions[get_column_letter(col)].width
	if not width or width < EFFECT_WIDTH:
		ws.column_dimensions[get_column_letter(col)].width = EFFECT_WIDTH
	wish = find_sheet(wb)[2].get("idea")
	if wish and (ws.column_dimensions[get_column_letter(wish)].width or 0) < 14:	# 13 = Excels Standardbreite, wenn nichts gesetzt ist
		ws.column_dimensions[get_column_letter(wish)].width = WISH_WIDTH
	wrap = Alignment(wrap_text=True, vertical="top")
	changed = 0
	for r, t in zip(rows, texts):
		cell = ws.cell(r, col)
		if multiline(cell.value) != multiline(t):
			changed += 1
		cell.value = t or None
		cell.alignment = wrap
		if wish:
			ws.cell(r, wish).alignment = wrap
	for von, r in sub_rows.items():
		cell, t = ws.cell(r, col), cue_texts.get(von)
		if multiline(cell.value) != multiline(t):
			changed += 1
			cell.value = t or None
		if t:
			cell.alignment = wrap
	end_cell = ws.cell(end_row, col)
	if empty(end_cell.value):
		end_cell.value = f"{fmt(end_black_ms / 1000).replace('.', ',')} sek. BLACK"
		changed += 1
		# Stand in der Ende-Zeile bisher keine Zeit, liest read_table() sie ab jetzt aus dieser Zelle: das ist die eine
		# gewollte Änderung am Inhalt und gehört für die Kontrolle unten zum erwarteten Stand.
		before.setdefault("end_black_ms", int(round(end_black_ms)))
	if not changed and not migrated:
		return f"Spalte '{EFFECT_TITLE}' der Tabelle ist schon aktuell"

	tmp = path.with_name(TMP_FILE)
	try:
		wb.save(tmp)
		after = read_table(tmp)
		if content_sha(after) != content_sha(before):
			raise TableError("Kontrolle nach dem Schreiben: der Inhalt der Tabelle wäre nicht mehr derselbe (Formeln?) - nichts geändert")
		try:
			os.replace(tmp, path)
		except OSError:
			raise TableError(f"{path.name} ist gesperrt (in Excel geöffnet?) - bitte schließen")
	finally:
		if tmp.exists():
			tmp.unlink()
	return (migrated + "\n   " if migrated else "") + f"Spalte '{EFFECT_TITLE}' der Tabelle geschrieben ({changed} Zeile(n) neu)"
