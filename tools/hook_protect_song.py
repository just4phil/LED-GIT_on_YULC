#!/usr/bin/env python3
"""
hook_protect_song.py - Claude-Code-Hook (PreToolUse, siehe .claude/settings.json)

songs/<Song>/quelle/struktur.xlsx gehört dem User: es ist die einzige Datei, die er pro Song pflegt. Der Hook
blockiert jeden Versuch von Claude, eine solche Tabelle zu schreiben: Edit/Write direkt, Shell-Befehle anhand
typischer Schreibmuster (Umleitung, mv, rm, sed -i ...). Neu angelegt wird sie nur von `songgen.py <Song> --neu`,
zurückgeholt nur von `songgen.py <Song> --restore`. Auf Wunsch des Users (06.10.2026) füllt `songgen.py` beim
Generieren außerdem die Spalte "Effekt (füllt KI)" (struktur.write_effects, mit Kontrolle, dass sonst nichts
anders ist) - das läuft über das Werkzeug und ist kein Schreibversuch von Claude. Auf demselben Weg schreibt
`songgen.py <Song> --vorschlag` die Spalte "Neuer Vorschlag (KI)" (sein Wunsch vom 10.10.2026, struktur.write_proposals). Genauso geschützt bleiben die alten song.yaml, solange der
User sie nicht selbst gelöscht hat. Lesen bleibt erlaubt. Exit 2 = blockieren, die Meldung auf stderr geht an Claude.
"""
import json
import re
import sys

# Die Suchmuster ("reguläre Ausdrücke"), an denen ein Schreibversuch erkannt wird.
# NAME = einer der beiden geschützten Dateinamen, END = "hier endet das Argument" (Ende des Befehls oder ; & | )).
NAME = r"(?:song\.yaml|struktur\.xlsx)"
END = r"""["']?\s*($|[;&|)])"""
WRITE_PATTERNS = [
	r">>?\s*[^\s;&|]*" + NAME,																	# Umleitung in die Datei
	r"\b(mv|rm|del|erase|ren|move|truncate|tee)\b[^;&|]*" + NAME,
	r"\bsed\b[^;&|]*\s-i[^;&|]*" + NAME,
	r"\b(Move-Item|Remove-Item|Rename-Item|Set-Content|Add-Content|Clear-Content|Out-File)\b[^;|]*" + NAME,
	r"\b(cp|copy|Copy-Item)\b[^;&|]*" + NAME + END,												# nur als Ziel
	r"\bgit\s+(checkout|restore|rm|mv)\b[^;&|]*" + NAME,
	r"open\([^)]*" + NAME + r"""["'][^)]*["'][wa]""",											# python -c "open(..., 'w')"
]


# Claude Code ruft dieses Skript vor jedem Werkzeug-Aufruf auf und übergibt die Beschreibung des Aufrufs als JSON
# auf der Standardeingabe: welches Werkzeug (tool_name) mit welchen Angaben (tool_input).
# Rückgabe 0 = erlaubt, 2 = blockiert.
def main():
	try:
		data = json.load(sys.stdin)
	except Exception:
		return 0
	tool = data.get("tool_name", "")
	inp = data.get("tool_input") or {}
	if tool in ("Edit", "Write", "NotebookEdit"):
		path = str(inp.get("file_path") or inp.get("notebook_path") or "").replace("\\", "/").lower()
		blocked = bool(re.search(r"(^|/)songs/.*(^|/)" + NAME + "$", path))
	elif tool in ("Bash", "PowerShell"):
		cmd = str(inp.get("command") or "")
		blocked = any(re.search(p, cmd, re.I) for p in WRITE_PATTERNS)
	else:
		blocked = False
	if blocked:
		sys.stderr.reconfigure(encoding="utf-8")
		print("BLOCKIERT: songs/<Song>/quelle/struktur.xlsx (und eine alte song.yaml) gehört dem User und wird von Claude "
			  "nie geschrieben, verschoben oder gelöscht. Änderungen dem User im Chat vorschlagen; Gestaltung gehört in "
			  "show.yaml. (Die Spalte 'Effekt (füllt KI)' schreibt nur songgen.py <Song> bzw. --tabelle.)", file=sys.stderr)
		return 2
	return 0


if __name__ == "__main__":
	sys.exit(main())
