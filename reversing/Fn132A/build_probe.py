"""Construye unicamente el banco Fn132A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Pools/PatrolRoutePool.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h',
    'source/game_sa/Scripts/Scripted2dEffects.cpp',
    'source/game_sa/Scripts/Scripted2dEffects.h',
    'source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp',
    'source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.h',
    'source/game_sa/Coronas.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'source/game_sa/DecisionMakers/Decision.cpp',
    'source/game_sa/DecisionMakers/Decision.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn132A/LiveComparison.hpp',
    'reversing/Fn132A/Core.hpp',
    'reversing/Fn132A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn132a', 'F132ALive', SOURCES)))
