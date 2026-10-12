import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Interior/Interior_c.cpp','Interior/Interior_c.h','Interior/Furniture_c.h','Interior/FurnitureManager_c.h','Interior/InteriorInfo_t.h')] + ['source/CMakeLists.txt','source/InjectHooksMain.cpp','source/reversiblehooks/RootHookCategory.h','source/reversiblehooks/ReversibleHooks.h','source/reversiblehooks/ReversibleHooks.cpp','source/reversiblehooks/HookCategory.h','source/reversiblehooks/ReversibleHook/Base.h','source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp','reversing/ProbeRuntime.hpp','reversing/CppInteriorProposal/run_local.py']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cpp-interior-proposal','CppInteriorProposalLive',SOURCES)))
