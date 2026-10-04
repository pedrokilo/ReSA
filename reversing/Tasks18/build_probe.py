"""Compila la sonda temporal del banco del gestor de tareas (CTaskManager, lista de 18)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/'
SOURCES = [T + 'TaskManager.cpp', T + 'TaskManager.h', T + 'Task.h', T + 'TaskComplex.h', T + 'TaskSimple.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'tasks18', 'Tasks18Live', SOURCES)))
