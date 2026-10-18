"""Banco par a par RH produccion de 20 funciones de biblioteca de RenderWare (Dependencies25B). Ver README.md."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies25b','Dependencies25BLive',
        ['source/game_sa/RenderWare/rw/rwcore.cpp','source/game_sa/RenderWare/rw/rpanisot.cpp','source/game_sa/RenderWare/rw/rpworld.cpp','source/game_sa/RenderWare/RenderWare.h',
         'source/game_sa/RenderWare/D3DResourceSystem.cpp','source/game_sa/RenderWare/D3DResourceSystem.h','source/game_sa/RenderWare/rw/rwplcore.cpp','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies25B/Bench.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
