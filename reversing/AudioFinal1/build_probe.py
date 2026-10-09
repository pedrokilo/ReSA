"""Compila la sonda temporal del banco AudioFinal1 (CAEAudioHardware::Initialise y CAEFrontendAudioEntity::{Initialise, IsLoadingTuneActive, Reset})."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# Ruta real de la copia de prueba (el junction D:/JUEGOS ya no existe). Se redirige en memoria, antes de importar
# probe_build (que enlaza GAME al importarse), sin tocar runtime_runner.py (hash congelado).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G = 'source/game_sa/Audio/'
SOURCES = [G + p for p in ('Hardware/AEAudioHardware.cpp', 'Hardware/AEAudioHardware.h',
                           'Entities/AEFrontendAudioEntity.cpp', 'Entities/AEFrontendAudioEntity.h',
                           'Loaders/AEMP3BankLoader.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audiofinal1', 'AudioFinal1Live', SOURCES)))
