"""Root-only build after applying the proposal; test copy only."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesworldwaterpath6','DependenciesWorldWaterPath6Live',[
        'source/game_sa/Cam.cpp','source/game_sa/Cam.h',
        'source/game_sa/Camera.cpp','source/game_sa/Camera.h',
        'source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h',
        'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
