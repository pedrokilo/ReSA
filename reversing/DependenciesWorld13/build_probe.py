"""Root-only build after applying proposal; test copy only."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesworld13','DependenciesWorld13Live',[
 'source/game_sa/PathFind.cpp','source/game_sa/PathFind.h','source/game_sa/NodeAddress.h',
 'source/game_sa/Interior/InteriorGroup_c.cpp','source/game_sa/Interior/InteriorGroup_c.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
