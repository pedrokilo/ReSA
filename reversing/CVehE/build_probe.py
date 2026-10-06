"""Compila la sonda temporal del banco de CVehicle, grupo E (armas de avion, barco, cabrestante, cuchillas y colision; 26 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Ropes.h']
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cvehe', 'CVehELive', SOURCES)))
