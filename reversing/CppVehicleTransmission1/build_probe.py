"""Sonda de CVehicle::SetTransmissionRotation (EXE 0x6DBBB0, hook RH apagado/activo) con estres x87 y CMatrix::SetRotateX/RotateY sustituidos en las dos entradas."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h','source/game_sa/Core/Matrix.cpp','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cppvehicletransmission1','CppVehicleTransmission1Live',SOURCES)))
