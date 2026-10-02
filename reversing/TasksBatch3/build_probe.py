"""Compila la sonda temporal del tercer banco de tareas (UseGoggles, UseAttractor*, UseEffect, UsePairedAttractor)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('UseGoggles', 'UseAttractor', 'UseAttractorPartner', 'UseEffect', 'UsePairedAttractor') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'tasks3', 'Tasks3Live', SOURCES)))
