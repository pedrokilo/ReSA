"""Construye únicamente el banco; no acredita aprobación."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=[
 'source/game_sa/Camera.cpp','source/game_sa/Camera.h','source/game_sa/Cam.h',
 'source/game_sa/Hud.cpp','source/game_sa/Hud.h',
 'source/game_sa/Text/Text.cpp','source/game_sa/Text/Text.h',
 'source/InjectHooksMain.cpp','source/reversiblehooks/ReversibleHook/Simple.cpp',
 'source/reversiblehooks/ReversibleHook/Simple.h','reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__=='__main__':
 root=Path(__file__).resolve().parents[2]
 absent=[name for name in SOURCES if not(root/name).is_file()]
 if absent:raise RuntimeError('Missing manifest source paths: '+str(absent))
 print(json.dumps(build(Path(__file__).parent,'dependencies21b','Dependencies21BLive',SOURCES)))
