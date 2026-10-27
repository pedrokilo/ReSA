"""Construye unicamente el banco HeliSwat74 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'heliswat74', 'Hs74Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'source/game_sa/Entity/Vehicle/Heli.cpp', 'source/game_sa/Entity/Vehicle/Heli.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/HeliSwat74/LiveComparison.hpp', 'reversing/HeliSwat74/Core.hpp', 'reversing/HeliSwat74/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
