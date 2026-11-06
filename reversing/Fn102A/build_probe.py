"""Construye unicamente el banco Fn102A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn102a', 'F102ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Fx/FxPrtMult.cpp', 'source/game_sa/Fx/FxPrtMult.h',
         'source/game_sa/SimpleTransform.cpp', 'source/game_sa/SimpleTransform.h',
         'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h',
         'source/game_sa/Audio/AESound.cpp', 'source/game_sa/Audio/AESound.h',
         'source/game_sa/RenderWare/rw/rpworld.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn102A/LiveComparison.hpp', 'reversing/Fn102A/Core.hpp', 'reversing/Fn102A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
