"""Compila la sonda temporal del banco Interior21 (gestor de interiores, lista de 21)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/Interior/'
SOURCES = [G + p for p in ('InteriorManager_c.cpp', 'InteriorManager_c.h', 'Interior_c.cpp', 'Interior_c.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'interior21', 'Interior21Live', SOURCES)))
