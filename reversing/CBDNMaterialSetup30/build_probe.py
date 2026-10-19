"""Construye unicamente el banco CBDNMaterialSetup30 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'cbdnmatsetup30', 'CBDNMaterialSetup30Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp', 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h',
         'source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/CBDNMaterialSetup30/LiveComparison.hpp', 'reversing/CBDNMaterialSetup30/Core.hpp', 'reversing/CBDNMaterialSetup30/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
