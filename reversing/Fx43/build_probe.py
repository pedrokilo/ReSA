"""Compila la sonda temporal del banco de audio del clima, efectos, fuego, entrada y JPEG (lista de 43)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Entities/AEWeatherAudioEntity.cpp', 'Audio/Entities/AEWeatherAudioEntity.h', 'BulletTraces.cpp', 'BulletTraces.h',
                           'Fire.cpp', 'Fire.h', 'MovingThings.cpp', 'MovingThings.h', 'ShotInfo.cpp', 'ShotInfo.h', 'WaterCannon.cpp', 'WaterCannon.h',
                           'Fx/FxMemoryPool.cpp', 'Fx/FxMemoryPool.h', 'Plugins/JPegCompressPlugin/JPegCompress.cpp',
                           'Plugins/JPegCompressPlugin/JPegCompress.h')] + ['source/app/app_input.cpp', 'source/app/app_input.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fx43', 'Fx43Live', SOURCES)))
