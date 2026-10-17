"""Banco par a par RH produccion para GetFirstTexture, RpGeometryReplaceOldMaterialWithNewMaterial y CQuaternion::Slerp (ver README.md)."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies21g','Dependencies21GLive',
        ['source/game_sa/Core/Quaternion.cpp','source/game_sa/Core/Quaternion.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h','source/game_sa/RwHelper.cpp','source/game_sa/RwHelper.h',
         'source/game_sa/RenderWare/rw/rwtexdict.cpp','source/game_sa/RenderWare/rw/rpworld.cpp','source/game_sa/Animation/AnimBlendNode.h','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies21G/Fixtures.hpp','reversing/Dependencies21G/Arenas.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
