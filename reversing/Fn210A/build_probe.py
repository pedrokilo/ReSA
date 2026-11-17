"""Construye unicamente el banco Fn210A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRouteX87.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h',
    'source/game_sa/Entity/Vehicle/Automobile.cpp',
    'source/game_sa/Entity/Vehicle/Automobile.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.h',
    'source/game_sa/Tasks/TaskComplexSequence.h',
    'source/game_sa/PatrolRoute.h',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn210A/LiveComparison.hpp',
    'reversing/Fn210A/Core.hpp',
    'reversing/Fn210A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn210a', 'F210ALive', SOURCES)))
