"""Construye y (opcionalmente) ejecuta el banco fuera del juego de CAutomobile::PreRender (0x6AAB50).

  python -B reversing/AutoPreRender60/offgame/build_offgame.py [--run] [--cases N] [--first K] [--out DIR] [--conan RAIZ]

1. Extrae de source/game_sa/Entity/Vehicle/Automobile.cpp (el arbol donde se ejecuta el script) el bloque `CAutomobile::PreRender` (desde el comentario
   "CAutomobile::PreRender (0x6AAB50..0x6ACCBA)" hasta el cierre de la funcion; autocontenido salvo los ayudantes AutoPre* de X87Math) a <out>/extract.inc.
2. Compila harness.cpp con las opciones del proyecto (Debug, x86, /MTd /Od /RTC1 /arch:SSE2) y X87Math.cpp con las suyas (/O2 /Oi /arch:IA32 /fp:fast); enlaza harness.exe en 0x20000000 fijo
   (la zona 0x400000..0xCB1000 debe estar libre para mapear el EXE).
3. Con --run ejecuta harness.exe. Solo LEE el gta-sa.exe canonico; no toca el juego ni el clon de pruebas, y no ejecuta GTA.
"""
import argparse, os, re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
INCLUDES = ["", "toolsmenu", "game_sa", "game_sa/Debug", "game_sa/Animation", "game_sa/Attractors", "game_sa/Audio", "game_sa/Audio/Config", "game_sa/Audio/Entities",
    "game_sa/Audio/Hardware", "game_sa/Audio/Loaders", "game_sa/Audio/Managers", "game_sa/Audio/Utility", "game_sa/Collision", "game_sa/Conversations", "game_sa/Core",
    "game_sa/Entity", "game_sa/Entity/Dummy", "game_sa/Entity/Object", "game_sa/Entity/Ped", "game_sa/Entity/Vehicle", "game_sa/Enums", "game_sa/Events", "game_sa/Frontend",
    "game_sa/Fx", "game_sa/Fx/Blueprints", "game_sa/Fx/Blueprints/Interp", "game_sa/Interior", "game_sa/Memory", "game_sa/Models", "game_sa/Plant", "game_sa/Plugins",
    "game_sa/Pipelines", "game_sa/Pipelines/CustomBuilding", "game_sa/Pipelines/CustomCarEnvMap", "game_sa/RenderWare", "game_sa/RenderWare/rw", "game_sa/Scripts", "game_sa/Tasks",
    "game_sa/Tasks/TaskTypes", "game_sa/Tasks/TaskTypes/SeekEntity", "app", "app/imgui", "app/platform", "app/platform/android", "app/platform/win",
    "app/platform/win/VideoPlayer", "app/platform/linux", "app/securom", "oswrapper", "libs/imgui"]
EXTERNAL = ["nlohmd014ef7748f4b/p", "b/ogg21219f4658423/p", "b/spdlo8029646db848c/p", "b/tracy2fefc9668738f/p", "b/vorbi9990e9d508d57/p", "b/imgui5396f0f7f8179/p",
    "b/libjpbe5e6c22fbdf6/p", "b/sdlc3cf2c66533bd/p"]
DEFINES = ["WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX", "EXTRA_DEBUG_FEATURES", "FIX_BUGS", "NOTSA_WINDOWED_MODE", "SA_SKINNED_PEDS", "USE_D3D9", "USE_DSOUND",
    "DIRECTINPUT_VERSION=0x0800", "NOTSA_USE_SDL3", "SPDLOG_USE_STD_FORMAT", "SPDLOG_COMPILED_LIB", "TRACY_ENABLE", "_MBCS", "gta_reversed_EXPORTS"]
VCVARS = r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat"


def extract(out: Path) -> Path:
    text = (ROOT / "source/game_sa/Entity/Vehicle/Automobile.cpp").read_text(encoding="utf-8")
    marker = text.index("// CAutomobile::PreRender (0x6AAB50..0x6ACCBA)")
    start = text.rfind("\n", 0, marker - 1) + 1   # linea de guiones anterior
    head = text.index("void CAutomobile::PreRender() {", start)
    end = text.index("\n}\n", head) + 3
    path = out / "extract.inc"
    path.write_text(text[start:end], encoding="utf-8")
    return path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=str(HERE / "build"))
    ap.add_argument("--conan", default=r"D:\Original\gta-reversed-master", help="raiz con reversing/CVehicle/conan-cache (los worktrees no la tienen)")
    ap.add_argument("--run", action="store_true")
    ap.add_argument("--cases", type=int, default=8192)
    ap.add_argument("--first", type=int, default=0)
    ap.add_argument("--exe", default=r"D:\Grand Theft Auto San Andreas\gta-sa.exe")
    a = ap.parse_args()
    out = Path(a.out)
    out.mkdir(parents=True, exist_ok=True)
    extract(out)
    inc = [str(ROOT / "source" / i).replace("/", "\\") for i in INCLUDES] + [str(out)] + [str(Path(a.conan) / "reversing/CVehicle/conan-cache/p" / e / "include") for e in EXTERNAL]
    common = ["/nologo", "/c", "/std:c++latest", "/EHsc", "/W3", "/utf-8", "/bigobj", "/MTd", "/Zc:preprocessor", "/Zi", "/FS", "/wd4200", "/wd4251", "/wd4275", "/wd4996", "/wd4005", "/wd4099"]
    common += ["/D" + d for d in DEFINES] + ["/I" + i for i in inc]
    harness_flags = common + ["/Od", "/RTC1", "/arch:SSE2", "/Fo" + str(out / "harness.obj"), "/Fd" + str(out / "harness.pdb"), str(HERE / "harness.cpp")]
    x87_flags = common + ["/O2", "/Oi", "/arch:IA32", "/fp:fast", "/Fo" + str(out / "X87Math.obj"), "/Fd" + str(out / "x87.pdb"), str(ROOT / "source/game_sa/X87Math.cpp")]
    rsp = out / "cl_harness.rsp"
    rsp.write_text("\n".join('"%s"' % f if " " in f else f for f in harness_flags), encoding="utf-8")
    rsp2 = out / "cl_x87.rsp"
    rsp2.write_text("\n".join('"%s"' % f if " " in f else f for f in x87_flags), encoding="utf-8")
    bat = out / "build.bat"
    bat.write_text("@echo off\r\ncall \"%s\" >nul 2>&1\r\ncl @%s\r\nif errorlevel 1 exit /b 1\r\ncl @%s\r\nif errorlevel 1 exit /b 1\r\n"
                   "link /nologo /DEBUG /OUT:%s /BASE:0x20000000 /FIXED /DYNAMICBASE:NO /SUBSYSTEM:CONSOLE /NODEFAULTLIB:MSVCRT /FORCE:UNRESOLVED %s %s kernel32.lib user32.lib > %s 2>&1\r\n"
                   % (VCVARS, rsp, rsp2, out / "harness.exe", out / "harness.obj", out / "X87Math.obj", out / "link.log"), encoding="ascii")
    r = subprocess.run(["cmd", "/c", str(bat)], cwd=str(out))
    if r.returncode:
        sys.exit(r.returncode)
    # /FORCE:UNRESOLVED: las clases con __declspec(dllexport) piden miembros que este banco no enlaza (nunca se ejecutan); solo importa que el .exe exista
    unresolved = sum(1 for l in (out / "link.log").read_text(encoding="latin-1").splitlines() if "LNK2019" in l or "LNK2001" in l)
    print("enlace: %d simbolos sin resolver (esperado, ver link.log) -> %s" % (unresolved, out / "harness.exe"))
    if a.run:
        sys.exit(subprocess.run([str(out / "harness.exe"), str(a.cases), str(a.first), a.exe]).returncode)


if __name__ == "__main__":
    main()
