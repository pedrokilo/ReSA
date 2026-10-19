"""Construye unicamente el banco PartnerDealAnims31 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'partnerdealanims31', 'PartnerDealAnims31Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.h',
         'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.h',
         'source/game_sa/Animation/AnimManager.cpp', 'source/game_sa/Animation/AnimManager.h', 'source/game_sa/Animation/AnimBlock.h',
         'source/game_sa/Streaming.cpp', 'source/game_sa/Streaming.h',
         'source/game_sa/Tasks/Task.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PartnerDealAnims31/LiveComparison.hpp', 'reversing/PartnerDealAnims31/Core.hpp', 'reversing/PartnerDealAnims31/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
