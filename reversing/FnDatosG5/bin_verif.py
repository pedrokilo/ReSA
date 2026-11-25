# Verifica con capstone cada referencia de bin_hits.json: desensambla desde el inicio de la funcion del mapa y conserva solo las instrucciones
# con un operando (desplazamiento o inmediato) igual al valor hallado.
import json, re, struct, sys, bisect, os, collections
import capstone
exe, mp = sys.argv[1], sys.argv[2]
d = open(exe, 'rb').read()
pe = struct.unpack_from('<I', d, 0x3C)[0]; base = struct.unpack_from('<I', d, pe + 52)[0]
o = pe + 24 + struct.unpack_from('<H', d, pe + 20)[0]
rva, _rsz, raw = struct.unpack_from('<III', d, o + 12)   # .text
fn = []
for l in open(mp, encoding='utf-8', errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})\s+f?', l)
    if m: fn.append((int(m.group(2), 16), m.group(1)))
fn.sort(); fa = [a for a, _ in fn]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
H = json.load(open('bin_hits.json'))
porfn = collections.defaultdict(list)
for n, L in H.items():
    for kind, val, va, f, ctx in L: porfn[(bisect.bisect_right(fa, va) - 1)].append((n, kind, val, va))
out = collections.defaultdict(list)
for fi, items in porfn.items():
    if fi < 0: continue
    s = fn[fi][0]; e = fn[fi + 1][0] if fi + 1 < len(fn) else s + 4096
    e = min(e, s + 0x20000)
    code = d[raw + (s - base - rva): raw + (e - base - rva)]
    want = {}
    for n, kind, val, va in items: want.setdefault(val, []).append((n, kind, va))
    pos = 0
    while pos < len(code):
        last = pos
        for ins in md.disasm(code[pos:], s + pos):
            last = ins.address + ins.size - s
            vals = set()
            if ins.disp: vals.add(ins.disp & 0xFFFFFFFF)
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_IMM: vals.add(op.imm & 0xFFFFFFFF)
                if op.type == capstone.x86.X86_OP_MEM and op.mem.disp: vals.add(op.mem.disp & 0xFFFFFFFF)
            for v in vals & want.keys():
                for n, kind, va in want[v]:
                    if ins.address <= va < ins.address + ins.size:
                        out[n].append((kind, v, ins.address, fn[fi][1], '%s %s' % (ins.mnemonic, ins.op_str)))
        pos = last + 1 if last == pos else last
        if last != pos: pass
json.dump(out, open('bin_verif.json', 'w'))
print('objetos verificados con referencia real en codigo:', len(out), 'referencias:', sum(len(v) for v in out.values()))
