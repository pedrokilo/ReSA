"""Compila la sonda temporal del banco de eventos (lista de 52): CEvent, CEventGlobalGroup y constructor + virtuales
de CEventAreaCodes, CEventPedCollisionWithPed, CEventPotentialWalkIntoPed, CEventSoundQuiet, CEventVehicleCollision,
CEventVehicleDied, CEventVehicleHitAndRun y CEventVehicleOnFire."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
EVENTS = ('', 'AreaCodes', 'GlobalGroup', 'PedCollisionWithPed', 'PotentialWalkIntoPed', 'SoundQuiet', 'VehicleCollision',
          'VehicleDied', 'VehicleHitAndRun', 'VehicleOnFire')
SOURCES = [E + f'Event{c}.{e}' for c in EVENTS for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'events52', 'Events52Live', SOURCES)))
