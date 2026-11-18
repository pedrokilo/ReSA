"""Construye unicamente el banco Fn235A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/app.cpp',
    'source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompressUpsample.cpp',
    'source/game_sa/Plugins/JPegCompressPlugin/LibJpegError.cpp',
    'source/game_sa/Plugins/JPegCompressPlugin/LibJpegMemory.cpp',
    'source/game_sa/Plugins/JPegCompressPlugin/LibJpegExe.h',
    'source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn235A/LiveComparison.hpp',
    'reversing/Fn235A/Core.hpp',
    'reversing/Fn235A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn235a', 'F235ALive', SOURCES)))
