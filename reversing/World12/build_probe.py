"""Compila la sonda temporal del banco de semáforos, pickups y explosiones (lista de 12):
CTrafficLights, CPickup y CExplosion."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + f'{c}.{e}' for c in ('TrafficLights', 'Pickup', 'Explosion') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'world12', 'World12Live', SOURCES)))
