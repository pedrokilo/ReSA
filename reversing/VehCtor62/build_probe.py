"""Compila la sonda temporal del banco de los constructores de CBmx y CQuadBike (vehctor62)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Vehicle/Bmx.cpp', 'Entity/Vehicle/Bmx.h', 'Entity/Vehicle/QuadBike.cpp', 'Entity/Vehicle/QuadBike.h',
                           'cHandlingDataMgr.cpp', 'cHandlingDataMgr.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'vehctor62', 'VehCtor62Live', SOURCES)))
