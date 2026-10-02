"""Compila la sonda temporal del banco de tareas complejas, repartidores y peatones (lista de 52):
CTaskComplexEnterCarAsDriver/AsPassenger, CTaskComplexGoToPointShooting, CTaskComplexLeaveCarAndDie, CTaskAllocator,
CTaskAllocatorPlayerCommandAttack, CCivilianPed, CCoverPoint, CPedPlacement, CPedToPlayerConversations y CSetPiece."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
A = G + 'Tasks/Allocators/'
SOURCES = [T + f'Task{c}.{e}' for c in ('ComplexEnterCarAsDriver', 'ComplexEnterCarAsPassenger', 'ComplexGoToPointShooting',
                                        'ComplexLeaveCarAndDie') for e in ('cpp', 'h')] + \
          [T + 'TaskComplexEnterCar.h'] + \
          [A + f'TaskAllocator{c}.{e}' for c in ('', 'PlayerCommandAttack') for e in ('cpp', 'h')] + \
          [G + f'Entity/Ped/CivilianPed.{e}' for e in ('cpp', 'h')] + \
          [G + f'{c}.{e}' for c in ('CoverPoint', 'PedPlacement', 'SetPiece') for e in ('cpp', 'h')] + \
          [G + f'Conversations/PedToPlayerConversations.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ptasks52', 'PTasks52Live', SOURCES)))
