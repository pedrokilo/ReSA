"""Compila la sonda temporal del banco Audio15 (CAEUserRadioTrackManager, CAnimBlendAssocGroup)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Managers/AEUserRadioTrackManager.cpp', 'Audio/Managers/AEUserRadioTrackManager.h',
                           'Animation/AnimBlendAssocGroup.cpp', 'Animation/AnimBlendAssocGroup.h')] + ['source/reversiblebugfixes/Bugs.hpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audio15', 'Audio15Live', SOURCES)))
