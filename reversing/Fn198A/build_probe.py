"""Construye unicamente el banco Fn198A (ASI de sonda); no acredita aprobacion. Ver README.md."""
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
    'source/game_sa/Collision/BoundingBox.cpp',
    'source/game_sa/Collision/BoundingBox.h',
    'source/game_sa/Entity/Ped/Ped.cpp',
    'source/game_sa/Events/EventScanner.cpp',
    'source/game_sa/Events/EventScanner.h',
    'source/game_sa/ObjectScanner.cpp',
    'source/game_sa/ObjectScanner.h',
    'source/game_sa/Streaming.cpp',
    'source/game_sa/Streaming.h',
    'source/game_sa/Tasks/TaskComplexSequence.cpp',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillX87.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePause.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
    'source/game_sa/VehicleScanner.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn198A/LiveComparison.hpp',
    'reversing/Fn198A/Core.hpp',
    'reversing/Fn198A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn198a', 'F198ALive', SOURCES)))
