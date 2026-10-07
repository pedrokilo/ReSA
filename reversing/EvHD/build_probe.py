"""Compila la sonda temporal del banco del grupo D de CEventHandler: 13 funciones Compute*Response
(SeenCop, SeenPanickedPed, SexyPed, SexyVehicle, ShotFired, ShotFiredWhizzedBy, SignalAtPed, Special, VehicleDied,
VehicleOnFire, VehiclePotentialPassiveCollision, VehicleToSteal y WaterCannon)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
SOURCES = [E + f'EventHandler.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'evhd', 'EvHDLive', SOURCES)))
