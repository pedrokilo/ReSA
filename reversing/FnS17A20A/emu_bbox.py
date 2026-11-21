"""Comprobacion SIN juego de RwBBoxAddPoint (0x809060) y RwBBoxContainsPoint (0x8090E0) del EXE con Unicorn, con casos NaN/inf/0/iguales, contra el modelo escrito en C++
(inf > v -> copia v; sup < v -> copia v; Contains = !(inf > v) y !(sup < v) por eje, en ese orden).   python -I emu_bbox.py [--exe gta_sa.exe]"""
import itertools, struct, sys
from pathlib import Path
import unicorn
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_PROT_ALL
from unicorn.x86_const import *
EXE = Path(sys.argv[sys.argv.index('--exe') + 1]) if '--exe' in sys.argv else Path('E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe')
d = EXE.read_bytes()
pe = struct.unpack_from('<I', d, 0x3C)[0]; nsec = struct.unpack_from('<H', d, pe + 6)[0]; opt = struct.unpack_from('<H', d, pe + 20)[0]
base = struct.unpack_from('<I', d, pe + 24 + 28)[0]; size = struct.unpack_from('<I', d, pe + 24 + 56)[0]
uc = Uc(UC_ARCH_X86, UC_MODE_32); uc.mem_map(base, (size + 0xFFF) & ~0xFFF, UC_PROT_ALL)
for i in range(nsec):
    n, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, pe + 24 + opt + i * 40)
    uc.mem_write(base + va, d[rp:rp + min(rs, vs)])
STK, DAT, RET = 0x7E000000, 0x7D000000, 0x7FFF0000
uc.mem_map(STK, 0x10000, UC_PROT_ALL); uc.mem_map(DAT, 0x1000, UC_PROT_ALL); uc.mem_map(RET, 0x1000, UC_PROT_ALL)
uc.mem_write(RET, b'\x90' * 16)

def run(addr, box, vert):
    uc.mem_write(DAT, struct.pack('<6I', *box) + struct.pack('<3I', *vert))
    esp = STK + 0x8000
    uc.mem_write(esp, struct.pack('<III', RET, DAT, DAT + 24))
    uc.reg_write(UC_X86_REG_ESP, esp)
    uc.emu_start(addr, RET)
    eax = uc.reg_read(UC_X86_REG_EAX)
    return eax, struct.unpack('<6I', bytes(uc.mem_read(DAT, 24)))

def f(b): return struct.unpack('<f', struct.pack('<I', b))[0]
vals = [0x3F800000, 0xBF800000, 0, 0x80000000, 0x40000000, 0x7FC00000, 0x7F800001, 0x7F800000, 0xFF800000, 0x00000001, 0x80000001]
# caja: sup (+0..+8), inf (+0xC..+0x14) -> en RwBBox del SDK: sup primero, luego inf (6 palabras)
bad = 0; n = 0
for s, i, v in itertools.product(vals, vals, vals):
    box = (s, s ^ 0, s, i, i, i); vert = (v, v, v)
    eax, out = run(0x809060, box, vert)
    exp = list(box)
    for k in range(3):
        if f(box[3 + k]) > f(vert[k]): exp[3 + k] = vert[k]
    for k in range(3):
        if f(box[k]) < f(vert[k]): exp[k] = vert[k]
    n += 1
    if eax != DAT or tuple(exp) != out: bad += 1; print('AddPoint', hex(s), hex(i), hex(v), [hex(x) for x in out], [hex(x) for x in exp])
    eax, _ = run(0x8090E0, box, vert)
    ok = not any(f(box[3 + k]) > f(vert[k]) for k in range(3)) and not any(f(box[k]) < f(vert[k]) for k in range(3))
    if (eax & 0xFF) != int(ok): bad += 1; print('Contains', hex(s), hex(i), hex(v), eax, ok)
print('casos', n, 'diferencias', bad)
