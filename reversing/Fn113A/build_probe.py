"""Construye unicamente el banco Fn113A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn113a', 'F113ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWander.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp',
         'source/game_sa/Tasks/Allocators/TaskAllocator.cpp', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PlayerRelationshipRecorder.cpp', 'source/game_sa/PlayerRelationshipRecorder.h',
         'source/game_sa/Events/EventScanner.cpp', 'source/game_sa/Events/EventScanner.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn113A/LiveComparison.hpp', 'reversing/Fn113A/Core.hpp', 'reversing/Fn113A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
