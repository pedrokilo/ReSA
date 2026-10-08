import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Audio/Hardware/AEStreamingChannel.cpp','Audio/Hardware/AEStreamingChannel.h','Audio/Hardware/AEAudioChannel.cpp','Audio/Hardware/AEAudioChannel.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'audio-streaming-pending','AudioStreamingPendingLive',SOURCES)))
