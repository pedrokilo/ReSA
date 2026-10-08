import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/RealTimeShadowManager.cpp', 'source/game_sa/RealTimeShadowManager.h', 'source/game_sa/RealTimeShadow.cpp', 'source/game_sa/RealTimeShadow.h', 'source/game_sa/ShadowCamera.cpp', 'source/game_sa/ShadowCamera.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'shadow-manager-pending', 'ShadowManagerPendingLive', SOURCES)))
