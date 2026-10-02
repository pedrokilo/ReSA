"""Compila la sonda temporal del banco CAEAudioChannel."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Audio/Hardware/AEAudioChannel.cpp', 'source/game_sa/Audio/Hardware/AEAudioChannel.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'channel', 'ChannelLive', SOURCES)))
