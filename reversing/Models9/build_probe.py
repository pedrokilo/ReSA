"""Compila la sonda temporal del banco Models9 (CAtomicModelInfo, CColModel, CCustomRoadsignMgr, CPedModelInfo, CText)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Models/AtomicModelInfo.cpp', 'Models/PedModelInfo.cpp', 'Collision/ColModel.cpp', 'Collision/ColModel.h',
                           'Pools/ColModelPool.h', 'CustomRoadsignMgr.cpp', 'Text/Text.cpp', 'Text/MissionTextOffsets.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'models9', 'Models9Live', SOURCES)))
