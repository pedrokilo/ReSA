"""Para cada llamador del EXE (CALL rel32 al destino): sigue el flujo tras la llamada (ambas ramas, hasta 400 instrucciones por camino) y busca una LECTURA de
EAX/ECX/EDX (o de sus partes) antes de que se escriba. Imprime las lecturas encontradas (con el camino) o 'ninguna'."""
import sys
import struct
import capstone
from capstone import x86_const as X

exe = open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3c)[0]
nsec = struct.unpack_from('<H', exe, pe + 6)[0]
opt = struct.unpack_from('<H', exe, pe + 20)[0]
base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
secs = []
o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, rp = struct.unpack_from('<IIII', exe, o + 8 + i * 40)
    secs.append((va, vs, rp, rs))


def off(a):
    r = a - base
    for va, vs, rp, rs in secs:
        if va <= r < va + max(vs, rs):
            return rp + r - va
    return None


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
text = secs[0]
data = exe[text[2]:text[2] + text[3]]
base_va = base + text[0]
FAMILY = {'eax': {X.X86_REG_EAX, X.X86_REG_AX, X.X86_REG_AL, X.X86_REG_AH}, 'ecx': {X.X86_REG_ECX, X.X86_REG_CX, X.X86_REG_CL, X.X86_REG_CH},
          'edx': {X.X86_REG_EDX, X.X86_REG_DX, X.X86_REG_DL, X.X86_REG_DH}}


def callers(target):
    out = []
    i = 0
    while True:
        i = data.find(b'\xe8', i)
        if i < 0:
            break
        rel = struct.unpack_from('<i', data, i + 1)[0] if i + 5 <= len(data) else 0
        if base_va + i + 5 + rel == target:
            out.append(base_va + i)
        i += 1
    return out


def decode(a):
    p = off(a)
    return next(md.disasm(exe[p:p + 16], a), None)


def scan(start, limit=400):
    """DFS por ambas ramas; devuelve {reg: [(direccion, texto)]} de lecturas antes de escribir y las salidas alcanzadas."""
    reads = {k: [] for k in FAMILY}
    seen = set()
    stack = [(start, frozenset())]   # (direccion, registros ya escritos en este camino)
    steps = 0
    while stack and steps < limit * 50:
        addr, written = stack.pop()
        count = 0
        while count < limit:
            key = (addr, written)
            if key in seen:
                break
            seen.add(key)
            ins = decode(addr)
            steps += 1
            count += 1
            if ins is None:
                break
            regs_read, regs_write = ins.regs_access()
            for name, fam in FAMILY.items():
                if name in written:
                    continue
                if any(r in fam for r in regs_read) and ins.mnemonic not in ('push',) or (ins.mnemonic == 'push' and any(r in fam for r in regs_read)):
                    # un xor reg,reg o sub reg,reg no es una lectura real
                    if ins.mnemonic in ('xor', 'sub') and len(ins.operands) == 2 and ins.operands[0].type == X.X86_OP_REG and ins.operands[1].type == X.X86_OP_REG and ins.operands[0].reg == ins.operands[1].reg:
                        pass
                    else:
                        reads[name].append((hex(addr), '%s %s' % (ins.mnemonic, ins.op_str)))
            newly = set(written)
            for name, fam in FAMILY.items():
                # solo cuenta como escritura completa si escribe el registro de 32 bits (mov eax / lea / xor eax,eax / pop / etc.)
                for r in regs_write:
                    if r in {X.X86_REG_EAX, X.X86_REG_ECX, X.X86_REG_EDX} and r in fam:
                        newly.add(name)
            written = frozenset(newly)
            if len(written) == 3:
                break
            if ins.mnemonic == 'ret':
                break
            if ins.mnemonic == 'call':
                # una llamada pisa EAX/ECX/EDX (convencion cdecl/thiscall/stdcall del EXE salvo funciones especiales): se corta el camino como "escrito"
                break
            if ins.group(X.X86_GRP_JUMP):
                tgt = ins.operands[0].imm if ins.operands and ins.operands[0].type == X.X86_OP_IMM else None
                if ins.mnemonic == 'jmp':
                    if tgt is None:
                        break
                    addr = tgt
                    continue
                if tgt is not None:
                    stack.append((tgt, written))
            addr += ins.size
    return reads


for arg in sys.argv[1:]:
    t = int(arg, 16)
    print('== destino %x' % t)
    for c in callers(t):
        r = scan(c + 5)
        found = {k: v for k, v in r.items() if v}
        print('  %x: %s' % (c, 'ninguna lectura de EAX/ECX/EDX' if not found else found))
