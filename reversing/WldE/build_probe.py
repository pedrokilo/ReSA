"""Compila la sonda temporal del banco de CWorld, grupo E (20 funciones: fuego, explosiones, pruebas de esfera, reposicion, apagado...)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/World.cpp', 'source/game_sa/World.h']
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'wlde', 'WldELive', SOURCES)))
