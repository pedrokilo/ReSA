import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/KeyboardState.h','source/game_sa/Fx/FxFrustumInfo.cpp','source/game_sa/Fx/FxFrustumInfo.h','source/game_sa/Fx/FxSphere.h','source/game_sa/Fx/FxPlane.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__': print(json.dumps(build(Path(__file__).parent,'depsqueries1','DependenciesQueries1Live',SOURCES)))
