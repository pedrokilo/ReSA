"""Desensambla un rango de VA del gta_sa.exe original (capstone). Uso: python -I desens.py <exe> <va_ini> <n_bytes>"""
import sys, struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
d = open(sys.argv[1], 'rb').read()
pe = struct.unpack_from('<I', d, 60)[0]
ns, = struct.unpack_from('<H', d, pe + 6)
opt, = struct.unpack_from('<H', d, pe + 20)
secs = []
for i in range(ns):
    p = pe + 24 + opt + 40 * i
    vs, rva, rs, rp = struct.unpack_from('<IIII', d, p + 8)
    secs.append((rva + 0x400000, vs, rp, rs))
va = int(sys.argv[2], 16); n = int(sys.argv[3], 0)
for sv, vs, rp, rs in secs:
    if sv <= va < sv + vs:
        off = rp + va - sv
        break
md = Cs(CS_ARCH_X86, CS_MODE_32)
for i in md.disasm(d[off:off + n], va):
    print(f'{i.address:08X}  {i.mnemonic} {i.op_str}')
