import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/'+name for name in ('Clouds.cpp','Clouds.h','CloudsMathCpp.cpp','CloudsMathCpp.h','Camera.h','Weather.h','TimeCycle.h','Sprite.h','PostEffects.h','CullZones.h','BrightLights.h','TxdStore.h','Coronas.h','Game.h')]+['source/CMakeLists.txt','source/InjectHooksMain.cpp','source/reversiblehooks/RootHookCategory.h','source/reversiblehooks/ReversibleHooks.cpp','source/reversiblehooks/ReversibleHooks.h','source/reversiblehooks/ReversibleHook/Base.h','source/reversiblehooks/ReversibleHook/Simple.cpp','source/reversiblehooks/ReversibleHook/Simple.h','reversing/ProbeSupport.hpp','reversing/ProbeRuntime.hpp','reversing/CppCloudsMath3/run_local.py']
SOURCES += ['source/game_sa/CloudsAccessCpp.cpp','source/game_sa/CloudsAccessCpp.h','source/game_sa/CloudsWeatherCpp.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cpp-clouds-math3','CppCloudsMath3Live',SOURCES)))
