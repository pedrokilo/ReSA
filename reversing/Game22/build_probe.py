"""Compila la sonda temporal del banco Game22 (CGame, lista de 22)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Game.cpp', 'source/game_sa/Game.h', 'source/app/app_camera.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'game22', 'Game22Live', SOURCES)))
