"""Construye unicamente el banco Fn107A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
EVENTS = ['Event', 'EventEditableResponse', 'EventAcquaintancePed', 'EventVehicleDamage', 'EventAreaCodes', 'EventGunShot', 'EventPedCollisionWithPed', 'EventCopCarBeingStolen', 'EventVehicleOnFire',
          'EventAttractor', 'EventVehicleToSteal', 'EventVehicleDied', 'EventSeenPanickedPed', 'EventPlayerCommandToGroup', 'EventCarUpsideDown', 'EventVehicleCollision', 'EventObjectCollision',
          'EventBuildingCollision', 'EventPotentialWalkIntoVehicle', 'EventPotentialWalkIntoObject', 'EventStuckInAir', 'EventDanger', 'EventPotentialWalkIntoPed', 'EventVehicleHitAndRun']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn107a', 'F107ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h', 'source/game_sa/common.cpp']
        + ['source/game_sa/Events/%s.cpp' % e for e in EVENTS]
        + ['reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn107A/LiveComparison.hpp', 'reversing/Fn107A/Core.hpp', 'reversing/Fn107A/Fixtures.hpp',
           'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
