"""Serial root-only build; complete sphere-query reconstruction proposal."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=[
 'reversing/DependenciesWorld15/geometry_oracle.py',
 'source/game_sa/Garage.cpp','source/game_sa/Garage.h',
 'source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h',
 'source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Placeable.h',
 'source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Entity/Vehicle/Vehicle.h',
 'source/game_sa/SimpleTransform.cpp','source/game_sa/SimpleTransform.h',
 'source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h',
 'source/game_sa/Core/MatrixLink.cpp','source/game_sa/Core/MatrixLink.h',
 'source/game_sa/Core/MatrixLinkList.cpp','source/game_sa/Core/MatrixLinkList.h',
 'source/game_sa/Collision/CollisionData.cpp','source/game_sa/Collision/CollisionData.h',
 'source/game_sa/Collision/ColModel.cpp','source/game_sa/Collision/ColModel.h',
 'source/game_sa/Collision/ColSphere.h','source/game_sa/Collision/Sphere.h',
 'source/game_sa/Models/BaseModelInfo.cpp','source/game_sa/Models/BaseModelInfo.h',
 'source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp','source/game_sa/Audio/Entities/AEDoorAudioEntity.h',
 'source/game_sa/Audio/Entities/AEAudioEntity.cpp','source/game_sa/Audio/Entities/AEAudioEntity.h',
 'source/game_sa/Audio/AESound.cpp','source/game_sa/Audio/AESound.h',
 'source/game_sa/Pools/Pools.h','source/game_sa/Pools/VehiclePool.h',
 'source/InjectHooksMain.cpp','source/reversiblehooks/ReversibleHook/Simple.h',
 'reversing/OriginalDependencies499/HookRoute.hpp',
 'reversing/DependenciesWorld15/MatrixPoolProof.hpp',
 'reversing/DependenciesWorld10/NewGameConfiguration.py',
]
if __name__=='__main__':
 root=Path(__file__).resolve().parents[2]
 missing=[p for p in SOURCES if not(root/p).is_file()]
 if missing:raise RuntimeError('Absent manifest paths: '+str(missing))
 print(json.dumps(build(Path(__file__).parent,'dependenciesgaragesphere15','DependenciesGarageSphere15Live',SOURCES)))
