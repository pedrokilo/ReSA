"""Construye unicamente el banco PartnerShoveSeq28 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'partnershoveseq28', 'PartnerShoveSeq28Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h',
         'source/game_sa/Tasks/TaskComplexSequence.cpp', 'source/game_sa/Tasks/TaskComplexSequence.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.h',
         'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Core/Pool.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PartnerShoveSeq28/LiveComparison.hpp', 'reversing/PartnerShoveSeq28/Core.hpp', 'reversing/PartnerShoveSeq28/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
