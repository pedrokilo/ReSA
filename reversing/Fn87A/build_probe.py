"""Construye unicamente el banco Fn87A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn87a', 'F87ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/Entity/Ped/PlayerPed.cpp', 'source/game_sa/Entity/Ped/PlayerPed.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn87A/LiveComparison.hpp', 'reversing/Fn87A/Core.hpp', 'reversing/Fn87A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
