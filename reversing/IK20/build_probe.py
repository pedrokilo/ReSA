"""Compila la sonda temporal del banco IK20 (IKChain_c, lista de 20)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Ragdoll/IKChain.cpp', 'source/game_sa/Ragdoll/IKChain.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ik20', 'IK20Live', SOURCES)))
