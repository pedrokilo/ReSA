"""Compila la sonda temporal del banco de búfer de render, cámara RW y modos de vídeo (lista de 8)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/RenderBuffer.cpp', 'source/game_sa/RenderBuffer.hpp', 'source/game_sa/RenderWare/rw/rwcore.cpp',
           'source/game_sa/RenderWare/rw/rwplcore.h', 'source/app/platform/win/VideoMode.cpp', 'source/app/platform/win/VideoMode.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'rendervideo8', 'RenderVideo8Live', SOURCES)))
