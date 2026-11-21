# Transcriptor literal de una funcion x86 (cdecl, con saltos) del EXE a C++ con etiquetas y goto.
# Registros -> variables uint32_t; pila -> matriz de bytes local `fr` (offsets estaticos respecto al ESP de entrada); banderas por instruccion.
# Uso: python lift.py <direccion> <bytes> <nombre> <nargs> [0xDIR=nombre/nargs ...]
import sys, re
sys.path.insert(0, '.')
from desens import rd, md

addr = int(sys.argv[1], 16)
size = int(sys.argv[2], 0)
name = sys.argv[3]
nargs = int(sys.argv[4])
callmap = {}
CALLREG = '4'
for a in sys.argv[5:]:
    k, v = a.split('=')
    n, c = v.split('/')
    callmap[int(k, 16)] = (n, int(c))
ins = []
for i in md.disasm(rd(addr, size), addr):
    ins.append((i.address, i.mnemonic, i.op_str, i.size))
by = {a: k for k, (a, _, _, _) in enumerate(ins)}
R32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp']
R16 = {'ax': 'eax', 'bx': 'ebx', 'cx': 'ecx', 'dx': 'edx', 'si': 'esi', 'di': 'edi', 'bp': 'ebp'}


R8 = {'al': 'eax', 'dl': 'edx', 'cl': 'ecx', 'bl': 'ebx'}


def isreg(s):
    return s in R32 or s in R16 or s in R8


def width(s):
    if s in R32: return 4
    if s in R16: return 2
    if s in R8: return 1
    return None


def tname(w):
    return {4: 'uint32_t', 2: 'uint16_t', 1: 'uint8_t'}[w]


def esp_delta(m, o):
    if m == 'push': return -4
    if m == 'pop': return 4
    if m == 'sub' and o.startswith('esp,'): return -int(o.split(',')[1], 0)
    if m == 'add' and o.startswith('esp,'): return int(o.split(',')[1], 0)
    return 0


off = {addr: 0}
stack = [addr]
while stack:
    a = stack.pop()
    k = by[a]
    cur = off[a]
    while k < len(ins):
        ad, m, o, sz = ins[k]
        if ad in off and ad != a:
            if off[ad] != cur:
                raise SystemExit(f'esp inconsistente en {ad:x}: {off[ad]} vs {cur}')
            break
        off[ad] = cur
        cur += esp_delta(m, o)
        if m == 'ret': break
        if m == 'jmp' or m.startswith('j'):
            t = int(o, 16)
            if t in by:
                if t in off:
                    if off[t] != cur:
                        raise SystemExit(f'esp inconsistente salto {ad:x}->{t:x}: {off[t]} vs {cur}')
                else:
                    off[t] = cur
                    stack.append(t)
            if m == 'jmp': break
        k += 1

BIAS = 0x100


def slot(o):
    return BIAS + o


def fr(o, w):
    return f'*({tname(w)}*)(fr+{slot(o)})'


def parse_mem(op):
    m = re.match(r'(?:(byte|word|dword) ptr )?\[(.*)\]$', op)
    sz = {'byte': 1, 'word': 2, 'dword': 4, None: None}[m.group(1)]
    e = m.group(2).replace(' ', '')
    terms = re.findall(r'([+-]?)([^+-]+)', e)
    base = None; idx = None; scale = 1; disp = 0
    for sg, t in terms:
        if '*' in t:
            r, s = t.split('*'); idx = r; scale = int(s)
        elif t in R32 or t == 'esp':
            if base is None: base = t
            else: idx = t
        else:
            v = int(t, 0); disp += -v if sg == '-' else v
    return sz, base, idx, scale, disp


def addr_terms(base, idx, scale, disp):
    a = []
    if base: a.append(base)
    if idx: a.append(idx if scale == 1 else f'{idx}*{scale}')
    if disp or not a: a.append(f'0x{disp & 0xffffffff:x}')
    return '+'.join(a)


def memexpr(op, eo, w=None):
    sz, base, idx, scale, disp = parse_mem(op)
    ww = sz or w
    if base == 'esp':
        assert idx is None
        return fr(eo + disp, ww), ww
    return f'*({tname(ww)}*)(uintptr_t)({addr_terms(base, idx, scale, disp)})', ww


def lea_expr(op, eo):
    sz, base, idx, scale, disp = parse_mem(op)
    if base == 'esp': return f'(uint32_t)(uintptr_t)(fr+{slot(eo + disp)})'
    return '(' + addr_terms(base, idx, scale, disp) + ')'


def rd_op(op, eo, w=None):
    op = op.strip()
    if isreg(op):
        ww = width(op)
        if ww == 4: return op, 4
        if ww == 2: return f'(uint16_t){R16[op]}', 2
        return f'(uint8_t){R8[op]}', 1
    if '[' in op:
        return memexpr(op, eo, w)
    v = int(op, 0)
    return f'0x{v & 0xffffffff:x}u', w


def wr_op(op, eo, val, w=None):
    op = op.strip()
    if isreg(op):
        ww = width(op)
        if ww == 4: return f'{op} = {val};'
        if ww == 2:
            r = R16[op]; return f'{r} = ({r} & 0xffff0000u) | (uint16_t)({val});'
        r = R8[op]; return f'{r} = ({r} & 0xffffff00u) | (uint8_t)({val});'
    e, ww = memexpr(op, eo, w)
    return f'{e} = {val};'


def opw(*ops):
    for o in ops:
        o = o.strip()
        if isreg(o): return width(o)
        mm = re.match(r'(byte|word|dword) ptr', o)
        if mm: return {'byte': 1, 'word': 2, 'dword': 4}[mm.group(1)]
    return 4


CC = {'je': 'zf', 'jne': '!zf', 'jb': 'cf', 'jae': '!cf', 'jbe': '(cf||zf)', 'ja': '(!cf&&!zf)', 'jl': '(sf!=of)', 'jle': '(zf||sf!=of)', 'jg': '(!zf&&sf==of)', 'jge': '(sf==of)'}
labels = {int(o, 16) for a, m, o, s in ins if (m == 'jmp' or m in CC) and int(o, 16) in by}
out = []
E = out.append
for a, m, o, sz in ins:
    if a not in off: continue
    eo = off[a]
    if a in labels: E(f'L_{a:x}:;')
    if m == 'nop': continue
    ops = [x.strip() for x in re.split(r',(?![^\[]*\])', o)] if o else []
    c = f'// {a:x} {m} {o}'
    if m == 'mov':
        w = opw(*ops); v, _ = rd_op(ops[1], eo, w); E(f'{wr_op(ops[0], eo, v, w)} {c}')
    elif m == 'lea':
        E(f'{wr_op(ops[0], eo, lea_expr(ops[1], eo))} {c}')
    elif m in ('add', 'sub', 'xor', 'and', 'or'):
        if ops[0] == 'esp': continue
        w = opw(*ops); T = tname(w)
        a_, _ = rd_op(ops[0], eo, w); b_, _ = rd_op(ops[1], eo, w)
        sym = {'add': '+', 'sub': '-', 'xor': '^', 'and': '&', 'or': '|'}[m]
        s = f'{{ {T} a_={a_}, b_={b_}; {T} r_=({T})(a_ {sym} b_); '
        if m == 'add': s += f'cf=r_<a_; of=(((a_^r_)&(b_^r_))>>{w*8-1})&1; '
        elif m == 'sub': s += f'cf=a_<b_; of=(((a_^b_)&(a_^r_))>>{w*8-1})&1; '
        else: s += 'cf=0; of=0; '
        s += f'zf=r_==0; sf=(r_>>{w*8-1})&1; {wr_op(ops[0], eo, "r_", w)} }} {c}'
        E(s)
    elif m in ('cmp', 'test'):
        w = opw(*ops); T = tname(w)
        a_, _ = rd_op(ops[0], eo, w); b_, _ = rd_op(ops[1], eo, w)
        if m == 'cmp':
            E(f'{{ {T} a_={a_}, b_={b_}; {T} r_=({T})(a_-b_); cf=a_<b_; of=(((a_^b_)&(a_^r_))>>{w*8-1})&1; zf=r_==0; sf=(r_>>{w*8-1})&1; }} {c}')
        else:
            E(f'{{ {T} r_=({T})({a_} & {b_}); cf=0; of=0; zf=r_==0; sf=(r_>>{w*8-1})&1; }} {c}')
    elif m in ('inc', 'dec'):
        w = opw(*ops); T = tname(w); a_, _ = rd_op(ops[0], eo, w); d = '+' if m == 'inc' else '-'
        ofc = f'a_==({T})0x{(1 << (w*8-1)) - 1:x}' if m == 'inc' else f'a_==({T})0x{1 << (w*8-1):x}'
        E(f'{{ {T} a_={a_}; {T} r_=({T})(a_{d}1); of={ofc}; zf=r_==0; sf=(r_>>{w*8-1})&1; {wr_op(ops[0], eo, "r_", w)} }} {c}')
    elif m == 'not':
        w = opw(*ops); T = tname(w); a_, _ = rd_op(ops[0], eo, w); E(f'{wr_op(ops[0], eo, f"({T})~{a_}", w)} {c}')
    elif m in ('shl', 'shr', 'sar'):
        w = opw(*ops); T = tname(w); a_, _ = rd_op(ops[0], eo, w); n = int(ops[1], 0)
        if m == 'shl': e = f'({T})({a_} << {n})'
        elif m == 'shr': e = f'({T})({a_} >> {n})'
        else: e = f'({T})((int32_t)({a_}) >> {n})'
        E(f'{wr_op(ops[0], eo, e, w)} {c}')
    elif m == 'push':
        v, _ = rd_op(ops[0], eo, 4); E(f'{fr(eo-4, 4)} = {v}; {c}')
    elif m == 'pop':
        E(f'{wr_op(ops[0], eo, fr(eo, 4), 4)} {c}')
    elif m == 'sete':
        E(f'{wr_op(ops[0], eo, "zf?1:0", 1)} {c}')
    elif m == 'div':
        d, _ = rd_op(ops[0], eo, 4)
        E(f'{{ uint64_t n_=((uint64_t)edx<<32)|eax; uint32_t d_={d}; eax=(uint32_t)(n_/d_); edx=(uint32_t)(n_%d_); }} {c}')
    elif m == 'call':
        if ops[0] in R32:
            k = int(CALLREG, 0)
            args = ', '.join(fr(eo + 4*i, 4) for i in range(k))
            E(f'eax = ((uint32_t(__cdecl*)({",".join(["uint32_t"]*k)}))({ops[0]}))({args}); {c}')
        elif ops[0].startswith('0x'):
            t = int(ops[0], 16)
            if t not in callmap: raise SystemExit(f'llamada desconocida {t:x}')
            n, k = callmap[t]
            args = ', '.join(fr(eo + 4*i, 4) for i in range(k))
            E(f'eax = (uint32_t)(uintptr_t){n}({args}); {c}')
        else:
            sz_, base, idx, scale, disp = parse_mem(ops[0])
            k = {0x134: 2, 0x138: 1, 0x13c: 3}.get(disp, 2)
            args = ', '.join(fr(eo + 4*i, 4) for i in range(k))
            E(f'eax = ((uint32_t(__cdecl*)({",".join(["uint32_t"]*k)}))(*(uint32_t*)(uintptr_t)({base}+0x{disp:x})))({args}); {c}')
    elif m == 'ret':
        E(f'return RET_VALUE; {c}')
    elif m.startswith('rep movs'):
        if m == 'rep movsd': E(f'while (ecx) {{ *(uint32_t*)(uintptr_t)edi = *(uint32_t*)(uintptr_t)esi; esi += 4; edi += 4; --ecx; }} {c} {o}')
        elif m == 'rep movsb': E(f'while (ecx) {{ *(uint8_t*)(uintptr_t)edi = *(uint8_t*)(uintptr_t)esi; ++esi; ++edi; --ecx; }} {c} {o}')
        else: raise SystemExit('rep no soportado')
    elif m == 'jmp':
        E(f'goto L_{int(o, 16):x}; {c}')
    elif m in CC:
        E(f'if ({CC[m]}) goto L_{int(o, 16):x}; {c}')
    else:
        raise SystemExit(f'instruccion no soportada {m} {o}')
print('\n'.join(out))
