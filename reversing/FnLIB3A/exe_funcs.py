"""Descubre las funciones de libjpeg 6b en el EXE (rango LO..HI) por descenso recursivo desde los inicios tras relleno 0x90 y las raices conocidas."""
import sys, csv
sys.path.insert(0, '.')
import exe_lib as L
from capstone.x86 import *
LO, HI = 0x5C2600, 0x5D8000
def analyze(start, lo=LO, hi=HI):
    seen = {}; work = [start]; calls = []
    while work:
        a = work.pop()
        while True:
            if a in seen or not (lo <= a < hi + 0x400): break
            ins = next(L.md.disasm(L.read(a, 16), a), None)
            if ins is None: break
            seen[a] = ins
            m = ins.mnemonic
            if m == 'ret': break
            if m == 'jmp':
                if ins.operands[0].type == X86_OP_IMM: work.append(ins.operands[0].imm)
                elif ins.operands[0].type == X86_OP_MEM and ins.operands[0].mem.scale == 4 and ins.operands[0].mem.base == 0:
                    tbl = ins.operands[0].mem.disp
                    for k in range(256):
                        t = int.from_bytes(L.read(tbl+4*k, 4).ljust(4, b'\0'), 'little')
                        if lo <= t < hi: work.append(t)
                        else: break
                break
            if m.startswith('j') and ins.operands[0].type == X86_OP_IMM: work.append(ins.operands[0].imm)
            if m == 'call' and ins.operands[0].type == X86_OP_IMM: calls.append((a, ins.operands[0].imm))
            a += ins.size
    return seen, calls
def features(seen):
    strings = []; imms = []; ind = 0
    for a, ins in sorted(seen.items()):
        if ins.mnemonic == 'call' and ins.operands[0].type != X86_OP_IMM: ind += 1
        for op in ins.operands:
            v = None
            if op.type == X86_OP_IMM: v = op.imm & 0xFFFFFFFF
            elif op.type == X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0: v = op.mem.disp & 0xFFFFFFFF
            if v is None: continue
            if 0x858000 <= v < 0xCB0000:
                s = L.cstring(v)
                if s and len(s) >= 3 and ins.mnemonic in ('push', 'mov', 'lea'): strings.append(s)
            elif ins.mnemonic in ('mov', 'cmp', 'push', 'and', 'add', 'sub', 'imul') and op.type == X86_OP_IMM: imms.append(v)
    return strings, imms, ind
def discover(extra):
    starts = set(extra)
    b = L.read(LO, HI-LO)
    i = 0
    while i < len(b):
        if b[i] == 0x90:
            j = i
            while j < len(b) and b[j] == 0x90: j += 1
            if j < len(b) and (j - i >= 1 or True): starts.add(LO + j)
            i = j
        else: i += 1
    funcs = {}; work = sorted(starts); done = set()
    while work:
        s = work.pop()
        if s in done or not (LO <= s < HI): continue
        done.add(s)
        seen, calls = analyze(s)
        for _, t in calls:
            if LO <= t < HI and t not in done: work.append(t)
        funcs[s] = (seen, calls)
    return funcs
if __name__ == '__main__':
    roots = [int(r['direccion'], 16) for r in csv.DictReader(open('../../docs/ExeIndependiente/raices.csv', encoding='utf-8')) if 'jpeg' in r['modulo'].lower()]
    f = discover(roots)
    out = []
    for s in sorted(f):
        seen, calls = f[s]
        if not seen: continue
        end = max(a+i.size for a, i in seen.items())
        strings, imms, ind = features(seen)
        out.append(dict(va=hex(s), size=end-s, ninstr=len(seen), calls=';'.join(sorted(set(hex(t) for _, t in calls))), ncalls=len(calls), indirect=ind,
                        x87=sum(1 for i in seen.values() if i.mnemonic.startswith('f')), strings='|'.join(strings), raiz=int(s in roots)))
    w = csv.DictWriter(open('exe_funcs.csv', 'w', newline='', encoding='utf-8'), fieldnames=list(out[0]))
    w.writeheader(); w.writerows(out)
    print(len(out), 'funciones;', sum(1 for r in out if r['raiz']), 'raices')
