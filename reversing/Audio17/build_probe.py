"""Compila la sonda temporal del banco Audio17 (CAECollisionAudioEntity)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Entities/AECollisionAudioEntity.cpp', 'Audio/Entities/AECollisionAudioEntity.h',
                           'Audio/Entities/AECollisionAudioEntity.CollisionLookup.h')] + ['source/reversiblebugfixes/Bugs.hpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audio17', 'Audio17Live', SOURCES)))
