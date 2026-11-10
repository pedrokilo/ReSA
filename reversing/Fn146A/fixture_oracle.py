"""Oraculo independiente (Python) de Fn146A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 146A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x676480  CTaskInteriorBeInShop (vtable 0x870244, ranura 0, destructor con borrado)    push esi; mov esi,ecx; call 675460; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   1  0x6764A0  CTaskInteriorShopKeeper (vtable 0x870290)                                      igual con call 6756A0
   2  0x64D1E0  CTaskSimpleWaitUntilPedIsOutCar (vtable 0x86F094)                              igual con call 6494F0
   3  0x64D200  CTaskSimpleWaitUntilPedIsInCar (vtable 0x86F0B8)                               igual con call 6496A0
   4  0x655EE0  CTaskComplexEvasiveStep (vtable 0x86F218)                                      igual con call 653350
   5  0x655F00  CTaskSimpleEvasiveDive (vtable 0x86F244)                                       igual con call 6535D0
   6  0x66DB60  CTaskSimpleAchieveHeading (vtable 0x86FD74)                                    igual con call 667E70
   7  0x66DB80  CTaskComplexGoToPointAndStandStill (vtable 0x86FDA8)                           igual con call 668240 (copia del cuerpo de 668630: mov [ecx],86FDA8h; jmp 61A3D0)
   8  0x4E7C50  CAEMFDecoder (vtable 0x85F370, ranura 6)                                       igual con call 4E77C0 y call 8214BD (operator delete del CRT)
   9  0x4E7850  CAEMFDecoder::Shutdown                                                         call 819840; jmp 819770
  10  0x4E7F20  CPad::IsRadioTrackSkipPressed                                                  cmp word [ecx+2Eh],0; je +; cmp word [ecx+5Eh],0; jne +; mov eax,1; ret; (+) xor eax,eax; ret
  11  0x7059E0  CRealTimeShadow::GetShadowCamera                                               lea eax,[ecx+8]; ret
  12  0x706770  CRealTimeShadowManager::Constructor                                            mov edx,ecx; xor eax,eax; [edx+44h]=[edx+48h]=[edx+4Ch]=[edx+50h]=eax; byte [edx]=al; byte [edx+1]=al;
                                                                                              lea edi,[edx+4]; mov ecx,10h; rep stosd; mov eax,edx; ret
  13  0x7067A0  CRealTimeShadowManager::Destructor                                             lea ecx,[esi+4Ch]; call 705400; lea ecx,[esi+44h]; jmp 705400
  14  0x404090  CBuilding::operator new                                                        mov ecx,[0B74498h]; jmp 403FA0
  15  0x4040A0  CBuilding::operator delete                                                     ecx=p-[pool]; edx=ecx*0x92492493>>32 (+ecx) >>5 (+ signo) = ecx/56; [[pool+4]+idx] |= 80h; if idx < [pool+0Ch] [pool+0Ch] = idx
  16  0x5326E0  CDummy::operator new                                                           mov ecx,[0B744A0h]; jmp 532630
  17  0x5326F0  CDummy::operator delete                                                        igual que 15 con el pool 0B744A0
  18  0x41B810  CPatrolRoute::operator new                                                     mov ecx,[0B744B4h]; jmp 41B660
  19  0x41B820  CPatrolRoute::operator delete                                                  igual que 15 con el pool 0B744B4 y 0x9C09C09D >> 8 = / 420 (0x1A4)
  20  0x5A2E60  ProcSurfaceInfo_c::ScaleMatrix                                                 ecx=3; eax=this+18h; bucle: fld [esp+4]; fmul [eax-18h]; fstp [eax-18h]; fld [esp+8]; fmul [eax-14h]; fstp [eax-14h]; fld [esp+0Ch]; fmul [eax-10h]; fstp [eax-10h]; eax+=10h; ret 0Ch
  21  0x5A2E90  ProcSurfaceInfo_c::Constructor                                                 lea ecx,[esi+3Ch]; call 4A8DD0 (eax=ecx; [eax]=[eax+4]=[eax+8]=0); eax=esi; ret

En los destructores con borrado el falso del cuerpo deja el dword del caso en [ECX] y devuelve un EAX que no es `this`; el falso de operator delete anota el dword de [argumento] al entrar (= lo que dejo
el cuerpo: prueba que se borra DESPUES de destruir). Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas. En operator new el falso de New anota ECX (el puntero del pool, etiqueta de arena
o crudo) y su EAX es el de la funcion. En operator delete el pool es un pool falso de la arena B (+0 almacenamiento, +4 estados, +0xC cursor): el indice es el cociente con signo, truncado hacia cero, de
(puntero - almacenamiento) como int32 entre el tamano del elemento (56, 56 y 420); el byte estados[indice] se marca con |= 0x80 y el cursor baja a indice solo si indice < cursor (con signo).
ScaleMatrix: cada componente es float(x87 a 24 bits(arg * elemento)): producto exacto de dos mantisas de 24 bits redondeado a 24 bits (mitad al par) con exponente ampliado y luego al float (con un segundo
redondeo en el rango denormal), NaN segun las reglas del x87 (el argumento entra por fld, que silencia un NaN senalizante; el elemento va como operando de memoria); inf * 0 es el NaN indefinido 0xFFC00000.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskInteriorBeInShop::ScalarDeletingDestructor', 'CTaskInteriorShopKeeper::ScalarDeletingDestructor', 'CTaskSimpleWaitUntilPedIsOutCar::ScalarDeletingDestructor',
         'CTaskSimpleWaitUntilPedIsInCar::ScalarDeletingDestructor', 'CTaskComplexEvasiveStep::ScalarDeletingDestructor', 'CTaskSimpleEvasiveDive::ScalarDeletingDestructor',
         'CTaskSimpleAchieveHeading::ScalarDeletingDestructor', 'CTaskComplexGoToPointAndStandStill::ScalarDeletingDestructor', 'CAEMFDecoder::ScalarDeletingDestructor', 'CAEMFDecoder::Shutdown',
         'CPad::IsRadioTrackSkipPressed', 'CRealTimeShadow::GetShadowCamera', 'CRealTimeShadowManager::Constructor', 'CRealTimeShadowManager::Destructor', 'CBuilding::operator new',
         'CBuilding::operator delete', 'CDummy::operator new', 'CDummy::operator delete', 'CPatrolRoute::operator new', 'CPatrolRoute::operator delete', 'ProcSurfaceInfo_c::ScaleMatrix',
         'ProcSurfaceInfo_c::Constructor']
ENTRIES = (0x676480, 0x6764A0, 0x64D1E0, 0x64D200, 0x655EE0, 0x655F00, 0x66DB60, 0x66DB80, 0x4E7C50, 0x4E7850, 0x4E7F20,
           0x7059E0, 0x706770, 0x7067A0, 0x404090, 0x4040A0, 0x5326E0, 0x5326F0, 0x41B810, 0x41B820, 0x5A2E60, 0x5A2E90)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


# ------------------------------------------------------------------------------------------------ tablas leidas del desensamblado
# destructores con borrado: (cuerpo del destructor, operator delete)
SDD = {0: (0x675460, 0x61A5B0), 1: (0x6756A0, 0x61A5B0), 2: (0x6494F0, 0x61A5B0), 3: (0x6496A0, 0x61A5B0), 4: (0x653350, 0x61A5B0), 5: (0x6535D0, 0x61A5B0),
       6: (0x667E70, 0x61A5B0), 7: (0x668240, 0x61A5B0), 8: (0x4E77C0, 0x8214BD)}
SHUTDOWN = (0x819840, 0x819770)
POOL_NEW = {14: 0x403FA0, 16: 0x532630, 18: 0x41B660}                    # funcion New del pool que sigue al `jmp`
POOL_DEL = {15: 56, 17: 56, 19: 0x1A4}                                   # tamano del elemento
THIS_FNS = tuple(range(0, 9)) + (10, 11, 12, 13, 20, 21)
SPAN = {10: 0x60, 11: 4, 12: 0x54, 13: 0x50, 20: 0x2C, 21: 0x48}


# ------------------------------------------------------------------------------------------------ el producto x87 (fld arg; fmul [elemento]; fstp [elemento]) a 24 bits
def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_zero(b):
    return (b & 0x7FFFFFFF) == 0


def decode(b):
    """(m, k): valor = m * 2**k para un float finito distinto de cero (valor absoluto)"""
    e, f = (b >> 23) & 0xFF, b & 0x7FFFFF
    return ((1 << 23) | f, e - 150) if e else (f, -149)


def round_half_even(num, shift):
    """num / 2**shift redondeado al entero par mas cercano (shift > 0)"""
    q, r = num >> shift, num & ((1 << shift) - 1)
    half = 1 << (shift - 1)
    if r > half or (r == half and (q & 1)):
        q += 1
    return q


def fmul24(x, e):
    """x: el argumento (entra por fld, silencia un NaN senalizante); e: el elemento (operando de memoria de fmul). Devuelve los bits del float guardado con fstp."""
    xn, en = is_nan(x), is_nan(e)
    if xn or en:
        xq = (x | 0x400000) if xn else x
        if xn and en:
            if not (e & 0x400000):          # NaN senalizante en el elemento y NaN silencioso en el argumento: gana el silencioso
                return xq
            return e if (e & 0x7FFFFF) > (xq & 0x7FFFFF) else xq     # dos NaN silenciosos: el de mayor significando (los significandos iguales no se generan)
        return xq if xn else (e | 0x400000)
    if (is_inf(x) and is_zero(e)) or (is_inf(e) and is_zero(x)):
        return 0xFFC00000
    sign = (x ^ e) & 0x80000000
    if is_inf(x) or is_inf(e):
        return sign | 0x7F800000
    if is_zero(x) or is_zero(e):
        return sign
    mx, kx = decode(x)
    me, ke = decode(e)
    p = mx * me
    k = kx + ke
    nb = p.bit_length()
    if nb > 24:
        shift = nb - 24
        q = round_half_even(p, shift)
        if q == 1 << 24:
            q >>= 1
            shift += 1
        k += shift
    else:
        q = p
    nq = q.bit_length()
    ve = nq - 1 + k                      # exponente (floor log2) del valor q * 2**k tras el redondeo a 24 bits
    if ve > 127:
        return sign | 0x7F800000
    if ve >= -126:
        frac = (q << (24 - nq)) & 0x7FFFFF
        return sign | ((ve + 127) << 23) | frac
    s2 = k + 149                         # unidades de 2**-149; negativo: segundo redondeo (al almacenar el float denormal)
    if s2 >= 0:
        r = q << s2
    else:
        r = round_half_even(q, -s2) if -s2 < nq + 2 else 0
    if r >= 1 << 23:
        return sign | 0x00800000
    return sign | r


def input_marks(b):
    if is_nan(b):
        return ['in_qnan' if b & 0x400000 else 'in_snan']
    if is_inf(b):
        return ['in_inf']
    if is_zero(b):
        return ['in_zero']
    if ((b >> 23) & 0xFF) == 0:
        return ['in_denorm']
    return []


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if fn in THIS_FNS or fn in POOL_NEW or fn in POOL_DEL:
        o = a[0]
        span = SPAN.get(fn, 4) if fn in THIS_FNS else 0x10
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + span > WIN:
            marks.append('spill')
    return marks


KEYS = {}
for _fn in range(22):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    if _fn in THIS_FNS or _fn in POOL_NEW or _fn in POOL_DEL:
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _fn in SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    elif _fn == 10:
        _k += ['new_zero', 'new_nonzero', 'old_zero', 'old_nonzero', 'new_high_only', 'old_high_only', 'pressed', 'not_pressed']
    elif _fn == 13:
        _k += ['prev_zero', 'prev_other']
    elif _fn in POOL_NEW:
        _k += ['pool_arena', 'pool_raw', 'ret_zero', 'ret_other']
    elif _fn in POOL_DEL:
        _k += ['idx_neg', 'idx_zero', 'idx_pos', 'idx_big', 'cursor_lowered', 'cursor_kept', 'cursor_equal', 'rem_nonzero']
    elif _fn == 20:
        _k += ['prod_normal', 'prod_denorm_out', 'prod_overflow', 'prod_zero_out', 'prod_nan_out', 'prod_inf_out', 'in_denorm', 'in_snan', 'in_qnan', 'in_inf', 'in_zero']
    KEYS[_fn] = _k


def trunc_div(d, e):
    q = abs(d) // e
    return q if d >= 0 else -q


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret = [], None
    o = a[0]
    if fn in SDD:
        body, delete = SDD[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events.append([body, tag(0, o), a[3]])
        A.wr(o, 4, a[5])                                    # el falso del cuerpo deja su dword en [this]
        flags = a[6] & 0xFF                                 # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([delete, tag(0, o), a[5]])        # operator delete ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 9:
        A.snap(); B.snap(); C.snap()
        events.append([SHUTDOWN[0]])
        events.append([SHUTDOWN[1]])
    elif fn == 10:
        nw, ow = a[3] & 0xFFFF, a[4] & 0xFFFF
        A.wr(o + 0x2E, 2, nw)
        A.wr(o + 0x5E, 2, ow)
        A.snap(); B.snap(); C.snap()
        ret = 1 if (nw != 0 and ow == 0) else 0
        marks.append('new_nonzero' if nw else 'new_zero')
        marks.append('old_nonzero' if ow else 'old_zero')
        if nw and not (nw & 0xFF):
            marks.append('new_high_only')
        if ow and not (ow & 0xFF):
            marks.append('old_high_only')
        marks.append('pressed' if ret else 'not_pressed')
    elif fn == 11:
        A.snap(); B.snap(); C.snap()
        ret = tag(0, o + 8)
    elif fn == 12:
        A.snap(); B.snap(); C.snap()
        A.wr(o + 0x44, 4, 0); A.wr(o + 0x48, 4, 0); A.wr(o + 0x4C, 4, 0); A.wr(o + 0x50, 4, 0)
        A.wr(o, 1, 0); A.wr(o + 1, 1, 0)
        A.wr(o + 4, 0x40, 0)                                # rep stosd: 16 dwords desde +4
        ret = tag(0, o)
    elif fn == 13:
        A.wr(o + 0x44, 4, a[3])
        A.wr(o + 0x4C, 4, a[6])
        A.snap(); B.snap(); C.snap()
        events.append([0x705400, tag(0, o + 0x4C), a[6]])   # primero la camara de +0x4C
        A.wr(o + 0x4C, 4, a[5])
        events.append([0x705400, tag(0, o + 0x44), a[3]])   # luego la de +0x44 (salto)
        A.wr(o + 0x44, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn in POOL_NEW:
        A.snap(); B.snap(); C.snap()
        raw = a[6] & 1
        events.append([POOL_NEW[fn], a[7] if raw else tag(1, o)])
        ret = a[4]
        marks.append('pool_raw' if raw else 'pool_arena')
        marks.append('ret_zero' if a[4] == 0 else 'ret_other')
    elif fn in POOL_DEL:
        e = POOL_DEL[fn]
        cursor = a[5] - (1 << 32) if a[5] >> 31 else a[5]
        B.wr(o + 0x0, 4, a[3])
        B.wr(o + 0x8, 4, a[7])
        B.wr(o + 0xC, 4, a[5])
        A.snap(); B.snap(); C.snap()
        d = a[4] - (1 << 32) if a[4] >> 31 else a[4]
        t = trunc_div(d, e)
        C.m[GUARD + a[6]] = C.get(GUARD + a[6]) | 0x80
        if t < cursor:
            B.wr(o + 0xC, 4, t & M32)
        marks.append('idx_neg' if t < 0 else 'idx_zero' if t == 0 else 'idx_pos')
        if abs(t) > 100000:
            marks.append('idx_big')
        marks.append('cursor_lowered' if t < cursor else 'cursor_equal' if t == cursor else 'cursor_kept')
        if d - t * e != 0:
            marks.append('rem_nonzero')
    elif fn == 20:
        for r in range(3):
            for k in range(3):
                A.wr(o + 16 * r + 4 * k, 4, a[16 + 3 * r + k])
        A.snap(); B.snap(); C.snap()
        for r in range(3):
            for k in range(3):
                x, el = a[3 + k], a[16 + 3 * r + k]
                res = fmul24(x, el)
                A.wr(o + 16 * r + 4 * k, 4, res)
                marks += input_marks(x) + input_marks(el)
                if is_nan(res):
                    marks.append('prod_nan_out')
                elif is_inf(res):
                    marks.append('prod_inf_out')
                    if not (is_inf(x) or is_inf(el)):
                        marks.append('prod_overflow')
                elif is_zero(res):
                    if not (is_zero(x) or is_zero(el)):
                        marks.append('prod_zero_out')
                elif ((res >> 23) & 0xFF) == 0:
                    marks.append('prod_denorm_out')
                else:
                    marks.append('prod_normal')
    else:   # 21
        A.snap(); B.snap(); C.snap()
        A.wr(o + 0x3C, 4, 0); A.wr(o + 0x40, 4, 0); A.wr(o + 0x44, 4, 0)
        ret = tag(0, o)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'st0': None, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
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
