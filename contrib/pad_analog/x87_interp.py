# -*- coding: utf-8 -*-
"""Interprete minimo de x87 + enteros para la rutina de lectura de sticks/botones de GInputSA.asi 1.11 (0x10007550..0x10007AC1).
Analisis estatico: NO ejecuta el .asi; lee los bytes con pefile, los desensambla con capstone y evalua cada instruccion en Python sobre una memoria simulada.
Los bytes del .asi solo se leen; ninguna instruccion se ejecuta en la CPU."""
import struct, math
import pefile, capstone
from capstone.x86 import X86_OP_IMM, X86_OP_REG, X86_OP_MEM

PATH = r"D:\Original\_ginput_ref\src\Original(Reworked)(Incl.All TXD's)\GInputSA.asi"
pe = pefile.PE(PATH)
BASE = pe.OPTIONAL_HEADER.ImageBase
data = open(PATH, 'rb').read()


def rd(va, n):
    rva = va - BASE
    for s in pe.sections:
        if s.VirtualAddress <= rva < s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData):
            off = s.PointerToRawData + rva - s.VirtualAddress
            return data[off:off + n]
    return b'\0' * n


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
OBJ = 0x50000000   # esi: objeto del pad (floats de SCP en +0x1c..+0x6c)
CPAD = 0x60000000  # ebx: CPad (se escribe PCTempJoyState en +0xa8)
M = 0xffffffff
R16 = {'ax': ('eax', 0xffff, 0), 'bx': ('ebx', 0xffff, 0), 'cx': ('ecx', 0xffff, 0), 'dx': ('edx', 0xffff, 0), 'si': ('esi', 0xffff, 0),
       'di': ('edi', 0xffff, 0), 'bp': ('ebp', 0xffff, 0), 'al': ('eax', 0xff, 0), 'bl': ('ebx', 0xff, 0), 'cl': ('ecx', 0xff, 0),
       'dl': ('edx', 0xff, 0), 'ah': ('eax', 0xff, 8)}


class Stop(Exception):
    pass


class Interp:
    def __init__(s, mem_over):
        s.mem = dict(mem_over)
        s.regs = {k: 0 for k in 'eax ebx ecx edx esi edi ebp esp'.split()}
        s.regs['esp'] = 0x70001000
        s.fp = []
        s.sw = 0
        s.zf = s.sf = s.cf = s.of = s.pf = 0
        s.writes = {}

    def rdm(s, a, n):
        a &= M
        b = bytearray()
        for i in range(n):
            x = a + i
            for k, v in s.mem.items():
                if k <= x < k + len(v):
                    b.append(v[x - k])
                    break
            else:
                b.append(rd(x, 1)[0] if 0x10000000 <= x < 0x10040000 else 0)
        return bytes(b)

    def wrm(s, a, b):
        a &= M
        s.mem[a] = bytes(b)
        if CPAD <= a < CPAD + 0x200:
            s.writes[a - CPAD] = bytes(b)

    def nm(s, r):
        return md.reg_name(r)

    def ea(s, op):
        m = op.mem
        v = m.disp
        if m.base:
            v += s.regs[s.nm(m.base)]
        if m.index:
            v += s.regs[s.nm(m.index)] * m.scale
        return v & M

    def getreg(s, name):
        if name in s.regs:
            return s.regs[name]
        r, mask, sh = R16[name]
        return (s.regs[r] >> sh) & mask

    def setreg(s, name, v):
        if name in s.regs:
            s.regs[name] = v & M
            return
        r, mask, sh = R16[name]
        s.regs[r] = (s.regs[r] & ~(mask << sh) & M) | ((v & mask) << sh)

    def get(s, op):
        if op.type == X86_OP_IMM:
            return op.imm & M
        if op.type == X86_OP_REG:
            return s.getreg(s.nm(op.reg))
        return int.from_bytes(s.rdm(s.ea(op), op.size), 'little')

    def put(s, op, v):
        if op.type == X86_OP_REG:
            s.setreg(s.nm(op.reg), v)
        else:
            n = op.size
            s.wrm(s.ea(op), (v & ((1 << (8 * n)) - 1)).to_bytes(n, 'little'))

    def flags(s, res, size):
        bits = 8 * size
        r = res & ((1 << bits) - 1)
        s.zf = int(r == 0)
        s.sf = (r >> (bits - 1)) & 1
        s.pf = int(bin(r & 0xff).count('1') % 2 == 0)

    def st(s, i):
        return s.fp[-1 - i]

    def setst(s, i, v):
        s.fp[-1 - i] = v

    def fcmp(s, a, b):
        if math.isnan(a) or math.isnan(b):
            s.sw = 0x4500
        elif a > b:
            s.sw = 0
        elif a < b:
            s.sw = 0x0100
        else:
            s.sw = 0x4000

    def fmem(s, op):
        if op.size == 4:
            return struct.unpack('<f', s.rdm(s.ea(op), 4))[0]
        if op.size == 8:
            return struct.unpack('<d', s.rdm(s.ea(op), 8))[0]
        raise Stop('fmem')

    def cc(s, mn):
        z, sf, cf, pf, of = s.zf, s.sf, s.cf, s.pf, s.of
        return {'je': z, 'jne': not z, 'jp': pf, 'jnp': not pf, 'jle': z or sf != of, 'jg': (not z) and sf == of, 'jl': sf != of,
                'jge': sf == of, 'jns': not sf, 'js': sf, 'ja': (not cf) and (not z), 'jbe': cf or z, 'jb': cf, 'jae': not cf}[mn]

    def run(s, pc, stop, trace=False, maxn=20000):
        n = 0
        while pc != stop:
            n += 1
            if n > maxn:
                raise Stop('bucle')
            ins = next(md.disasm(rd(pc, 16), pc, 1))
            mn, ops, nxt = ins.mnemonic, ins.operands, pc + ins.size
            if trace:
                print('%08x %s %s | st=%s' % (pc, mn, ins.op_str, ['%.6g' % x for x in reversed(s.fp)]))
            jmp = None
            if mn in ('sub', 'add', 'cmp', 'xor', 'and', 'or'):
                sz = ops[0].size
                mask = (1 << (8 * sz)) - 1
                a = s.get(ops[0])
                b = s.get(ops[1])
                if ops[1].type == X86_OP_IMM:
                    b = ops[1].imm & mask
                if mn == 'add':
                    r = a + b
                    s.cf = int(r > mask)
                elif mn in ('sub', 'cmp'):
                    r = a - b
                    s.cf = int((a & mask) < (b & mask))
                elif mn == 'xor':
                    r = a ^ b
                    s.cf = 0
                elif mn == 'and':
                    r = a & b
                    s.cf = 0
                else:
                    r = a | b
                    s.cf = 0
                top = 8 * sz - 1
                sa, sb, sr = (a >> top) & 1, (b >> top) & 1, (r >> top) & 1
                if mn in ('sub', 'cmp'):
                    s.of = int(sa != sb and sr != sa)
                elif mn == 'add':
                    s.of = int(sa == sb and sr != sa)
                else:
                    s.of = 0
                s.flags(r, sz)
                if mn != 'cmp':
                    s.put(ops[0], r)
            elif mn == 'test':
                s.flags(s.get(ops[0]) & s.get(ops[1]), ops[0].size)
                s.cf = s.of = 0
            elif mn in ('mov', 'movzx'):
                s.put(ops[0], s.get(ops[1]))
            elif mn == 'movsx':
                v = s.get(ops[1])
                b = 8 * ops[1].size
                if v >> (b - 1):
                    v -= 1 << b
                s.put(ops[0], v & M)
            elif mn == 'lea':
                s.put(ops[0], s.ea(ops[1]))
            elif mn == 'imul':
                if len(ops) == 3:
                    a, b = s.get(ops[1]), s.get(ops[2])
                else:
                    a, b = s.get(ops[0]), s.get(ops[1])
                bits = 8 * ops[0].size

                def sg(v, bits=bits):
                    v &= (1 << bits) - 1
                    return v - (1 << bits) if v >> (bits - 1) else v
                s.put(ops[0], (sg(a) * sg(b)) & ((1 << bits) - 1))
            elif mn == 'neg':
                a = s.get(ops[0])
                sz = ops[0].size
                s.cf = int(a != 0)
                s.put(ops[0], (-a) & ((1 << (8 * sz)) - 1))
                s.flags(-a, sz)
            elif mn == 'sbb':
                sz = ops[0].size
                mask = (1 << (8 * sz)) - 1
                a, b = s.get(ops[0]), s.get(ops[1])
                r = (a - b - s.cf) & mask
                s.cf = int((a & mask) < ((b & mask) + s.cf))
                s.put(ops[0], r)
                s.flags(r, sz)
            elif mn == 'cdq':
                s.regs['edx'] = M if s.regs['eax'] >> 31 else 0
            elif mn == 'push':
                s.regs['esp'] -= 4
            elif mn == 'pop':
                s.regs['esp'] += 4
            elif mn.startswith('j'):
                if mn == 'jmp' or s.cc(mn):
                    jmp = ops[0].imm
            elif mn == 'call':
                tgt = ops[0].imm if ops[0].type == X86_OP_IMM else None
                if tgt == 0x1002af10:  # _ftol2: trunca hacia cero
                    v = s.fp.pop()
                    s.regs['eax'] = (int(math.trunc(v)) & M) if abs(v) < 2 ** 31 else 0x80000000
                elif tgt == 0x10017846:  # CRT cbrt(double) estatico del .asi (argumento en [esp], cdecl)
                    x = struct.unpack('<d', s.rdm(s.regs['esp'], 8))[0]
                    s.fp.append(math.copysign(abs(x) ** (1.0 / 3.0), x) if x else 0.0)
                    s.cbrt_calls = getattr(s, 'cbrt_calls', 0) + 1
                else:
                    raise Stop('call a %s' % hex(tgt or 0))
            elif mn == 'ret':
                raise Stop('ret')
            elif mn == 'fld':
                if ops[0].type == X86_OP_REG:
                    s.fp.append(s.st(int(ins.op_str[3:-1])))
                else:
                    s.fp.append(s.fmem(ops[0]))
            elif mn == 'fld1':
                s.fp.append(1.0)
            elif mn == 'fldz':
                s.fp.append(0.0)
            elif mn == 'fild':
                v = s.get(ops[0])
                b = 8 * ops[0].size
                if v >> (b - 1):
                    v -= 1 << b
                s.fp.append(float(v))
            elif mn == 'fabs':
                s.setst(0, abs(s.st(0)))
            elif mn == 'fchs':
                s.setst(0, -s.st(0))
            elif mn == 'fxch':
                i = int(ins.op_str[3:-1]) if ins.op_str else 1
                a, b = s.st(0), s.st(i)
                s.setst(0, b)
                s.setst(i, a)
            elif mn in ('fcom', 'fcomp', 'fcompp'):
                if mn == 'fcompp':
                    b = s.st(1)
                elif not ops or ops[0].type == X86_OP_REG:
                    b = s.st(int(ins.op_str[3:-1])) if ins.op_str else s.st(1)
                else:
                    b = s.fmem(ops[0])
                s.fcmp(s.st(0), b)
                if mn in ('fcomp', 'fcompp'):
                    s.fp.pop()
                if mn == 'fcompp':
                    s.fp.pop()
            elif mn == 'fnstsw':
                s.regs['eax'] = (s.regs['eax'] & 0xffff0000) | s.sw
            elif mn == 'fimul':
                v = s.get(ops[0])
                b = 8 * ops[0].size
                if v >> (b - 1):
                    v -= 1 << b
                s.setst(0, s.st(0) * v)
            elif mn in ('fadd', 'fsub', 'fsubr', 'fmul', 'fdiv', 'fdivr', 'faddp', 'fsubp', 'fsubrp', 'fmulp', 'fdivp', 'fdivrp'):
                pop = mn.endswith('p')
                base = mn[:-1] if pop else mn
                if ops and ops[0].type == X86_OP_MEM:
                    di = 0
                    a, b = s.st(0), s.fmem(ops[0])
                else:
                    t = ins.op_str.replace(' ', '').split(',') if ins.op_str else []
                    if not t:
                        di, a, b = 1, s.st(1), s.st(0)
                    else:
                        if len(t) == 1:  # un solo operando st(i): las formas 'p' escriben en st(i); las otras en st(0)
                            i = int(t[0][3:-1])
                            di, si = (i, 0) if pop else (0, i)
                        else:
                            di = int(t[0][3:-1]) if t[0].startswith('st(') else 0
                            si = int(t[1][3:-1]) if t[1].startswith('st(') else 0
                        if di == 0:
                            a, b = s.st(0), s.st(si)
                        else:
                            a, b = s.st(di), s.st(0)
                def dv(x, y):
                    return x / y if y else (math.copysign(math.inf, x) if x else math.nan)
                # fsub: dst = dst - src ; fsubr: dst = src - dst ; (a = dst, b = src)
                r = {'fadd': a + b, 'fsub': a - b, 'fsubr': b - a, 'fmul': a * b, 'fdiv': dv(a, b), 'fdivr': dv(b, a)}[base]
                s.setst(di, r)
                if pop:
                    s.fp.pop()
            elif mn in ('fstp', 'fst'):
                if ops[0].type == X86_OP_REG:
                    s.setst(int(ins.op_str[3:-1]), s.st(0))
                else:
                    v = s.st(0)
                    s.wrm(s.ea(ops[0]), struct.pack('<f' if ops[0].size == 4 else '<d', v))
                if mn == 'fstp':
                    s.fp.pop()
            elif mn.startswith('int') or mn == 'nop':
                pass
            else:
                raise Stop('instr no soportada %x %s %s' % (pc, mn, ins.op_str))
            pc = jmp if jmp is not None else nxt
        return pc
