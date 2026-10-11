import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Garage.cpp','source/game_sa/Garage.h','source/game_sa/Garages.cpp','source/game_sa/Garages.h','source/game_sa/Entity/Object/Object.h','source/game_sa/Entity/Entity.h','source/game_sa/Entity/Placeable.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__': print(json.dumps(build(Path(__file__).parent,'depsgarage2','DependenciesGarage2Live',SOURCES)))
