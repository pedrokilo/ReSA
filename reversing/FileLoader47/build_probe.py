"""Compila la sonda temporal del banco de CFileLoader (lista 34): lectores dat/ide/ipl, colision, RW y LinkLods."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('FileLoader.cpp', 'FileLoader.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fileloader47', 'FileLoader47Live', SOURCES)))
