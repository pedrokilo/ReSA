"""Compila la sonda temporal del banco de CVehicle grupo A (consultas, banderas y estado del vehiculo, 31 funciones):
AddPassenger, CanBeDeleted, CanBeDriven, CanDoorsBeDamaged, CanPed*, CanVehicleBeDamaged, CarHasRoof, ChangeLawEnforcerState,
Set/Clear de las banderas de entrada, salida y ventanilla, IsDriver/IsPassenger, IsLawEnforcementVehicle, IsOnItsSide, IsUpsideDown,
IsVehicleNormal, IsVehiclePointerValid, UsesSiren, SetVehicleCreatedBy y MakeDirty."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [f'source/game_sa/Entity/Vehicle/Vehicle.{e}' for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cveha', 'CVehALive', SOURCES)))
