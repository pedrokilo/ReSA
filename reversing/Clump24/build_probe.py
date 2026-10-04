"""Compila la sonda temporal del banco Clump24 (CClumpModelInfo, lista de 24)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Models/ClumpModelInfo.cpp', 'Models/ClumpModelInfo.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'clump24', 'Clump24Live', SOURCES)))
