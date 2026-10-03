"""Compila la sonda temporal del banco Mix14 (gestor de atractores y matriculas, lista de 14)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('PedAttractorManager.cpp', 'PedAttractorManager.h', 'Core/SArray.h', 'CustomCarPlateMgr.cpp', 'CustomCarPlateMgr.h', 'Attractors/PedPizzaAttractor.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'mix14', 'Mix14Live', SOURCES)))
