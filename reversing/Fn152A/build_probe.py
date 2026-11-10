"""Construye unicamente el banco Fn152A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Entity/Vehicle/Bmx.cpp',
    'source/game_sa/Events/EventKnockOffBike.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoObject.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoObject.h',
    'source/game_sa/ModelIndices.cpp',
    'source/game_sa/Pad.cpp',
    'source/game_sa/Scripts/Scripted2dEffects.cpp',
    'source/game_sa/Scripts/Scripted2dEffects.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn152A/LiveComparison.hpp',
    'reversing/Fn152A/Core.hpp',
    'reversing/Fn152A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn152a', 'F152ALive', SOURCES)))
