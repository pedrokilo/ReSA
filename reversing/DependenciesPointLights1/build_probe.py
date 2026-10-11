"""Compila la sonda de CPointLights::AddLight; sólo hook temporal de CGame::Process."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = ['source/game_sa/PointLights.cpp', 'source/game_sa/PointLights.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'dependenciespointlights1', 'DependenciesPointLights1Live', SOURCES)))
