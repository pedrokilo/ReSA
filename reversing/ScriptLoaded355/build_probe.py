import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Audio/Entities/AEScriptAudioEntity.cpp','Audio/Entities/AEScriptAudioEntity.h','Audio/Managers/AESoundManager.cpp','Audio/Managers/AESoundManager.h','Audio/Hardware/AEAudioHardware.cpp','Audio/Hardware/AEAudioHardware.h','Audio/AESound.cpp','Audio/AESound.h','Audio/Entities/AEAudioEntity.cpp','Audio/Entities/AEAudioEntity.h','Core/Vector.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'script-loaded355','ScriptLoaded355Live',SOURCES)))
