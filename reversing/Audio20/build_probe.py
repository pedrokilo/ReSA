"""Compila la sonda temporal del banco Audio20 (flujo de datos y audio de armas, lista de 20)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Loaders/AEDataStream.cpp', 'Audio/Loaders/AEDataStream.h',
                           'Audio/Entities/AEWeaponAudioEntity.cpp', 'Audio/Entities/AEWeaponAudioEntity.h',
                           'Audio/Entities/AEAudioEntity.cpp', 'Audio/Entities/AEAudioEntity.h',
                           'Audio/AESound.cpp', 'Audio/AESound.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audio20', 'Audio20Live', SOURCES)))
