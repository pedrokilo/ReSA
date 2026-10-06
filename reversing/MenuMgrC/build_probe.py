"""Compila la sonda temporal del banco CMenuManager grupo C (flujo y opciones: 14 funciones)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('MenuManager.cpp', 'MenuManager.h', 'Frontend/MenuManager_Process.cpp', 'Frontend/MenuManager_Input.cpp', 'Frontend/MenuManager_Internal.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'menumgrc', 'MenuMgrCLive', SOURCES)))
