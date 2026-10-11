"""Root-only build after applying proposal; test game copy only."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesformationmemory8','DependenciesFormationMemory8Live',[
    'source/game_sa/Formation.cpp','source/game_sa/Formation.h','source/game_sa/PointList.h',
    'source/game_sa/Core/Vector.h',
    'source/app/platform/platform.cpp','source/app/platform/platform.h',
    'source/game_sa/RenderWare/rw/rwplcore.h',
    'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
