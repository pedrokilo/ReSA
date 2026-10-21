"""Construye unicamente el banco Helpers65D (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'helpers65d', 'Hp65Live',
        ['source/CMakeLists.txt',
         'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Matrix.h', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h', 'source/InjectHooksMain.cpp',
         'source/game_sa/Entity/Vehicle/Plane.cpp',
         'source/game_sa/RenderWare/rw/rwtexdict.cpp',
         'source/game_sa/RwHelper.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/Helpers65D/LiveComparison.hpp', 'reversing/Helpers65D/Core.hpp', 'reversing/Helpers65D/Cases.hpp', 'reversing/Helpers65D/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
