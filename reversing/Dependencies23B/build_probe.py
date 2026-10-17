"""Banco par a par RH produccion para CVehicle::FlyingControl (0x6D85F0). Ver README.md."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies23b','Dependencies23BLive',
        ['source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies23B/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
