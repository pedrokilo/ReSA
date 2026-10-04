"""Compila la sonda temporal del banco de CTaskComplexFollowNodeRoute (lista de 19)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplexFollowNodeRoute.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'route19', 'Route19Live', SOURCES)))
