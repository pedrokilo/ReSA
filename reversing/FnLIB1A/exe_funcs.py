"""Descubre las funciones de libpng+zlib en el EXE (0x7CF600-0x7EB5C0) por descenso recursivo y extrae
llamadas directas, cadenas, constantes y datos."""
import sys, csv, collections
sys.path.insert(0, '.')
import exe_lib as L
from capstone.x86 import *
LO, HI = 0x7CF600, 0x7EB5C0
def known_starts():
    s = set()
    for x in csv.DictReader(open('../../docs/RenderWareExe/funciones.csv')):
        if x['clase_modulo'] == 'libpng + zlib': s.add(int(x['va'], 16))
    return s
def analyze(start, lo=LO, hi=HI):
    seen = {}; work = [start]; calls = []; jt = []
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
                if ins.operands[0].type == X86_OP_IMM:
                    work.append(ins.operands[0].imm)
                elif ins.operands[0].type == X86_OP_MEM and ins.operands[0].mem.scale == 4 and ins.operands[0].mem.base == 0:
                    tbl = ins.operands[0].mem.disp
                    jt.append(tbl)
                    for k in range(256):
                        t = int.from_bytes(L.read(tbl+4*k, 4).ljust(4, b'\0'), 'little')
                        if lo <= t < hi: work.append(t)
                        else: break
                break
            if m.startswith('j'):
                if ins.operands[0].type == X86_OP_IMM: work.append(ins.operands[0].imm)
            if m == 'call' and ins.operands[0].type == X86_OP_IMM:
                calls.append((a, ins.operands[0].imm))
            a += ins.size
    return seen, calls
def features(seen, calls):
    strings = []; imms = []; dref = []; ind = 0; iat = []
    for a, ins in sorted(seen.items()):
        if ins.mnemonic == 'call' and ins.operands[0].type != X86_OP_IMM:
            ind += 1
            if ins.operands[0].type == X86_OP_MEM and ins.operands[0].mem.base == 0 and ins.operands[0].mem.index == 0:
                iat.append(ins.operands[0].mem.disp)
        for op in ins.operands:
            v = None
            if op.type == X86_OP_IMM: v = op.imm & 0xFFFFFFFF
            elif op.type == X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0: v = op.mem.disp & 0xFFFFFFFF
            if v is None: continue
            if 0x858000 <= v < 0xCB0000:
                s = L.cstring(v)
                if s and len(s) >= 3 and ins.mnemonic in ('push', 'mov', 'lea'): strings.append(s)
                else: dref.append(v)
            elif ins.mnemonic in ('mov', 'cmp', 'push') and op.type == X86_OP_IMM:
                imms.append(v)
    return strings, imms, dref, ind, iat
def discover():
    starts = set(known_starts())
    # candidatos extra: tras relleno 0x90 y que parezcan prologo
    b = L.read(LO, HI-LO)
    i = 0
    while i < len(b):
        if b[i] == 0x90:
            j = i
            while j < len(b) and b[j] == 0x90: j += 1
            if j-i >= 1 and j < len(b) and b[j] in (0x53, 0x55, 0x56, 0x57, 0x51, 0x8b, 0x83, 0x81, 0xa1, 0x33, 0x6a, 0x64, 0xb8):
                starts.add(LO + j)
            i = j
        else: i += 1
    funcs = {}
    work = sorted(starts)
    done = set()
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
    f = discover()
    ks = known_starts()
    print(len(f), len(ks), len([s for s in f if s not in ks]))
    ss = sorted(f)
    out = []
    for s in ss:
        seen, calls = f[s]
        end = max(a+i.size for a, i in seen.items())
        strings, imms, dref, ind, iat = features(seen, calls)
        out.append(dict(va=hex(s), size=end-s, ninstr=len(seen), calls=';'.join(sorted(set(hex(t) for _, t in calls))), ncalls=len(calls),
                        indirect=ind, x87=sum(1 for i in seen.values() if i.mnemonic.startswith('f') and i.mnemonic not in ('fs',)), strings='|'.join(strings), known=int(s in ks)))
    w = csv.DictWriter(open('exe_funcs.csv', 'w', newline='', encoding='utf-8'), fieldnames=list(out[0]))
    w.writeheader(); w.writerows(out)
