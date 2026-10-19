"""Construye unicamente el banco WalkRoundFirst31 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'walkroundfirst31', 'WalkRoundFirst31Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.h',
         'source/game_sa/Tasks/TaskComplex.h',
         'source/game_sa/Tasks/Task.cpp',
         'source/game_sa/Core/Pool.h',
         'source/game_sa/X87Math.cpp',
         'source/game_sa/X87Math.h',
         'source/game_sa/Entity/Placeable.cpp',
         'source/game_sa/Entity/Placeable.h',
         'source/game_sa/SimpleTransform.cpp',
         'source/game_sa/SimpleTransform.h',
         'source/game_sa/Core/Vector.cpp',
         'source/game_sa/Core/Vector.h',
         'source/game_sa/General.cpp',
         'source/game_sa/General.h',
         'source/game_sa/PedGeometryAnalyser.h',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h',
         'source/game_sa/Timer.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h',
         'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h',
         'source/reversiblehooks/ReversibleHook/Virtual.cpp',
         'reversing/WalkRoundFirst31/LiveComparison.hpp',
         'reversing/WalkRoundFirst31/Core.hpp',
         'reversing/WalkRoundFirst31/Fixtures.hpp'])))
