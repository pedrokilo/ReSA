"""Construye unicamente el banco Fn709CF0 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/game_sa/Shadows.cpp',
    'source/game_sa/Shadows.h',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'source/game_sa/RenderBuffer.cpp',
    'source/game_sa/RenderBuffer.hpp',
    'reversing/Fn709CF0/LiveComparison.hpp',
    'reversing/Fn709CF0/Core.hpp',
    'reversing/Fn709CF0/Fixtures.hpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn709cf0', 'F709Live', SOURCES)))
