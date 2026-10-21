"""Construye unicamente el banco SmallHelpers64B (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'smallhelpers64b', 'Small64Live',
        ['source/CMakeLists.txt',
         'source/game_sa/GangWars.cpp', 'source/game_sa/WaterLevel.cpp', 'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Tasks/Task.h', 'source/game_sa/common.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/SmallHelpers64B/LiveComparison.hpp', 'reversing/SmallHelpers64B/Core.hpp', 'reversing/SmallHelpers64B/Cases.hpp', 'reversing/SmallHelpers64B/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
