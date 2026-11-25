#!/usr/bin/env python3
"""
build_sim.py - baut die Simulator-DLLs (die ECHTEN Firmware-Quellen als Windows-DLL, je Gerät eine)

    python tools/build_sim.py                  # alle Geräte, die es im Simulator schon gibt
    python tools/build_sim.py scrollmatrix     # nur dieses Gerät
    python tools/build_sim.py --test           # bauen + Schnelltest (sim/test/sim_smoke.cpp) laufen lassen
    python tools/build_sim.py --zeittest       # bauen + Zeit-Test (tools/sim_timetest.py: Part-Wechsel aller generierten Songs gegen songgen.py)
    python tools/build_sim.py --clean          # alle Zwischendateien (sim/bin/obj_*) löschen und neu bauen
    python tools/build_sim.py -j 4             # höchstens 4 Übersetzungen gleichzeitig

Was passiert (Überblick: docs/Simulator-Plan.md):
  - Aus den Quellen in src/ (Effekte, Szenen, Songs, Ausgabestufe, Marker, showEngine ...), den Bibliotheken
    aus .pio/libdeps/<gerät>/ (FastLED-Rechenteile, Adafruit GFX, Framebuffer GFX, FastLED NeoMatrix) und den
    Ersatz-Dateien aus sim/ (Ersatz für Arduino/FastLED-Hardware in sim/shim/, Schnittstelle sim/simApi.cpp)
    entsteht mit g++ (MinGW, w64devkit) die DLL  sim/bin/sim_<gerät>.dll.
  - Das Gerät wird wie in der Firmware per -D gewählt (-D SCROLLMATRIX ...). Die Defines stehen in platformio.ini
    (Abschnitt [env:<gerät>] und die Basis [esp32_base]); sie werden von hier AUS DER DATEI GELESEN, nicht
    abgeschrieben. Dazu kommt -D SIMULATOR (damit src/ die wenigen Hardware-Stellen auslassen kann).
  - Eine DLL je Gerät, weil #ifdef LAMPE1 / SCROLLMATRIX usw. LED-Zahl und Verhalten festlegen und jede DLL ihre
    eigenen globalen Variablen braucht.
  - Statisch gelinkt (-static): die DLL braucht keine MinGW-DLLs (libstdc++, libgcc, winpthread) und läuft
    daher auch aus dem Rust-Programm (MSVC) heraus.
  - Zwischenergebnisse (.o-Dateien) liegen in sim/bin/obj_<gerät>/; beim nächsten Lauf wird nur neu übersetzt,
    was sich (samt eingebundener Header) geändert hat. sim/bin/ steht in .gitignore.

Braucht g++ im Suchpfad (w64devkit: https://github.com/skeeto/w64devkit) - dasselbe wie "pio test -e native".
"""
import argparse
import configparser
import os
import re
import shlex
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

# Path(__file__) ist diese Datei selbst; zweimal ".parent" führt von tools/build_sim.py zum Projektordner.
ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
SIM = ROOT / "sim"
BIN = SIM / "bin"

# Die Geräte, die der Simulator schon kann. Kommt ein Gerät dazu (Gitarre, Bass, Lampen: eigene Issues),
# wird es hier eingetragen - der Name ist zugleich der Name der PlatformIO-Umgebung in platformio.ini.
DEVICES = ["scrollmatrix"]

# Dateien aus src/, die NICHT in die DLL kommen: Hardware (Funk, OTA, Drehknopf, Akku, MIDI-Eingang, Pins), die
# Firmware-Hauptdatei main.cpp (setup/loop; ihren Platz nimmt sim/simApi.cpp ein), die Dreh-Knopf-Bibliothek
# und das Gurt-Testprogramm. Alles andere in src/*.cpp gehört zur Show und wird übersetzt.
SRC_EXCLUDE = {
    "main.cpp", "AiEsp32RotaryEncoder.cpp", "BLE_client_nimBLE.cpp", "midiProxyBLEserver_nimBLE.cpp",
    "espNowSync.cpp", "espNowFrame.cpp", "ledSync.cpp", "syncClient.cpp", "syncProxy.cpp",
    "lipoVoltageCheck.cpp", "midi_in.cpp", "otaUpdate.cpp", "rotaryEncoder.cpp", "gurtTest.cpp",
}

# Quellen der Bibliotheken, relativ zu .pio/libdeps/<gerät>/. Von FastLED nur die portablen Rechenteile
# (Begründung und Funktionsweise: sim/shim/FastLED.h); von Adafruit GFX nur die Zeichen-Grundlage (ohne Display-
# Treiber SPITFT/GrayOLED).
LIB_SOURCES = [
    "FastLED/src/lib8tion.cpp", "FastLED/src/colorutils.cpp", "FastLED/src/hsv2rgb.cpp",
    "FastLED/src/noise.cpp",
    "Adafruit GFX Library/Adafruit_GFX.cpp",
    "Framebuffer GFX/Framebuffer_GFX.cpp",
    "FastLED NeoMatrix/FastLED_NeoMatrix.cpp",
]
# Ordner, die als Include-Pfad gebraucht werden (nach sim/shim, damit unsere Ersatz-Header zuerst gefunden werden)
LIB_INCLUDES = ["FastLED/src", "Adafruit GFX Library", "Framebuffer GFX", "FastLED NeoMatrix"]


def find_gpp():
    """Sucht g++ im Suchpfad; bricht mit Hinweis ab, wenn es fehlt."""
    gpp = shutil.which("g++")
    if not gpp:
        sys.exit("g++ nicht gefunden (w64devkit im PATH?) - siehe Kopf von tools/build_sim.py")
    return gpp


def read_env_defines(env):
    """
    Liest die -D-Angaben (Build-Flags) der PlatformIO-Umgebung <env> aus platformio.ini.
    Rückgabe: Liste wie ["-DUSE_ESP32", "-DSCROLLMATRIX", ...] (nur -D, keine Linker-/Compiler-Optionen).
    configparser liest die ini-Datei. PlatformIOs Schreibweise ${esp32_base.build_flags} (Wert aus einem anderen
    Abschnitt einsetzen) kennt configparser nicht selbst; sie wird unten mit einem regulären Ausdruck (re.sub)
    aufgelöst. Kommentare hinter einem Semikolon (" ; Text") werden verworfen.
    """
    cp = configparser.ConfigParser(interpolation=None, inline_comment_prefixes=(";",),
                                   comment_prefixes=(";", "#"), strict=False)
    cp.read(ROOT / "platformio.ini", encoding="utf-8")
    section = f"env:{env}"
    if section not in cp:
        sys.exit(f"Umgebung [{section}] fehlt in platformio.ini")

    def resolve(text):
        # ${abschnitt.schlüssel} durch den Wert aus platformio.ini ersetzen (auch verschachtelt)
        return re.sub(r"\$\{([^.}]+)\.([^}]+)\}", lambda m: resolve(cp.get(m.group(1), m.group(2), fallback="")), text)

    flags = resolve(cp.get(section, "build_flags"))
    toks = shlex.split(flags.replace("\n", " "))
    out, i = [], 0
    while i < len(toks):
        t = toks[i]
        if t == "-D" and i + 1 < len(toks):     # "-D NAME" (mit Leerzeichen) -> "-DNAME"
            out.append("-D" + toks[i + 1])
            i += 2
            continue
        if t.startswith("-D") and len(t) > 2:
            out.append(t)
        i += 1
    return out


def compile_flags(env):
    """Alle Compiler-Optionen eines Geräts: Sprache, Optimierung, Defines, Include-Pfade, Zwangs-Header."""
    libs = ROOT / ".pio" / "libdeps" / env
    if not libs.exists():
        sys.exit(f"{libs} fehlt - einmal 'pio run -e {env}' ausführen, damit PlatformIO die Bibliotheken lädt")
    flags = ["-std=gnu++17", "-O2", "-DSIMULATOR", "-DNDEBUG"] + read_env_defines(env)
    # Reihenfolge der Include-Pfade zählt: erst unsere Ersatz-Header (sim/shim), dann src/, dann die Bibliotheken
    flags += ["-I", str(SIM / "shim"), "-I", str(SRC), "-I", str(SIM)]
    flags += ["-I", str(libs / "FastLED/src")] + sum((["-I", str(libs / d)] for d in LIB_INCLUDES[1:]), [])
    # "-include FastLED.h": diese (unsere!) Datei wird vor jeder Quelldatei eingelesen; Begründung in sim/shim/FastLED.h
    flags += ["-include", "FastLED.h"]
    return flags


def sources_for(env):
    """Liste der zu übersetzenden Dateien: (Pfad, ist_bibliothek)."""
    libs = ROOT / ".pio" / "libdeps" / env
    files = [(p, False) for p in sorted(SRC.glob("*.cpp")) if p.name not in SRC_EXCLUDE]
    # simApi/simShim: Schnittstelle + Ersatz-Objekte; simPalettes: Umweg für FastLEDs colorpalettes.cpp (Erklärung dort)
    files += [(SIM / "simApi.cpp", False), (SIM / "simShim.cpp", False), (SIM / "simPalettes.cpp", True)]
    files += [(libs / rel, True) for rel in LIB_SOURCES]
    return files


def deps_of(dfile):
    """Liest eine von g++ -MMD geschriebene .d-Datei: alle Dateien, von denen die .o-Datei abhängt."""
    try:
        text = dfile.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return []
    text = text.replace("\\\r\n", " ").replace("\\\n", " ")
    # Format: "ziel.o: abhängigkeit1 abhängigkeit2 ..." (Leerzeichen in Pfaden sind mit Backslash geschützt)
    _, _, rest = text.partition(": ")
    parts, cur = [], ""
    toks = rest.split(" ")
    for t in toks:
        if cur:
            cur += " " + t
        else:
            cur = t
        if cur.endswith("\\"):
            cur = cur[:-1]
            continue
        if cur.strip():
            parts.append(cur.strip())
        cur = ""
    return [Path(p) for p in parts]


def up_to_date(obj, dfile, extra_mtime):
    """True, wenn obj neuer ist als Quelle, alle eingebundenen Header und die Build-Optionen (extra_mtime)."""
    if not obj.exists() or not dfile.exists():
        return False
    t = obj.stat().st_mtime
    if t < extra_mtime:
        return False
    for d in deps_of(dfile):
        try:
            if d.stat().st_mtime > t:
                return False
        except OSError:
            return False
    return True


def build_device(env, jobs, clean):
    gpp = find_gpp()
    objdir = BIN / f"obj_{env}"
    if clean and objdir.exists():
        shutil.rmtree(objdir)
    objdir.mkdir(parents=True, exist_ok=True)
    flags = compile_flags(env)
    # Ändern sich die Optionen (z. B. ein Define in platformio.ini), muss alles neu übersetzt werden:
    # wir merken uns sie in einer Datei und vergleichen.
    flagfile = objdir / "flags.txt"
    flagtext = " ".join(flags)
    if not flagfile.exists() or flagfile.read_text(encoding="utf-8") != flagtext:
        for o in objdir.glob("*.o"):
            o.unlink()
        flagfile.write_text(flagtext, encoding="utf-8")
    extra_mtime = max(Path(__file__).stat().st_mtime, (ROOT / "platformio.ini").stat().st_mtime)

    todo, objs = [], []
    for src, is_lib in sources_for(env):
        obj = objdir / (src.stem + ".o")
        dfile = obj.with_suffix(".d")
        objs.append(obj)
        if not up_to_date(obj, dfile, extra_mtime):
            todo.append((src, obj, dfile, is_lib))

    print(f"[{env}] {len(objs)} Dateien, davon {len(todo)} zu übersetzen ...", flush=True)
    t0 = time.time()
    failed = []

    def compile_one(item):
        src, obj, dfile, is_lib = item
        # -MMD -MF: Abhängigkeitsliste schreiben (für das nächste Mal); -w bei Bibliotheken: deren Warnungen sind nicht unsere
        cmd = [gpp, *flags, "-MMD", "-MF", str(dfile), "-c", str(src), "-o", str(obj)]
        if is_lib:
            cmd.insert(1, "-w")
        r = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace")
        if r.returncode != 0:
            failed.append(src.name)
            print(f"FEHLER in {src.name}:\n{r.stdout}{r.stderr}", flush=True)
        else:
            print(f"  ok {src.name}", flush=True)

    with ThreadPoolExecutor(max_workers=jobs) as pool:
        list(pool.map(compile_one, todo))
    if failed:
        sys.exit(f"[{env}] Übersetzen fehlgeschlagen: {', '.join(failed)}")

    dll = BIN / f"sim_{env}.dll"
    # Linken: alle .o zur DLL. -static: libstdc++/libgcc/winpthread fest einbauen (keine MinGW-DLLs nötig).
    cmd = [gpp, "-shared", "-O2", "-static-libgcc", "-static-libstdc++", "-static", "-o", str(dll),
           *[str(o) for o in objs]]
    r = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if r.returncode != 0:
        sys.exit(f"[{env}] Linken fehlgeschlagen:\n{r.stdout}{r.stderr}")
    print(f"[{env}] -> {dll.relative_to(ROOT)} ({dll.stat().st_size // 1024} KB, {time.time() - t0:.1f} s)", flush=True)
    return dll


def run_smoke_test(env, dll):
    """Übersetzt sim/test/sim_smoke.cpp und lässt es die DLL prüfen (Song 30 laufen lassen)."""
    gpp = find_gpp()
    exe = BIN / "sim_smoke.exe"
    cmd = [gpp, "-std=gnu++17", "-O2", "-static", "-o", str(exe), str(SIM / "test" / "sim_smoke.cpp")]
    r = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if r.returncode != 0:
        sys.exit(f"Test übersetzen fehlgeschlagen:\n{r.stdout}{r.stderr}")
    print(f"[{env}] Schnelltest ...", flush=True)
    r = subprocess.run([str(exe), str(dll)], text=True)
    if r.returncode != 0:
        sys.exit(f"[{env}] Schnelltest FEHLGESCHLAGEN (Rückgabewert {r.returncode})")
    print(f"[{env}] Schnelltest bestanden", flush=True)


def main():
    ap = argparse.ArgumentParser(description="Baut die Simulator-DLLs (echte Firmware-Quellen als Windows-DLL).")
    ap.add_argument("devices", nargs="*", help=f"Geräte (Standard: alle: {', '.join(DEVICES)})")
    ap.add_argument("--test", action="store_true", help="nach dem Bauen den Schnelltest sim/test/sim_smoke.cpp laufen lassen")
    ap.add_argument("--zeittest", action="store_true", help="nach dem Bauen den Zeit-Test tools/sim_timetest.py laufen lassen (alle generierten Songs gegen songgen.py)")
    ap.add_argument("--clean", action="store_true", help="Zwischendateien löschen, alles neu übersetzen")
    ap.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 4, help="gleichzeitige Übersetzungen")
    args = ap.parse_args()

    devices = args.devices or DEVICES
    for d in devices:
        if d not in DEVICES:
            sys.exit(f"unbekanntes Gerät '{d}' (im Simulator gibt es: {', '.join(DEVICES)})")
    BIN.mkdir(parents=True, exist_ok=True)
    for d in devices:
        dll = build_device(d, args.jobs, args.clean)
        if args.test:
            run_smoke_test(d, dll)
    if args.zeittest:
        # Der Zeit-Test prüft von selbst alle DLLs in sim/bin/ (nicht nur die eben gebauten); Rückgabewert weiterreichen.
        r = subprocess.run([sys.executable, str(ROOT / "tools" / "sim_timetest.py")])
        if r.returncode != 0:
            sys.exit(r.returncode)


if __name__ == "__main__":
    main()
