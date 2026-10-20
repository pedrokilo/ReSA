"""Construye unicamente el banco KillPedOnFoot62 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'killpedonfoot62', 'KillPedOnFoot62Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp',
         'source/game_sa/Events/EventAreaCodes.cpp',
         'source/game_sa/Tasks/TaskTimer.cpp',
         'source/game_sa/Tasks/Task.cpp',
         'source/game_sa/General.cpp',
         'source/game_sa/General.h',
         'source/game_sa/Entity/Entity.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/KillPedOnFoot62/LiveComparison.hpp', 'reversing/KillPedOnFoot62/Core.hpp', 'reversing/KillPedOnFoot62/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
