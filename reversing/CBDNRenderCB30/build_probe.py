"""Construye unicamente el banco CBDNRenderCB30 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'cbdnrendercb30', 'CBDNRenderCB30Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp', 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h',
         'source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h', 'source/game_sa/RenderWare/rw/rpworld.cpp', 'source/game_sa/RenderWare/rw/rwcore.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/CBDNRenderCB30/LiveComparison.hpp', 'reversing/CBDNRenderCB30/Core.hpp', 'reversing/CBDNRenderCB30/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
