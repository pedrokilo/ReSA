"""Banco par a par RH produccion para CCam::WrapAngle y FxEmitter_c::CreateParticles (ver README.md)."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies22c','Dependencies22CLive',
        ['source/game_sa/Cam.cpp','source/game_sa/Cam.h','source/game_sa/Fx/FxEmitter.cpp','source/game_sa/Fx/FxEmitter.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h',
         'source/game_sa/Fx/FxManager.cpp','source/game_sa/Fx/FxPrimBP.cpp','source/game_sa/Fx/FxInfoManager.cpp','source/game_sa/Fx/FxPrtMult.cpp','source/game_sa/RenderWare/rw/rwplcore.cpp',
         'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies22C/Fixtures.hpp','reversing/Dependencies22C/Arenas.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
