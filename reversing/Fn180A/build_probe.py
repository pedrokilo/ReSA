"""Construye unicamente el banco Fn180A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/Hardware/AEAudioChannel.cpp',
    'source/game_sa/Audio/Hardware/AEAudioChannel.h',
    'source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp',
    'source/game_sa/Entity/Object/HandObject.cpp',
    'source/game_sa/Entity/Object/HandObject.h',
    'source/game_sa/Entity/Object/Object.cpp',
    'source/game_sa/Entity/Object/Object.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn180A/LiveComparison.hpp',
    'reversing/Fn180A/Core.hpp',
    'reversing/Fn180A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn180a', 'F180ALive', SOURCES)))
