"""Compila la sonda temporal del banco de eventos, repartidores y datos de peatón/vehículo (lista de 43):
CEventGunShot, CEventVehicleDamage, CTaskAllocatorAttack, CTaskAllocatorKillThreatsDriveby, CPedScriptedTaskRecord,
CPlayerPedData, CUpsideDownCarCheck, CVehicleAnimGroup y CVehicleAnimGroupData."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
A = G + 'Tasks/Allocators/'
SOURCES = [G + f'Events/Event{c}.{e}' for c in ('GunShot', 'VehicleDamage') for e in ('cpp', 'h')] + \
          [A + f'TaskAllocator{c}.{e}' for c in ('Attack', 'KillThreatsDriveby') for e in ('cpp', 'h')] + \
          [G + f'{c}.{e}' for c in ('Tasks/PedScriptedTaskRecord', 'PlayerPedData', 'UpsideDownCarCheck', 'VehicleAnimGroupData')
           for e in ('cpp', 'h')] + \
          [G + f'Tasks/TaskTypes/TaskSimpleGangDriveBy.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'events43', 'Events43Live', SOURCES)))
