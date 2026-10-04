"""Compila la sonda temporal del banco del almacen de diccionarios de texturas (lista de 25): CTxdStore."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('TxdStore.cpp', 'TxdStore.h', 'TxdStore_Plugin.cpp', 'Core/Pool.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'txd26', 'Txd26Live', SOURCES)))
