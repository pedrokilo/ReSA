"""Compila la sonda temporal del banco de colision de mundo/camara (lista de 59): CCollision, 20 funciones."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Collision/Collision.cpp', 'Collision/Collision.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'colworld59c', 'ColWorld59cLive', SOURCES)))
