"""Compila la sonda temporal del banco Sys12 (CStreamedScripts, Game, CQuadTreeNode<T>)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('StreamedScripts.cpp', 'StreamedScripts.h', 'Core/QuadTreeNode.h', 'Pools/PtrNodeSingleLinkPool.h')] + ['source/app/app_game.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'sys12', 'Sys12Live', SOURCES)))
