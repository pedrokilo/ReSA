"""Compila la sonda temporal del banco de CWanted (lista de 33)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Wanted.cpp', 'Wanted.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'wanted33', 'Wanted33Live', SOURCES)))
