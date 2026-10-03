"""Compila la sonda temporal del banco de tareas de salto, huida, tienda y coche (lista de 6): CTaskComplexJump,
CTaskComplexWanderFlee, CTaskInteriorBeInShop, CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse,
CTaskSimpleCarSetPedSlowDraggedOut, CTaskSimpleCarSetTempAction y CTaskSimpleCarWaitToSlowDown."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'{c}.{e}' for c in ('TaskComplexJump', 'TaskComplexWanderFlee', 'TaskComplexWander', 'Interior/TaskInteriorBeInShop',
                                     'TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse', 'TaskSimpleCarSetPedSlowDraggedOut',
                                     'TaskSimpleCarSetTempAction', 'TaskSimpleCarWaitToSlowDown') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks6b', 'CTasks6bLive', SOURCES)))
