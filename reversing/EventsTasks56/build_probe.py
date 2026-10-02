"""Compila la sonda temporal del banco de eventos y tareas (lista de 56): constructores de eventos, AffectsPed,
constructores de tareas, CTaskSimpleAnim::MakeAbortable y ProcessPed de CTaskSimpleDrown/CTaskSimpleDrownInCar."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
T = 'source/game_sa/Tasks/'
EVENTS = ('AcquaintancePedHate', 'DontJoinPlayerGroup', 'LeaderEntryExit', 'NewGangMember', 'PedCollisionWithPlayer',
          'PlayerCollisionWithPed', 'PlayerCommandToGroupGather', 'ScriptedAttractor', 'SeenCop', 'Escalator', 'OnFire')
TASKS = ('ComplexFollowPedFootsteps', 'ComplexKillPedOnFoot', 'ComplexPartner', 'ComplexPartnerChat', 'ComplexPartnerDeal',
         'ComplexPartnerShove', 'ComplexWalkRoundObject', 'SimpleUninterruptable', 'SimpleAnim', 'SimpleDrown', 'SimpleDrownInCar')
SOURCES = [E + f'Event{c}.{e}' for c in EVENTS for e in ('cpp', 'h')] + \
          [T + f'TaskTypes/Task{c}.{e}' for c in TASKS for e in ('cpp', 'h')] + \
          [T + f'ScriptedBrainTaskStore.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'evtask56', 'EvTask56Live', SOURCES)))
