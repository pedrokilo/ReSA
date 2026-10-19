"""Construye unicamente el banco PedSaveExtract28 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'pedsaveextract28', 'PedSaveExtract28Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Entity/Ped/PedSaveStructure.cpp', 'source/game_sa/Entity/Ped/PedSaveStructure.h',
         'source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/Entity/Ped/Ped.h',
         'source/game_sa/EntryExitManager.cpp', 'source/game_sa/EntryExitManager.h',
         'source/game_sa/Core/Pool.h', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PedSaveExtract28/LiveComparison.hpp', 'reversing/PedSaveExtract28/Core.hpp', 'reversing/PedSaveExtract28/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
