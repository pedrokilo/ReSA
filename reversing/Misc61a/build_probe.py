"""Compila la sonda temporal del banco de CScriptsForBrains, CPedGroup, CWeather y CRopes (misc61a)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('ScriptsForBrains.cpp', 'ScriptsForBrains.h', 'PedGroup.cpp', 'PedGroup.h', 'PedGroupIntelligence.h',
                           'Weather.cpp', 'Weather.h', 'Data/Weather.def', 'Ropes.cpp', 'Ropes.h', 'Rope.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'misc61a', 'Misc61aLive', SOURCES)))
