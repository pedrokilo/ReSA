"""Construye unicamente el banco PartnerNext60 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'partnernext60', 'PartnerNext60Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h',
         'source/game_sa/Tasks/TaskManager.cpp',
         'source/game_sa/Tasks/TaskManager.h',
         'source/game_sa/General.cpp',
         'source/game_sa/General.h',
         'source/game_sa/X87Math.cpp',
         'source/game_sa/X87Math.h',
         'source/game_sa/Tasks/Task.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp',
         'source/game_sa/Entity/Entity.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PartnerNext60/LiveComparison.hpp', 'reversing/PartnerNext60/Core.hpp', 'reversing/PartnerNext60/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
