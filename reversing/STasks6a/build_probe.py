"""Compila la sonda temporal del banco de tareas simples A (lista de 6): ClearLookAt, CreateCarAndGetIn, DoHandSignal,
DuckToggle, Fall, GiveCPR y GoToPoint."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
TASKS = ('SimpleClearLookAt', 'SimpleCreateCarAndGetIn', 'SimpleDoHandSignal', 'SimpleDuckToggle', 'SimpleFall', 'SimpleGiveCPR',
         'SimpleGoToPoint', 'ComplexPlayHandSignalAnim', 'SimpleGoTo', 'SimpleDuck', 'SimpleStandStill', 'SimpleCarSetPedInAsDriver')
SOURCES = [T + f'Task{c}.{e}' for c in TASKS for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'stasks6a', 'STasks6aLive', SOURCES)))
