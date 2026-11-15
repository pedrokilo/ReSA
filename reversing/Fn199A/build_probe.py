"""Construye unicamente el banco Fn199A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Cam.cpp',
    'source/game_sa/Camera.cpp',
    'source/game_sa/Entity/Ped/Ped.cpp',
    'source/game_sa/Pools/PedPool.h',
    'source/game_sa/Events/EventChatPartner.cpp',
    'source/game_sa/Events/EventChatPartner.h',
    'source/game_sa/Events/EventPedEnteredMyVehicle.cpp',
    'source/game_sa/Events/EventPedEnteredMyVehicle.h',
    'source/game_sa/Events/EventPedToChase.cpp',
    'source/game_sa/Events/EventPedToChase.h',
    'source/game_sa/Events/EventPedToFlee.cpp',
    'source/game_sa/Events/EventPedToFlee.h',
    'source/game_sa/Events/EventSexyPed.cpp',
    'source/game_sa/Events/EventSexyPed.h',
    'source/game_sa/Events/EventSexyVehicle.cpp',
    'source/game_sa/Events/EventSexyVehicle.h',
    'source/game_sa/Events/EventVehicleHitAndRun.cpp',
    'source/game_sa/Events/EventVehicleHitAndRun.h',
    'source/game_sa/HandShaker.cpp',
    'source/game_sa/HandShaker.h',
    'source/game_sa/IdleCam.cpp',
    'source/game_sa/IdleCam.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn199A/LiveComparison.hpp',
    'reversing/Fn199A/Core.hpp',
    'reversing/Fn199A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn199a', 'F199ALive', SOURCES)))
