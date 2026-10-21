"""Construye unicamente el banco CarCtrlRest63C (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'carctrlrest63c', 'Cc63cLive',
        ['source/CMakeLists.txt',
         'source/game_sa/CarCtrl.cpp', 'source/game_sa/CarCtrl.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/CarCtrlRest63C/LiveComparison.hpp', 'reversing/CarCtrlRest63C/Core.hpp', 'reversing/CarCtrlRest63C/Cases.hpp', 'reversing/CarCtrlRest63C/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
