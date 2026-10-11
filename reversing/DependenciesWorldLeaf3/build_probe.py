"""Compile only by the root, after applying source-unapplied.patch."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesworldleaf3','DependenciesWorldLeaf3Live',[
        'source/game_sa/Pad.cpp','source/game_sa/Pad.h',
        'source/game_sa/Fx/FxFrustumInfo.cpp','source/game_sa/Fx/FxFrustumInfo.h',
        'source/game_sa/Fx/FxSphere.h','source/game_sa/Fx/FxPlane.h',
        'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
