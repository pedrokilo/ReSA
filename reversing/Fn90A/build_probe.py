"""Construye unicamente el banco Fn90A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn90a', 'F90Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Pad.cpp', 'source/game_sa/RGBA.cpp', 'source/game_sa/RGBA.h', 'source/game_sa/Tasks/Task.cpp',
         'source/game_sa/Animation/AnimManager.cpp', 'source/game_sa/Animation/AnimManager.h', 'source/game_sa/Fx/Fx.cpp', 'source/game_sa/Collision/Sphere.cpp', 'source/game_sa/Collision/Sphere.h',
         'source/game_sa/Fx/FxPrtMult.cpp', 'source/game_sa/Fx/FxPrtMult.h', 'source/game_sa/PedDamageResponseCalculator.cpp', 'source/game_sa/PedDamageResponseCalculator.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn90A/LiveComparison.hpp', 'reversing/Fn90A/Core.hpp', 'reversing/Fn90A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
