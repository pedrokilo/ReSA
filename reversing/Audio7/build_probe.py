"""Compila la sonda temporal del banco de audio, pipeline de edificios y decisiones (lista de 7)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Entities/AEFireAudioEntity.cpp', 'Audio/Entities/AEFireAudioEntity.h',
                           'Audio/Entities/AETwinLoopSoundEntity.cpp', 'Audio/Entities/AETwinLoopSoundEntity.h',
                           'Audio/Entities/AEWaterCannonAudioEntity.cpp', 'Audio/Entities/AEWaterCannonAudioEntity.h',
                           'Audio/Loaders/AEMP3TrackLoader.cpp', 'Audio/Loaders/AEMP3TrackLoader.h',
                           'Pipelines/CustomBuilding/CustomBuildingPipeline.cpp', 'Pipelines/CustomBuilding/CustomBuildingPipeline.h',
                           'DecisionMakers/DecisionMakerTypesFileLoader.cpp', 'DecisionMakers/DecisionMakerTypesFileLoader.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audio7', 'Audio7Live', SOURCES)))
