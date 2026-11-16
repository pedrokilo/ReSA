"""Construye unicamente el banco Fn202A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/AttractorScanner.cpp',
    'source/game_sa/AttractorScanner.h',
    'source/game_sa/Core/Matrix.cpp',
    'source/game_sa/Core/Matrix.h',
    'source/game_sa/Events/EventBuildingCollision.cpp',
    'source/game_sa/Events/EventBuildingCollision.h',
    'source/game_sa/Events/EventDeadPed.cpp',
    'source/game_sa/Events/EventDeath.cpp',
    'source/game_sa/Events/EventDeath.h',
    'source/game_sa/Events/EventDraggedOutCar.cpp',
    'source/game_sa/Events/EventGroupEvent.cpp',
    'source/game_sa/Events/EventGroupEvent.h',
    'source/game_sa/Events/EventObjectCollision.cpp',
    'source/game_sa/Events/EventObjectCollision.h',
    'source/game_sa/Events/EventPedCollisionWithPed.cpp',
    'source/game_sa/Events/EventPedCollisionWithPed.h',
    'source/game_sa/Events/EventPotentialGetRunOver.cpp',
    'source/game_sa/Events/EventPotentialGetRunOver.h',
    'source/game_sa/Events/EventPotentialWalkIntoObject.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoObject.h',
    'source/game_sa/Events/EventPotentialWalkIntoPed.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoPed.h',
    'source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoVehicle.h',
    'source/game_sa/Events/EventVehicleCollision.cpp',
    'source/game_sa/Events/EventVehicleCollision.h',
    'source/game_sa/Interior/InteriorGroup_c.cpp',
    'source/game_sa/Interior/Interior_c.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.h',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn202A/LiveComparison.hpp',
    'reversing/Fn202A/Core.hpp',
    'reversing/Fn202A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn202a', 'F202ALive', SOURCES)))
