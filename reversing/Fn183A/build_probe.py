"""Construye unicamente el banco Fn183A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Animation/AnimManager.cpp',
    'source/game_sa/Core/LinkList.h',
    'source/game_sa/Pad.cpp',
    'source/game_sa/Pad.h',
    'source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn183A/LiveComparison.hpp',
    'reversing/Fn183A/Core.hpp',
    'reversing/Fn183A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn183a', 'F183ALive', SOURCES)))
