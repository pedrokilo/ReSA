"""Compila la sonda temporal del banco de CCustomCarEnvMapPipeline (lista de 34)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/Pipelines/CustomCarEnvMap/'
SOURCES = [G + p for p in ('CustomCarEnvMapPipeline.cpp', 'CustomCarEnvMapPipeline.h', 'CustomCarEnvMapPipeline_EnvMat.cpp',
                           'CustomCarEnvMapPipeline_EnvAtm.cpp', 'CustomCarEnvMapPipeline_SpecMat.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'envmap37', 'EnvMap37Live', SOURCES)))
