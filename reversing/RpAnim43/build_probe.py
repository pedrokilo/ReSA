"""Compila la sonda temporal del banco del plugin RpAnimBlend (lista de 43): RpAnimBlend."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp', 'Plugins/RpAnimBlendPlugin/RpAnimBlend.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'rpanim43', 'RpAnim43Live', SOURCES)))
