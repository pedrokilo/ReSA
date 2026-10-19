"""Construye unicamente el banco PlantAtomicPrelit28 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'plantprelit28', 'PlantAtomicPrelit28Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/PlantMgr.cpp', 'source/game_sa/PlantMgr.h',
         'source/game_sa/RenderWare/rw/rpworld.cpp', 'source/game_sa/RenderWare/rw/rpworld.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PlantAtomicPrelit28/LiveComparison.hpp', 'reversing/PlantAtomicPrelit28/Core.hpp', 'reversing/PlantAtomicPrelit28/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
