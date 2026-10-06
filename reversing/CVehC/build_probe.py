"""Compila la sonda temporal del banco CVehicle grupo C (mejoras, atomics, remaps y componentes) (cvehc)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Vehicle/Vehicle.cpp', 'Entity/Vehicle/Vehicle.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cvehc', 'CVehCLive', SOURCES)))
