"""Banco par a par RH produccion para 20 funciones de biblioteca RenderWare / controlador D3D9 (RwCore + RwPlCore), 1024 pares cada una (ver README.md)."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies25d','Dependencies25DLive',
        ['source/game_sa/RenderWare/rw/rwcore.cpp','source/game_sa/RenderWare/rw/rwcore.h','source/game_sa/RenderWare/rw/rwplcore.cpp','source/game_sa/RenderWare/RenderWare.h',
         'source/app/platform/win/WindowedMode.cpp','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies25D/Fixtures.hpp','reversing/Dependencies25D/Scenes.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
