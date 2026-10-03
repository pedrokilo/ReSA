"""Compila la sonda temporal del banco de tareas simples (lista de 19): Cower, GoTo, HandsUp, HitHead, InAir, None,
Pause, PickUpEntity, UseAtm y WaitUntilLeaderAreaCodesMatch."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
TASKS = ('Cower', 'GoTo', 'HandsUp', 'HitHead', 'InAir', 'None', 'Pause', 'PickUpEntity', 'UseAtm',
         'WaitUntilLeaderAreaCodesMatch', 'WaitUntilAreaCodesMatch', 'StandStill', 'RunAnim', 'RunTimedAnim', 'Anim', 'HoldEntity')
SOURCES = [T + f'TaskSimple{c}.{e}' for c in TASKS for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'stasks19', 'STasks19Live', SOURCES)))
