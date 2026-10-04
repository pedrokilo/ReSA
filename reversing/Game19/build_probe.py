"""Compila la sonda temporal del banco de Darkel y grafitis (lista de 19): CDarkel y CTagManager."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Darkel.cpp', 'Darkel.h', 'TagManager.cpp', 'TagManager.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'game19', 'Game19Live', SOURCES)))
