"""Compila la sonda temporal del banco de hooks duplicados y virtuales con hook plano (FixHooks)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
SOURCES = [T + f'Task{c}.{e}' for c in ('ComplexDiveFromAttachedEntityAndGetUp', 'ComplexEvasiveDiveAndGetUp', 'ComplexWanderFlee', 'ComplexWander',
                                        'ComplexCarSlowBeDraggedOutAndStandUp', 'ComplexGoToPointAndStandStillAndAchieveHeading',
                                        'ComplexGangFollower', 'ComplexGotoDoorAndOpen', 'ComplexGoToPointAnyMeans', 'GangHassleVehicle',
                                        'GoToVehicleAndLean', 'SimpleDead') for e in ('cpp', 'h')]
SOURCES += [G + p for p in ('Audio/Loaders/AEDataStream.cpp', 'Audio/Loaders/AEDataStream.h', 'Audio/Hardware/AEStreamingChannel.cpp',
                            'Audio/Hardware/AEStreamingChannel.h', 'Entity/Entity.cpp', 'Entity/Entity.h', 'Entity/Vehicle/Bike.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fixhooks', 'FixHooksLive', SOURCES)))
