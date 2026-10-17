import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/PedGroups.cpp','source/game_sa/PedGroups.h','source/game_sa/PedGroup.h','source/game_sa/PedGroupMembership.h','source/game_sa/PedGroup.cpp','source/game_sa/PedGroupMembership.cpp','source/game_sa/PedGroupIntelligence.cpp','source/game_sa/PedGroupIntelligence.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__': print(json.dumps(build(Path(__file__).parent,'depspedgroups3','DependenciesPedGroups3Live',SOURCES)))
