"""Construye unicamente el banco Fn191A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/platform/platform.cpp',
    'source/app/platform/platform.h',
    'source/app/platform/preset_view.cpp',
    'source/game_sa/Entity/Physical.cpp',
    'source/game_sa/EntryExitManager.cpp',
    'source/game_sa/EntryExitManager.h',
    'source/game_sa/Pad.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn191A/LiveComparison.hpp',
    'reversing/Fn191A/Core.hpp',
    'reversing/Fn191A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp'
]
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn191a', 'F191ALive', SOURCES)))
