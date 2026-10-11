"""Sonda de tres funciones Common/Sprite, después de aplicar el parche propuesto."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Sprite2d.cpp','source/game_sa/Sprite2d.h','source/game_sa/common.cpp',
         'source/game_sa/common.h','source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h',
         'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesmathsprite2','DependenciesMathSprite2Live',SOURCES)))
