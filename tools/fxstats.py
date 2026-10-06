#!/usr/bin/env python3
"""
fxstats.py - wertet aus, welche Effekte die alten, handgeschriebenen Songs an welcher Stelle nutzen

    python tools/fxstats.py              # Bericht nach docs/effekt-statistik.md + Kurzfassung auf der Konsole
    python tools/fxstats.py --stdout     # ganzen Bericht nur ausgeben, nichts schreiben

Quelle ist die Spalte 'bisher (alter Code)' in songs/*/quelle/struktur.xlsx (beim Import der alten Songs aus
src/songs.cpp übernommen: Effekt des alten Codes auf der Gitarre, dahinter ggf. '(Matrix: ...)').
Die Tabellen werden nur gelesen. Parameternamen kommen aus den Deklarationen in src/*.h.

Der Bericht ist die Grundlage für docs/effekt-katalog.yaml (Steckbriefe der Effekte).
"""
import argparse
import re
import statistics
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import struktur as st  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
SONGS_DIR = ROOT / "songs"
HEADERS = [ROOT / "src" / n for n in ("FXprograms.h", "guitarShapeFX.h", "scenes.h")]
OUT = ROOT / "docs" / "effekt-statistik.md"

# Part-Typ aus dem Namen des Abschnitts; die erste passende Regel gilt
PART_TYPES = [
	("solo", r"solo"),
	("pause", r"^pause|^black$|klick"),
	("outro", r"outro|fade out|abschluss|letzter durchgang"),
	("intro", r"intro"),
	("uebergang", r"[üu]e?bergang|snare|auftakt|fill|wirbel|roll"),
	("akzent", r"strob|boom|^fx$"),
	("break", r"stop|mini pause|nur vocals|stehender|break|half ?time|halftime|drumloop"),
	("prechorus", r"pre.?chorus"),
	("chorus", r"chorus|chrous|refrain"),
	("verse", r"verse|^v:"),
	("bridge", r"bridge"),
	("instrumental", r"instrumental|intrumental|zwischenspiel|synth|\bgit\b|bass"),
]
OTHER = "textzeile"		# Abschnitte, die nach einer Textzeile/Hook benannt sind
TYPE_ORDER = ["pause", "intro", "verse", "prechorus", "chorus", "bridge", "solo", "instrumental",
			  "break", "uebergang", "akzent", "outro", OTHER]
BEAT_FRACTIONS = [(0.125, "1/8 Beat"), (0.25, "1/4 Beat"), (0.5, "1/2 Beat"), (1, "1 Beat"), (2, "2 Beats"),
				  (4, "1 Takt"), (8, "2 Takte"), (16, "4 Takte")]


def part_type(name):
	n = name.lower()
	for typ, pat in PART_TYPES:
		if re.search(pat, n):
			return typ
	return OTHER


def split_args(s):
	"""Argumente an Kommas der obersten Ebene trennen (Klammern und Strings beachten)."""
	args, depth, cur, quote = [], 0, "", False
	for ch in s:
		if ch == '"':
			quote = not quote
		if not quote:
			if ch == "(":
				depth += 1
			elif ch == ")":
				depth -= 1
			elif ch == "," and depth == 0:
				args.append(cur.strip())
				cur = ""
				continue
		cur += ch
	if cur.strip():
		args.append(cur.strip())
	return args


def parse_call(text):
	"""'progX(a, b(c), d)   rest' -> (name, [args], rest) oder None."""
	m = re.match(r"\s*(\w+)\s*\(", text)
	if not m:
		return None
	depth, quote = 0, False
	for i in range(m.end() - 1, len(text)):
		ch = text[i]
		if ch == '"':
			quote = not quote
		if quote:
			continue
		if ch == "(":
			depth += 1
		elif ch == ")":
			depth -= 1
			if depth == 0:
				return m.group(1), split_args(text[m.end():i]), text[i + 1:]
	return None


def load_signatures():
	"""Effektname -> Liste von (Mindestzahl Argumente, [Parameternamen], [Typen]) aus den Headern."""
	sigs = defaultdict(list)
	for path in HEADERS:
		for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
			m = re.match(r"\s*void\s+(prog\w+|scene)\s*\((.*)\)\s*;", line)
			if not m:
				continue
			names, types, required = [], [], 0
			for p in split_args(m.group(2)):
				has_default = "=" in p
				p = p.split("=")[0].strip()
				names.append(re.sub(r"\[\]$", "", p.split()[-1].lstrip("*&")))
				types.append(" ".join(p.split()[:-1]))
				if not has_default:
					required += 1
			sigs[m.group(1)].append((required, names, types))
	return sigs


def param_names(sigs, fx, args):
	# bei Überladungen mit gleicher Anzahl entscheidet, ob true/false auf einen bool-Parameter trifft
	best, best_score = None, -1
	for required, names, types in sigs.get(fx, []):
		if not required <= len(args) <= len(names):
			continue
		score = sum((a in ("true", "false")) == t.startswith("bool") for a, t in zip(args, types))
		if score > best_score:
			best, best_score = names[:len(args)], score
	if best:
		return best
	return [f"arg{i + 1}" for i in range(len(args))]


def load_parts():
	"""Alle Parts mit einem Eintrag in der Spalte 'bisher (alter Code)' als Liste von dicts, in Song-Reihenfolge."""
	parts = []
	for path in sorted(SONGS_DIR.glob(f"*/quelle/{st.TABLE_FILE}")):
		try:
			song = st.read_table(path)
		except st.TableError as e:
			print(f"  ! {path.parent.parent.name}: {e}", file=sys.stderr)
			continue
		for sec in song["sections"]:
			old = next((l for l in sec.get("old", "").splitlines() if not l.startswith("!")), None)	# erste Zeile = Effekt des Parts
			call = parse_call(old) if old else None
			if not call:
				continue
			cur = {"song": path.parent.parent.name, "name": sec["name"], "bars": float(sec.get("bars", 0)),
				   "beats": float(sec.get("beats", 0)), "bpm": float(sec.get("bpm", song["bpm"])), "bpb": song["beats_per_bar"]}
			cur["fx"], cur["args"], rest = call
			cur["raw"] = old.split("   (Matrix:")[0].strip()
			cur["matrix"] = []
			mm = re.search(r"\(Matrix:\s*(.*)\)\s*$", rest)
			if mm:
				for piece in mm.group(1).split(";"):
					c = parse_call(piece)
					if c:
						cur["matrix"].append(c[0])
			cur["len"] = cur["bars"] + cur["beats"] / cur["bpb"]
			cur["type"] = part_type(cur["name"])
			parts.append(cur)
	return parts


def in_beats(value_ms, bpm):
	"""ms-Wert als Notenwert, wenn er auf 8 % genau passt, sonst None."""
	beat = 60000.0 / bpm
	for frac, label in BEAT_FRACTIONS:
		if abs(value_ms - frac * beat) <= 0.08 * frac * beat:
			return label
	return None


def table(header, rows):
	out = ["| " + " | ".join(header) + " |", "|" + "|".join("---" for _ in header) + "|"]
	out += ["| " + " | ".join(str(c) for c in r) + " |" for r in rows]
	return out


def top(counter, n=4):
	return ", ".join(f"{k} ({v})" for k, v in counter.most_common(n))


def report(parts, sigs):
	L = []
	songs = sorted({p["song"] for p in parts})
	total_bars = sum(p["len"] for p in parts)
	by_fx = defaultdict(list)
	for p in parts:
		by_fx[p["fx"]].append(p)
	fx_sorted = sorted(by_fx, key=lambda f: -sum(p["len"] for p in by_fx[f]))

	L += ["# Effekt-Statistik der alten Songs", "",
		  "Erzeugt von `tools/fxstats.py` - nicht von Hand ändern, neu erzeugen.", "",
		  f"Grundlage: {len(parts)} Parts aus {len(songs)} handgeschriebenen Songs ({total_bars:.0f} Takte), "
		  "jeweils der Effekt auf der Gitarre (Spalte 'bisher (alter Code)' in `songs/*/quelle/struktur.xlsx`).",
		  "Die alten Songs haben keine `energy`-Werte, deshalb ist hier nur nach Part-Typ ausgewertet.", ""]

	L += ["## 1. Effekte nach Spielzeit", ""]
	rows = []
	for fx in fx_sorted:
		ps = by_fx[fx]
		lens = [p["len"] for p in ps]
		rows.append([f"`{fx}`", len(ps), f"{sum(lens):.0f}", f"{100 * sum(lens) / total_bars:.0f} %",
					 len({p["song"] for p in ps}), f"{statistics.median(lens):g}", f"{max(lens):g}",
					 top(Counter(p["type"] for p in ps), 3)])
	L += table(["Effekt", "Parts", "Takte", "Anteil", "Songs", "Takte Median", "Takte max", "häufigste Part-Typen"], rows)

	L += ["", "## 2. Effekt × Part-Typ (Anzahl Parts)", ""]
	types = [t for t in TYPE_ORDER if any(p["type"] == t for p in parts)]
	rows = []
	for fx in fx_sorted:
		c = Counter(p["type"] for p in by_fx[fx])
		rows.append([f"`{fx}`"] + [c[t] or "" for t in types])
	L += table(["Effekt"] + types, rows)

	L += ["", "## 3. Part-Typ → Effekte", ""]
	rows = []
	for t in types:
		ps = [p for p in parts if p["type"] == t]
		c = Counter(p["fx"] for p in ps)
		rows.append([t, len(ps), f"{statistics.median([p['len'] for p in ps]):g}", top(c, 6)])
	L += table(["Part-Typ", "Parts", "Takte Median", "Effekte (Anzahl)"], rows)

	L += ["", "## 4. progPalette nach Palette", "",
		  "`progPalette` hat keinen Tempo-Parameter: das Tempo und 'mit/ohne Fades' stecken in der Palette selbst.", ""]
	pal = defaultdict(list)
	for p in by_fx.get("progPalette", []):
		pal[p["args"][1]].append(p)
	rows = []
	for pid in sorted(pal, key=lambda x: -len(pal[x])):
		ps = pal[pid]
		rows.append([pid, len(ps), f"{sum(p['len'] for p in ps):.0f}", len({p["song"] for p in ps}),
					 top(Counter(p["type"] for p in ps), 4), ", ".join(sorted({p["song"].split("_")[0] for p in ps}))])
	L += table(["Palette", "Parts", "Takte", "Songs", "Part-Typen", "in"], rows)

	L += ["", "## 5. Parameter je Effekt", "",
		  "Dauer und Folge-Part sind weggelassen. Zeitwerte, die zum Tempo des Songs passen, stehen als Notenwert.", ""]
	for fx in fx_sorted:
		ps = by_fx[fx]
		per_param = defaultdict(Counter)
		for p in ps:
			for name, val in zip(param_names(sigs, fx, p["args"]), p["args"]):
				if name in ("durationMillis", "nextPart", "words", "text"):
					continue
				shown = val
				if re.fullmatch(r"\d+", val) and re.search(r"ms|del|speed|millis", name, re.I) and p["bpm"]:
					shown = in_beats(int(val), p["bpm"]) or val
				per_param[name][shown] += 1
		if not per_param:
			continue
		L.append(f"**`{fx}`** ({len(ps)} Parts)")
		L.append("")
		for name, c in per_param.items():
			L.append(f"- `{name}`: {top(c, 8)}")
		L.append("")

	L += ["## 6. Abfolgen", "", "### Was steht direkt vor einem Chorus?", ""]
	before, pairs = Counter(), Counter()
	for a, b in zip(parts, parts[1:]):
		if a["song"] != b["song"]:
			continue
		if a["fx"] != b["fx"]:
			pairs[(a["fx"], b["fx"])] += 1
		if b["type"] == "chorus" and a["type"] != "chorus":
			before[(a["type"], a["fx"], b["fx"])] += 1
	L += table(["Part davor", "Effekt davor", "Effekt im Chorus", "Anzahl"],
			   [[t, f"`{a}`", f"`{b}`", n] for (t, a, b), n in before.most_common(20)])
	L += ["", "### Häufigste Wechsel", ""]
	L += table(["von", "nach", "Anzahl"], [[f"`{a}`", f"`{b}`", n] for (a, b), n in pairs.most_common(20)])

	L += ["", "## 7. Abweichende Effekte auf der Matrix", ""]
	mat = Counter()
	for p in parts:
		for m in p["matrix"]:
			mat[(m, p["fx"])] += 1
	L += table(["Matrix", "während Gitarre", "Anzahl"], [[f"`{m}`", f"`{g}`", n] for (m, g), n in mat.most_common(25)])

	L += ["", "## 8. Abschnitte ohne erkannten Part-Typ", "",
		  f"Nach einer Textzeile benannt, als `{OTHER}` gezählt:", ""]
	c = Counter(p["fx"] for p in parts if p["type"] == OTHER)
	L.append(top(c, 12))
	L.append("")
	return "\n".join(L) + "\n"


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("--stdout", action="store_true", help="Bericht nur ausgeben, nichts schreiben")
	args = ap.parse_args()

	parts = load_parts()
	if not parts:
		sys.exit("keine Einträge in der Spalte 'bisher (alter Code)' der Tabellen gefunden")
	text = report(parts, load_signatures())
	if args.stdout:
		print(text)
		return
	with open(OUT, "w", encoding="utf-8", newline="\n") as f:
		f.write(text)
	print(f"{len(parts)} Parts aus {len({p['song'] for p in parts})} Songs -> {OUT.relative_to(ROOT)}")
	print(text.split("## 2.")[0])


if __name__ == "__main__":
	main()
