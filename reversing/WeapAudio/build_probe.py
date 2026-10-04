"""Compila la sonda temporal del banco WeapAudio (constructores de CAEWeaponAudioEntity y CAEPedWeaponAudioEntity)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Entities/AEWeaponAudioEntity.cpp', 'Audio/Entities/AEWeaponAudioEntity.h',
                           'Audio/Entities/AEPedWeaponAudioEntity.cpp', 'Audio/Entities/AEPedWeaponAudioEntity.h',
                           'Audio/Entities/AEAudioEntity.h', 'Audio/AESound.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'weapaudio', 'WeapAudioLive', SOURCES)))
