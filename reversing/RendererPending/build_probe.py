import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp', 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.h', 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp', 'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'renderer-pending', 'RendererPendingLive', SOURCES)))
