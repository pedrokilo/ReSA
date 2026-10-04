"""Compila la sonda temporal del banco FixAudio (arreglos de audio fuera de listas)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/AESound.cpp', 'Audio/AESound.h',
                           'Audio/Hardware/AEAudioChannel.h', 'Audio/Hardware/AEStaticChannel.cpp', 'Audio/Hardware/AEStaticChannel.h',
                           'Audio/Hardware/AEStreamingChannel.cpp', 'Audio/Hardware/AEStreamingChannel.h',
                           'Audio/Hardware/AEAudioHardware.cpp', 'Audio/AEStreamThread.cpp',
                           'Audio/Entities/AECollisionAudioEntity.cpp', 'Audio/Entities/AECollisionAudioEntity.h',
                           'Audio/Entities/AEPedAudioEntity.cpp', 'Audio/Entities/AEPedAudioEntity.h',
                           'Audio/Loaders/AEVorbisDecoder.cpp')] + ['source/reversiblebugfixes/Bugs.hpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fixaudio', 'FixAudioLive', SOURCES)))
