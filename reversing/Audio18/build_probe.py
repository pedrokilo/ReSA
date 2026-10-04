"""Compila la sonda temporal del banco Audio18 (CAESound, lista de 18)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/AESound.cpp', 'Audio/AESound.h',
                           'Audio/AEAudioEnvironment.cpp', 'Audio/AEAudioEnvironment.h',
                           'Audio/Hardware/AEAudioHardware.cpp', 'Audio/Hardware/AEAudioHardware.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audio18', 'Audio18Live', SOURCES)))
