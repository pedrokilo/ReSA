"""Oraculo independiente (Python) de Fn129A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las catorce funciones del lote 129A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y llamadores:
  0x463370  mov eax,ecx; mov ecx,[esp+4]; mov dl,[ecx]; mov [eax],dl; mov dl,[ecx+1]; mov [eax+1],dl; mov dl,[ecx+2]; mov [eax+2],dl; mov cl,[ecx+3]; mov [eax+3],cl; ret 4   (31 B)
            copia de un CRGBA (CFont::ParseToken 0x718F4E y 0x719057, 0x46E65B, 0x477121, 0x483F75): cuatro bytes en orden, cargando y escribiendo uno a uno. Devuelve this.
  0x4B25E0  push esi; mov esi,ecx; mov ecx,[esp+8]; mov eax,[ecx]; push edi; call [eax+4]; mov edx,[esi]; mov ecx,esi; mov edi,eax; call [edx+4]; cmp edi,eax; pop edi; setne al; pop esi; ret 4
            (32 B) ranura 11 de la vtable 0x86CD00 (CEventEscalator::TakesPriorityOver): tipo del otro evento y despues el propio (ranura 1 de cada vtable), AL = distintos.
  0x4DA510  movzx eax,[esp+4]; shl eax,4; fild dword [eax+8AD914h]; fmul dword [858C58h]; fldlg2; fxch st(1); fyl2x; fmul dword [858BA4h]; ret   (33 B) cdecl: ST0 = 20 * log10(n * 0.01f), con
            n = dword de la tabla de superficies y la x87 a 24 bits de mantisa (0.01f = 0x3C23D70A, 20.0f = 0x41A00000). Llamadores 0x4E1FE5 y 0x4E2077.
  0x41BDD0  test [ecx+42Dh],2; jne +; mov al,[esp+4]; mov dl,[ecx+428h]; shl al,4; xor al,dl; and al,10h; xor dl,al; mov [ecx+428h],dl; ret 4; (rama) and [ecx+428h],0EFh; ret 4   (47 B)
  0x4E0380  mov al,[ecx+14h]; test al,al; jne +; mov eax,-1; ret 8; movsx edx,word [esp+8]; movzx eax,word [esp+4]; imul edx,edx,12D4h; mov ecx,[ecx]; add eax,2; lea eax,[eax+eax*2];
            lea eax,[edx+eax*4]; mov eax,[eax+ecx]; ret 8   (48 B) dword en [m_BankSlots] + slot * 0x12D4 + (sonido + 2) * 12 (= LoopStartOffset de la muestra).
  0x56CE30  push esi; push 56CDF0h; push 56CDE0h; push 10h; mov esi,ecx; push 8; push esi; call 821D30; lea eax,[esi+5]; mov ecx,10h; xor edx,edx; (bucle) mov [eax-5],edx; mov [eax-1],dl;
            mov [eax],dl; add eax,8; dec ecx; jne; mov eax,esi; pop esi; ret   (51 B) CAccidentManager::CAccidentManager; 0x56CDE0 = CAccident::CAccident (dword +0, bytes +4 y +5 a 0).
  0x4EE9A0  mov eax,ecx; xor ecx,ecx; mov [eax],-1; mov [eax+4],ecx; mov [eax+808h],cl; mov [eax+809h],cl; mov [eax+80Ah],cx; mov [eax+80Ch],cx; mov [eax+810h],ecx; mov [eax+818h],ecx; ret
            (52 B) constructor de CAESmoothFadeThread (instancia 0xB608D0, inicializador estatico 0x84C295).
  0x40F700  push esi; mov esi,ecx; mov al,[esi+29h]; test al,4; je fin; mov ecx,[esi+2Ch]; test ecx,ecx; je fin; test al,2; je ramaB; push ecx; call 416400; mov eax,[esi+2Ch]; push eax; call 72F430;
            add esp,8; pop esi; ret; ramaB: call 40F070 (ECX = [esi+2Ch]); mov ecx,[esi+2Ch]; push ecx; call 8214BD; add esp,4; pop esi; ret   (52 B) ~CColModel.
  0x6C4130  fld [esp+4]; mov eax,[ecx+14h]; fsin; xor ecx,ecx; mov [eax+8],ecx; mov [eax+18h],ecx; mov [eax+20h],ecx; mov [eax+24h],ecx; mov [eax+28h],3F800000h; fst [eax]; fld [esp+4]; fcos;
            fld st0; fchs; fstp [eax+4]; fstp [eax+10h]; fstp [eax+14h]; ret 4   (54 B) matriz de rotacion en [this+0x14]. Llamadores 0x6CD3EC y 0x6CD707 (CPlane::DoPlaneGenerationAndRemoval).
  0x4049C0  push esi; mov esi,ecx; mov eax,[esi+8]; test eax,eax; jle fin; mov al,[esi+10h]; test al,al; je +; mov eax,[esi]; push eax; call 8213AE; mov ecx,[esi+4]; push ecx; call 8213AE; add esp,8;
            mov [esi],0; mov [esi+4],0; mov [esi+8],0; mov [esi+0Ch],0; pop esi; ret   (54 B) cuerpo del destructor de CPool (sin llamadores).
  0x43AA40  mov edx,[esp+4]; xor ecx,ecx; mov eax,9691E0h; (bucle) cmp [eax],edx; je hallado; add eax,1Ch; inc ecx; cmp eax,969368h; jl; (no hallado: escribe en la direccion 0x18); hallado: mov al,[esp+8];
            imul ecx,ecx,1Ch; add ecx,9691D8h; mov [ecx+18h],al; ret   (55 B) cdecl (ped, byte): CConversations::EnableConversation. Llamador 0x4749E3 y 0x4749F5 (comando de script).
  0x705480  push esi; mov esi,ecx; mov eax,[esi]; mov ecx,[eax+60h]; push edi; mov edi,[ecx+0Ch]; push ecx; mov [eax+60h],0; call 7FB020; push 5; push 0; push edi; push edi; call 7FB230;
            mov ecx,[esi]; mov [ecx+60h],eax; mov edx,[esi+4]; push eax; push edx; call 7F35D0; add esp,1Ch; pop edi; pop esi; ret   (57 B) CShadowCamera (sin llamadores).
  0x70F890  mov edx,[esp+8]; mov eax,[esp+4]; fld [edx+8]; fmul [eax+4]; fld [eax+8]; fmul [edx+4]; fsubp; fstp [ecx]; fld [eax+8]; fmul [edx]; fld [eax]; fmul [edx+8]; fsubp; fstp [ecx+4];
            fld [eax]; fmul [edx+4]; fld [edx]; fmul [eax+4]; fsubp; fstp [ecx+8]; ret 8   (57 B) CVector::Cross_OG (a = [esp+4], b = [esp+8]); sin llamadores.
  0x40F030  mov eax,ecx; mov dl,[eax+7]; xor ecx,ecx; and dl,0F8h; mov [eax],cx; mov [eax+2],cx; mov [eax+4],cx; mov [eax+6],cl; mov [eax+7],dl; mov [eax+8..2Ch],ecx (10 dwords); ret   (58 B) CCollisionData
            constructor. Llamadores 0x53760D, 0x56618B, 0x566F39 y por salto 0x852015, 0x853E15.

x87: la palabra de control del juego es de 24 bits de mantisa (redondeo al mas cercano) y las excepciones enmascaradas. La funcion 2 deja UN registro valido (ST0) con el resultado en 80 bits; las demas
dejan la pila vacia. Los productos y restas de la funcion 12 se redondean a 24 bits con el exponente ampliado y solo el resultado se guarda como float.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from decimal import Decimal, getcontext
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['CRGBA::Copy', 'CEventEscalator::TakesPriorityOver', 'CAEPedAudioEntity::SurfaceVolumeDb', 'CVehicle::SetEngineOn', 'CAEMP3BankLoader::GetLoopOffset', 'CAccidentManager::Constructor',
         'CAESmoothFadeThread::Constructor', 'CColModel::Destructor', 'CPlane::SetHeadingMatrix', 'CPools::PoolDestructorBody', 'CConversations::EnableConversation', 'CShadowCamera::RecreateRaster',
         'CVector::Cross_OG', 'CCollisionData::Constructor']
ENTRIES = (0x463370, 0x4B25E0, 0x4DA510, 0x41BDD0, 0x4E0380, 0x56CE30, 0x4EE9A0, 0x40F700, 0x6C4130, 0x4049C0, 0x43AA40, 0x705480, 0x70F890, 0x40F030)
SPAN = (16, 4, 0, 0x42E, 0x15, 0x80, 0x81C, 0x30, 0x18, 0x11, 0, 8, 0, 0x30)
HAS_THIS = (True, True, False, True, True, True, True, True, True, True, False, True, False, True)
GUARD = 0x40
WIN = 0x1000
TABLE = 0x8AD914
SLOT_SIZE = 0x12D4
CONV_STRIDE = 0x1C
FLOAT_001 = 0x3C23D70A   # 0.01f (0x858C58)


def t0(off):
    """etiqueta de un puntero dentro de la arena del objeto (arena 0): 0x10000000 + guarda + desplazamiento"""
    return 0x10000000 + GUARD + off


def t1(off):
    """etiqueta de un puntero dentro de la arena secundaria (arena 1)"""
    return 0x10000000 + (1 << 20) + GUARD + off


def t2(off):
    """etiqueta de un puntero dentro de la arena terciaria (arena 2)"""
    return 0x10000000 + (2 << 20) + GUARD + off


def dword_bytes(v):
    return [(v >> (8 * k)) & 0xFF for k in range(4)]


def signed32(v):
    return v - (1 << 32) if v >> 31 else v


def signed16(v):
    v &= 0xFFFF
    return v - (1 << 16) if v >> 15 else v


# ---------------------------------------------------------------------------------------------------- aritmetica x87 (24 bits de mantisa, exponente ampliado)
def f32_parts(bits):
    """(clase, signo, m, e, mant23): clase 'zero' | 'finite' | 'inf' | 'nan'; valor = m * 2**e"""
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 255:
        return ('inf' if man == 0 else 'nan', sign, 0, 0, man)
    if exp == 0:
        return ('zero', sign, 0, 0, 0) if man == 0 else ('finite', sign, man, -149, 0)
    return ('finite', sign, man | 0x800000, exp - 150, 0)


def round_to_bits(m, e, bits=24, sticky=False):
    """redondea m * 2**e (+ epsilon si sticky) a `bits` bits de mantisa, al mas cercano, empate al par"""
    if m == 0:
        return (0, 0)
    n = m.bit_length()
    if n <= bits:
        return (m, e)
    drop = n - bits
    q = m >> drop
    rem = m & ((1 << drop) - 1)
    half = 1 << (drop - 1)
    if rem > half or (rem == half and (sticky or (q & 1))):
        q += 1
    if q == (1 << bits):
        q >>= 1
        drop += 1
    return (q, e + drop)


def ext80(sign, m, e):
    """codificacion de 80 bits de (-1)**sign * m * 2**e (m entero > 0 o 0)"""
    if m == 0:
        return (b'\x00' * 8) + ((sign << 15)).to_bytes(2, 'little')
    nb = m.bit_length()
    sig = m << (64 - nb) if nb <= 64 else m >> (nb - 64)
    biased = e + nb - 1 + 16383
    return sig.to_bytes(8, 'little') + ((sign << 15) | biased).to_bytes(2, 'little')


def ext80_nan(sign, man23):
    sig = (1 << 63) | ((man23 | 0x400000) << 40)
    return sig.to_bytes(8, 'little') + ((sign << 15) | 0x7FFF).to_bytes(2, 'little')


def ext80_inf(sign):
    return (1 << 63).to_bytes(8, 'little') + ((sign << 15) | 0x7FFF).to_bytes(2, 'little')


def f32_class(bits):
    cls, sign, m, e, man = f32_parts(bits)
    if cls == 'finite':
        return 'denormal' if (bits >> 23) & 0xFF == 0 else 'normal'
    return cls


# Valores de la x87: ('nan', signo, mantisa23 silenciada) | ('inf', signo) | ('fin', signo, m, e) (m == 0: cero con signo)
def val_of_f32(bits, mem=False):
    cls, sign, m, e, man = f32_parts(bits)
    if cls == 'nan':
        # mem: operando de memoria de fmul; si es senalizante no se convierte antes de la operacion y pierde frente a un QNaN del registro
        return ('nan', sign, man | 0x400000, bool(mem and not man & 0x400000))
    if cls == 'inf':
        return ('inf', sign)
    return ('fin', sign, m, e)


DEFAULT_NAN = ('nan', 1, 0x400000, False)


def nan_pick(a, b):
    """dos operandos con al menos un NaN: gana el de mayor significando; con uno solo, ese"""
    if a[0] == 'nan' and b[0] == 'nan':
        if a[2] == b[2]:
            return b if (a[1] == 1 and b[1] == 0) else a   # medido en la x87: con el mismo significando gana el de signo positivo
        return a if a[2] > b[2] else b
    return a if a[0] == 'nan' else b


def fmul24(a, b):
    if a[0] == 'nan' and b[0] == 'nan' and len(b) > 3 and b[3]:
        return a
    if a[0] == 'nan' or b[0] == 'nan':
        return nan_pick(a, b)
    sign = a[1] ^ b[1]
    if a[0] == 'inf' or b[0] == 'inf':
        other = b if a[0] == 'inf' else a
        if other[0] == 'fin' and other[2] == 0:
            return DEFAULT_NAN
        return ('inf', sign)
    if a[2] == 0 or b[2] == 0:
        return ('fin', sign, 0, 0)
    m, e = round_to_bits(a[2] * b[2], a[3] + b[3])
    return ('fin', sign, m, e)


def fsub24(a, b):
    """a - b"""
    if a[0] == 'nan' or b[0] == 'nan':
        return nan_pick(a, b)
    nb_sign = b[1] ^ 1
    if a[0] == 'inf' or b[0] == 'inf':
        if a[0] == 'inf' and b[0] == 'inf':
            return ('inf', a[1]) if a[1] == nb_sign else DEFAULT_NAN
        return ('inf', a[1]) if a[0] == 'inf' else ('inf', nb_sign)
    if a[2] == 0 and b[2] == 0:
        return ('fin', 1 if (a[1] and nb_sign) else 0, 0, 0)
    if a[2] == 0:
        return ('fin', nb_sign, b[2], b[3])
    if b[2] == 0:
        return ('fin', a[1], a[2], a[3])
    e = min(a[3], b[3])
    sa = (a[2] << (a[3] - e)) * (-1 if a[1] else 1)
    sb = (b[2] << (b[3] - e)) * (-1 if nb_sign else 1)
    total = sa + sb
    if total == 0:
        return ('fin', 0, 0, 0)
    sign = 1 if total < 0 else 0
    m, e2 = round_to_bits(abs(total), e)
    return ('fin', sign, m, e2)


def val_to_f32(v):
    """fstp dword: redondeo al mas cercano (con denormales y desbordamiento) del valor de la x87"""
    if v[0] == 'nan':
        return (v[1] << 31) | 0x7F800000 | v[2]
    if v[0] == 'inf':
        return (v[1] << 31) | 0x7F800000
    sign, m, e = v[1], v[2], v[3]
    if m == 0:
        return sign << 31
    top = e + m.bit_length() - 1
    lsb = max(top - 23, -149)
    if e >= lsb:
        q = m << (e - lsb)
    else:
        shift = lsb - e
        q = m >> shift
        rem = m & ((1 << shift) - 1)
        half = 1 << (shift - 1)
        if rem > half or (rem == half and (q & 1)):
            q += 1
    if q == 0:
        return sign << 31
    if q >= (1 << 24):
        q >>= 1
        lsb += 1
    if lsb == -149 and q < (1 << 23):
        return (sign << 31) | q
    exp = lsb + 23 + 127
    if exp >= 255:
        return (sign << 31) | 0x7F800000
    return (sign << 31) | (exp << 23) | (q & 0x7FFFFF)


# ---------------------------------------------------------------------------------------------------- funciones 2 y 8: log10 y sin/cos con decimales de precision alta
getcontext().prec = 120
PI = Decimal('3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196')
TWO_PI = PI * 2


def dec_of(m, e):
    return Decimal(m) * (Decimal(2) ** e)


def dec_to_val(d):
    """Decimal -> ('fin', signo, m, e) con unos 160 bits de mantisa"""
    if d == 0:
        return ('fin', 0, 0, 0)
    sign = 1 if d < 0 else 0
    d = abs(d)
    exp2 = int(d.ln() / Decimal(2).ln()) - 160
    m = int((d / (Decimal(2) ** exp2)).to_integral_value())
    return ('fin', sign, m, exp2)


def dec_sin(x):
    """sin de un Decimal con reduccion a [-pi, pi] y serie de Taylor"""
    k = (x / TWO_PI).to_integral_value()
    r = x - k * TWO_PI
    term = r
    total = r
    r2 = r * r
    n = 1
    while abs(term) > Decimal(10) ** -110:
        term = -term * r2 / ((2 * n) * (2 * n + 1))
        total += term
        n += 1
    return total


def sincos_f32(bits):
    """(sin, cos, -cos) del angulo (float) como bits de float, con el comportamiento de fsin/fcos"""
    cls, sign, m, e, man = f32_parts(bits)
    if cls == 'nan':
        q = (sign << 31) | 0x7FC00000 | man
        return q, q, q ^ 0x80000000
    if cls == 'inf':
        return 0xFFC00000, 0xFFC00000, 0x7FC00000
    if cls == 'zero':
        return bits, 0x3F800000, 0xBF800000
    if e + m.bit_length() - 1 >= 63:
        # fuera de rango (|x| >= 2**63): fsin y fcos dejan el operando
        return bits, bits, bits ^ 0x80000000
    x = dec_of(m, e) * (-1 if sign else 1)
    s = dec_sin(x)
    c = dec_sin(x + PI / 2)
    sb = val_to_f32(dec_to_val(s))
    cb = val_to_f32(dec_to_val(c))
    return sb, cb, cb ^ 0x80000000


def surface_st0(n):
    """ST0 de 80 bits tras 20 * log10(fild(n) * 0.01f): (bytes, marca)"""
    n = signed32(n)
    if n == 0:
        return ext80_inf(1), 'zero'
    cls, sign, m, e, man = f32_parts(FLOAT_001)
    if n < 0:
        return ext80_nan(1, 0), 'negative'
    pm, pe = round_to_bits(n * m, e)
    x = dec_of(pm, pe)
    y = x.log10() * 20
    if y == 0:
        return ext80(0, 0, 0), 'one'
    v = dec_to_val(y)
    q, qe = round_to_bits(v[2], v[3])
    return ext80(v[1], q, qe), 'normal'


# ---------------------------------------------------------------------------------------------------- modelo
def changes_from(mem, orig, arena):
    return [[arena, p, mem[p]] for p in sorted(mem) if mem[p] != orig[p]]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    kind, fill = a[1], a[2]
    marks = ['fill%d' % fill, 'kind%d' % kind] if HAS_THIS[fn] else ['family%d' % c['family']]
    out = {'events': [], 'changes': [], 'ret': None, 'al': None, 'st0': None, 'gchanges': None, 'fb': None, 'marks': marks}
    off = a[0]
    if HAS_THIS[fn] and fn != 12:
        marks.append('this_unaligned' if off % 4 else 'this_aligned')
        if off + SPAN[fn] > WIN:
            marks.append('this_spill')
    if fn == 0:
        mem = {}
        for i in range(6):
            for k, b in enumerate(dword_bytes(a[6 + i])):
                mem[off - 8 + 4 * i + k] = b
        orig = dict(mem)
        if a[12] == 0:
            sb = dword_bytes(a[15])
            for k in range(4):
                mem[off + k] = sb[k]
            marks.append('separate')
        else:
            delta = signed32(a[13])
            for k in range(4):
                mem[off + k] = mem[off + delta + k]
            marks.append('overlap_zero' if delta == 0 else 'overlap_forward' if delta > 0 else 'overlap_backward')
        out['changes'] = changes_from(mem, orig, 0)
        out['ret'] = t0(off)
        marks.append('changes0' if not out['changes'] else 'changes_some')
    elif fn == 1:
        tt, to = a[6], a[7]
        out['events'] = [[2, t1(a[8]), to], [1, t0(off), tt]]
        out['al'] = 1 if tt != to else 0
        marks.append('differ' if tt != to else 'equal')
        marks.append('differ_high' if tt != to and (tt ^ to) >> 8 and not (tt ^ to) & 0xFF else 'differ_other')
    elif fn == 2:
        out['st0'], mark = surface_st0(a[0])
        marks.append(mark)
        marks.append('index_high' if a[6] & 0xFFFFFF00 else 'index_plain')
    elif fn == 3:
        b0, b1, arg = a[6] & 0xFF, a[7] & 0xFF, a[8] & 1
        nb = (b0 & 0xEF) if b1 & 2 else ((b0 & 0xEF) | (arg << 4))
        out['changes'] = [[0, off + 0x428, nb]] if nb != b0 else []
        marks.append('broken' if b1 & 2 else 'sound')
        marks.append('arg_on' if arg else 'arg_off')
        marks.append('was_on' if b0 & 0x10 else 'was_off')
        marks.append('changes0' if not out['changes'] else 'changes_some')
    elif fn == 4:
        init = a[9] & 0xFF
        out['ret'] = M32 if init == 0 else a[6]
        slot, sound = signed16(a[7]), a[8] & 0xFFFF
        marks.append('uninit' if init == 0 else 'init')
        marks.append('slot_negative' if slot < 0 else 'slot_zero' if slot == 0 else 'slot_positive')
        marks.append('sound_beyond' if sound >= 400 else 'sound_in_range')
        marks.append('dirty_high' if (a[7] | a[8]) >> 16 else 'clean_high')
    elif fn == 5:
        mem, orig = {}, {}
        for e in range(16):
            active = (a[9] >> e) & 1
            d = ((e + 1) * a[6]) & M32 if active else 0
            b4 = ((e + 3) * a[7]) & 0xFF if active else 0
            b5 = ((e + 5) * a[8]) & 0xFF if active else 0
            for k, b in enumerate(dword_bytes(d)):
                orig[off + 8 * e + k] = b
                mem[off + 8 * e + k] = 0
            orig[off + 8 * e + 4] = b4
            mem[off + 8 * e + 4] = 0
            orig[off + 8 * e + 5] = b5
            mem[off + 8 * e + 5] = 0
        out['changes'] = changes_from(mem, orig, 0)
        out['ret'] = t0(off)
        cnt = bin(a[9] & 0xFFFF).count('1')
        marks.append('active%d' % (cnt if cnt < 5 else 5 if cnt < 8 else 8 if cnt < 16 else 16))
        marks.append('changes0' if not out['changes'] else 'changes_some')
    elif fn == 6:
        fields = [(0, 4, a[6], M32), (4, 4, a[7], 0), (0x808, 1, a[8] & 0xFF, 0), (0x809, 1, a[9] & 0xFF, 0), (0x80A, 2, a[10] & 0xFFFF, 0), (0x80C, 2, a[11] & 0xFFFF, 0),
                  (0x810, 4, a[12], 0), (0x818, 4, a[13], 0)]
        mem, orig = {}, {}
        for pos, size, prior, new in fields:
            for k in range(size):
                orig[off + pos + k] = (prior >> (8 * k)) & 0xFF
                mem[off + pos + k] = (new >> (8 * k)) & 0xFF
        out['changes'] = changes_from(mem, orig, 0)
        out['ret'] = t0(off)
        marks.append('changes0' if not out['changes'] else 'changes_many' if len(out['changes']) > 12 else 'changes_some')
        marks.append('handle_was_ones' if a[6] == M32 else 'handle_other')
    elif fn == 7:
        flags = a[6] & 0xFF
        ptr = 0 if a[7] == 0 else t1(a[9]) if a[7] == 1 else a[8]
        active = bool(flags & 4) and ptr != 0
        if active:
            if flags & 2:
                out['events'] = [[0x416400, ptr], [0x72F430, ptr]]
            else:
                out['events'] = [[0x40F070, ptr], [0x8214BD, ptr]]
        marks.append('inactive_flag' if not flags & 4 else 'null_data' if ptr == 0 else 'single_alloc' if flags & 2 else 'separate_alloc')
        marks.append('ptr_arena' if a[7] == 1 else 'ptr_null' if a[7] == 0 else 'ptr_raw')
    elif fn == 8:
        s, cs, ncs = sincos_f32(a[6])
        mem, orig = {}, {}
        prior = dword_bytes(a[8])
        base = a[7]
        news = {0: s, 4: ncs, 8: 0, 0x10: cs, 0x14: s, 0x18: 0, 0x20: 0, 0x24: 0, 0x28: 0x3F800000}
        for k in range(11):
            for j in range(4):
                orig[base + 4 * k + j] = prior[j]
                mem[base + 4 * k + j] = prior[j]
        for pos, val in news.items():
            for j, b in enumerate(dword_bytes(val)):
                mem[base + pos + j] = b
        out['changes'] = changes_from(mem, orig, 1)
        cls = f32_class(a[6])
        marks.append('angle_' + cls)
        marks.append('angle_neg' if a[6] >> 31 else 'angle_pos')
        ex = (a[6] >> 23) & 0xFF
        marks.append('angle_huge' if cls == 'normal' and ex >= 190 else 'angle_small' if cls == 'normal' and ex < 100 else 'angle_mid' if cls == 'normal' else 'angle_special')
    elif fn == 9:
        cap = signed32(a[8])
        owns = a[10] & 0xFF
        if cap > 0:
            if owns:
                out['events'] = [[0x8213AE, a[6]], [0x8213AE, a[7]]]
            mem, orig = {}, {}
            for pos, val in ((0, a[6]), (4, a[7]), (8, a[8]), (0xC, a[9])):
                for j, b in enumerate(dword_bytes(val)):
                    orig[off + pos + j] = b
                    mem[off + pos + j] = 0
            out['changes'] = changes_from(mem, orig, 0)
        marks.append('empty' if cap <= 0 else 'owner' if owns else 'not_owner')
        marks.append('cap_negative' if cap < 0 else 'cap_zero' if cap == 0 else 'cap_positive')
    elif fn == 10:
        j = a[7]
        pos = CONV_STRIDE * j + 0x18
        prior = (a[9] + j * 37) & 0xFF
        new = a[6] & 0xFF
        out['gchanges'] = [[pos, new]] if new != prior else []
        marks.append('slot%d' % (0 if j == 0 else 1 if j < 4 else 2 if j < 13 else 3))
        marks.append('duplicate' if a[8] != 14 else 'unique')
        marks.append('byte_zero' if new == 0 else 'byte_one' if new == 1 else 'byte_other')
        marks.append('dirty_high' if a[6] >> 8 else 'clean_high')
        marks.append('changes0' if new == prior else 'changes1')
    elif fn == 11:
        w = a[8]
        old = t2(a[7])
        newr = 0 if a[9] == 0 else t2(a[13]) if a[9] == 1 else a[10]
        tex = 0 if a[11] == 0 else t1(0x10) if a[11] == 1 else a[12]
        out['events'] = [[0x7FB020, old, 0], [0x7FB230, w, w, 0, 5, 0], [0x7F35D0, tex, newr, newr]]
        out['fb'] = newr
        marks.append('raster_null' if a[9] == 0 else 'raster_arena' if a[9] == 1 else 'raster_raw')
        marks.append('tex_null' if a[11] == 0 else 'tex_arena' if a[11] == 1 else 'tex_raw')
        marks.append('width_zero' if w == 0 else 'width_negative' if w >> 31 else 'width_positive')
    elif fn == 12:
        pa, pb, po, P = a[6], a[7], a[8], a[9]
        mem = {}
        for k in range(3):
            mem[po + 4 * k] = P
        for k in range(3):
            mem[pa + 4 * k] = a[10 + k]
        for k in range(3):
            mem[pb + 4 * k] = a[13 + k]
        orig = dict(mem)

        def rd(p):
            return val_of_f32(mem[p])

        def rdm(p):
            return val_of_f32(mem[p], True)
        # x = b.z*a.y - a.z*b.y
        x = fsub24(fmul24(rd(pb + 8), rdm(pa + 4)), fmul24(rd(pa + 8), rdm(pb + 4)))
        mem[po] = val_to_f32(x)
        # y = a.z*b.x - a.x*b.z (leidos tras escribir x)
        y = fsub24(fmul24(rd(pa + 8), rdm(pb)), fmul24(rd(pa), rdm(pb + 8)))
        mem[po + 4] = val_to_f32(y)
        # z = a.x*b.y - b.x*a.y
        z = fsub24(fmul24(rd(pa), rdm(pb + 4)), fmul24(rd(pb), rdm(pa + 4)))
        mem[po + 8] = val_to_f32(z)
        ch = []
        for p in sorted(mem):
            ob = dword_bytes(orig[p])
            nbv = dword_bytes(mem[p])
            for j in range(4):
                if ob[j] != nbv[j]:
                    ch.append([1, p + j, nbv[j]])
        out['changes'] = ch
        marks.append('place%d' % (c['family'] % 8))
        res = [x, y, z]
        marks.append('res_nan' if any(r[0] == 'nan' for r in res) else 'res_inf' if any(r[0] == 'inf' for r in res) else 'res_zero' if all(r[0] == 'fin' and r[2] == 0 for r in res) else 'res_finite')
        ins = [a[10 + k] for k in range(3)] + [a[13 + k] for k in range(3)]
        marks.append('in_nan' if any(f32_class(v) == 'nan' for v in ins) else 'in_inf' if any(f32_class(v) == 'inf' for v in ins) else 'in_denormal' if any(f32_class(v) == 'denormal' for v in ins) else 'in_plain')
        marks.append('overlap' if abs(po - pa) < 12 or abs(po - pb) < 12 else 'apart')
    else:
        P, Q = a[6], a[7]
        mem, orig = {}, {}
        for k in range(12):
            pb = dword_bytes(Q if k == 1 else P)
            nbv = [0, 0, 0, pb[3] & 0xF8] if k == 1 else [0, 0, 0, 0]
            for j in range(4):
                orig[off + 4 * k + j] = pb[j]
                mem[off + 4 * k + j] = nbv[j]
        out['changes'] = changes_from(mem, orig, 0)
        out['ret'] = t0(off)
        marks.append('changes0' if not out['changes'] else 'changes_some')
        marks.append('flags_kept' if (Q >> 24) & 0xF8 else 'flags_clear')
        marks.append('low_bits_set' if (Q >> 24) & 7 else 'low_bits_clear')
    return out


def fp_stack_valid(hexstr):
    """posiciones validas de una imagen fnsave normalizada: [(st, 10 bytes)]"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return valid


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    for k in ('ret', 'al', 'gchanges', 'fb'):
        if o.get(k) != e[k]:
            return 'campo %s %s != %s' % (k, o.get(k), e[k])
    valid = fp_stack_valid(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != e['st0']:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], e['st0'].hex())
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
