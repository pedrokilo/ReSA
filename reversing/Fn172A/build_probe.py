"""Construye unicamente el banco Fn172A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp',
    'source/game_sa/Audio/Managers/AESoundManager.cpp',
    'source/game_sa/Audio/Managers/AESoundManager.h',
    'source/game_sa/Audio/Loaders/AEStreamTransformer.cpp',
    'source/game_sa/Audio/Loaders/AEStreamTransformer.h',
    'source/game_sa/Audio/Entities/AEAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEAudioEntity.h',
    'source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEFrontendAudioEntity.h',
    'source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEScriptAudioEntity.h',
    'source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp',
    'source/game_sa/Audio/Entities/AETwinLoopSoundEntity.h',
    'source/game_sa/Audio/Hardware/AEStaticChannel.cpp',
    'source/game_sa/Audio/Hardware/AEStaticChannel.h',
    'source/game_sa/Audio/AudioZones.cpp',
    'source/game_sa/Entity/Object/CutsceneObject.cpp',
    'source/game_sa/Entity/Object/CutsceneObject.h',
    'source/game_sa/StuntJumpManager.cpp',
    'source/game_sa/Cheat.cpp',
    'source/game_sa/Cheat.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn172A/LiveComparison.hpp',
    'reversing/Fn172A/Core.hpp',
    'reversing/Fn172A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn172a', 'F172ALive', SOURCES)))
