"""Root-only build; proposal must first be applied."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[2]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cppintegertexture','CppIntegerTextureLive',[
 'source/game_sa/RwHelper.cpp','source/game_sa/RwHelper.h',
 'source/game_sa/Core/KeyGen.cpp','source/game_sa/Core/KeyGen.h',
 'source/game_sa/TxdStore.cpp','source/game_sa/TxdStore.h','source/game_sa/TxdDef.h',
 'source/game_sa/Core/Pool.h','source/game_sa/RenderWare/RenderWare.h',
 'source/game_sa/RenderWare/rw/rwcore.h','source/game_sa/RenderWare/rw/rwplcore.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
