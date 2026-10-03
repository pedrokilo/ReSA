"""Compila la sonda temporal del banco de tareas simples de coche (lista de 9): CarFallOut, CarGetIn,
CarOpenLockedDoorFromOutside, CarShuffle y CarSlowBeDraggedOut."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
TASKS = ('CarFallOut', 'CarGetIn', 'CarOpenLockedDoorFromOutside', 'CarShuffle', 'CarSlowBeDraggedOut')
SOURCES = [T + f'TaskSimple{c}.{e}' for c in TASKS for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'stasks9', 'STasks9Live', SOURCES)))
