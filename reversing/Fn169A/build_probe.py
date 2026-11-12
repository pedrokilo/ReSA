"""Construye unicamente el banco Fn169A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Events/Event.cpp',
    'source/game_sa/Events/Event.h',
    'source/game_sa/Events/EventEditableResponse.cpp',
    'source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp',
    'source/game_sa/Events/EventLeaderEnteredCarAsDriver.h',
    'source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp',
    'source/game_sa/Events/EventLeaderExitedCarAsDriver.h',
    'source/game_sa/InformFriendsEventQueue.cpp',
    'source/game_sa/Pools/PoolRawDelete.h',
    'source/game_sa/Tasks/Task.cpp',
    'source/game_sa/Tasks/TaskComplex.cpp',
    'source/game_sa/Tasks/TaskComplex.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn169A/LiveComparison.hpp',
    'reversing/Fn169A/Core.hpp',
    'reversing/Fn169A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn169a', 'F169ALive', SOURCES)))
