"""Construye unicamente el banco Fn125A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Entity/Vehicle/Boat.cpp',
    'source/game_sa/Entity/Vehicle/Boat.h',
    'source/game_sa/Entity/Object/CutsceneObject.cpp',
    'source/game_sa/Entity/Object/CutsceneObject.h',
    'source/game_sa/Entity/Dummy/DummyObject.cpp',
    'source/game_sa/Entity/Dummy/DummyObject.h',
    'source/game_sa/Entity/Vehicle/MonsterTruck.cpp',
    'source/game_sa/Entity/Vehicle/MonsterTruck.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexClimb.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.h',
    'source/game_sa/Tasks/Task.cpp',
    'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'source/game_sa/Entity/Object/Object.cpp',
    'source/game_sa/Entity/Dummy/Dummy.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn125A/LiveComparison.hpp',
    'reversing/Fn125A/Core.hpp',
    'reversing/Fn125A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn125a', 'F125ALive', SOURCES)))
