import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/GenericGameStorage.cpp', 'source/game_sa/GenericGameStorage.h', 'source/game_sa/C_PcSave.h', 'source/game_sa/Core/KeyGen.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'storage-game355', 'StorageGame355Live', SOURCES)))
