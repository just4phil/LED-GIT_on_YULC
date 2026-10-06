"""
fw_version.py - PlatformIO pre-Script: setzt FW_VERSION und FW_GIT für src/otaUpdate.cpp

FW_VERSION = Unix-Zeit des Builds (monoton steigend, einfacher Zahlenvergleich im OTA-Check).
tools/build_ota.py gibt per Umgebungsvariable FW_VERSION eine gemeinsame Version für alle
Geräte vor; ohne die Variable nimmt jeder Build die aktuelle Zeit.

Die Defines gelten nur für otaUpdate.cpp, damit eine neue Version nicht das ganze
Projekt neu kompiliert.
"""
import os
import subprocess
import time

# Dieses Skript wird nicht von Hand gestartet: PlatformIO führt es vor jedem Build aus (Eintrag "extra_scripts"
# in platformio.ini). "env" ist die Build-Umgebung, die PlatformIO dafür bereitstellt.
Import("env")  # noqa: F821 (von PlatformIO bereitgestellt)

# Version: von build_ota.py vorgegeben, sonst die jetzige Zeit in Sekunden seit 1970
version = os.environ.get("FW_VERSION") or str(int(time.time()))
# Kurze Kennung des Git-Stands, nur zur Anzeige ("+" = es gibt nicht committete Änderungen)
try:
    git = subprocess.check_output(
        ["git", "rev-parse", "--short", "HEAD"], cwd=env["PROJECT_DIR"], text=True  # noqa: F821
    ).strip()
    if subprocess.call(["git", "diff", "--quiet", "HEAD"], cwd=env["PROJECT_DIR"]) != 0:  # noqa: F821
        git += "+"  # ungespeicherte Änderungen
except Exception:
    git = "unknown"

print(f"fw_version: FW_VERSION={version} FW_GIT={git}")


# Wird von PlatformIO für JEDE Quelldatei aufgerufen. Nur für src/otaUpdate.cpp werden die beiden Werte als
# #define mitgegeben (wie eine Zeile "#define FW_VERSION 1759740000UL" am Dateianfang); alle anderen Dateien
# bleiben unberührt und müssen deshalb bei einer neuen Version nicht neu übersetzt werden.
def add_version(env, node):
    if not node.get_path().replace("\\", "/").endswith("src/otaUpdate.cpp"):
        return node
    local = env.Clone()
    local.Append(CPPDEFINES=[("FW_VERSION", version + "UL"), ("FW_GIT", env.StringifyMacro(git))])
    return local.Object(node)


env.AddBuildMiddleware(add_version)  # noqa: F821
