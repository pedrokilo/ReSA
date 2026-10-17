"""Crea x87src/X87Math.cpp y .h = fuente actual del repositorio + los ayudantes VehHeadTail (g22b-x87.cpp.txt / g22b-x87.h.txt; si X87Math.cpp ya los contiene, no se anaden).
Uso: python make_x87src.py <raiz del repositorio> [<g22b-x87.cpp.txt> <g22b-x87.h.txt>]"""
import sys
from pathlib import Path
root = Path(sys.argv[1])
out = Path(__file__).parent / 'x87src'
out.mkdir(exist_ok=True)
extras = {'X87Math.cpp': Path(sys.argv[2]) if len(sys.argv) > 3 else None, 'X87Math.h': Path(sys.argv[3]) if len(sys.argv) > 3 else None}
for name, extra in extras.items():
    base = (root / 'source/game_sa' / name).read_bytes().decode('utf-8').replace('\r\n', '\n')
    if not base.endswith('\n'):
        base += '\n'
    if extra is not None and 'VehHeadTailHeadGeometryCpp' not in base:
        base += extra.read_text(encoding='utf-8')
    (out / name).write_text(base, encoding='utf-8', newline='\n')
print('x87src listo en', out)
