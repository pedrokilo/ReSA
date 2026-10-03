"""Compila la sonda temporal del banco de tareas complejas C y golpes (lista de 8): CTaskComplexMoveBackAndJump,
CTaskComplexObserveTrafficLights, CTaskComplexReactToGunAimedAt, CTaskComplexWalkRoundFire, CTaskSimpleBeHit y
CTaskSimpleBikeJacked."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'Task{c}.{e}' for c in ('ComplexMoveBackAndJump', 'ComplexObserveTrafficLights', 'ComplexReactToGunAimedAt',
                                        'ComplexWalkRoundFire', 'SimpleBeHit', 'SimpleBikeJacked') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks8c', 'CTasks8cLive', SOURCES)))
