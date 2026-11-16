"""Construye unicamente el banco Fn193A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Fx/Fx.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn193A/LiveComparison.hpp',
    'reversing/Fn193A/Core.hpp',
    'reversing/Fn193A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn193a', 'F193ALive', SOURCES)))
