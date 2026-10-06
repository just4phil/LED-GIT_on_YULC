"""
markers.py - Bund-Marker-LEDs für generierte Songs (Bibliothek für songgen.py, kein eigenes Kommando)

Regeln (vom User):
- Marker = Grundtöne der Akkorde (nach transpose), jeweils auf E- UND A-Saite; eine LED pro Bund,
  E- und A-Saite teilen sich die LED (z. B. ESaite_C = ASaite_F = 8. Bund).
- Kein Marker auf dem 5. und 12. Bund (dort leuchten immer die blauen LEDs) und auf der Leersaite (0).
- Bass kann andere Marker haben als Gitarre, einzelne Parts können Spezial-Marker haben.

SCHUTZ: Marker, die der User gesetzt oder akzeptiert hat, werden NIE geändert:
- hat markerLEDs.cpp (setMarkerLEDs) einen case für die Song-ID, gewinnt immer diese Handarbeit,
  es wird weder vorgeschlagen noch generiert;
- ein markers:-Block in der show.yaml eines Songs wird von keinem Tool verändert.

Was diese Datei tut: Sie prüft den markers:-Block der show.yaml (validate) und erzeugt daraus C++-Code - den case
für setGeneratedMarkerLEDs() in src/songs_generated.cpp (gen_case) und, für Marker, die nur in einzelnen Parts
gelten, Zeilen für den Anfang der Song-Funktion (inline_code). Die Markernamen (ESaite_G ...) sind dieselben wie
in src/definitions.h.

propose() und yaml_block() stammen aus dem früheren Akkord-Import (sheet2song.py, entfernt) und werden von
songgen.py nicht mehr aufgerufen.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent		# Projektordner (eine Ebene über tools/)
MARKER_CPP = ROOT / "src" / "markerLEDs.cpp"		# hier stehen die von Hand gesetzten Marker der alten Songs
MAX_MARKERS = 7				# markerLED1..markerLED7
SKIP_FRETS = {0, 5, 12}		# Leersaite + 5./12. Bund (blaue LEDs)

# Die Markernamen in der Reihenfolge der Bünde: Platz 0 = Leersaite, Platz 1 = 1. Bund usw.
# E_NAMES benennt die Bünde nach dem Ton auf der E-Saite, A_NAMES nach dem Ton auf der A-Saite.
E_NAMES = ["ESaite_E", "ESaite_F", "ESaite_Fis", "ESaite_G", "ESaite_Gis", "ESaite_A", "ESaite_Bb", "ESaite_B",
		   "ESaite_C", "ESaite_Cis", "ESaite_D", "ESaite_Dis", "ESaite_E_hoch", "ESaite_F_hoch", "ESaite_Fis_hoch", "ESaite_G_hoch"]
A_NAMES = ["ASaite_A", "ASaite_Bb", "ASaite_B", "ASaite_C", "ASaite_Cis", "ASaite_D", "ASaite_Dis", "ASaite_E",
		   "ASaite_F", "ASaite_Fis", "ASaite_G", "ASaite_Gis", "ASaite_A_hoch", "ASaite_Bb_hoch", "ASaite_B_hoch", "ASaite_C_hoch"]
# Nachschlagetabelle Markername -> Bundnummer (für beide Saiten zusammen); dient auch als Liste aller gültigen Namen
FRET = {n: i for i, n in enumerate(E_NAMES)} | {n: i for i, n in enumerate(A_NAMES)}
E_ROOT, A_ROOT = 4, 9		# Tonhöhe der Leersaiten (E, A) als Halbtonschritte über C (C = 0, Cis = 1 ... H = 11)


# Liest src/markerLEDs.cpp als Text und sucht mit einem Suchmuster die "case <Nummer>:" in setMarkerLEDs().
# Kommentare werden vorher entfernt, damit auskommentierte cases nicht mitzählen.
def handwritten_ids():
	"""Song-IDs mit von Hand gesetzten Markern (case im switch von setMarkerLEDs)."""
	text = MARKER_CPP.read_text(encoding="utf-8", errors="ignore")
	m = re.search(r"void setMarkerLEDs\s*\(.*?\)\s*\{(.*?)\n\}", text, re.S)
	body = m.group(1) if m else text
	body = re.sub(r"//[^\n]*|/\*.*?\*/", "", body, flags=re.S)
	return {int(x) for x in re.findall(r"\bcase\s+(\d+)\s*:", body)}


def handwritten_slots(song_id):
	"""Marker-Slots (1..7), die der handgeschriebene case dieser Song-ID in setMarkerLEDs() setzt."""
	text = MARKER_CPP.read_text(encoding="utf-8", errors="ignore")
	m = re.search(r"void setMarkerLEDs\s*\(.*?\)\s*\{(.*?)\n\}", text, re.S)
	body = re.sub(r"//[^\n]*|/\*.*?\*/", "", m.group(1) if m else text, flags=re.S)
	c = re.search(r"\bcase\s+" + str(song_id) + r"\s*:(.*?)(?=\bcase\s+\d+\s*:|\bdefault\s*:|\Z)", body, re.S)
	return {int(x) for x in re.findall(r"markerLED(\d)\s*=", c.group(1))} if c else set()


# "Slot" = einer der sieben Marker-Plätze markerLED1..markerLED7.
def slot_parts(markers):
	"""[(abschnitt, instrument, slot, name)] aller Slot-Angaben ({slot: Name}) in markers.parts."""
	out = []
	for p, s in ((markers or {}).get("parts") or {}).items():
		for inst, v in (s or {}).items():
			if isinstance(v, dict):
				out += [(p, inst, slot, name) for slot, name in v.items()]
	return out


def inline_code(markers, part_cases, base_slots):
	"""Slot-Angaben einzelner Parts als Code für den Anfang der Song-Funktion. Er läuft in jedem Durchlauf
	nach setMarkerLEDs(), genau wie die Inline-Marker der handgeschriebenen Songs. Slots, die die
	Grund-Marker nicht selbst setzen (base_slots), werden außerhalb der Parts wieder ausgeschaltet."""
	per = {}
	for p, inst, slot, name in slot_parts(markers):
		per.setdefault((inst, int(slot)), []).append((sorted(part_cases.get(p, [])), str(name), p))
	lines = []
	for inst in ("all", "guitar", "bass"):
		items = sorted((k, v) for k, v in per.items() if k[0] == inst)
		if not items:
			continue
		if inst != "all":
			lines.append("#ifdef " + ("GIT" if inst == "guitar" else "BASS"))
		for (_inst, slot), entries in items:
			for j, (cases, name, p) in enumerate(entries):
				cond = " || ".join(f"prog == {c}" for c in cases)
				lines.append(f"\t{'if' if j == 0 else 'else if'} ({cond}) markerLED{slot} = {name};\t// {p}")
			if slot not in base_slots:
				lines.append(f"\telse markerLED{slot} = 0;")
		if inst != "all":
			lines.append("#endif")
	return lines


# (nicht mehr benutzt, siehe Dateikopf)
def propose(chord_counts):
	"""Marker aus Akkord-Grundtönen. chord_counts: {(root, quality): Anzahl halber Takte im Song}.
	Liefert (Markernamen nach Häufigkeit, Hinweise)."""
	roots = {}
	for (root, _q), n in chord_counts.items():
		roots[root] = roots.get(root, 0) + n
	frets = {}		# Bund -> (Gewicht, Name)
	for root, n in sorted(roots.items(), key=lambda kv: -kv[1]):
		for base, names in ((E_ROOT, E_NAMES), (A_ROOT, A_NAMES)):
			f = (root - base) % 12
			if f in SKIP_FRETS:
				continue
			w, name = frets.get(f, (0, names[f]))
			frets[f] = (w + n, name)
	order = sorted(frets.items(), key=lambda kv: -kv[1][0])
	notes = []
	if len(order) > MAX_MARKERS:
		dropped = ", ".join(v[1] for _f, v in order[MAX_MARKERS:])
		notes.append(f"mehr als {MAX_MARKERS} Bünde - seltenste weggelassen: {dropped}")
		order = order[:MAX_MARKERS]
	return [v[1] for _f, v in sorted(order, key=lambda kv: kv[0])], notes


# Prüft den markers:-Block auf Tippfehler: unbekannte Schlüssel, unbekannte Markernamen, zu viele Marker,
# Parts, die es im Song nicht gibt. Eine leere Liste heißt: alles in Ordnung.
def validate(markers, part_names):
	"""Fehlerliste für einen markers:-Block."""
	errs = []

	def check_set(where, s):
		if not isinstance(s, dict):
			errs.append(f"markers {where}: erwartet all/guitar/bass")
			return
		for k, v in s.items():
			if k not in ("all", "guitar", "bass"):
				errs.append(f"markers {where}: unbekannter Schlüssel '{k}' (all, guitar, bass)")
				continue
			if isinstance(v, dict):		# {slot: Name}: einzelne Slots in diesem Part setzen (nur unter parts)
				if not where.startswith("parts."):
					errs.append(f"markers {where}.{k}: Slot-Angaben {{slot: Name}} gibt es nur unter parts")
				for slot, n in v.items():
					if not (isinstance(slot, int) and 1 <= slot <= MAX_MARKERS):
						errs.append(f"markers {where}.{k}: Slot '{slot}' muss 1..{MAX_MARKERS} sein")
					if n != 0 and n not in FRET:
						errs.append(f"markers {where}.{k}: unbekannter Marker '{n}'")
				continue
			v = v or []
			if len(v) > MAX_MARKERS:
				errs.append(f"markers {where}.{k}: höchstens {MAX_MARKERS} Marker")
			for n in v:
				if n not in FRET:
					errs.append(f"markers {where}.{k}: unbekannter Marker '{n}'")

	check_set("", {k: v for k, v in markers.items() if k != "parts"})
	for p, s in (markers.get("parts") or {}).items():
		if p not in part_names:
			errs.append(f"markers.parts: Abschnitt '{p}' gibt es nicht")
		check_set(f"parts.{p}", s or {})
	return errs


# "all" gilt für Gitarre und Bass; "guitar" bzw. "bass" ersetzen es für das jeweilige Instrument.
def resolve(s, fallback):
	"""(guitar, bass) aus einem Satz all/guitar/bass, fehlende Angaben aus fallback."""
	allv = s.get("all")
	g = s.get("guitar", allv if allv is not None else fallback[0])
	b = s.get("bass", allv if allv is not None else fallback[1])
	return list(g or []), list(b or [])


# C++-Zeilen "markerLED1 = ...;" bis "markerLED7 = ...;" - nicht belegte Plätze werden ausdrücklich auf 0 gesetzt
def assign_lines(names, indent):
	vals = list(names) + ["0"] * (MAX_MARKERS - len(names))
	return [f"{indent}markerLED{i + 1} = {v};" for i, v in enumerate(vals)]


def gen_case(song_id, markers, part_cases):
	"""C++ für einen Song im switch von setGeneratedMarkerLEDs. part_cases: {Abschnittsname: [case-Nummern]}."""
	base = resolve({k: v for k, v in markers.items() if k != "parts"}, ([], []))
	lines = [f"\tcase {song_id}:"]
	overrides = []
	for p, s in (markers.get("parts") or {}).items():
		s = {k: v for k, v in (s or {}).items() if not isinstance(v, dict)}	# Slot-Angaben laufen inline in der Song-Funktion
		if s:
			overrides.append((sorted(part_cases.get(p, [])), resolve(s, base), p))

	def block(gb, indent):
		g, b = gb
		out = []
		if g == b:
			return assign_lines(g, indent)
		out.append("#ifdef GIT")
		out += assign_lines(g, indent)
		out.append("#else")
		out += assign_lines(b, indent)
		out.append("#endif")
		return out

	first = True
	for cases, gb, p in overrides:
		cond = " || ".join(f"partID == {c}" for c in cases)
		lines.append(f"\t\t{'if' if first else 'else if'} ({cond}) {{\t// Spezial-Marker: {p}")
		lines += block(gb, "\t\t\t")
		lines.append("\t\t}")
		first = False
	if overrides:
		lines.append("\t\telse {")
		lines += block(base, "\t\t\t")
		lines.append("\t\t}")
	else:
		lines += block(base, "\t\t")
	lines.append("\t\tbreak;")
	return lines


# (nicht mehr benutzt, siehe Dateikopf)
def yaml_block(names, notes, chords_text):
	"""markers:-Block (Vorschlag) für die Song-YAML."""
	lines = [
		"# Bund-Marker. VORSCHLAG aus den Akkord-Grundtönen (E- und A-Saite, ohne Leersaite/5./12. Bund).",
		"# Sobald du ihn behältst oder änderst, gilt er als deiner: kein Tool und kein Claude ändert ihn danach.",
		f"# Akkorde im Song (transponiert): {chords_text}",
		"markers:",
		f"  all: [{', '.join(names)}]",
		"  # guitar: [...]            # nur Gitarre (ersetzt all)",
		"  # bass: [...]              # nur Bass (ersetzt all)",
		"  # parts:                   # Spezial-Marker für einzelne Abschnitte",
		"  #   solo: {bass: [ESaite_Cis, ESaite_Dis]}",
	]
	lines += [f"# ! {n}" for n in notes]
	return lines
