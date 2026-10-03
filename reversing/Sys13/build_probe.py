"""Compila la sonda temporal del banco Sys13 (CPedType, CTimer, CTrailer)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('PedType.cpp', 'PedType.h', 'Timer.cpp', 'Timer.h', 'Entity/Vehicle/Trailer.cpp', 'Entity/Vehicle/Trailer.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'sys13', 'Sys13Live', SOURCES)))
