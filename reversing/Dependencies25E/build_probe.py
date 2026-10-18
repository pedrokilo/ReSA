"""Construye únicamente el banco; no acredita aprobación."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies25e','Dependencies25ELive',
        ['source/game_sa/RenderWare/rw/rwcore.cpp','source/game_sa/RenderWare/rw/rwplcore.cpp','source/game_sa/RenderWare/RenderWare.h','source/game_sa/VehicleRecording.cpp',
         'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies25E/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
