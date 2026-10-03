"""Compila la sonda temporal del banco de audio, animacion, objetos de cinematica, puertas y video (lista de 6)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Door.cpp', 'Door.h', 'Animation/AnimBlendClumpData.cpp', 'Animation/AnimBlendClumpData.h',
                           'Animation/AnimBlendStaticAssociation.cpp', 'Animation/AnimBlendStaticAssociation.h',
                           'Audio/Loaders/AEBankLoader.cpp', 'Audio/Loaders/AEBankLoader.h',
                           'Audio/Entities/AEGlobalWeaponAudioEntity.cpp', 'Audio/Entities/AEGlobalWeaponAudioEntity.h',
                           'Entity/Object/CutsceneObject.cpp', 'Entity/Object/CutsceneObject.h')] + \
          ['source/app/platform/win/VideoPlayer/VideoPlayer.cpp', 'source/app/platform/win/VideoPlayer/VideoPlayer.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'animaudio6', 'AnimAudio6Live', SOURCES)))
