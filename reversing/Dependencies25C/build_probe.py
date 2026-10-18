"""Banco par a par RH produccion para 20 funciones de biblioteca de RenderWare (grupo g25c). Ver README.md."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies25c','Dependencies25CLive',
        ['source/game_sa/RenderWare/rw/rpworld.cpp','source/game_sa/RenderWare/rw/rphanim.cpp','source/game_sa/RenderWare/rw/rpskin.cpp','source/game_sa/RenderWare/rw/rpuvanim.cpp',
         'source/game_sa/RenderWare/rw/rtanim.cpp','source/game_sa/RenderWare/rw/rtdict.cpp','source/game_sa/RenderWare/rw/rwplcore.cpp','source/game_sa/RenderWare/rw/rwcore.cpp',
         'source/game_sa/RenderWare/RenderWare.h','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies25C/Fixtures.hpp','reversing/Dependencies25C/Core.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
