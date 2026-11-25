#!/usr/bin/env python3
"""sim_timetest.py - Zeit-Test des LED-Simulators: kommen die Part-Wechsel zu den Zeiten, die songgen.py berechnet?

Aufruf (aus dem Projektordner):
    tools/.venv/Scripts/python tools/sim_timetest.py            # alle generierten Songs, alle vorhandenen Geräte-DLLs
    tools/.venv/Scripts/python tools/sim_timetest.py Maniac_v1  # nur ein Song (Ordnername unter songs/)
    tools/.venv/Scripts/python tools/sim_timetest.py --geraet scrollmatrix --toleranz 4
oder gebaut + getestet in einem Zug:  python tools/build_sim.py --zeittest

Was der Test macht (je Song und je Gerät, ohne Fenster):
  1. Die DLL des Geräts (sim/bin/sim_<gerät>.dll) wird in einen Temp-Ordner KOPIERT und frisch geladen. Das ist nötig,
     weil manche Effekte versteckte "static"-Variablen haben, die sim_reset() nicht zurücksetzen kann: eine frisch
     geladene DLL verhält sich wie ein frisch eingeschaltetes Gerät. Jeder Song läuft in einem eigenen Prozess.
  2. sim_reset(), dann MIDI Kanal 10, CC 22 = Song-ID (wie von der Gitarre per Funk).
  3. Die Zeit wird in 2-ms-Schritten vorgestellt (sim_advance(2)). Jede Änderung von prog (= Part-Nummer, "case")
     wird mit der Zeit in ms seit CC 22 notiert, bis der Song zu Ende ist (die Song-ID ändert sich: Song 0 bzw.
     bei einem Trailer der Ziel-Song).
  4. Vergleich mit "songgen.py <Song> --json": für jeden besuchten case die berechnete Startzeit ("start").
     Erlaubt sind +-TOLERANZ ms (Standard 4 = zwei Timer-Takte; mehr wäre eine Verspätung, die sich über den
     Song summiert, also ein Fehler des Verspätungsausgleichs in showEngine_step()).

Sonderfälle der Matrix (Titel-Lauftext, "scroll" in der JSON-Ausgabe):
  - Die Matrix überspringt Parts, die der Lauftext überdeckt, und hat dafür ZUSATZ-cases: "fill_case" (Rest des
    Parts, in dem der Lauftext noch läuft, Start = scroll.fill_start). Diese cases stehen nicht in parts[] und
    werden deshalb gegen scroll.<GERÄT>.fill_start geprüft (siehe aber auch "wait"); der Einstieg "join_case" ist ein normaler Part
    (Zeit = parts[].start). Parts, die ein Gerät gar nicht besucht, sind keine Abweichung.
  - "wait"-Variante: der Zusatz-case (Lauftext verzögert starten) beginnt bei scroll.<GERÄT>.wait ms; der erste
    unbekannte case wird gegen diese Zeit geprüft.
  - Ein besuchter case, den weder parts[] noch scroll kennt, wird als FEHLER gemeldet.
Ausgabe: je Song die größte Abweichung; bei Fehlern die ersten fünf. Rückgabewert 0 = alles grün, 1 = Fehler.
"""
import argparse
import ctypes
import json
import multiprocessing
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BIN = ROOT / "sim" / "bin"
SONGGEN = ROOT / "tools" / "songgen.py"
STEP_MS = 2          # so groß ist ein Zeitschritt des Tests (= ein Timer-Takt der Firmware)
MAX_END_LATE_MS = 25  # so spät darf das Songende höchstens bemerkt werden (ein Bild: 540 LEDs = 16,3 ms, plus Takt)
MAX_EXTRA_MS = 8000  # so lange nach dem berechneten Songende wird noch gewartet, bevor der Song als "hängt" gilt


def songgen_json(*args):
    """Ruft songgen.py mit --json auf und liefert die ausgewertete JSON-Antwort (Dictionary)."""
    env = dict(os.environ, PYTHONIOENCODING="utf-8")
    r = subprocess.run([sys.executable, str(SONGGEN), *args, "--json"], capture_output=True, text=True,
                       encoding="utf-8", env=env, cwd=str(ROOT))
    if r.returncode != 0:
        raise RuntimeError(f"songgen.py {' '.join(args)} --json: {r.stderr.strip() or r.stdout.strip()}")
    return json.loads(r.stdout)


def run_song(job):
    """Läuft in einem eigenen Prozess: einen Song auf einer frischen DLL-Kopie abspielen und die Part-Wechsel notieren.
    job = (dll_pfad, song_id, ende_ms). Rückgabe: (Liste [(bemerkt_ms, case, Beginn_ms), ...], Ende erreicht?, Endzeit_ms)."""
    dll_path, song_id, end_ms = job
    # Frische Kopie laden (jeder Prozess eine eigene, damit nichts von einem früheren Lauf übrig ist)
    tmp = Path(tempfile.mkdtemp(prefix="simtest_"))
    copy = tmp / f"sim_{os.getpid()}.dll"
    shutil.copy2(dll_path, copy)
    dll = ctypes.CDLL(str(copy))
    song = ctypes.c_int(); prog = ctypes.c_int(); part_ms = ctypes.c_uint()
    dll.sim_reset(ctypes.c_uint(1))
    dll.sim_midi(10, 22, song_id)
    t = 0
    changes = []
    last = None
    ended = False
    limit = end_ms + MAX_EXTRA_MS
    while t <= limit:
        dll.sim_advance(STEP_MS)
        t += STEP_MS
        dll.sim_state(ctypes.byref(song), ctypes.byref(prog), ctypes.byref(part_ms))
        if song.value != song_id:       # Songende (Song 0 oder Ziel-Song eines Trailers)
            ended = True
            break
        if prog.value != last:
            # t = wann die Schleife den Wechsel bemerkt hat (wie das Auge: erstes Bild des neuen Parts);
            # t - part_ms = wann der Part nach SEINER Uhr begonnen hat (millisCounterForProgChange zählt vom
            # Part-Beginn; der Verspätungsausgleich in showEngine_step() schreibt die Verspätung dort gut).
            changes.append((t, prog.value, t - part_ms.value))
            last = prog.value
    return changes, ended, t


def expected_times(info, device):
    """Aus der JSON-Ausgabe von songgen.py: ({case: berechnete Startzeit in ms}, Songende in ms, Zeit des Lauftext-Zusatz-cases oder None)."""
    exp = {p["case"]: p["start"] for p in info["parts"]}
    scroll = (info.get("scroll") or {}).get(device.upper())
    extra = None    # Zeit des Zusatz-cases (Titel-Lauftext): "fill" = Rest-Part ab fill_start, "wait" = Warte-Part der Länge wait
    if scroll:
        extra = scroll.get("fill_start") if scroll.get("mode") == "fill" else scroll.get("wait")
    last = info["parts"][-1]
    return exp, last["start"] + last["dur"], extra


def check(device, entry, info, result, tol):
    """Vergleicht die gemessenen Wechsel mit den berechneten.
    Rückgabe (größte Abweichung des Part-Beginns, größte Verspätung des ersten Bildes, Liste der Fehlertexte).
    Geprüft wird der Beginn nach der Part-Uhr (dritter Wert der Wechsel). Die "Verspätung" (bemerkt - berechnet)
    ist nur eine Anzeige: sie ist höchstens so lang wie ein FastLED.show() und wird im Part wieder aufgeholt."""
    changes, ended, t_end = result
    exp, end, extra = expected_times(info, device)
    errors, worst, late_max = [], 0, 0
    extra_seen = 0
    for t, case, origin in changes:
        if case == 0:          # Part 0 beginnt mit dem Song bei 0 ms
            continue
        if case in exp:
            want = exp[case]
        elif extra is not None and extra_seen == 0:
            # Zusatz-case des Titel-Lauftexts (siehe Kopf der Datei): steht nicht in parts[]
            want = extra
            extra_seen += 1
        else:
            errors.append(f"case {case} bei {t} ms: kommt in der songgen-Ausgabe nicht vor")
            continue
        dev = origin - want
        worst = max(worst, abs(dev))
        late_max = max(late_max, t - want)
        if abs(dev) > tol:
            errors.append(f"case {case}: Part-Beginn {origin} ms, berechnet {want} ms, Abweichung {dev:+d} ms (bemerkt bei {t} ms)")
    if not ended:
        errors.append(f"Song endet nicht (nach {t_end} ms noch Song {entry['id']}), berechnetes Ende {end} ms")
    else:
        dev = t_end - end    # wird erst im Bild danach bemerkt (Song-Wechsel in showEngine_step()): 0..ein Bild später
        late_max = max(late_max, dev)
        if dev < -tol or dev > MAX_END_LATE_MS:
            errors.append(f"Songende: gemessen {t_end} ms, berechnet {end} ms, Abweichung {dev:+d} ms")
    return worst, late_max, errors


def main():
    # Umlaute in der Windows-Konsole: Ausgabe auf UTF-8 umstellen (wie in songgen.py main())
    for stream in (sys.stdout, sys.stderr):
        stream.reconfigure(encoding="utf-8")
    ap = argparse.ArgumentParser(description="Zeit-Test: Part-Wechsel des Simulators gegen die Zeiten von songgen.py")
    ap.add_argument("songs", nargs="*", help="Song-Ordner (Standard: alle generierten Songs)")
    ap.add_argument("--geraet", action="append", help="nur dieses Gerät (Standard: jede DLL in sim/bin/sim_<gerät>.dll)")
    ap.add_argument("--toleranz", type=int, default=4, help="erlaubte Abweichung je Wechsel in ms (Standard 4)")
    ap.add_argument("-j", "--jobs", type=int, default=max(1, (os.cpu_count() or 4) - 1), help="Songs gleichzeitig")
    args = ap.parse_args()

    devices = args.geraet or sorted(p.stem[len("sim_"):] for p in BIN.glob("sim_*.dll"))
    if not devices:
        sys.exit("Keine Simulator-DLL in sim/bin/ - erst  python tools/build_sim.py  ausführen.")
    index = songgen_json()
    entries = [s for s in index["songs"] if s.get("generated")]
    if args.songs:
        entries = [s for s in entries if s["dir"] in args.songs]
        if not entries:
            sys.exit(f"Kein generierter Song mit diesem Namen: {args.songs}")
    infos = {s["dir"]: songgen_json(s["dir"]) for s in entries}

    failed = False
    with multiprocessing.Pool(args.jobs, maxtasksperchild=1) as pool:
        for dev in devices:
            dll = BIN / f"sim_{dev}.dll"
            if not dll.exists():
                sys.exit(f"{dll} fehlt")
            jobs = []
            for s in entries:
                info = infos[s["dir"]]
                _, end, _ = expected_times(info, dev)
                jobs.append((str(dll), s["id"], end))
            results = pool.map(run_song, jobs, chunksize=1)
            print(f"=== Gerät {dev}: {len(entries)} Songs, Toleranz +-{args.toleranz} ms ===")
            for s, res in zip(entries, results):
                worst, late, errors = check(dev, s, infos[s["dir"]], res, args.toleranz)
                print(f"  {'FEHLER' if errors else 'ok    '} {s['dir']:<30} Song {s['id']:>3}: {len(res[0]):>3} Wechsel, größte Abweichung {worst} ms, späteste Bemerkung +{late} ms")
                for e in errors[:5]:
                    print(f"         {e}")
                if errors:
                    failed = True
    print("Zeit-Test " + ("FEHLGESCHLAGEN" if failed else "bestanden"))
    return 1 if failed else 0


if __name__ == "__main__":
    multiprocessing.freeze_support()
    sys.exit(main())
