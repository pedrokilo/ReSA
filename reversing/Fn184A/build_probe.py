"""Construye unicamente el banco Fn184A (ASI de sonda); no acredita aprobacion. Ver README.md."""
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
    'source/game_sa/Events/EventAreaCodes.cpp',
    'source/game_sa/Events/EventDanger.cpp',
    'source/game_sa/Events/EventFireNearby.cpp',
    'source/game_sa/Events/EventGunShotWhizzedBy.cpp',
    'source/game_sa/Events/EventInteriorUseInfo.cpp',
    'source/game_sa/Events/EventInteriorUseInfo.h',
    'source/game_sa/Events/EventPlayerCommandToGroup.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoFire.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoFire.h',
    'source/game_sa/Events/EventSeenPanickedPed.cpp',
    'source/game_sa/Fx/Fx.cpp',
    'source/game_sa/Fx/Fx.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWander.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn184A/LiveComparison.hpp',
    'reversing/Fn184A/Core.hpp',
    'reversing/Fn184A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn184a', 'F184ALive', SOURCES)))
