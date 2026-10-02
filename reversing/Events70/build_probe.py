"""Compila la sonda temporal del banco de eventos (lista de 70): constructor + AffectsPed/AffectsPedGroup/IsValid/
CloneEditable de 21 clases de eventos."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
EVENTS = ('AcquaintancePedHateBadlyLit', 'CarUpsideDown', 'CopCarBeingStolen', 'GunShotWhizzedBy', 'HitByWaterCannon', 'InAir',
          'LeaderEnteredCarAsDriver', 'LeaderExitedCarAsDriver', 'LeaderQuitEnteringCarAsDriver', 'LeanOnVehicle', 'ObjectCollision',
          'PassObject', 'PlayerCommandToGroup', 'PlayerCommandToGroupAttack', 'PotentialWalkIntoFire', 'PotentialWalkIntoObject',
          'PotentialWalkIntoVehicle', 'SeenPanickedPed', 'VehicleDamageCollision', 'VehicleDamageWeapon', 'VehicleToSteal')
SOURCES = [E + f'Event{c}.{e}' for c in EVENTS for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'events70', 'Events70Live', SOURCES)))
