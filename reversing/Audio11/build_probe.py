"""Compila la sonda temporal del banco Audio11 (CAEPedlessSpeechAudioEntity, CAEWMADecoder, CClothes, CColAccel)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Entities/AEPedlessSpeechAudioEntity.cpp', 'Audio/Entities/AEPedlessSpeechAudioEntity.h',
                           'Audio/Loaders/AEWMADecoder.cpp', 'Audio/Loaders/AEWMADecoder.h', 'Clothes.cpp', 'Clothes.h',
                           'Collision/ColAccel.cpp', 'Collision/ColAccel.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audio11', 'Audio11Live', SOURCES)))
