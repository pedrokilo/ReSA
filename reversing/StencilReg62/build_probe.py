"""Compila la sonda temporal del banco de CStencilShadows::RegisterStencilShadows (stencilreg62)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('StencilShadows.cpp', 'StencilShadows.h', 'StencilShadowObject.cpp', 'StencilShadowObject.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'stencilreg62', 'StencilReg62Live', SOURCES)))
