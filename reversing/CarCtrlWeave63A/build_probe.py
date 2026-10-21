"""Construye unicamente el banco CarCtrlWeave63A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'carctrlweave63a', 'CwALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/CarCtrl.cpp', 'source/game_sa/CarCtrl.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/CarCtrlWeave63A/LiveComparison.hpp', 'reversing/CarCtrlWeave63A/Core.hpp', 'reversing/CarCtrlWeave63A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
