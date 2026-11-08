"""Construye unicamente el banco Fn129A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/RGBA.cpp',
    'source/game_sa/Events/EventEscalator.cpp',
    'source/game_sa/Events/EventEscalator.h',
    'source/game_sa/Audio/Entities/AEPedAudioEntity.cpp',
    'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp',
    'source/game_sa/AccidentManager.cpp',
    'source/game_sa/Audio/AESmoothFadeThread.cpp',
    'source/game_sa/Collision/ColModel.cpp',
    'source/game_sa/Entity/Vehicle/Plane.cpp',
    'source/game_sa/Pools.cpp',
    'source/game_sa/Conversations/Conversations.cpp',
    'source/game_sa/ShadowCamera.cpp',
    'source/game_sa/Core/Vector.cpp',
    'source/game_sa/Collision/CollisionData.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn129A/LiveComparison.hpp',
    'reversing/Fn129A/Core.hpp',
    'reversing/Fn129A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn129a', 'F129ALive', SOURCES)))
