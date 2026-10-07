"""Compila la sonda temporal del banco CEventHandler grupo C (17 funciones Compute*Response)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [E + 'EventHandler.cpp', E + 'EventHandler.h'] + \
          [T + f'Task{c}.{e}' for c in ('ComplexPolicePursuit', 'SimpleOnEscalator') for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'evhc', 'EvHCLive', SOURCES)))
