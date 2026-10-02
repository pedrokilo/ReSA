"""Compila la sonda temporal del banco de CIplStore (subconjunto sin carga de archivos)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [f'source/game_sa/IplStore.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ipl', 'IplLive', SOURCES)))
