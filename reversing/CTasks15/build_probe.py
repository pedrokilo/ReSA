"""Compila la sonda temporal del banco de evitar peatones al deambular, trepar y agacharse (lista de 15):
CTaskComplexAvoidOtherPedWhileWandering, CTaskSimpleClimb y CTaskSimpleDuck."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'{c}.{e}' for c in ('TaskComplexAvoidOtherPedWhileWandering', 'TaskSimpleClimb', 'TaskSimpleDuck') for e in ('cpp', 'h')] + [T + 'TaskSimpleClimb_models.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks15', 'CTasks15Live', SOURCES)))
