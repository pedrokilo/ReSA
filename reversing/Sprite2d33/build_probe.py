"""Compila la sonda temporal del banco de sprites 2D CSprite2d (lista de 33)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Sprite2d.cpp', 'Sprite2d.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'sprite2d33', 'Sprite2d33Live', SOURCES)))
