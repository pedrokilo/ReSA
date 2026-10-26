"""Construye unicamente el banco LeaveCarFirst70 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'leavecarfirst70', 'Leave70Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.h',
         'source/game_sa/Tasks/Task.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/LeaveCarFirst70/LiveComparison.hpp', 'reversing/LeaveCarFirst70/Core.hpp', 'reversing/LeaveCarFirst70/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
