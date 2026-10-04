"""Compila la sonda temporal del banco de arreglos de peatones y grupos (CPedAttractor, FlushTasks, CancelPlayerEnteringCars)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Attractors/PedAttractor.cpp', 'Attractors/PedAttractor.h', 'PedGroupIntelligence.cpp', 'PedGroupIntelligence.h',
                           'PlayerInfo.cpp', 'PlayerInfo.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fixpeds', 'FixPedsLive', SOURCES)))
