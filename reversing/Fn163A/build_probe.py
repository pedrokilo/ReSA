"""Construye unicamente el banco Fn163A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn163A/LiveComparison.hpp',
    'reversing/Fn163A/Core.hpp',
    'reversing/Fn163A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn163a', 'F163ALive', SOURCES)))
