"""Construye unicamente el banco Fn124A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Ragdoll/BoneNode.cpp',
    'source/game_sa/Zone.cpp',
    'source/game_sa/GenericGameStorage.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp',
    'source/game_sa/Entity/Object/Object.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp',
    'source/game_sa/Core/Vector.cpp',
    'source/game_sa/Core/Vector2D.cpp',
    'source/game_sa/Timer.cpp',
    'source/game_sa/BreakManager_c.cpp',
    'source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp',
    'source/game_sa/Audio/Loaders/AEVorbisDecoder.h',
    'source/game_sa/Audio/Loaders/AEWaveDecoder.cpp',
    'source/game_sa/Audio/Loaders/AEWaveDecoder.h',
    'source/game_sa/Wanted.cpp',
    'source/game_sa/Wanted.h',
    'source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.h',
    'source/game_sa/RealTimeShadow.cpp',
    'source/game_sa/RealTimeShadow.h',
    'source/oswrapper/oswrapper_win.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn124A/LiveComparison.hpp',
    'reversing/Fn124A/Core.hpp',
    'reversing/Fn124A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn124a', 'F124ALive', SOURCES)))
