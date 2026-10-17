import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=['source/game_sa/Cam.cpp','source/game_sa/Cam.h','source/game_sa/Core/Vector.cpp','source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h','source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Entity.h','source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Models/BaseModelInfo.h','source/game_sa/Models/ModelInfo.h','source/game_sa/Collision/ColModel.h']
SOURCES+=['source/game_sa/Camera.cpp','source/game_sa/Camera.h','source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/Weapon.cpp','source/game_sa/Weapon.h','source/game_sa/WeaponInfo.cpp','source/game_sa/WeaponInfo.h','source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp','source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h']
SOURCES += [f'source/game_sa/X87Math.{e}' for e in ('cpp', 'h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cam-shooting355','CamShooting355Live',SOURCES)))
