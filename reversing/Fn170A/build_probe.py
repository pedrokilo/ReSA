"""Construye unicamente el banco Fn170A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/NodeRoute.cpp',
    'source/game_sa/NodeRoute.h',
    'source/game_sa/PatrolRoute.cpp',
    'source/game_sa/PatrolRoute.h',
    'source/game_sa/PedGeometryAnalyser.cpp',
    'source/game_sa/PedGeometryAnalyser.h',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp',
    'source/game_sa/WeaponInfo.cpp',
    'source/game_sa/WeaponInfo.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn170A/LiveComparison.hpp',
    'reversing/Fn170A/Core.hpp',
    'reversing/Fn170A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn170a', 'F170ALive', SOURCES)))
