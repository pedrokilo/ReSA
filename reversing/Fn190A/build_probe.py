"""Construye unicamente el banco Fn190A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Events/EventAcquaintancePedHate.cpp',
    'source/game_sa/Events/EventAcquaintancePedHate.h',
    'source/game_sa/Events/EventAcquaintancePed.cpp',
    'source/game_sa/Events/EventGunShot.cpp',
    'source/game_sa/Events/EventVehicleCollision.cpp',
    'source/game_sa/Events/EventPedCollisionWithPed.cpp',
    'source/game_sa/Events/EventObjectCollision.cpp',
    'source/game_sa/Events/EventBuildingCollision.cpp',
    'source/game_sa/Events/EventDraggedOutCar.cpp',
    'source/game_sa/Events/Event.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
    'source/game_sa/StencilShadows.cpp',
    'source/game_sa/StencilShadowObject.cpp',
    'source/game_sa/StencilShadowObject.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn190A/LiveComparison.hpp',
    'reversing/Fn190A/Core.hpp',
    'reversing/Fn190A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn190a', 'F190ALive', SOURCES)))
