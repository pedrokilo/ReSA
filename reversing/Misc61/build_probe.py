"""Construye unicamente el banco Misc61 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'misc61', 'Misc61Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Memory/MemoryHeap.cpp', 'source/game_sa/Memory/MemoryHeap.h',
         'source/game_sa/SearchLight.cpp', 'source/game_sa/SearchLight.h',
         'source/game_sa/TrainNode.cpp', 'source/game_sa/TrainNode.h',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/Misc61/LiveComparison.hpp', 'reversing/Misc61/Core.hpp', 'reversing/Misc61/Cases.hpp', 'reversing/Misc61/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
