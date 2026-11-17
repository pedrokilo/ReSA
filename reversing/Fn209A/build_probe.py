"""Construye unicamente el banco Fn209A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/app.cpp',
    'source/game_sa/Attractors/PedAttractor.cpp',
    'source/game_sa/Attractors/PedAttractor.h',
    'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp',
    'source/game_sa/Audio/Managers/AEUserRadioTrackManager.h',
    'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'source/game_sa/Entity/Vehicle/Vehicle.h',
    'source/game_sa/PatrolRoute.cpp',
    'source/game_sa/PatrolRoute.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn209A/LiveComparison.hpp',
    'reversing/Fn209A/Core.hpp',
    'reversing/Fn209A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn209a', 'F209ALive', SOURCES)))
