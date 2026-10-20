"""Construye unicamente el banco D3DResource62 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'd3dresource62', 'D3D62Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/RenderWare/D3DResourceSystem.cpp', 'source/game_sa/RenderWare/D3DResourceSystem.h',
         'source/game_sa/RenderWare/D3DTextureBuffer.cpp', 'source/game_sa/RenderWare/D3DTextureBuffer.h',
         'source/game_sa/RenderWare/D3DIndexDataBuffer.cpp', 'source/game_sa/RenderWare/D3DIndexDataBuffer.h',
         'source/game_sa/MemoryMgr.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/D3DResource62/LiveComparison.hpp', 'reversing/D3DResource62/Core.hpp', 'reversing/D3DResource62/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
