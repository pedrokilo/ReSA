"""Compila la sonda temporal del banco CppRpAnimUpdate1: RpAnimBlendClumpUpdateAnimations (0x4D34F0) y FrameUpdateCallBackOffscreen (0x4D2E10)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp', 'Plugins/RpAnimBlendPlugin/RpAnimBlend.h', 'X87Math.cpp', 'X87Math.h')]
SOURCES += ['reversing/RpAnim43/LiveComparison.hpp', 'reversing/ProbeRuntime.hpp', 'reversing/CppRpAnimUpdate1/run_local.py']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpp-rpanim-update1', 'CppRpAnimUpdate1Live', SOURCES)))
