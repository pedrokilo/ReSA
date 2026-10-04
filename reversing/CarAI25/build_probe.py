"""Compila la sonda temporal del banco CarAI25 (CCarAI, lista 25)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('CarAI.cpp', 'CarAI.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'carai25', 'CarAI25Live', SOURCES)))
