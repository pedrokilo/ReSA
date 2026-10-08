import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Weapon.cpp', 'source/game_sa/Weapon.h', 'source/game_sa/Darkel.cpp', 'source/game_sa/Sprite.cpp', 'source/game_sa/Core/Pool.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'weapon-screen355', 'WeaponScreen355Live', SOURCES)))
