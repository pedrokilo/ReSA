"""Compila la sonda temporal del banco de núcleo y datos (lista de 70)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Collision/Box.cpp', 'Entity/Building.cpp', 'Plugins/CollisionPlugin/CollisionPlugin.cpp', 'Collision/ColTrianglePlane.cpp',
                           'CompressedMatrixNotAligned.cpp', 'ControllerState.cpp', 'Text/Data.cpp', 'Draw.cpp', 'Draw.h', 'Entity/Dummy/DummyObject.cpp',
                           'Core/EntryInfoNode.cpp', 'Pools/EntryInfoNodePool.h', 'NodeRoute.cpp', 'Pools/NodeRoutePool.h', 'ObjectData.cpp',
                           'Pools/ObjectPool.h', 'Pools/PedPool.h', 'Core/Pool.h', 'ObjectSaveStructure.cpp', 'SimpleVariablesSaveStructure.cpp',
                           'VehicleSaveStructure.cpp', 'Collision/TempColModels.cpp', 'ModelIndices.cpp', 'Fx/FxPrimBP.cpp', 'WindModifiers.cpp',
                           'Plant/PlantColEntEntry.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'core70', 'Core70Live', SOURCES)))
