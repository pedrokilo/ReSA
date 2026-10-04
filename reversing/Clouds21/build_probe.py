"""Compila la sonda temporal del banco de nubes CClouds (lista de 21)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Clouds.cpp', 'Clouds.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'clouds21', 'Clouds21Live', SOURCES)))
