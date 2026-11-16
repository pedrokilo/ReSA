"""Construye unicamente el banco Fn194A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
M = 'source/game_sa/Models/'
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    T + 'TaskComplexWaitForPed.cpp',
    T + 'TaskComplexWaitForPed.h',
    T + 'TaskComplexWander.cpp',
    T + 'TaskComplexWander.h',
    T + 'TaskComplexWanderGang.cpp',
    T + 'TaskComplexWanderGang.h',
    T + 'TaskComplexWanderCop.h',
    T + 'TaskComplexWanderMedic.h',
    T + 'TaskComplexWanderCriminal.h',
    T + 'TaskComplexWanderProstitute.h',
    T + 'TaskComplexWanderStandard.h',
    T + 'TaskSimpleStandStill.h',
    T + 'SeekEntity/TaskComplexSeekEntity.h',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'source/game_sa/ModelInfoAccelerator.cpp',
    'source/game_sa/ModelInfoAccelerator.h',
    M + 'ModelInfo.cpp',
    M + 'BaseModelInfo.cpp',
    M + 'AtomicModelInfo.cpp',
    M + 'DamageAtomicModelInfo.cpp',
    M + 'LodAtomicModelInfo.cpp',
    M + 'TimeModelInfo.cpp',
    M + 'LodTimeModelInfo.cpp',
    M + 'ClumpModelInfo.cpp',
    M + 'WeaponModelInfo.cpp',
    M + 'PedModelInfo.cpp',
    'source/game_sa/Collision/ColModel.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn194A/LiveComparison.hpp',
    'reversing/Fn194A/Core.hpp',
    'reversing/Fn194A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn194a', 'F194ALive', SOURCES)))
