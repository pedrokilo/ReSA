"""Compila la sonda temporal del banco de zonas (lista de 21): CCullZones y CTheZones."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('CullZones.cpp', 'CullZones.h', 'TheZones.cpp', 'TheZones.h', 'Renderer.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'zones21', 'Zones21Live', SOURCES)))
