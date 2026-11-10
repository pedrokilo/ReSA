"""Construye unicamente el banco Fn153A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Animation/AnimBlendAssociation.cpp',
    'source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp',
    'source/game_sa/Audio/Entities/AETwinLoopSoundEntity.h',
    'source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEVehicleAudioEntity.h',
    'source/game_sa/Collision/ColDisk.cpp',
    'source/game_sa/Collision/ColDisk.h',
    'source/game_sa/Collision/ColStore.cpp',
    'source/game_sa/Collision/ColStore.h',
    'source/game_sa/Core/Vector.cpp',
    'source/game_sa/Core/Vector.h',
    'source/game_sa/Entity/Vehicle/Trailer.cpp',
    'source/game_sa/Entity/Vehicle/Trailer.h',
    'source/game_sa/Events/Event.cpp',
    'source/game_sa/Events/EventEditableResponse.cpp',
    'source/game_sa/Events/EventInteriorUseInfo.cpp',
    'source/game_sa/Events/EventInteriorUseInfo.h',
    'source/game_sa/Events/EventStuckInAir.cpp',
    'source/game_sa/Events/EventStuckInAir.h',
    'source/game_sa/RideAnimData.cpp',
    'source/game_sa/RideAnimData.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn153A/LiveComparison.hpp',
    'reversing/Fn153A/Core.hpp',
    'reversing/Fn153A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn153a', 'F153ALive', SOURCES)))
