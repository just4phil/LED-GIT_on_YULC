#!/usr/bin/env python3
"""
build_ota.py - baut die Firmware für alle Geräte und legt sie für das OTA-Update ab

    python tools/build_ota.py                  # alle Geräte bauen -> ota/<gerät>/
    python tools/build_ota.py lampe1 lampe2    # nur diese Geräte
    python tools/build_ota.py --serve          # bauen + HTTP-Server auf Port 8080 starten
    python tools/build_ota.py --serve-only     # nur den Server starten (ohne neu zu bauen)

Pro Gerät entsteht:
    ota/<gerät>/firmware.bin
    ota/<gerät>/version.json   {"version": <unix-zeit>, "git": "<hash>", "md5": "...", "size": ...}

Alle Geräte eines Laufs bekommen dieselbe FW_VERSION (über die Umgebungsvariable,
die tools/fw_version.py beim Build ausliest). Ein Gerät lädt nur, wenn die Server-
Version neuer ist als seine eigene.

Update auslösen: Gitarre (andresgit) mit gedrücktem Rotary-Knopf einschalten, während
die anderen Geräte laufen. Server-Adresse + WLAN stehen in src/secrets.h.
"""
import argparse
import functools
import hashlib
import http.server
import json
import os
import shutil
import socket
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OTA_DIR = ROOT / "ota"
DEVICES = ["andresgit", "rinasbass", "lampe1", "lampe2", "scrollmatrix"]
PORT = 8080


def find_pio():
    for cand in ("pio", "platformio", str(Path.home() / ".platformio/penv/Scripts/pio.exe")):
        if shutil.which(cand) or Path(cand).exists():
            return cand
    sys.exit("PlatformIO (pio) nicht gefunden")


def git_hash():
    try:
        h = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], cwd=ROOT, text=True).strip()
        if subprocess.call(["git", "diff", "--quiet", "HEAD"], cwd=ROOT) != 0:
            h += "+"
        return h
    except Exception:
        return "unknown"


def build(devices):
    version = str(int(time.time()))
    env = dict(os.environ, FW_VERSION=version)
    cmd = [find_pio(), "run"]
    for d in devices:
        cmd += ["-e", d]
    print(f"Baue {', '.join(devices)} mit FW_VERSION={version} ...")
    if subprocess.call(cmd, cwd=ROOT, env=env) != 0:
        sys.exit("Build fehlgeschlagen - nichts abgelegt")

    git = git_hash()
    for d in devices:
        src = ROOT / ".pio" / "build" / d / "firmware.bin"
        data = src.read_bytes()
        out = OTA_DIR / d
        out.mkdir(parents=True, exist_ok=True)
        (out / "firmware.bin").write_bytes(data)
        info = {"version": int(version), "git": git, "md5": hashlib.md5(data).hexdigest(), "size": len(data)}
        (out / "version.json").write_text(json.dumps(info, indent=1) + "\n", encoding="utf-8")
        print(f"  {d:13s} {len(data) / 1024:7.0f} KB  -> ota/{d}/")


def local_ips():
    ips = set()
    try:
        for info in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            ips.add(info[4][0])
    except OSError:
        pass
    return sorted(ip for ip in ips if not ip.startswith("127."))


def serve():
    if not OTA_DIR.exists():
        sys.exit("ota/ fehlt - erst bauen")
    print(f"\nOTA-Server auf Port {PORT}, Ordner {OTA_DIR}")
    for ip in local_ips():
        print(f"  http://{ip}:{PORT}/")
    print("(OTA_SERVER in src/secrets.h muss eine dieser Adressen sein; Strg+C beendet)\n")
    handler = functools.partial(http.server.SimpleHTTPRequestHandler, directory=str(OTA_DIR))
    with http.server.ThreadingHTTPServer(("0.0.0.0", PORT), handler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("devices", nargs="*", metavar="gerät", help="Standard: alle (" + ", ".join(DEVICES) + ")")
    ap.add_argument("--serve", action="store_true", help="nach dem Bauen den HTTP-Server starten")
    ap.add_argument("--serve-only", action="store_true", help="nur den HTTP-Server starten")
    args = ap.parse_args()
    unknown = [d for d in args.devices if d not in DEVICES]
    if unknown:
        ap.error(f"unbekanntes Gerät: {', '.join(unknown)} (möglich: {', '.join(DEVICES)})")

    if not args.serve_only:
        build(args.devices or DEVICES)
    if args.serve or args.serve_only:
        serve()


if __name__ == "__main__":
    main()
