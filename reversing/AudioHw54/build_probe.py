"""Compila la sonda temporal del banco AudioHw54 (CAEAudioHardware, lista de 34)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Hardware/AEAudioHardware.cpp', 'Audio/Hardware/AEAudioHardware.h',
                           'Audio/Hardware/AEAudioChannel.h', 'Audio/Hardware/AEStreamingChannel.h',
                           'Audio/AEStreamThread.cpp', 'Audio/AEStreamThread.h',
                           'Audio/Loaders/AEMP3BankLoader.h', 'Audio/Loaders/AEMP3TrackLoader.h',
                           'Audio/Loaders/AEBankLoader.cpp', 'Audio/Loaders/AEMP3TrackLoader.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'audiohw54', 'AudioHw54Live', SOURCES)))
