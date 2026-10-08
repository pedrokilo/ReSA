"""Compila la sonda temporal del banco de CWorld, grupo C (27 funciones: jugadores, sectores, Add/Remove y blancos desprevenidos)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('World.cpp', 'World.h', 'World_Helper.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'wldc', 'WldCLive', SOURCES)))
