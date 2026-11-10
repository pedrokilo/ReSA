"""Construye unicamente el banco Fn147A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Attractors/PedShelterAttractor.cpp',
    'source/game_sa/Events/EventGlobalGroup.cpp',
    'source/game_sa/Events/EventGroup.cpp',
    'source/game_sa/Fx/FxPrtMult.cpp',
    'source/game_sa/Fx/FxSystem.cpp',
    'source/game_sa/Tasks/TaskComplexSequence.cpp',
    'source/game_sa/Tasks/TaskComplexSequence.h',
    'source/game_sa/Tasks/TaskComplexUseSequence.cpp',
    'source/game_sa/Tasks/TaskComplexUseSequence.h',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn147A/LiveComparison.hpp',
    'reversing/Fn147A/Core.hpp',
    'reversing/Fn147A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn147a', 'F147ALive', SOURCES)))
