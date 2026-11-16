"""Construye unicamente el banco Fn192A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Collision/Box.cpp',
    'source/game_sa/Core/QuadTreeNode.h',
    'source/game_sa/Core/Pool.h',
    'source/game_sa/Core/Rect.cpp',
    'source/game_sa/Entity/Object/Object.cpp',
    'source/game_sa/Entity/Ped/PedSaveStructure.cpp',
    'source/game_sa/Entity/Ped/PlayerPed.cpp',
    'source/game_sa/Entity/Physical.cpp',
    'source/game_sa/Events/EventCarUpsideDown.cpp',
    'source/game_sa/Events/EventCopCarBeingStolen.cpp',
    'source/game_sa/Events/EventGotKnockedOverByCar.cpp',
    'source/game_sa/Events/EventHitByWaterCannon.cpp',
    'source/game_sa/Events/EventInAir.cpp',
    'source/game_sa/Events/EventPotentialWalkIntoFire.cpp',
    'source/game_sa/Events/EventScriptCommand.cpp',
    'source/game_sa/Events/EventScriptCommand.h',
    'source/game_sa/Events/EventStuckInAir.cpp',
    'source/game_sa/Events/EventVehicleDamage.cpp',
    'source/game_sa/Events/EventVehicleDied.cpp',
    'source/game_sa/Events/EventVehicleOnFire.cpp',
    'source/game_sa/Pools/ObjectPool.h',
    'source/game_sa/Renderer.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp',
    'source/game_sa/VehicleSaveStructure.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn192A/LiveComparison.hpp',
    'reversing/Fn192A/Core.hpp',
    'reversing/Fn192A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn192a', 'F192ALive', SOURCES)))
