"""Compila la sonda temporal del banco del gestor de efectos FxManager_c (lista de 25)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Fx/FxManager.cpp', 'Fx/FxManager.h', 'Fx/FxEmitterPrt.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fx25', 'Fx25Live', SOURCES)))
