"""Compila la sonda temporal del banco de CPedGroupIntelligence (lista de 26)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('PedGroupIntelligence.cpp', 'PedGroupIntelligence.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'pgi26', 'PGI26Live', SOURCES)))
