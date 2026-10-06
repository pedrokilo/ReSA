"""Compila la sonda temporal del banco de CTaskComplexCarSlowBeDraggedOutAndStandUp (arrastrar fuera despacio y levantarse)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplexCarSlowBeDraggedOutAndStandUp.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'dragout60', 'DragOut60Live', SOURCES)))
