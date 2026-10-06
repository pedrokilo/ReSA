"""Compila la sonda temporal del banco de CBmx, CQuadBike, CCarFXRenderer y CStencilShadows (misc61b)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/Vehicle/Bmx.cpp', 'Entity/Vehicle/Bmx.h', 'Entity/Vehicle/QuadBike.cpp', 'Entity/Vehicle/QuadBike.h',
                           'Fx/CarFXRenderer.cpp', 'Fx/CarFXRenderer.h', 'StencilShadows.cpp', 'StencilShadows.h',
                           'StencilShadowObject.cpp', 'StencilShadowObject.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'misc61b', 'Misc61bLive', SOURCES)))
