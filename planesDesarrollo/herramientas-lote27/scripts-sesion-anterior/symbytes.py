import struct, sys, re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
path = sys.argv[1]
want = re.compile(sys.argv[2])
data = open(path, 'rb').read()
big = struct.unpack_from('<HH', data, 0) == (0, 0xFFFF)
if big:
    nsec, symptr, nsym = struct.unpack_from('<III', data, 44)
    off = 56
    SYMSZ = 20
    secfmt = '<8sIIIIIIHHI'
else:
    machine, nsec, ts, symptr, nsym, optsz, chars = struct.unpack_from('<HHIIIHH', data, 0)
    off = 20 + optsz
    SYMSZ = 18
secs = []
for i in range(nsec):
    name, vsz, vaddr, rawsz, rawptr, relptr, lnptr, nrel, nln, ch = struct.unpack_from('<8sIIIIIIHHI', data, off + 40 * i)
    secs.append((rawsz, rawptr))
strtab = symptr + nsym * SYMSZ
def symname(raw):
    if raw[:4] == b'\0\0\0\0':
        o = struct.unpack_from('<I', raw, 4)[0]
        e = data.index(b'\0', strtab + o)
        return data[strtab + o:e].decode('latin1')
    return raw.rstrip(b'\0').decode('latin1')
md = Cs(CS_ARCH_X86, CS_MODE_32)
i = 0
while i < nsym:
    base = symptr + i * SYMSZ
    if big:
        raw = data[base:base + 8]
        value, secnum, typ, sclass, naux = struct.unpack_from('<IIHBB', data, base + 8)
    else:
        raw = data[base:base + 8]
        value, secnum, typ, sclass, naux = struct.unpack_from('<IhHBB', data, base + 8)
    n = symname(raw)
    if want.search(n) and secnum > 0 and (typ & 0x20):
        rawsz, rawptr = secs[secnum - 1]
        code = data[rawptr + value: rawptr + min(rawsz, value + 64)]
        print(n)
        for ins in md.disasm(code, 0):
            print('   %s %s  %s' % (ins.mnemonic, ins.op_str, ins.bytes.hex()))
            if ins.mnemonic.startswith('ret') or ins.mnemonic == 'jmp':
                break
    i += 1 + naux
