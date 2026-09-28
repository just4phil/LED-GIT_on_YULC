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

Import("env")  # noqa: F821 (von PlatformIO bereitgestellt)

version = os.environ.get("FW_VERSION") or str(int(time.time()))
try:
    git = subprocess.check_output(
        ["git", "rev-parse", "--short", "HEAD"], cwd=env["PROJECT_DIR"], text=True  # noqa: F821
    ).strip()
    if subprocess.call(["git", "diff", "--quiet", "HEAD"], cwd=env["PROJECT_DIR"]) != 0:  # noqa: F821
        git += "+"  # ungespeicherte Änderungen
except Exception:
    git = "unknown"

print(f"fw_version: FW_VERSION={version} FW_GIT={git}")


def add_version(env, node):
    if not node.get_path().replace("\\", "/").endswith("src/otaUpdate.cpp"):
        return node
    local = env.Clone()
    local.Append(CPPDEFINES=[("FW_VERSION", version + "UL"), ("FW_GIT", env.StringifyMacro(git))])
    return local.Object(node)


env.AddBuildMiddleware(add_version)  # noqa: F821
