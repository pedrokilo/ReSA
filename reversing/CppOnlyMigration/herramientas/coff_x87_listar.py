"""Lista, por función de un .obj COFF x86 (normal o bigobj), la secuencia de instrucciones x87 y las llamadas (con el símbolo de la relocación).

Uso: python -B coff_x87_listar.py <objeto.obj> [regex de símbolos]. Requiere capstone. Sirve para comprobar que fsin/fcos/fpatan/fyl2x salen en línea y sin derrames intermedios."""
import struct, sys, re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

path = sys.argv[1]
want = re.compile(sys.argv[2]) if len(sys.argv) > 2 else None
data = open(path, 'rb').read()
big = struct.unpack_from('<HH', data, 0) == (0, 0xFFFF)
if big:
    nsec, symptr, nsym = struct.unpack_from('<III', data, 44)
    off = 56
    SYMSZ = 20
else:
    machine, nsec, ts, symptr, nsym, optsz, chars = struct.unpack_from('<HHIIIHH', data, 0)
    off = 20 + optsz
    SYMSZ = 18
secs = []
for i in range(nsec):
    name, vsz, vaddr, rawsz, rawptr, relptr, lnptr, nrel, nln, ch = struct.unpack_from('<8sIIIIIIHHI', data, off + 40 * i)
    secs.append(dict(name=name.rstrip(b'\0').decode('latin1'), rawsz=rawsz, rawptr=rawptr, relptr=relptr, nrel=nrel, ch=ch))
strtab_off = symptr + SYMSZ * nsym
strsz = struct.unpack_from('<I', data, strtab_off)[0]
strtab = data[strtab_off:strtab_off + strsz]


def sname(raw):
    if raw[:4] == bytes(4):
        o = struct.unpack_from('<I', raw, 4)[0]
        return strtab[o:strtab.index(bytes(1), o)].decode('latin1')
    return raw.rstrip(bytes(1)).decode('latin1')


syms = []
i = 0
while i < nsym:
    raw = data[symptr + SYMSZ * i: symptr + SYMSZ * i + SYMSZ]
    n = sname(raw[:8])
    if big:
        val, secn, typ, sc, naux = struct.unpack_from('<IiHBB', raw, 8)
    else:
        val, secn, typ, sc, naux = struct.unpack_from('<IhHBB', raw, 8)
    syms.append(dict(i=i, name=n, val=val, sec=secn, typ=typ, sc=sc))
    i += 1 + naux
byidx = {s['i']: s for s in syms}
md = Cs(CS_ARCH_X86, CS_MODE_32)
out = []
for s in syms:
    if s['typ'] != 0x20 or s['sec'] <= 0 or s['sc'] != 2:
        continue
    if want and not want.search(s['name']):
        continue
    sec = secs[s['sec'] - 1]
    code = data[sec['rawptr']: sec['rawptr'] + sec['rawsz']]
    rel = {}
    for k in range(sec['nrel']):
        va, si, rt = struct.unpack_from('<IIH', data, sec['relptr'] + 10 * k)
        rel[va] = byidx.get(si, {}).get('name', '?')
    seq = []
    for ins in md.disasm(code, 0):
        m = ins.mnemonic
        if m.startswith('f') or m == 'call' or m == 'sahf':
            t = m
            if m == 'call':
                tgt = next((rel[a] for a in range(ins.address, ins.address + ins.size) if a in rel), '?')
                t = 'call ' + tgt
            elif m in ('fstp', 'fst', 'fld') and 'qword' in ins.op_str:
                t = m + '.q'
            elif m in ('fstp', 'fst', 'fld') and 'tbyte' in ins.op_str:
                t = m + '.t'
            seq.append(t)
    out.append((s['name'], len(code), seq))
for n, sz, seq in out:
    print(f'{n} ({sz} bytes): ' + ' '.join(seq))
