"""Construye unicamente el banco Fn164A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Entity/Vehicle/Bike.cpp',
    'source/game_sa/Entity/Vehicle/Bike.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn164A/LiveComparison.hpp',
    'reversing/Fn164A/Core.hpp',
    'reversing/Fn164A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn164a', 'F164ALive', SOURCES)))
