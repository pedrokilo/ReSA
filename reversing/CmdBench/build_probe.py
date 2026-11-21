"""Construye unicamente el banco CmdBench (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/game_sa/Scripts/RunningScript.cpp',
    'source/game_sa/Scripts/RunningScript.h',
    'source/game_sa/Scripts/CommandParser/Parser.hpp',
    'source/game_sa/Scripts/CommandParser/Signature.hpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/CmdBench/LiveComparison.hpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'cmdbench', 'CmdBenchLive', SOURCES)))
