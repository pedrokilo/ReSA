"""Genera exebytes.inc: los bytes REALES de las 20 funciones del EXE original (gta-sa.exe sin parchear) que ejecuta el arnes aislado en sus direcciones verdaderas.

Uso:  python gen_exebytes.py [ruta\\gta-sa.exe]
La longitud de cada funcion es la de la columna `bytes` de reversing/OriginalDependencies2026-10-17 (lote25-lib2): todas terminan en `ret` (o `jmp`) dentro de ese rango.
"""
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
exe = open(sys.argv[1] if len(sys.argv) > 1 else r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3c)[0]
nsec = struct.unpack_from('<H', exe, pe + 6)[0]
opt = struct.unpack_from('<H', exe, pe + 20)[0]
base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
secs = []
o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, rp = struct.unpack_from('<IIII', exe, o + 8 + i * 40)
    secs.append((va, vs, rp, rs))


def off(a):
    r = a - base
    for va, vs, rp, rs in secs:
        if va <= r < va + max(vs, rs):
            return rp + r - va
    raise KeyError(hex(a))


FUNCTIONS = [(0x758920, 26), (0x7591B0, 18), (0x75E9F0, 6), (0x761C90, 39), (0x761CC0, 34), (0x7C5160, 15), (0x7C8820, 5), (0x7CC010, 6), (0x7CC9D0, 32), (0x7CD4F0, 35),
             (0x7CD590, 12), (0x7CED70, 22), (0x7CEEE0, 8), (0x7ED460, 28), (0x7EE1A0, 40), (0x7EE450, 39), (0x7F0340, 17), (0x7F1920, 5), (0x7F2BB0, 36), (0x7F2BE0, 19)]
lines = ['// Generado por gen_exebytes.py: bytes del gta-sa.exe original (direccion, longitud, bytes).', 'struct ExeFunction { unsigned address, size; const unsigned char* bytes; };']
rows = []
for a, n in FUNCTIONS:
    p = off(a)
    data = exe[p:p + n]
    lines.append('static const unsigned char kExe_%X[]={%s};' % (a, ','.join('0x%02x' % x for x in data)))
    rows.append('{0x%X,%d,kExe_%X}' % (a, n, a))
lines.append('static const ExeFunction kExeFunctions[]={%s};' % ','.join(rows))
(HERE / 'exebytes.inc').write_text('\n'.join(lines) + '\n')
print('exebytes.inc: %d funciones' % len(FUNCTIONS))
