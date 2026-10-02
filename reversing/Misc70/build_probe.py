"""Compila la sonda temporal del banco Misc70 (audio, interfaz y render, lista de 70)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/Loaders/AEStreamingDecoder.cpp', 'Audio/Loaders/AEStreamingDecoder.h', 'Audio/Loaders/AEStreamTransformer.cpp',
                           'Audio/Loaders/AEStreamTransformer.h', 'Collision/ColourSet.cpp', 'Collision/ColourSet.h', 'Crime.cpp', 'Crime.h',
                           'CurrentVehicle.cpp', 'CurrentVehicle.h', 'FallingGlassPane.cpp', 'FallingGlassPane.h', 'Gamma.cpp', 'Gamma.h',
                           'OnscreenCounterEntry.cpp', 'OnscreenCounterEntry.h', 'PlaceName.cpp', 'PlaceName.h', 'RegisteredMotionBlurStreak.cpp',
                           'RegisteredMotionBlurStreak.h', 'UserDisplay.cpp', 'UserDisplay.h')] + ['source/app/securom/Securom.cpp', 'source/app/securom/Securom.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'misc70', 'Misc70Live', SOURCES)))
