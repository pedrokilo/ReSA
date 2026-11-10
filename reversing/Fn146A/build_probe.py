"""Construye unicamente el banco Fn146A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/Loaders/AEMFDecoder.cpp',
    'source/game_sa/Entity/Building.cpp',
    'source/game_sa/Entity/Dummy/Dummy.cpp',
    'source/game_sa/PatrolRoute.cpp',
    'source/game_sa/PatrolRoute.h',
    'source/game_sa/Pad.cpp',
    'source/game_sa/Plant/ProcSurfaceInfo.cpp',
    'source/game_sa/Plant/ProcSurfaceInfo.h',
    'source/game_sa/Pools/BuildingPool.h',
    'source/game_sa/Pools/DummyPool.h',
    'source/game_sa/Pools/PatrolRoutePool.h',
    'source/game_sa/Pools/PoolRawDelete.h',
    'source/game_sa/RealTimeShadow.cpp',
    'source/game_sa/RealTimeShadowManager.cpp',
    'source/game_sa/RealTimeShadowManager.h',
    'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp',
    'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn146A/LiveComparison.hpp',
    'reversing/Fn146A/Core.hpp',
    'reversing/Fn146A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn146a', 'F146ALive', SOURCES)))
