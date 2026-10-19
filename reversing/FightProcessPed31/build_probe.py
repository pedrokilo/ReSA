"""Construye unicamente el banco FightProcessPed31 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fightprocessped31', 'FightProcessPed31Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h',
         'source/game_sa/Tasks/TaskSimple.h', 'source/game_sa/Tasks/Task.h',
         'source/game_sa/Animation/AnimBlendAssociation.cpp', 'source/game_sa/Animation/AnimBlendAssociation.h',
         'source/game_sa/Animation/AnimManager.cpp', 'source/game_sa/Animation/AnimManager.h',
         'source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/Entity/Ped/PlayerPed.cpp', 'source/game_sa/Entity/Ped/PlayerPed.h',
         'source/game_sa/Pad.cpp', 'source/game_sa/Pad.h', 'source/game_sa/WeaponInfo.cpp', 'source/game_sa/WeaponInfo.h', 'source/game_sa/Streaming.cpp', 'source/game_sa/Streaming.h',
         'source/game_sa/Audio/Entities/AEPedAudioEntity.cpp', 'source/game_sa/Audio/Entities/AEPedAudioEntity.h',
         'source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp', 'source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/FightProcessPed31/LiveComparison.hpp', 'reversing/FightProcessPed31/Core.hpp', 'reversing/FightProcessPed31/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
