"""Compila la sonda temporal del banco de CObject (lista 34, 48 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Object/Object.cpp', 'Entity/Object/Object.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'object48', 'Object48Live', SOURCES)))
