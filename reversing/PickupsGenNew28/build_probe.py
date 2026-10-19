"""Construye unicamente el banco PickupsGenNew28 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'pickupsgen28', 'PickupsGen28Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Pickups.cpp', 'source/game_sa/Pickups.h', 'source/game_sa/Pickup.cpp', 'source/game_sa/Pickup.h',
         'source/game_sa/World.cpp', 'source/game_sa/World.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PickupsGenNew28/LiveComparison.hpp', 'reversing/PickupsGenNew28/Core.hpp', 'reversing/PickupsGenNew28/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
