"""Compila la sonda temporal del banco del gestor de animaciones (lista de 31): CAnimManager."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Animation/AnimManager.cpp', 'Animation/AnimManager.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'animmgr31', 'AnimMgr31Live', SOURCES)))
