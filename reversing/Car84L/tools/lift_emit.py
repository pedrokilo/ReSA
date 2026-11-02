"""Emision de C++ del elevador (ver lift.py). Uso: python lift_emit.py out.inl"""
import sys
from lift import *
RDATA = (0x858000, 0x8A4000)

FRAME = 0x300
BASE = 0x2F0
MASK = {1: 0xFF, 2: 0xFFFF, 4: 0xFFFFFFFF}
CT = {1: 'uint8', 2: 'uint16', 4: 'uint32'}
COND = {'je': 'zf', 'jne': '!zf', 'jge': 'sf==of', 'jl': 'sf!=of', 'jle': 'zf||sf!=of', 'jg': '!zf&&sf==of', 'ja': '!cf&&!zf',
        'jb': 'cf', 'jae': '!cf', 'jbe': 'cf||zf', 'jns': '!sf', 'jp': 'pf', 'jnp': '!pf'}


def reg_size(r):
    return 4 if r in REG32 else 2 if r in REG16 else 1


def reg_get(r):
    if r in REG32:
        return r
    if r in REG16:
        return '(%s&0xFFFFu)' % REG16[r]
    if r in REG8L:
        return '(%s&0xFFu)' % REG8L[r]
    return '((%s>>8)&0xFFu)' % REG8H[r]


def reg_set(r, v):
    if r in REG32:
        return '%s=(uint32)(%s);' % (r, v)
    if r in REG16:
        f = REG16[r]
        return '%s=(%s&0xFFFF0000u)|((uint32)(%s)&0xFFFFu);' % (f, f, v)
    if r in REG8L:
        f = REG8L[r]
        return '%s=(%s&0xFFFFFF00u)|((uint32)(%s)&0xFFu);' % (f, f, v)
    f = REG8H[r]
    return '%s=(%s&0xFFFF00FFu)|(((uint32)(%s)&0xFFu)<<8);' % (f, f, v)


def sgn(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


class Emitter:
    def __init__(self, ins, delta, xd):
        self.ins = ins
        self.delta = delta
        self.xd = xd
        self.out = []
        self.labels = set()
        for k, i in enumerate(ins):
            if (i.mn == 'jmp' or i.mn.startswith('j')) and i.target is not None:
                self.labels.add(i.target)
            if i.mn == 'jmp' and i.indirect is not None:
                for t in table_targets(i.indirect.disp):
                    self.labels.add(t)

    def addr(self, op, d):
        if op.base == 'esp' and op.index is None:
            return '(F+%d)' % (BASE + d + sgn(op.disp))
        parts = []
        if op.base:
            parts.append(op.base)
        if op.index:
            parts.append('%s*%d' % (op.index, op.scale) if op.scale != 1 else op.index)
        if op.disp or not parts:
            parts.append('0x%Xu' % op.disp)
        return '(uintptr_t)(uint32)(%s)' % '+'.join(parts)

    def is_const(self, op):
        return op.base is None and op.index is None and RDATA[0] <= op.disp < RDATA[1]

    def const_bits(self, op, size):
        return int.from_bytes(read_va(op.disp, size), 'little')

    def rd(self, op, size, d):
        if op.kind == 'reg':
            if op.reg == 'esp':
                return '(uint32)(uintptr_t)(F+%d)' % (BASE + d)
            return reg_get(op.reg)
        if op.kind == 'imm':
            return '0x%Xu' % (op.val & MASK[size])
        if op.kind == 'mem':
            if self.is_const(op) and op.size in (1, 2, 4):
                return '0x%Xu' % self.const_bits(op, op.size)
            return 'Ld<%s>(%s)' % (CT[op.size], self.addr(op, d))
        raise ValueError(op.text)

    def wr(self, op, v, d):
        if op.kind == 'reg':
            return reg_set(op.reg, v)
        if op.kind == 'mem':
            return 'St<%s>(%s,(%s)(%s));' % (CT[op.size], self.addr(op, d), CT[op.size], v)
        raise ValueError(op.text)

    def osize(self, op):
        return reg_size(op.reg) if op.kind == 'reg' else op.size

    def fload(self, op, d, kind):
        if op.kind == 'st':
            return 'x%d' % (self.xdepth - 1 - op.idx)
        if kind == 'f':
            if self.is_const(op):
                return '(double)F32(0x%Xu)' % self.const_bits(op, 4)
            return '(double)Ld<float>(%s)' % self.addr(op, d)
        if kind == 'i':
            return '(double)Ld<int32>(%s)' % self.addr(op, d)
        return 'Ld<double>(%s)' % self.addr(op, d)

    def emit(self, s):
        self.out.append('    ' + s)

    def run(self):
        for k, i in enumerate(self.ins):
            d = self.delta[k]
            self.xdepth = self.xd[k]
            if i.addr in self.labels:
                self.out.append('L_%08X:;' % i.addr)
            self.emit('// %08X: %s' % (i.addr, i.text))
            self.one(k, i, d)
        return self.out

    def one(self, k, i, d):
        o = self.emit
        mn = i.mn
        ops = i.ops
        if mn == 'push':
            o('St<uint32>(F+%d,%s);' % (BASE + d - 4, self.rd(ops[0], 4, d)))
        elif mn == 'pop':
            o(reg_set(ops[0].reg, 'Ld<uint32>(F+%d)' % (BASE + d)))
        elif mn in ('sub', 'add') and ops[0].kind == 'reg' and ops[0].reg == 'esp':
            pass
        elif mn == 'mov':
            sz = self.osize(ops[0])
            o(self.wr(ops[0], self.rd(ops[1], sz, d), d))
        elif mn in ('movzx', 'movsx'):
            ssz = self.osize(ops[1])
            v = self.rd(ops[1], ssz, d)
            if mn == 'movsx' and ssz != 4:
                v = '(uint32)(int32)(int%d)(%s)' % (ssz * 8, v)
            o(self.wr(ops[0], v, d))
        elif mn == 'lea':
            o(reg_set(ops[0].reg, '(uint32)%s' % self.addr(ops[1], d)))
        elif mn in ('add', 'sub', 'and', 'or', 'xor', 'cmp', 'test'):
            dst, src = ops
            sz = self.osize(dst)
            bits = sz * 8
            m = MASK[sz]
            a = self.rd(dst, sz, d)
            b = self.rd(src, sz, d)
            if mn == 'test':
                o('{const uint32 r_=((%s)&(%s))&%s;zf=r_==0;sf=(r_>>%d)&1;cf=0;of=0;pf=Parity(r_);}' % (a, b, m, bits - 1))
            elif mn in ('cmp', 'sub'):
                wr = self.wr(dst, 'r_', d) if mn == 'sub' else ''
                o('{const uint32 a_=(%s)&%s,b_=(%s)&%s,r_=(a_-b_)&%s;zf=r_==0;sf=(r_>>%d)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>%d)&1;pf=Parity(r_);%s}' % (a, m, b, m, m, bits - 1, bits - 1, wr))
            elif mn == 'add':
                o('{const uint32 a_=(%s)&%s,b_=(%s)&%s,r_=(a_+b_)&%s;zf=r_==0;sf=(r_>>%d)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>%d)&1;pf=Parity(r_);%s}' % (a, m, b, m, m, bits - 1, bits - 1, self.wr(dst, 'r_', d)))
            else:
                op = {'and': '&', 'or': '|', 'xor': '^'}[mn]
                o('{const uint32 r_=((%s)%s(%s))&%s;zf=r_==0;sf=(r_>>%d)&1;cf=0;of=0;pf=Parity(r_);%s}' % (a, op, b, m, bits - 1, self.wr(dst, 'r_', d)))
        elif mn in ('inc', 'dec'):
            dst = ops[0]
            sz = self.osize(dst)
            bits = sz * 8
            m = MASK[sz]
            a = self.rd(dst, sz, d)
            if mn == 'inc':
                o('{const uint32 a_=(%s)&%s,r_=(a_+1)&%s;zf=r_==0;sf=(r_>>%d)&1;of=r_==%du;pf=Parity(r_);%s}' % (a, m, m, bits - 1, 1 << (bits - 1), self.wr(dst, 'r_', d)))
            else:
                o('{const uint32 a_=(%s)&%s,r_=(a_-1)&%s;zf=r_==0;sf=(r_>>%d)&1;of=a_==%du;pf=Parity(r_);%s}' % (a, m, m, bits - 1, 1 << (bits - 1), self.wr(dst, 'r_', d)))
        elif mn in ('shl', 'shr', 'sar'):
            dst, cnt = ops
            sz = self.osize(dst)
            bits = sz * 8
            m = MASK[sz]
            a = self.rd(dst, sz, d)
            c = cnt.val & 31
            if mn == 'shl':
                res = '((a_<<%d)&%s)' % (c, m)
                cfe = '(a_>>%d)&1' % (bits - c)
            elif mn == 'shr':
                res = '(a_>>%d)' % c
                cfe = '(a_>>%d)&1' % (c - 1)
            else:
                if sz != 4:
                    raise ValueError('sar sin soporte de tamano ' + i.text)
                res = '(uint32)((int32)a_>>%d)' % c
                cfe = '(a_>>%d)&1' % (c - 1)
            o('{const uint32 a_=(%s)&%s,r_=%s;zf=r_==0;sf=(r_>>%d)&1;cf=%s;pf=Parity(r_);%s}' % (a, m, res, bits - 1, cfe, self.wr(dst, 'r_', d)))
        elif mn == 'imul':
            if len(ops) == 3:
                o(self.wr(ops[0], '(uint32)((int32)%s*(int32)%s)' % (self.rd(ops[1], 4, d), self.rd(ops[2], 4, d)), d))
            elif len(ops) == 2:
                o(self.wr(ops[0], '(uint32)((int32)%s*(int32)%s)' % (self.rd(ops[0], 4, d), self.rd(ops[1], 4, d)), d))
            else:
                o('{const int64 p_=(int64)(int32)eax*(int64)(int32)%s;eax=(uint32)p_;edx=(uint32)((uint64)p_>>32);}' % self.rd(ops[0], 4, d))
        elif mn == 'cdq':
            o('edx=((int32)eax<0)?0xFFFFFFFFu:0u;')
        elif mn == 'idiv':
            o('{const int64 n_=((int64)(int32)edx<<32)|(int64)eax;const int32 d_=(int32)%s;eax=(uint32)(int32)(n_/d_);edx=(uint32)(int32)(n_%%d_);}' % self.rd(ops[1], 4, d))
        elif mn == 'jmp':
            if i.target is not None:
                o('goto L_%08X;' % i.target)
            else:
                o('switch(%s){' % i.indirect.index)
                for ti, t in enumerate(table_targets(i.indirect.disp)):
                    o('case %d: goto L_%08X;' % (ti, t))
                o('default: __assume(0);}')
        elif mn in COND:
            o('if(%s) goto L_%08X;' % (COND[mn], i.target))
        elif mn == 'ret':
            o('return;')
        elif mn == 'call':
            self.call(k, i, d)
        elif mn.startswith('f'):
            self.fpu(k, i, d)
        else:
            raise ValueError('mnemonico sin soporte: %s en %08X' % (mn, i.addr))

    def call(self, k, i, d):
        o = self.emit
        xin = self.xd[k]
        if i.target is None:
            op = i.indirect
            if i.addr in (0x431AAB, 0x431B33, 0x431F2E):
                o('{typedef void(__fastcall*Fn_)(uint32,uint32,uint32);((Fn_)(uintptr_t)Ld<uint32>((uintptr_t)(uint32)(%s+0x%Xu)))(ecx,edx,Ld<uint32>(F+%d));}' % (op.base, op.disp, BASE + d))
            else:
                o('{typedef float(__fastcall*Fn_)(uint32,uint32);x%d=(double)((Fn_)(uintptr_t)Ld<uint32>((uintptr_t)(uint32)(%s+0x%Xu)))(ecx,edx);}' % (xin, op.base, op.disp))
            return
        name, conv, nargs, pop, ret = CALLS[i.target]
        if i.target == 0x821B40:
            o('eax=(uint32)Heli83GFtol(x%d);' % (xin - 1))
            return
        args = ['Ld<uint32>(F+%d)' % (BASE + d + 4 * a) for a in range(nargs)]
        rt = {'v': 'void', 'i': 'uint32', 'f': 'float', 'b': 'uint8'}[ret]
        if conv == 'c':
            proto = '%s(__cdecl*)(%s)' % (rt, ','.join(['uint32'] * nargs) or 'void')
            call = '((%s)0x%X)(%s)' % (proto, i.target, ','.join(args))
        else:
            proto = '%s(__fastcall*)(%s)' % (rt, ','.join(['uint32'] * (nargs + 2)))
            call = '((%s)0x%X)(%s)' % (proto, i.target, ','.join(['ecx', 'edx'] + args))
        if ret == 'v':
            o('%s;' % call)
        elif ret == 'f':
            o('x%d=(double)%s;' % (xin, call))
        else:
            o('eax=%s;' % call)

    def fpu(self, k, i, d):
        o = self.emit
        mn = i.mn
        ops = i.ops
        xd = self.xdepth
        top = 'x%d' % (xd - 1)
        if mn == 'fld':
            if ops[0].kind == 'st':
                o('x%d=x%d;' % (xd, xd - 1 - ops[0].idx))
            else:
                o('x%d=%s;' % (xd, self.fload(ops[0], d, 'q' if ops[0].size == 8 else 'f')))
        elif mn == 'fild':
            o('x%d=%s;' % (xd, self.fload(ops[0], d, 'i')))
        elif mn in ('fstp', 'fst'):
            if ops[0].kind == 'st':
                if ops[0].idx != 0 or mn == 'fst':
                    o('x%d=%s;' % (xd - 1 - ops[0].idx, top))
            else:
                o('St<float>(%s,Veh83DStore(%s));' % (self.addr(ops[0], d), top))
        elif mn in ('fadd', 'fsub', 'fmul', 'fdiv', 'fsubr', 'fdivr', 'fiadd', 'fimul'):
            base = mn[2:] if mn[1] == 'i' else mn[1:]
            src = ops[0] if len(ops) == 1 else ops[1]
            kind = 'i' if mn[1] == 'i' else 'f'
            if len(ops) == 2 and ops[0].kind == 'st' and ops[0].idx != 0:
                raise ValueError('operando destino no soportado ' + i.text)
            b = self.fload(src, d, kind)
            fn = {'add': 'Veh83DAdd', 'sub': 'Veh83DSub', 'mul': 'Veh83DMul', 'div': 'Veh83DDiv', 'subr': 'Veh83DSub', 'divr': 'Veh83DDiv'}[base]
            if base in ('subr', 'divr'):
                o('%s=%s(%s,%s);' % (top, fn, b, top))
            else:
                o('%s=%s(%s,%s);' % (top, fn, top, b))
        elif mn in ('faddp', 'fsubp', 'fmulp', 'fdivp'):
            fn = {'faddp': 'Veh83DAdd', 'fsubp': 'Veh83DSub', 'fmulp': 'Veh83DMul', 'fdivp': 'Veh83DDiv'}[mn]
            idx = ops[0].idx if ops else 1
            dst = 'x%d' % (xd - 1 - idx)
            o('%s=%s(%s,%s);' % (dst, fn, dst, top))
        elif mn == 'fchs':
            o('%s=-%s;' % (top, top))
        elif mn == 'fsqrt':
            o('%s=Veh83DSqrt(%s);' % (top, top))
        elif mn == 'fxch':
            idx = ops[0].idx if ops else 1
            o('{const double t_=%s;%s=x%d;x%d=t_;}' % (top, top, xd - 1 - idx, xd - 1 - idx))
        elif mn in ('fcom', 'fcomp'):
            o('fcc=X87Compare(%s,%s);' % (top, self.fload(ops[0], d, 'f')))
        elif mn == 'fcompp':
            o('fcc=X87Compare(%s,x%d);' % (top, xd - 2))
        elif mn == 'fnstsw':
            o('eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);')
        else:
            raise ValueError('x87 sin soporte: ' + i.text)


def emit_cpp(path):
    ins = load()
    delta, xd, problems = analyze(ins)
    if problems:
        raise SystemExit('\n'.join(problems))
    body = Emitter(ins, delta, xd).run()
    with open(path, 'w') as f:
        f.write('\n'.join(body) + '\n')
    print('emitido', len(body), 'lineas')


if __name__ == '__main__':
    emit_cpp(sys.argv[1])
