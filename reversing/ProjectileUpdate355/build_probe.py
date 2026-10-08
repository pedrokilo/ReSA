import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h', 'source/game_sa/Fx/FxSystem.cpp', 'source/game_sa/Fx/FxPrtMult.cpp', 'source/game_sa/Weapon.cpp', 'source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/Entity/Physical.cpp', 'source/game_sa/World.cpp', 'source/game_sa/common.cpp', 'source/game_sa/General.cpp', 'source/game_sa/Audio/AudioEngine.cpp', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Matrix.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'projectile-update355', 'ProjectileUpdate355Live', SOURCES)))
