"""Construye unicamente el banco Helpers66D (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'helpers66d', 'H66Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'source/game_sa/RwHelper.cpp', 'source/game_sa/RwHelper.h',
         'source/game_sa/Garage.cpp', 'source/game_sa/Garage.h',
         'source/game_sa/Camera.cpp',
         'source/game_sa/Cam.cpp', 'source/game_sa/Cam.h',
         'source/game_sa/Entity/Vehicle/Heli.cpp', 'source/game_sa/Entity/Vehicle/Heli.h',
         'source/game_sa/Entity/Vehicle/Plane.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Helpers66D/LiveComparison.hpp', 'reversing/Helpers66D/Core.hpp', 'reversing/Helpers66D/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
