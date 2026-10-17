"""Construye unicamente el banco Dependencies21E; no acredita aprobacion."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/game_sa/Population.cpp', 'source/game_sa/Population.h',
    'source/game_sa/CullZones.cpp', 'source/game_sa/CullZones.h',
    'source/game_sa/Camera.cpp', 'source/game_sa/Cam.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.h',
    'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
    'source/game_sa/General.cpp', 'source/game_sa/General.h',
    'source/game_sa/Pad.cpp', 'source/game_sa/Pad.h',
    'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__ == '__main__':
    root = Path(__file__).resolve().parents[2]
    absent = [name for name in SOURCES if not (root / name).is_file()]
    if absent:
        raise RuntimeError('Missing manifest source paths: ' + str(absent))
    print(json.dumps(build(Path(__file__).parent, 'dependencies21e', 'Dependencies21ELive', SOURCES)))
