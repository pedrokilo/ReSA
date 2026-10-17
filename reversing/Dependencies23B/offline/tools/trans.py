"""Traductor mecanico de un tramo de FlyingControl (rutas B y C) a C++ con la maquina FlcX87.
Uso: python trans.py <nombre> <inicio hex> <fin hex> <salida.inc>
- Registros como uint32, pila x86 emulada en un bufer (esp real), memoria por direccion, salto con goto, banderas por la ultima instruccion que las fija.
- Instrucciones x87 -> llamadas FlcX87 (decodificadas por bytes con x87gen).
- Llamadas a funciones del EXE -> colaboradores C++ (tabla CALLS)."""
import sys, re
import x87gen as g

name, a0, a1, outp = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), sys.argv[4]
COV = len(sys.argv) > 5 and sys.argv[5] == '--cov'
EXIT_TARGETS = {0x6da900}

code = g.EXE[g.off(a0):g.off(a0) + (a1 - a0) + 16]
ins_list = []
for i in g.md.disasm(code, a0):
    if i.address >= a1:
        break
    ins_list.append(i)

REG32 = {'eax', 'ecx', 'edx', 'ebx', 'esi', 'edi', 'ebp', 'esp'}


def reg_read(r):
    if r in REG32:
        return r
    if r in ('ax', 'bx', 'cx', 'dx'):
        return '(e%s & 0xFFFFu)' % r
    if r in ('al', 'bl', 'cl', 'dl'):
        return '(e%sx & 0xFFu)' % r[0]
    if r in ('ah', 'bh', 'ch', 'dh'):
        return '((e%sx >> 8) & 0xFFu)' % r[0]
    raise ValueError(r)


def reg_write(r, val):
    if r in REG32:
        return '%s = %s;' % (r, val)
    if r in ('ax', 'bx', 'cx', 'dx'):
        e = 'e' + r
        return '%s = (%s & 0xFFFF0000u) | ((%s) & 0xFFFFu);' % (e, e, val)
    if r in ('al', 'bl', 'cl', 'dl'):
        e = 'e%sx' % r[0]
        return '%s = (%s & 0xFFFFFF00u) | ((%s) & 0xFFu);' % (e, e, val)
    if r in ('ah', 'bh', 'ch', 'dh'):
        e = 'e%sx' % r[0]
        return '%s = (%s & 0xFFFF00FFu) | (((%s) & 0xFFu) << 8);' % (e, e, val)
    raise ValueError(r)


def reg_bits(r):
    if r in REG32:
        return 32
    if r in ('ax', 'bx', 'cx', 'dx'):
        return 16
    return 8


def addr_expr(inner):
    """inner = texto entre corchetes: 'esi + 0x14', 'edx*4 + 0x6daee4', '0xb7cb5c', 'esp'."""
    t = inner.replace(' ', '')
    t = t.replace('+', ' + ').replace('-', ' - ')
    t = re.sub(r'\b(e[a-d]x|e[sd]i|e[bs]p)\b', lambda m: m.group(1), t)
    return '(uint32)(%s)' % t


def mem_ref(opstr):
    m = re.match(r'(byte|word|dword|qword) ptr \[(.*)\]$', opstr.strip())
    if not m:
        return None
    sz, inner = m.group(1), m.group(2)
    return sz, addr_expr(inner)


def is_imm(s):
    return re.fullmatch(r'-?0x[0-9a-f]+|-?\d+', s.strip()) is not None


def imm(s):
    s = s.strip()
    v = int(s, 16) if '0x' in s else int(s)
    return '0x%Xu' % (v & 0xFFFFFFFF)


CTYPE = {'byte': 'uint8', 'word': 'uint16', 'dword': 'uint32'}


def read_op(s, size=None):
    s = s.strip()
    if is_imm(s):
        return imm(s)
    if s in ('eax', 'ecx', 'edx', 'ebx', 'esi', 'edi', 'ebp', 'esp', 'ax', 'bx', 'cx', 'dx', 'al', 'bl', 'cl', 'dl', 'ah', 'bh', 'ch', 'dh'):
        return reg_read(s)
    r = mem_ref(s)
    if r:
        return '(uint32)(*(%s*)(uintptr_t)%s)' % (CTYPE[r[0]], r[1])
    raise ValueError(s)


def op_bits(s):
    s = s.strip()
    if s in REG32:
        return 32
    if re.fullmatch(r'[a-d][xlh]', s):
        return 16 if s[1] == 'x' else 8
    r = mem_ref(s)
    if r:
        return {'byte': 8, 'word': 16, 'dword': 32}[r[0]]
    return 32


def write_op(s, val):
    s = s.strip()
    if re.fullmatch(r'(e[a-d]x|e[sd]i|e[bs]p|[a-d][xlh])', s):
        return reg_write(s, val)
    r = mem_ref(s)
    if r:
        return '*(%s*)(uintptr_t)%s = (%s)(%s);' % (CTYPE[r[0]], r[1], CTYPE[r[0]], val)
    raise ValueError(s)


# ------------------------------------------------------------------ llamadas
def call_code(t, ip):
    A = lambda k: 'M32(esp + %d)' % (4 * k)
    FP = lambda k: '(*(float*)(uintptr_t)(esp + %d))' % (4 * k)
    d = {
        0x542a50: 'v.ApplyTurnForce(CVector{ %s, %s, %s }, CVector{ %s, %s, %s }); esp += 0x18;' % (FP(0), FP(1), FP(2), FP(3), FP(4), FP(5)),
        0x5429f0: 'v.ApplyMoveForce(CVector{ %s, %s, %s }); esp += 0xC;' % (FP(0), FP(1), FP(2)),
        0x542ce0: '{ CVector* out_ = (CVector*)(uintptr_t)%s; *out_ = v.GetSpeed(CVector{ %s, %s, %s }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }' % (A(0), FP(1), FP(2), FP(3)),
        0x6d2600: '{ const float h_ = v.HeightAboveCeiling(%s, (eFlightModel)(int)%s); fp.FldRet(h_); esp += 8; }' % (FP(0), A(1)),
        0x53fb80: 'eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();',
        0x53fbd0: 'eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();',
        0x53fc10: 'eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();',
        0x53fc50: 'eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();',
        0x5403f0: 'eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();',
        0x540080: 'eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();',
        0x53fe10: 'eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;',
        0x53fdd0: 'eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;',
        0x535300: 'eax = (uint32)(uintptr_t)v.GetColModel();',
        0x40fe30: 'FlcVecAdd(fp, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s); eax = %s;' % (A(0), A(1), A(2), A(0)),
        0x40fe60: 'FlcVecSub(fp, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s); eax = %s;' % (A(0), A(1), A(2), A(0)),
        0x40fe90: 'FlcVecMulLeft(fp, (float*)(uintptr_t)%s, %s, (float*)(uintptr_t)%s); eax = %s;' % (A(0), FP(1), A(2), A(0)),
        0x40fec0: 'FlcVecMulRight(fp, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s, %s); eax = %s;' % (A(0), A(1), FP(2), A(0)),
        0x40fef0: 'FlcVecScale(fp, (float*)(uintptr_t)ecx, %s); eax = ecx; esp += 4;' % FP(0),
        0x411a00: 'FlcVecAddTo(fp, (float*)(uintptr_t)ecx, (float*)(uintptr_t)%s); eax = ecx; esp += 4;' % A(0),
        0x4082c0: 'FlcVecMagnitude(fp, (float*)(uintptr_t)ecx);',
        0x406da0: 'FlcVecMagnitudeSquared(fp, (float*)(uintptr_t)ecx);',
        0x40fdb0: 'FlcVecDot(fp, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s);' % (A(0), A(1)),
        0x59c730: 'FlcVecCross(fp, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s, (float*)(uintptr_t)%s); eax = %s;' % (A(0), A(1), A(2), A(0)),
        0x59c790: 'FlcMatMul3x3(fp, (float*)(uintptr_t)%s, (const float*)(uintptr_t)%s, (const float*)(uintptr_t)%s); eax = %s;' % (A(0), A(1), A(2), A(0)),
        0x59c810: 'FlcMatInvMul3x3(fp, (float*)(uintptr_t)%s, (const float*)(uintptr_t)%s, (const float*)(uintptr_t)%s); eax = %s;' % (A(0), A(1), A(2), A(0)),
        0x59c910: 'FlcVecNormalise(fp, (float*)(uintptr_t)ecx);',
        0x420800: 'FlcFloatMax(fp, %s, %s);' % (FP(0), FP(1)),
        0x404330: 'FlcFloatMin(fp, %s, %s);' % (FP(0), FP(1)),
        0x822130: 'FlcCrtPow(fp);',
        0x821e70: 'FlcCrtAsin(fp);',
        0x822380: 'FlcCrtAcos(fp);',
        0x821b40: 'eax = (uint32)FlcCrtFtol2(fp);',
    }
    if t not in d:
        raise ValueError('llamada desconocida %x en %x' % (t, ip))
    return d[t]


# ------------------------------------------------------------------ traduccion
labels = set()
for i in ins_list:
    if i.mnemonic.startswith('j'):
        tgt = int(i.op_str, 16)
        if a0 <= tgt < a1:
            labels.add(tgt)

flag_setters = {'cmp', 'test', 'add', 'sub', 'and', 'or', 'xor', 'neg', 'shr', 'inc', 'dec'}
out = []
last_flag_ip = None
addrs = {i.address for i in ins_list}


def mem_fp(opstr, kind):
    r = mem_ref(opstr)
    if kind == 'int':
        return '(*(int*)(uintptr_t)%s)' % r[1]
    if r[0] == 'qword':
        return '(*(double*)(uintptr_t)%s)' % r[1]
    return '(*(float*)(uintptr_t)%s)' % r[1]


def abs_const(opstr, kind):
    m = re.match(r'(dword|qword) ptr \[(0x[0-9a-f]+)\]$', opstr.strip())
    if m and int(m.group(2), 16) == 0xb7cb5c:
        return 'CTimer::ms_fTimeStep'
    return None


for i in ins_list:
    ip = i.address
    mn, ops = i.mnemonic, i.op_str
    if ip in labels:
        out.append('L_%x:;' % ip)
    cm = '    // %x: %s %s' % (ip, mn, ops)
    if COV:
        out.append('    FLC_COV(0x%x);' % ip)
    # --- x87
    if g.EXE[g.off(ip)] in range(0xD8, 0xE0):
        class J: pass
        j = J(); j.op_str = ops; j.mnemonic = mn
        txt = g.decode(bytes(i.bytes), j)
        # sustituir el operando de memoria
        mm = re.search(r'(dword|qword) ptr \[[^\]]*\]', ops)
        if mm:
            kind = 'int' if re.match(r'fp\.(FldI|Fi)', txt or '') else 'f'
            c = abs_const(mm.group(0), kind)
            ref = c if c else mem_fp(mm.group(0), kind)
            txt = txt.replace(g.mem_name(ops), ref)
        if txt.startswith('fp.Fnstsw()'):
            txt = 'eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);'
        if txt.startswith('//'):
            raise ValueError('x87 sin traducir %x %s %s' % (ip, mn, ops))
        out.append('    %s   //%s' % (txt, cm[6:]))
        continue
    # --- no x87
    sp = [s.strip() for s in ops.split(',')] if ops else []
    line = None
    if mn == 'mov':
        line = write_op(sp[0], read_op(sp[1]))
    elif mn == 'movsx' or mn == 'movzx':
        src = sp[1]
        b = op_bits(src)
        val = read_op(src)
        if b < 32 and mn == 'movsx':
            val = '(uint32)(int32_t)(%s)(%s)' % ({8: 'int8_t', 16: 'int16_t'}[b], read_op(src))
        line = write_op(sp[0], val)
    elif mn == 'lea':
        m = re.match(r'\[(.*)\]$', sp[1])
        line = write_op(sp[0], addr_expr(m.group(1)))
    elif mn == 'push':
        line = 'esp -= 4; M32(esp) = %s;' % read_op(sp[0])
    elif mn == 'pop':
        line = '%s esp += 4;' % write_op(sp[0], 'M32(esp)')
    elif mn in ('add', 'sub'):
        b = op_bits(sp[0])
        a_ = read_op(sp[0]); b_ = read_op(sp[1])
        op = '+' if mn == 'add' else '-'
        if sp[0] == 'esp':
            line = 'esp %s= %s;' % (op, b_)
        else:
            mask = {8: '0xFFu', 16: '0xFFFFu', 32: '0xFFFFFFFFu'}[b]
            line = 'fA = %s; fB = %s; fR = (fA %s fB) & %s; %s' % (a_, b_, op, mask, write_op(sp[0], 'fR'))
    elif mn in ('cmp', 'test'):
        b = op_bits(sp[0])
        mask = {8: '0xFFu', 16: '0xFFFFu', 32: '0xFFFFFFFFu'}[b]
        a_ = read_op(sp[0]); b_ = read_op(sp[1])
        if mn == 'cmp':
            line = 'fA = %s; fB = %s; fR = (fA - fB) & %s;' % (a_, b_, mask)
        else:
            line = 'fA = %s; fB = %s; fR = (fA & fB) & %s;' % (a_, b_, mask)
    elif mn == 'xor':
        line = 'fR = %s ^ %s; %s' % (read_op(sp[0]), read_op(sp[1]), write_op(sp[0], 'fR'))
    elif mn == 'neg':
        line = 'fR = (uint32)(0 - %s); %s' % (read_op(sp[0]), write_op(sp[0], 'fR'))
    elif mn == 'shr':
        line = 'fR = %s >> %s; %s' % (read_op(sp[0]), read_op(sp[1]), write_op(sp[0], 'fR'))
    elif mn == 'call':
        line = call_code(int(ops, 16), ip)
    elif mn == 'jmp':
        t = int(ops, 16)
        line = 'return;' if t in EXIT_TARGETS else 'goto L_%x;' % t
    elif mn.startswith('j'):
        t = int(ops, 16)
        tgt = 'return;' if t in EXIT_TARGETS else 'goto L_%x;' % t
        cond = {
            'je': 'fR == 0', 'jne': 'fR != 0',
            'jp': 'FlcParityEven(fR)', 'jnp': '!FlcParityEven(fR)',
            'ja': 'fA > fB', 'jae': 'fA >= fB', 'jb': 'fA < fB', 'jbe': 'fA <= fB',
        }[mn]
        line = 'if (%s) %s' % (cond, tgt)
    elif mn == 'nop' or mn == 'int3':
        line = ';'
    else:
        raise ValueError('instruccion sin traducir %x %s %s' % (ip, mn, ops))
    out.append('    ' + line + '   // %x: %s %s' % (ip, mn, ops))

body = '\n'.join(out)
hdr = '''void %s(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
''' % name
open(outp, 'wb').write((hdr + body + '\n}\n').encode())
print('ok', len(ins_list), 'instrucciones')
