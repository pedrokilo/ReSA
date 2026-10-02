"""Compila la sonda temporal del banco C_PcSave."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/C_PcSave.cpp', 'source/game_sa/C_PcSave.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'pcsavebatch', 'PcSaveLive', SOURCES)))
