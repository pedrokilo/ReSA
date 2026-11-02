"""Construye unicamente el banco Car84G (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'car84g', 'C84GLive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'source/game_sa/CarCtrl.cpp', 'source/game_sa/CarCtrl.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Car84G/LiveComparison.hpp', 'reversing/Car84G/Core.hpp', 'reversing/Car84G/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
