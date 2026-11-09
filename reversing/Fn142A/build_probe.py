"""Construye unicamente el banco Fn142A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/AEStreamThread.cpp',
    'source/game_sa/Audio/AEStreamThread.h',
    'source/game_sa/Audio/Hardware/AEStaticChannel.cpp',
    'source/game_sa/Audio/Hardware/AEStaticChannel.h',
    'source/game_sa/Entity/Object/Object.cpp',
    'source/game_sa/Entity/Object/Projectile.cpp',
    'source/game_sa/Entity/Object/Projectile.h',
    'source/game_sa/Events/EventDanger.cpp',
    'source/game_sa/Events/EventVehicleHitAndRun.cpp',
    'source/game_sa/Models/LodTimeModelInfo.cpp',
    'source/game_sa/Models/TimeModelInfo.cpp',
    'source/game_sa/PedList.cpp',
    'source/game_sa/PedList.h',
    'source/game_sa/PointList.cpp',
    'source/game_sa/PointList.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn142A/LiveComparison.hpp',
    'reversing/Fn142A/Core.hpp',
    'reversing/Fn142A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp'
]
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn142a', 'F142ALive', SOURCES)))
