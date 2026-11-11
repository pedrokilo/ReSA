"""Construye unicamente el banco Fn165A (ASI de sonda); no acredita aprobacion. Ver README.md."""
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
    'source/game_sa/Animation/AnimBlendAssociation.h',
    'source/game_sa/Animation/AnimBlendStaticAssociation.cpp',
    'source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp',
    'source/game_sa/Events/EventSoundQuiet.cpp',
    'source/game_sa/Events/EventSoundQuiet.h',
    'source/game_sa/Interior/FurnitureGroup_c.cpp',
    'source/game_sa/Interior/FurnitureGroup_c.h',
    'source/game_sa/Interior/FurnitureSubGroup_c.cpp',
    'source/game_sa/Interior/FurnitureSubGroup_c.h',
    'source/game_sa/Interior/Furniture_c.cpp',
    'source/game_sa/Interior/Furniture_c.h',
    'source/game_sa/PedIntelligence.cpp',
    'source/game_sa/PedIntelligence.h',
    'source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h',
    'source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn165A/LiveComparison.hpp',
    'reversing/Fn165A/Core.hpp',
    'reversing/Fn165A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn165a', 'F165ALive', SOURCES)))
