"""Compila la sonda temporal del banco de lineas de vision y lineas verticales de CWorld (grupo B, 16 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('World.cpp', 'World.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'wldb', 'WldBLive', SOURCES)))
