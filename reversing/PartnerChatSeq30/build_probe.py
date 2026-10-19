"""Construye unicamente el banco PartnerChatSeq30 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'partnerchatseq30', 'PartnerChatSeq30Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h',
         'source/game_sa/Tasks/TaskComplexSequence.cpp', 'source/game_sa/Tasks/TaskComplexSequence.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexChat.h',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.h',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
         'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Core/Pool.h',
         'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PedIntelligence.h', 'source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PartnerChatSeq30/LiveComparison.hpp', 'reversing/PartnerChatSeq30/Core.hpp', 'reversing/PartnerChatSeq30/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
