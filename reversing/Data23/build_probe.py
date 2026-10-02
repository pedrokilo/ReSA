"""Compila la sonda temporal del banco Data23 (audio, balas, modelos, garajes y datos)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Entities/AEPedWeaponAudioEntity.cpp', 'Audio/Entities/AEPedWeaponAudioEntity.h', 'BulletInfo.cpp', 'BulletInfo.h',
                           'Models/DamageAtomicModelInfo.cpp', 'Models/DamageAtomicModelInfo.h', 'Garage.cpp', 'Garage.h', 'PedStats.cpp', 'PedStats.h',
                           'PlantSurfPropMgr.cpp', 'PlantSurfPropMgr.h', 'ScriptResourceManager.cpp', 'ScriptResourceManager.h',
                           'SpecialPlateHandler.cpp', 'SpecialPlateHandler.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'data23', 'Data23Live', SOURCES)))
