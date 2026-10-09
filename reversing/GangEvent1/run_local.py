import re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
import probe_case
from probe_case import main
# El juego completo solo arranca desde la ruta real de E: (a traves del junction D:/JUEGOS se cierra a los ~5 s).
# Se redirige en memoria, sin tocar runtime_runner.py (su hash esta congelado en los manifiestos de los bancos).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src)}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'gangevent1', 'CGroupEventHandler + CGangWars (banco simple)', EXPECTED, timeout=1500))
