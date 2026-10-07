"""Compila la sonda temporal del banco de CEventHandler (grupo B): AreaCodes, Attractor, BuildingCollision, CarUpsideDown,
ChatPartner, CopCarBeingStolen, CreatePartnerTask, Danger, DontJoinGroup, DraggedOutCar, FireNearby, GunAimedAt,
HighAngerAtPlayer, InWater, InteriorUseInfo, LowAngerAtPlayer, LowHealth, ObjectCollisionPassive y ObjectCollision."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
SOURCES = [E + f'EventHandler.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'evhb', 'EvHBLive', SOURCES)))
