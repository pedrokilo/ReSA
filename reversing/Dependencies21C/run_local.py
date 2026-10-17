"""Exige 3 x 1536 pares OFF/ON exactos de FightSetUpCol, FightHitCar y FightStrike (analizador comun de probe_case)."""
import re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import main
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src)}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'dependencies21c', 'Funciones de pelea CTaskSimpleFight (grupo C)', EXPECTED, timeout=2400))
