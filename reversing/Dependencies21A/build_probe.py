"""Construye únicamente el banco; no acredita aprobación."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies21a','Dependencies21ALive',
        ['source/game_sa/VehicleRecording.cpp', 'source/game_sa/VehicleRecording.h', 'source/game_sa/World.cpp', 'source/game_sa/World.h', 'source/game_sa/CutsceneMgr.cpp', 'source/game_sa/CutsceneMgr.h',
         'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Dependencies21A/FixtureTables.hpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
