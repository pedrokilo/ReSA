"""Compila la sonda temporal del banco de CPedIntelligence (lista de 34, 52 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('PedIntelligence.cpp', 'PedIntelligence.h', 'Events/EventScanner.cpp', 'Events/EventScanner.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'pedintel52', 'PedIntel52Live', SOURCES)))
