"""Compila la sonda temporal del banco de clases pequenas del lote 4 (CMaths ... CModelInfo)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Maths.cpp', 'Core/MatrixLink.cpp', 'Core/MatrixLinkList.cpp', 'Models/LodTimeModelInfo.cpp', 'Text/MissionTextOffsets.cpp',
                           'Audio/Entities/AEAudioEntity.cpp', 'Audio/Entities/AEAudioEntity.h', 'Audio/AEAudioUtility.cpp', 'Audio/AEAudioUtility.h',
                           'Audio/AEAudioEnvironment.cpp', 'Audio/Entities/AEExplosionAudioEntity.cpp', 'Audio/Entities/AEExplosionAudioEntity.h',
                           'Audio/Entities/AEDoorAudioEntity.cpp', 'Models/ModelInfo.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'small2', 'Small2Live', SOURCES)))
