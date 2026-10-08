import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Audio/Entities/AEPoliceScannerAudioEntity.cpp','Audio/Entities/AEPoliceScannerAudioEntity.h','Audio/AESound.cpp','Audio/AESound.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'audio-scanner-pending','AudioScannerPendingLive',SOURCES)))
