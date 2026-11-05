"""Construye unicamente el banco Fn97A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn97a', 'F97ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h', 'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Matrix.h',
         'source/game_sa/Core/Rect.cpp', 'source/game_sa/Core/Rect.h', 'source/game_sa/Collision/ColLine.cpp', 'source/game_sa/Collision/ColLine.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn97A/LiveComparison.hpp', 'reversing/Fn97A/Core.hpp', 'reversing/Fn97A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
