"""Compila la sonda temporal del banco de tareas y grupos (lista de 70): asignadores de grupo, CPedGroups,
CTaskAllocatorKillThreatsBasicRandomGroup, CTaskComplexDragPedFromCar, CTaskSequences, FinishBrain, LeaveGroup,
PlayHandSignalAnim, ScratchHead y CTimeModelInfo."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
A = G + 'Tasks/Allocators/'
T = G + 'Tasks/TaskTypes/'
SOURCES = [A + f'PedGroup/PedGroupDefaultTaskAllocator{c}.h' for c in ('Chat', 'FollowAnyMeans', 'FollowLimited', 'Random', 'SitInLeaderCar', 'StandStill')] + \
          [G + f'PedGroups.{e}' for e in ('cpp', 'h')] + \
          [A + f'TaskAllocatorKillThreatsBasicRandomGroup.{e}' for e in ('cpp', 'h')] + \
          [T + f'Task{c}.{e}' for c in ('ComplexDragPedFromCar', 'SimpleFinishBrain', 'SimpleLeaveGroup', 'SimplePlayHandSignalAnim', 'SimpleScratchHead') for e in ('cpp', 'h')] + \
          [G + f'Tasks/TaskSequences.{e}' for e in ('cpp', 'h')] + \
          [G + f'Models/TimeModelInfo.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'tasks70', 'Tasks70Live', SOURCES)))
