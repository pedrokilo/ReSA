"""Banco par a par RH produccion para D3DResourceSystem::TidyUpD3DTextures y TidyUpD3DIndexBuffers (conversion a C++, ver README.md)."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies21f','Dependencies21FLive',
        ['source/game_sa/RenderWare/D3DResourceSystem.cpp', 'source/game_sa/RenderWare/D3DResourceSystem.h', 'source/game_sa/RenderWare/D3DTextureBuffer.h', 'source/game_sa/RenderWare/D3DIndexDataBuffer.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Dependencies21F/HelperRoutes.hpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
