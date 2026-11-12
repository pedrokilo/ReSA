"""Construye unicamente el banco Fn171A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp',
    'source/game_sa/VehicleAnimGroupData.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn171A/LiveComparison.hpp',
    'reversing/Fn171A/Core.hpp',
    'reversing/Fn171A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn171a', 'F171ALive', SOURCES)))
