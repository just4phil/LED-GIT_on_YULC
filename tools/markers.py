"""
markers.py - Bund-Marker-LEDs für generierte Songs (genutzt von sheet2song.py und songgen.py)

Regeln (vom User):
- Marker = Grundtöne der Akkorde (nach transpose), jeweils auf E- UND A-Saite; eine LED pro Bund,
  E- und A-Saite teilen sich die LED (z. B. ESaite_C = ASaite_F = 8. Bund).
- Kein Marker auf dem 5. und 12. Bund (dort leuchten immer die blauen LEDs) und auf der Leersaite (0).
- Bass kann andere Marker haben als Gitarre, einzelne Parts können Spezial-Marker haben.

SCHUTZ: Marker, die der User gesetzt oder akzeptiert hat, werden NIE geändert:
- hat markerLEDs.cpp (setMarkerLEDs) einen case für die Song-ID, gewinnt immer diese Handarbeit,
  es wird weder vorgeschlagen noch generiert;
- ein markers:-Block in der Song-YAML wird von keinem Tool verändert (sheet2song übernimmt ihn wörtlich).
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MARKER_CPP = ROOT / "src" / "markerLEDs.cpp"
MAX_MARKERS = 7				# markerLED1..markerLED7
SKIP_FRETS = {0, 5, 12}		# Leersaite + 5./12. Bund (blaue LEDs)

E_NAMES = ["ESaite_E", "ESaite_F", "ESaite_Fis", "ESaite_G", "ESaite_Gis", "ESaite_A", "ESaite_Bb", "ESaite_B",
		   "ESaite_C", "ESaite_Cis", "ESaite_D", "ESaite_Dis", "ESaite_E_hoch", "ESaite_F_hoch", "ESaite_Fis_hoch", "ESaite_G_hoch"]
A_NAMES = ["ASaite_A", "ASaite_Bb", "ASaite_B", "ASaite_C", "ASaite_Cis", "ASaite_D", "ASaite_Dis", "ASaite_E",
		   "ASaite_F", "ASaite_Fis", "ASaite_G", "ASaite_Gis", "ASaite_A_hoch", "ASaite_Bb_hoch", "ASaite_B_hoch", "ASaite_C_hoch"]
FRET = {n: i for i, n in enumerate(E_NAMES)} | {n: i for i, n in enumerate(A_NAMES)}
E_ROOT, A_ROOT = 4, 9		# Tonhöhe der Leersaiten (E, A)


def handwritten_ids():
	"""Song-IDs mit von Hand gesetzten Markern (case im switch von setMarkerLEDs)."""
	text = MARKER_CPP.read_text(encoding="utf-8", errors="ignore")
	m = re.search(r"void setMarkerLEDs\s*\(.*?\)\s*\{(.*?)\n\}", text, re.S)
	body = m.group(1) if m else text
	body = re.sub(r"//[^\n]*|/\*.*?\*/", "", body, flags=re.S)
	return {int(x) for x in re.findall(r"\bcase\s+(\d+)\s*:", body)}


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


def resolve(s, fallback):
	"""(guitar, bass) aus einem Satz all/guitar/bass, fehlende Angaben aus fallback."""
	allv = s.get("all")
	g = s.get("guitar", allv if allv is not None else fallback[0])
	b = s.get("bass", allv if allv is not None else fallback[1])
	return list(g or []), list(b or [])


def assign_lines(names, indent):
	vals = list(names) + ["0"] * (MAX_MARKERS - len(names))
	return [f"{indent}markerLED{i + 1} = {v};" for i, v in enumerate(vals)]


def gen_case(song_id, markers, part_cases):
	"""C++ für einen Song im switch von setGeneratedMarkerLEDs. part_cases: {Abschnittsname: [case-Nummern]}."""
	base = resolve({k: v for k, v in markers.items() if k != "parts"}, ([], []))
	lines = [f"\tcase {song_id}:"]
	overrides = []
	for p, s in (markers.get("parts") or {}).items():
		overrides.append((sorted(part_cases.get(p, [])), resolve(s or {}, base), p))

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
