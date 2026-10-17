"""x87gen con operandos canonicos (fc.ann) y tabla de simbolos opcional.
Uso: python x87gen2.py <inicio> <fin> [simbolos.json]
simbolos.json: {"F+0x1c": "tx", "esi + 0x90": "turnMass", "0x858b50": "kZero", ...}  (clave = texto entre corchetes sin espacios extra)."""
import sys, json, re
import x87gen as g

ann = {}
for l in open('fc.ann'):
    m = re.match(r'([0-9a-f]+): (\S+)\s*(.*?)(\s+;\?)?$', l.rstrip('\n'))
    ann[int(m.group(1), 16)] = (m.group(2), m.group(3))

syms = json.load(open(sys.argv[3])) if len(sys.argv) > 3 else {}


def subst(text):
    def rep(m):
        k = m.group(1)
        return syms.get(k, '[' + k + ']')
    return re.sub(r'\[([^\]]+)\]', rep, text)


a = int(sys.argv[1], 16)
e = int(sys.argv[2], 16)
code = g.EXE[g.off(a):g.off(a) + (e - a) + 16]
for ins in g.md.disasm(code, a):
    if ins.address >= e:
        break
    opstr = ann.get(ins.address, (None, ins.op_str))[1]

    class I:
        pass
    j = I()
    j.op_str = opstr
    j.mnemonic = ins.mnemonic
    t = g.decode(bytes(ins.bytes), j)
    if t:
        print('    %-52s // %x: %s %s' % (subst(t), ins.address, ins.mnemonic, opstr))
    else:
        print('    // %x: %s %s' % (ins.address, ins.mnemonic, opstr))
