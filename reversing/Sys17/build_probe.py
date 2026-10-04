"""Compila la sonda temporal del banco Sys17 (CFileMgr e IKChainManager_c, lista de 17)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('FileMgr.cpp', 'FileMgr.h', 'Ragdoll/IKChainManager.cpp', 'Ragdoll/IKChainManager.h',
                           'Tasks/TaskTypes/TaskSimpleIKChain.cpp', 'Tasks/TaskTypes/TaskSimpleIKChain.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'sys17', 'Sys17Live', SOURCES)))
