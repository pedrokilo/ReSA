"""Compila la sonda temporal del banco del gestor de plantas (lista de 19): CPlantMgr."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('PlantMgr.cpp', 'PlantMgr.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'plant19', 'Plant19Live', SOURCES)))
