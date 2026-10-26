"""Construye unicamente el banco PickUpFirst68 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'pickupfirst68', 'PickUp68Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp',
         'source/game_sa/Entity/Placeable.cpp',
         'source/game_sa/SimpleTransform.cpp',
         'source/game_sa/Core/Matrix.cpp',
         'source/game_sa/Entity/Entity.cpp',
         'source/game_sa/Tasks/Task.cpp',
         'source/game_sa/X87Math.cpp',
         'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PickUpFirst68/LiveComparison.hpp', 'reversing/PickUpFirst68/Core.hpp', 'reversing/PickUpFirst68/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
