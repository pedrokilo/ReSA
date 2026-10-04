"""Compila la sonda temporal del banco de coberturas y CPlaceable (lista de 16)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Cover.cpp', 'Cover.h', 'Entity/Placeable.cpp', 'Entity/Placeable.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'world16', 'World16Live', SOURCES)))
