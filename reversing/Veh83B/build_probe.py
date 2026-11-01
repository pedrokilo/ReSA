"""Construye unicamente el banco Veh83B (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'veh83b', 'V83Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Veh83B/LiveComparison.hpp', 'reversing/Veh83B/Core.hpp', 'reversing/Veh83B/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
