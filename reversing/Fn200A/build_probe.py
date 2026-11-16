"""Construye unicamente el banco Fn200A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Collision/BoundingBox.cpp',
    'source/game_sa/Collision/ColPoint.cpp',
    'source/game_sa/Core/MatrixLinkList.cpp',
    'source/game_sa/Core/PtrNodeSingleLink.cpp',
    'source/game_sa/Core/PtrNodeSingleLink.h',
    'source/game_sa/Core/Vector.cpp',
    'source/game_sa/Entity/Placeable.cpp',
    'source/game_sa/Entity/Vehicle/Train.cpp',
    'source/game_sa/Entity/Vehicle/Train.h',
    'source/game_sa/Pools/PtrNodeSingleLinkPool.h',
    T + 'TaskComplexFallAndGetUp.cpp',
    T + 'TaskComplexFallAndGetUp.h',
    T + 'TaskComplexFallToDeath.cpp',
    T + 'TaskComplexFallToDeath.h',
    T + 'TaskComplexGetUpAndStandStill.cpp',
    T + 'TaskComplexGetUpAndStandStill.h',
    T + 'TaskComplexInAirAndLand.cpp',
    T + 'TaskComplexInAirAndLand.h',
    T + 'TaskComplexStuckInAir.cpp',
    T + 'TaskSimpleFall.cpp',
    T + 'TaskSimpleFall.h',
    T + 'TaskSimpleGetUp.cpp',
    T + 'TaskSimpleGetUp.h',
    T + 'TaskSimpleInAir.cpp',
    T + 'TaskSimpleInAir.h',
    T + 'TaskSimpleLand.cpp',
    T + 'TaskSimpleLand.h',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn200A/LiveComparison.hpp',
    'reversing/Fn200A/Core.hpp',
    'reversing/Fn200A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn200a', 'F200ALive', SOURCES)))
