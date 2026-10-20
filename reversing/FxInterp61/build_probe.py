"""Construye unicamente el banco FxInterp61 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fxinterp61', 'FxInterp61Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.h', 'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo.cpp',
         'source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp', 'source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.h',
         'source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp', 'source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.h',
         'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp', 'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.h',
         'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp', 'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/FxInterp61/LiveComparison.hpp', 'reversing/FxInterp61/Core.hpp', 'reversing/FxInterp61/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
