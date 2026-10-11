"""Sonda de dos adaptadores de cache de colision, después de aplicar el parche propuesto."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Collision/Collision.cpp', 'source/game_sa/Collision/Collision.h', 'source/game_sa/Collision/ColModel.cpp', 'source/game_sa/Collision/ColModel.h', 'source/game_sa/Collision/CollisionData.cpp', 'source/game_sa/Collision/CollisionData.h', 'source/game_sa/Collision/ColTrianglePlane.cpp', 'source/game_sa/Collision/ColTrianglePlane.h', 'source/game_sa/Collision/ColTriangle.h', 'source/game_sa/CompressedVector.h', 'source/game_sa/MemoryMgr.cpp', 'source/game_sa/MemoryMgr.h', 'source/game_sa/Core/Link.h', 'source/game_sa/Core/LinkList.h', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h', 'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/DependenciesCollisionAdapters1/HelperHookRoute.hpp', 'reversing/DependenciesCollisionAdapters1/AllocationTrace.hpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciescollisionadapters1','DependenciesCollisionAdapters1Live',SOURCES)))
