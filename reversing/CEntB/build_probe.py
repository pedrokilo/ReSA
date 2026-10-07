"""Compila la sonda temporal del banco CEntB (CEntity, grupo B: ciclo de vida, referencias, estado y colision)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Entity.cpp', 'Entity/Entity.h', 'Core/Pool.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'centb', 'CEntBLive', SOURCES)))
