"""Construye unicamente el banco ForcePedOutVt71 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'forcepedoutvt71', 'Vt71Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/ForcePedOutVt71/LiveComparison.hpp', 'reversing/ForcePedOutVt71/Core.hpp', 'reversing/ForcePedOutVt71/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
