"""Construye unicamente el banco Fn213A (ASI de sonda); no acredita aprobacion. Ver README.md."""
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
    'source/game_sa/Pools.cpp',
    'source/game_sa/Cover.cpp',
    'source/game_sa/Cover.h',
    'source/game_sa/Core/PtrList.h',
    'source/game_sa/Replay.cpp',
    'source/game_sa/Replay.h',
    'source/game_sa/ReplayBlockType.cpp',
    'source/game_sa/ReplayBlockType.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn213A/LiveComparison.hpp',
    'reversing/Fn213A/Core.hpp',
    'reversing/Fn213A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn213a', 'F213ALive', SOURCES)))
