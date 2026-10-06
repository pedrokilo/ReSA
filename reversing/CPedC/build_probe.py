"""Compila la sonda temporal del banco de CPed grupo C (mirar, apuntar y animaciones, 29 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/Entity/Ped/Ped.h']
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpedc', 'CPedCLive', SOURCES)))
