"""Construye unicamente el banco Fn156A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/platform/win/VideoPlayer/VideoPlayer.cpp',
    'source/app/platform/win/VideoPlayer/VideoPlayer.h',
    'source/game_sa/Events/Event.h',
    'source/game_sa/Events/EventEditableResponse.h',
    'source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp',
    'source/game_sa/Events/EventLeaderExitedCarAsDriver.h',
    'source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp',
    'source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleJump.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn156A/LiveComparison.hpp',
    'reversing/Fn156A/Core.hpp',
    'reversing/Fn156A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn156a', 'F156ALive', SOURCES)))
