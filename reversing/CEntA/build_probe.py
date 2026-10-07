"""Compila la sonda temporal del banco de CEntity, grupo A (matrices, transformaciones, limites y visibilidad; 26 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Entity.h', 'source/game_sa/common.cpp', 'source/game_sa/SimpleTransform.cpp']
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'centa', 'CEntALive', SOURCES)))
