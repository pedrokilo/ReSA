import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Core/Matrix.h','source/game_sa/Entity/Placeable.cpp','source/game_sa/Collision/ColModel.h','source/game_sa/Models/BaseModelInfo.h']
SOURCES += ['source/game_sa/DamageManager.cpp', 'source/game_sa/Entity/Vehicle/Automobile.cpp', 'source/game_sa/Collision/ColLine.cpp', 'source/game_sa/Collision/ColSphere.cpp', 'source/game_sa/SurfaceInfos_c.cpp', 'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Collision/CollisionData.h']
SOURCES += ['source/game_sa/Weapon.cpp', 'source/game_sa/Weapon.h', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Collision/Collision.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'weapon-occupant355', 'WeaponOccupant355Live', SOURCES)))
