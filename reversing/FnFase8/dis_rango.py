"""Desensambla un rango completo del gta_sa.exe original (sin parar en ret). Uso: python -I dis_rango.py <gta_sa.exe> <va_ini> <va_fin>"""
import struct, sys
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
d = Path(sys.argv[1]).read_bytes()
pe = struct.unpack_from('<I', d, 60)[0]
ns, = struct.unpack_from('<H', d, pe + 6); opt, = struct.unpack_from('<H', d, pe + 20)
secs = []
for i in range(ns):
    p = pe + 24 + opt + 40 * i
    vs, rva, rs, rp = struct.unpack_from('<IIII', d, p + 8)
    secs.append((0x400000 + rva, min(vs, rs), rp))
def rd(a, n):
    for va, sz, rp in secs:
        if va <= a < va + sz: return d[rp + a - va: rp + a - va + n]
    return b''
a0 = int(sys.argv[2], 16); a1 = int(sys.argv[3], 16)
md = Cs(CS_ARCH_X86, CS_MODE_32)
for ins in md.disasm(rd(a0, a1 - a0), a0):
    print('%08X  %-6s %s' % (ins.address, ins.mnemonic, ins.op_str))
