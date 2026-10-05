"""Compila la sonda temporal del banco de geometria pura de CCollision (lista 59a): 25 funciones de pruebas, distancias y puntos cercanos."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + 'Collision/Collision.cpp', G + 'Collision/Collision.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'colgeom59a', 'ColGeom59aLive', SOURCES)))
