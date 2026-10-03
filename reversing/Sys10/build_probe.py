"""Compila la sonda temporal del banco Sys10 (CAEMFDecoder, CAESmoothFadeThread, CConversations, CDirectory, CEventHandlerHistory)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Loaders/AEMFDecoder.cpp', 'Audio/Loaders/AEMFDecoder.h', 'Audio/AESmoothFadeThread.cpp', 'Audio/AESmoothFadeThread.h',
                           'Conversations/Conversations.cpp', 'Conversations/Conversations.h', 'Directory.cpp', 'Directory.h',
                           'Events/EventHandlerHistory.cpp', 'Events/EventHandlerHistory.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'sys10', 'Sys10Live', SOURCES)))
