"""Construye unicamente el banco Fn157A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Events/EventPlayerCommandToGroup.cpp',
    'source/game_sa/Events/EventPlayerCommandToGroup.h',
    'source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp',
    'source/game_sa/Events/EventPlayerCommandToGroupAttack.h',
    'source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.cpp',
    'source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn157A/LiveComparison.hpp',
    'reversing/Fn157A/Core.hpp',
    'reversing/Fn157A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn157a', 'F157ALive', SOURCES)))
