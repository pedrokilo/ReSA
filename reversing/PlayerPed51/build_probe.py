"""Compila la sonda temporal del banco de CPlayerPed (lista de 34)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Ped/PlayerPed.cpp', 'Entity/Ped/PlayerPed.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'playerped51', 'PlayerPed51Live', SOURCES)))
