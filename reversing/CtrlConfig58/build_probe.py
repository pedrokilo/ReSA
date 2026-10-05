"""Compila la sonda temporal del banco CtrlConfig58 (CControllerConfigManager, 58 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/ControllerConfigManager.cpp', 'source/game_sa/ControllerConfigManager.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctrlconfig58', 'CtrlConfig58Live', SOURCES)))
