"""Construye unicamente el banco Fn111A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn111a', 'F111ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/common.cpp', 'source/game_sa/GxtChar.cpp', 'source/game_sa/GxtChar.h', 'source/game_sa/C_PcSave.cpp',
         'source/game_sa/Tasks/TaskCategories.cpp', 'source/game_sa/Tasks/TaskCategories.h',
         'source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp', 'source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.h',
         'source/game_sa/WeaponInfo.cpp', 'source/game_sa/WeaponInfo.h', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn111A/LiveComparison.hpp', 'reversing/Fn111A/Core.hpp', 'reversing/Fn111A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
