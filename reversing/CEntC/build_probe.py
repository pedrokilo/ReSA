"""Compila la sonda temporal del banco CEntity grupo C (efectos 2D, dibujo e iluminacion):
CreateEffects ... MaterialUpdateUVAnimCB (16 funciones de CEntity)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [f'source/game_sa/Entity/Entity.{e}' for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'centc', 'CEntCLive', SOURCES)))
