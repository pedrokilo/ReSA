"""Root-only compilation, after source-unapplied.patch is applied."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesworldfxfont5','DependenciesWorldFxFont5Live',[
        'source/game_sa/Fx/Fx.cpp','source/game_sa/Fx/Fx.h',
        'source/game_sa/Font.cpp','source/game_sa/Font.h',
        'source/game_sa/Camera.cpp','source/game_sa/Camera.h',
        'source/game_sa/Radar.cpp','source/game_sa/Radar.h',
        'source/game_sa/Scripts/TheScripts.h',
        'source/game_sa/RenderWare/rw/rwplcore.cpp','source/game_sa/RenderWare/rw/rwplcore.h',
        'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
