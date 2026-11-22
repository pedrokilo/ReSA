"""Desensambla funciones del gta_sa.exe original. Uso: python -I dis.py <gta_sa.exe> <va> [<bytes=160>] [<va> [<bytes>]...]"""
import struct, sys
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
d = Path(sys.argv[1]).read_bytes()
pe = struct.unpack_from('<I', d, 60)[0]
ns, = struct.unpack_from('<H', d, pe + 6); opt, = struct.unpack_from('<H', d, pe + 20)
secs = []
for i in range(ns):
    p = pe + 24 + opt * 1 + 40 * i
    vs, rva, rs, rp = struct.unpack_from('<IIII', d, p + 8)
    secs.append((0x400000 + rva, min(vs, rs), rp))
def rd(a, n):
    for va, sz, rp in secs:
        if va <= a < va + sz: return d[rp + a - va: rp + a - va + n]
    return b''
md = Cs(CS_ARCH_X86, CS_MODE_32)
args = sys.argv[2:]
i = 0
while i < len(args):
    a = int(args[i], 16); n = 160
    if i + 1 < len(args) and not args[i + 1].startswith('0x'): n = int(args[i + 1]); i += 1
    i += 1
    print('--- 0x%08X' % a)
    for ins in md.disasm(rd(a, n), a):
        print('  %08X  %-6s %s' % (ins.address, ins.mnemonic, ins.op_str))
        if ins.mnemonic in ('ret', 'retn') and ins.address > a + 8 and 'int3' not in ins.mnemonic: break
