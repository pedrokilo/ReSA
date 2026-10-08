import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Audio/Managers/AERadioTrackManager.cpp','Audio/Managers/AERadioTrackManager.h','Audio/Managers/AEUserRadioTrackManager.cpp','Audio/Managers/AEUserRadioTrackManager.h','Audio/Entities/AEVehicleAudioEntity.cpp','Audio/Entities/AEVehicleAudioEntity.h','Audio/AEAudioUtility.cpp','Audio/AEAudioUtility.h','Audio/Hardware/AEAudioHardware.cpp','Audio/Hardware/AEAudioHardware.h','Stats.cpp','Stats.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'radio-track355','RadioTrack355Live',SOURCES)))
