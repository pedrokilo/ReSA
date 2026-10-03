"""Compila la sonda temporal del banco de pistas de cinemática, canal estático, generador de coches, checkpoints,
colisión y bandas (lista de 8): CAECutsceneTrackManager, CAEStaticChannel, CCarGenerator, CCheckpoints,
CCollisionData y CGangs."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + f'{c}.{e}' for c in ('Audio/Managers/AECutsceneTrackManager', 'Audio/Hardware/AEStaticChannel', 'CarGenerator',
                                     'Checkpoints', 'Collision/CollisionData', 'Gangs') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'data8a', 'Data8aLive', SOURCES)))
