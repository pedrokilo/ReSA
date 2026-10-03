"""Compila la sonda temporal del banco de coronas, entradas/salidas, reaparición y manejo de vehículos (lista de 11):
CCoronas, CEntryExit, CRestart y cHandlingDataMgr."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + f'{c}.{e}' for c in ('Coronas', 'EntryExit', 'Restart', 'cHandlingDataMgr') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'world11', 'World11Live', SOURCES)))
