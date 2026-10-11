"""Only the coordinator compiles this probe after applying the proposed sources."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesworld4','DependenciesWorld4Live',[
        'source/game_sa/Garages.cpp','source/game_sa/Garages.h',
        'source/game_sa/Garage.cpp','source/game_sa/Garage.h',
        'source/game_sa/RenderWare/D3DResourceSystem.cpp','source/game_sa/RenderWare/D3DResourceSystem.h',
        'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
