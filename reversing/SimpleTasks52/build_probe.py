"""Compila la sonda temporal del banco de tareas simples (lista de 52): golpes (HitByGunFrom*, HitFrom*, HitWall),
LookAbout, PutDownEntity, StandStill, Tired, Chat y ArrestPed."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
TASKS = ('HitByGunFromFront', 'HitByGunFromLeft', 'HitByGunFromRear', 'HitByGunFromRight', 'HitFromBack', 'HitFromBehind',
         'HitFromFront', 'HitFromLeft', 'HitFromRight', 'HitWall', 'LookAbout', 'PutDownEntity', 'StandStill', 'Tired', 'Chat',
         'ArrestPed', 'RunTimedAnim', 'RunAnim', 'Anim', 'HoldEntity')
SOURCES = [T + f'TaskSimple{c}.{e}' for c in TASKS for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'stasks52', 'STasks52Live', SOURCES)))
