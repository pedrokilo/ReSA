"""Construye unicamente el banco Fn92C (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn92c', 'F92CLive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'source/game_sa/Events/EventScanner.cpp', 'source/game_sa/Events/EventScanner.h', 'source/game_sa/AttractorScanner.cpp', 'source/game_sa/AttractorScanner.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn92C/LiveComparison.hpp', 'reversing/Fn92C/Core.hpp', 'reversing/Fn92C/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
