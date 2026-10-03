#!/usr/bin/env python3
"""
hook_protect_song.py - Claude-Code-Hook (PreToolUse, siehe .claude/settings.json)

songs/<Song>/song.yaml gehört dem User. Der Hook blockiert jeden Versuch von Claude, eine solche Datei zu
schreiben: Edit/Write direkt, Shell-Befehle anhand typischer Schreibmuster (Umleitung, mv, rm, sed -i ...).
Lesen bleibt erlaubt. Exit 2 = blockieren, die Meldung auf stderr geht an Claude.
"""
import json
import re
import sys

NAME = r"song\.yaml"
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
		print("BLOCKIERT: songs/<Song>/song.yaml gehört dem User und wird von Claude nie geschrieben, verschoben oder "
			  "gelöscht. Änderungen dem User im Chat vorschlagen; Gestaltung gehört in show.yaml.", file=sys.stderr)
		return 2
	return 0


if __name__ == "__main__":
	sys.exit(main())
