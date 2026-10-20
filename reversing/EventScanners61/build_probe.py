"""Construye unicamente el banco EventScanners61 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'eventscanners61', 'EventScanners61Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/MentalHealth.cpp', 'source/game_sa/MentalHealth.h',
         'source/game_sa/Events/EventScanner.cpp', 'source/game_sa/Events/EventScanner.h',
         'source/game_sa/PedStuckChecker.cpp', 'source/game_sa/PedStuckChecker.h',
         'source/game_sa/Events/EventLowAngerAtPlayer.h', 'source/game_sa/Events/EventHealthLow.h', 'source/game_sa/Events/EventHealthReallyLow.h', 'source/game_sa/Events/EventHighAngerAtPlayer.h',
         'source/game_sa/Events/EventGroup.cpp', 'source/game_sa/Events/EventGroup.h', 'source/game_sa/Events/EventEditableResponse.cpp',
         'source/game_sa/Events/EventInAir.cpp', 'source/game_sa/Events/EventOnFire.cpp', 'source/game_sa/Events/EventAreaCodes.cpp', 'source/game_sa/Events/EventEscalator.cpp',
         'source/game_sa/Events/EventPedEnteredMyVehicle.cpp', 'source/game_sa/Events/EventDamage.cpp', 'source/game_sa/Events/EventStuckInAir.cpp', 'source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp',
         'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Tasks/TaskManager.cpp', 'source/game_sa/Tasks/TaskManager.h',
         'source/game_sa/PedGeometryAnalyser.cpp', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/Core/Vector.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/EventScanners61/LiveComparison.hpp', 'reversing/EventScanners61/Core.hpp', 'reversing/EventScanners61/Fixtures.hpp',
         'reversing/EventScanners61/CaseMental.hpp', 'reversing/EventScanners61/CaseVpc.hpp', 'reversing/EventScanners61/CaseScan.hpp', 'reversing/EventScanners61/CaseStuck.hpp',
         'reversing/EventScanners61/FixVpc.hpp', 'reversing/EventScanners61/FixScan.hpp', 'reversing/EventScanners61/FixStuck.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
