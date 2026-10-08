import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Cam.cpp','source/game_sa/Cam.h','source/game_sa/ControllerConfigManager.cpp','source/game_sa/ControllerConfigManager.h','source/game_sa/Entity/Vehicle/Automobile.cpp','source/game_sa/Entity/Vehicle/Automobile.h','source/game_sa/Camera.h','source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/RwHelper.cpp','source/game_sa/RwHelper.h','source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Entity.cpp','source/game_sa/Core/Vector.cpp','source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h']
SOURCES+=['source/game_sa/PedIntelligence.cpp','source/game_sa/PedIntelligence.h','source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h','source/game_sa/PlayerPedData.h','source/game_sa/PedClothesDesc.cpp','source/game_sa/PedClothesDesc.h','source/game_sa/Weapon.cpp','source/game_sa/Weapon.h','source/game_sa/WeaponInfo.cpp','source/game_sa/WeaponInfo.h','source/game_sa/Camera.cpp']
SOURCES+=['source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h','source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp','source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.h','source/game_sa/Tasks/TaskManager.cpp','source/game_sa/Tasks/TaskManager.h','source/game_sa/Ragdoll/IKChainManager.cpp','source/game_sa/Ragdoll/IKChainManager.h','source/game_sa/IdleCam.cpp','source/game_sa/IdleCam.h']
SOURCES+=['source/game_sa/General.cpp','source/game_sa/General.h','source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Collision/ColModel.cpp','source/game_sa/Collision/ColModel.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cam-follow-car355', 'CamFollowCar355Live', SOURCES)))

