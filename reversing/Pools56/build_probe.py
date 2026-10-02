"""Compila la sonda temporal del banco de pools y clases sueltas (lista de 56)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Core/Pool.h', 'Pools/BuildingPool.h', 'Pools/ColModelPool.h', 'Pools/DummyPool.h', 'Pools/EntryInfoNodePool.h', 'Pools/EventPool.h',
                           'Pools/NodeRoutePool.h', 'Pools/PedAttractorPool.h', 'Pools/PedIntelligencePool.h', 'Pools/PointRoutePool.h',
                           'Pools/PtrNodeDoubleLinkPool.h', 'Pools/PtrNodeSingleLinkPool.h', 'Pools/TaskAllocatorPool.h', 'Pools/TaskPool.h', 'Pools/VehiclePool.h',
                           'Entity/Dummy/Dummy.cpp', 'Core/EntryInfoList.cpp', 'Core/EntryInfoNode.cpp', 'PedTaskPair.cpp', 'Shadows.cpp',
                           'Conversations/ConversationNode.cpp', 'Tasks/TaskTimer.cpp', 'Tasks/TaskTimer.h', 'PathFind.h', 'PathFind.cpp',
                           'Collision/ColLine.cpp', 'Fx/FxEmitterPrt.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'pools56', 'Pools56Live', SOURCES)))
