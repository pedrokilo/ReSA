"""Construye unicamente el banco Helpers65B (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Camera.cpp', 'Camera.h', 'Cam.cpp', 'Entity/Vehicle/Heli.cpp', 'Entity/Vehicle/Heli.h',
                           'Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp', 'Tasks/TaskTypes/TaskComplexGoPickUpEntity.h', 'PlayerInfoProcess.cpp.inc')] + ['source/InjectHooksMain.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'helpers65b', 'Helpers65BLive', SOURCES)))
