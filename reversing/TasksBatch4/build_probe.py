"""Compila la sonda temporal del cuarto banco de tareas (UseScriptedAttractor, UseScriptedBrain, UseSequence, UseSwatRope, UseWaterCannon)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/'
SOURCES = [T + f'TaskTypes/TaskComplex{c}.{e}' for c in ('UseScriptedAttractor', 'UseScriptedBrain', 'UseSwatRope', 'UseWaterCannon') for e in ('cpp', 'h')] + \
          [T + f'TaskComplexUseSequence.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'tasks4', 'Tasks4Live', SOURCES)))
