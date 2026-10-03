"""Compila la sonda temporal del banco de decodificador WAV, reloj, CdStream, CGeneral y comunes (lista de 9)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Loaders/AEWaveDecoder.cpp', 'Audio/Loaders/AEWaveDecoder.h', 'Clock.cpp', 'Clock.h',
                           'CdStreamInfo.cpp', 'CdStreamInfo.h', 'General.cpp', 'General.h', 'common.cpp', 'common.h', 'GxtChar.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'core9', 'Core9Live', SOURCES)))
