"""Genera exebytes.inc (bytes reales del EXE usados por los arneses sin juego) y gates_extract.hpp (compuertas del banco).

Uso:  python gen_exebytes.py [ruta\\gta-sa.exe]
El EXE debe ser el original (SHA-256 en reversing/runtime_runner.py: EXE_HASH aplica al gta_sa.exe de pruebas; aqui se lee gta-sa.exe sin parchear).
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


def dump(name, a, n):
    p = off(a)
    return 'static const unsigned char %s[]={%s};\n' % (name, ','.join('0x%02x' % x for x in exe[p:p + n]))


text = (dump('kSlerp', 0x59C300, 0xD8) + dump('kGetFirst', 0x734930, 0x40) + dump('kForAllTex', 0x7F3730, 0x40) + dump('kReplace', 0x734DE0, 0x70)
        + dump('kFindIdx', 0x74E420, 0x28) + dump('kMatDestroy', 0x74DA20, 0x70))
(HERE / 'exebytes.inc').write_text(text)

# compuertas del banco (sin dependencias del juego) para gates_test.cpp
src = (HERE.parent / 'LiveComparison.hpp').read_text(encoding='utf-8')
seg = src[src.index('inline void SaveFP'):src.index('inline std::string Hex')].replace('inline void Write', '//')
(HERE / 'gates_extract.hpp').write_text('namespace Gates {\n' + seg + '\n}\n', encoding='utf-8')
print('exebytes.inc y gates_extract.hpp generados')
