"""Compila la sonda temporal del banco de huesos (BoneNode_c, BoneNodeManager_c)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Ragdoll/BoneNode.cpp', 'source/game_sa/Ragdoll/BoneNode.h',
           'source/game_sa/Ragdoll/BoneNodeManager.cpp', 'source/game_sa/Ragdoll/BoneNodeManager.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'bones', 'BoneLive', SOURCES)))
