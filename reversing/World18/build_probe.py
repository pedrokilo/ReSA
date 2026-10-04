"""Compila la sonda temporal del banco del gestor de entradas y salidas (lista de 18): CEntryExitManager."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('EntryExitManager.cpp', 'EntryExitManager.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'world18', 'World18Live', SOURCES)))
