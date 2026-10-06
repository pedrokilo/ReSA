"""Compila la sonda temporal del banco CPedD (CPed, grupo D: ciclo de vida, estado y consultas)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Ped/Ped.cpp', 'Entity/Ped/Ped.h', 'Core/Pool.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpedd', 'CPedDLive', SOURCES)))
