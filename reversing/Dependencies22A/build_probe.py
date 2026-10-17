"""Banco par a par RH produccion para CPointLights (GenerateLightsAffectingObject, GetLightMultiplier, ProcessVerticalLineUsingCache, RenderFogEffect). Ver README.md."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies22a','Dependencies22ALive',
        ['source/game_sa/PointLights.cpp','source/game_sa/PointLights.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies22A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
