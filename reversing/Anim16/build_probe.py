"""Compila la sonda temporal del banco de jerarquías y nodos de animación (lista de 16)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Animation/AnimBlendHierarchy.cpp', 'Animation/AnimBlendHierarchy.h',
                           'Animation/AnimBlendNode.cpp', 'Animation/AnimBlendNode.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'anim16', 'Anim16Live', SOURCES)))
