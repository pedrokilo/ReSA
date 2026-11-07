"""Construye unicamente el banco Fn115A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = ['Events/Event.cpp', 'Events/EventDontJoinPlayerGroup.cpp', 'Events/EventPassObject.cpp', 'Tasks/Task.cpp', 'Tasks/TaskComplexSequence.cpp',
           'Tasks/Allocators/TaskAllocatorAttack.h', 'Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp'] + ['Tasks/TaskTypes/' + n for n in (
    'TaskComplexKillPedOnFoot.cpp', 'TaskSimpleCarSetPedInAsDriver.cpp', 'TaskSimpleCarSetPedOut.cpp', 'TaskSimpleGoTo.cpp', 'TaskComplexWander.cpp', 'TaskComplexJump.cpp',
    'TaskSimpleAnim.cpp', 'TaskSimplePlayHandSignalAnim.cpp', 'TaskSimpleDrown.cpp', 'TaskComplexAttractorPartnerWait.cpp', 'TaskComplexUseGoggles.cpp', 'TaskComplexUseEffect.h',
    'TaskComplexUseEffectRunning.cpp', 'TaskComplexUseEffectSprinting.cpp', 'TaskComplexStuckInAir.cpp', 'TaskSimpleArrestPed.cpp', 'TaskSimpleFacial.cpp')]
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn115a', 'F115ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp'] + ['source/game_sa/' + s for s in SOURCES]
        + ['reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn115A/LiveComparison.hpp', 'reversing/Fn115A/Core.hpp', 'reversing/Fn115A/Fixtures.hpp',
           'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
