"""Construye unicamente el banco CarCtrlRest63E (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'carctrlrest63e', 'Cc63eLive',
        ['source/CMakeLists.txt',
         'source/game_sa/CarCtrl.cpp', 'source/game_sa/CarCtrl.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/CarCtrlRest63E/LiveComparison.hpp', 'reversing/CarCtrlRest63E/Core.hpp', 'reversing/CarCtrlRest63E/Cases.hpp', 'reversing/CarCtrlRest63E/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
