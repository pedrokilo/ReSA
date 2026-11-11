"""Construye unicamente el banco Fn160A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/PathFind.cpp',
    'source/game_sa/PathFind.h',
    'source/game_sa/Events/EventPedCollisionWithPed.cpp',
    'source/game_sa/Events/EventPedCollisionWithPed.h',
    'source/game_sa/Events/EventPedCollisionWithPlayer.cpp',
    'source/game_sa/Events/EventPedCollisionWithPlayer.h',
    'source/game_sa/Events/EventPlayerCollisionWithPed.cpp',
    'source/game_sa/Events/EventPlayerCollisionWithPed.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexJump.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn160A/LiveComparison.hpp',
    'reversing/Fn160A/Core.hpp',
    'reversing/Fn160A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn160a', 'F160ALive', SOURCES)))
