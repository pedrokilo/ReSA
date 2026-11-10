"""Construye unicamente el banco Fn145A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/Loaders/AEWMADecoder.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp',
    'source/game_sa/Tasks/TaskComplexUseSequence.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn145A/LiveComparison.hpp',
    'reversing/Fn145A/Core.hpp',
    'reversing/Fn145A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn145a', 'F145ALive', SOURCES)))
