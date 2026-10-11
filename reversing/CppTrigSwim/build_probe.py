"""Compila la sonda temporal del banco de helicoptero, ir a la puerta del coche y nadar (lista de 11):
CHeli, CTaskComplexGoToCarDoorAndStandStill y CTaskSimpleSwim."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/game_sa/Entity/Vehicle/Heli.cpp', 'source/game_sa/Entity/Vehicle/Heli.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h',
]
SOURCES += ['source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h', 'source/CMakeLists.txt', 'CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/reversiblehooks/RootHookCategory.h', 'source/reversiblehooks/ReversibleHook/Base.h', 'reversing/OriginalDependencies499/HookRoute.hpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpptrigswim', 'CppTrigSwimLive', SOURCES)))
