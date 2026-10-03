"""Compila la sonda temporal del banco de tareas simples (lista de 7): CarWaitForDoorNotToBeInUse, Die,
KillPedWithCar, RunTimedAnim, SitIdle y StealthKill."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
TASKS = ('CarWaitForDoorNotToBeInUse', 'Die', 'KillPedWithCar', 'RunTimedAnim', 'SitIdle', 'StealthKill', 'Anim')
SOURCES = [T + f'TaskSimple{c}.{e}' for c in TASKS for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'stasks7', 'STasks7Live', SOURCES)))
