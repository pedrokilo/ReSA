"""Construye unicamente el banco Helpers66C (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'helpers66c', 'Hp66Live',
        ['source/CMakeLists.txt',
         'source/game_sa/Audio/Loaders/AEMFDecoder.cpp', 'source/game_sa/Audio/Loaders/AEMFDecoder.h',
         'source/app/platform/win/WinPs.cpp',
         'source/game_sa/RenderWare/rw/rwtexdict.cpp',
         'source/game_sa/RwHelper.cpp',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/Helpers66C/LiveComparison.hpp', 'reversing/Helpers66C/Core.hpp', 'reversing/Helpers66C/Cases.hpp', 'reversing/Helpers66C/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
