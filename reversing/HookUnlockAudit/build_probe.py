"""Sonda de inventario y conmutación de CEntryExit dentro del juego completo."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/EntryExit.cpp','source/game_sa/EntryExitManager.cpp',
         'source/reversiblehooks/ReversibleHook/Base.h','source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/game_sa/PostEffects.cpp','source/game_sa/PostEffects.h','source/app/app_game.cpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'hookunlock','HookUnlockAuditLive',SOURCES)))
