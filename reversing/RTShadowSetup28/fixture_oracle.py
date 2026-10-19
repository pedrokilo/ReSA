"""Oraculo independiente del banco RTShadowSetup28: CRealTimeShadow::SetupForThisEntity (0x706520).

Se modela desde el DESENSAMBLADO del EXE (gta-sa.exe 1.0 US; no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

thiscall: ECX = this (esi), [esp+4] = owner (CPhysical*); `ret 4`; push esi/edi (y ebx en las ramas internas) restaurados. Devuelve bool en AL.
  706520 eax=owner ; esi=this ; [esi+0] = owner                       (m_pOwner se guarda ANTES de mirar el objeto)
  70652A edi=[eax+18h] (m_pRwObject) ; test edi,edi ; je 7065F7        (nulo: AL=0, ret 4; no se escribe nada mas)
  706535 eax=movzx byte [edi] (tipo) ; [esi+24h]=eax (m_nRwObjectType, DWORD) ; tipo==0 o tipo>2 (sin signo) -> 7065F0
  7065F0 call 705990 (Destroy, ecx=this) ; AL=0 ; ret 4
  tipo 1 (atomo) -> 70657D: test byte [edi+4Ch],2 ; si bit 1: call 7491F0 (_rpAtomicResyncInterpolatedSphere(edi)) ; despues copia 4 dwords
      [edi+1Ch..2Bh] -> [esi+2Ch..3Bh] (esfera de limites: centro xyz + radio) ; [esi+48h] = [esi+38h] (radio, dword) ;
      call 7EDD90 (RwV3dTransformPoints(esi+3Ch, esi+2Ch, 1, [edi+4]+10h)) ; add esp,10h
  tipo 2 (clump): call 734FC0 (RpClumpGetBoundingSphere(edi, esi+2Ch, 1)) ; [esi+48h] = [esi+38h] ; call 7EDD90 igual ; add esp,1Ch
  7065C6 fld dword [esi+38h] ; fmul dword [858F14h] (1,1f) ; fstp dword [esp] (redondeo a 24 bits) ; ecx=esi+8 ; call 7054C0 (SetFrustum, ret 4)
  7065DD ecx=esi+8 ; push esi+3Ch ; call 705590 (SetCenter(const CVector&), ret 4) ; AL=1 ; ret 4
Bytes de la sombra que se escriben: [0..3] owner, [24..27] tipo, [2C..3B] esfera de limites, [3C..47] centro base (por RwV3dTransformPoints), [48..4B] radio base. El resto, intacto.

Falsos del banco (identicos en ambos lados; ver README.md):
  Destroy (0x705990)                      -> traza [6, this]; NO modifica nada
  _rpAtomicResyncInterpolatedSphere (0x7491F0) -> traza [1, objeto, dword de +4Ch]; dword_i(+1Ch..2Bh) ^= 0x13579BDF + i*0x01010101
  RpClumpGetBoundingSphere (0x734FC0)     -> traza [2, objeto, esfera destino, 3.er argumento]; destino.dword_i = objeto.dword_i(+1Ch) ^ (0x2468ACE0 + i*0x00FF00FF)
  RwV3dTransformPoints (0x7EDD90)         -> traza [3, destino, origen, cuenta, matriz]; destino_k = origen_k ^ M_k ^ 0x3C6EF372, M_k = posicion del marco[k] si la matriz cae en la arena de marcos,
                                             si no (padre nulo / invalido) M_k = matriz + k*0x1111
  SetFrustum (0x7054C0)                   -> traza [4, this_camara, bits del argumento]
  SetCenter (0x705590)                    -> traza [5, this_camara, puntero, 3 dwords leidos]
"""
import ctypes
import math
import struct
import sys

M32 = 0xFFFFFFFF
NAME = 'CRealTimeShadow::SetupForThisEntity'
NAMES = [NAME]
CASES = {NAME: 5120}
KGUARD = 0x40
SHADOW_AT, SHADOW_BYTES = KGUARD, 0x4C
OBJ_AT, OBJ_BYTES = KGUARD, 0x80
FRAME_AT, FRAME_BYTES = KGUARD, 0x60
BOGUS_PARENT = 0x00C0FFE0
MUL_BITS = 0x3F8CCCCD
OWNER_LABEL = 0x0A110000
LABELS_OBJ_PARENT = {0: 0, 1: 1, 2: 2, 3: 3, 4: 4}


def u32le(b, at):
    return int.from_bytes(b[at:at + 4], 'little')


def put(b, at, v):
    b[at:at + 4] = (v & M32).to_bytes(4, 'little')


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def mul_single(rbits):
    """float * 1,1f con un solo redondeo a precision simple (SSE mulss)."""
    r = rbits & M32
    x = fl(r)
    if math.isnan(x):
        return r | 0x400000
    if math.isinf(x):
        return r
    return bits(ctypes.c_float(x * fl(MUL_BITS)).value)       # el producto 24x24 bits cabe exacto en un double


def mul_x87(rbits):
    """float * 1,1f en el x87 con la palabra de control a 24 bits (mantisa de 24 bits, exponente extendido) y fstp a float (segundo redondeo)."""
    r = rbits & M32
    x = fl(r)
    if math.isnan(x):
        return r | 0x400000
    if math.isinf(x) or x == 0:
        return bits(ctypes.c_float(x * fl(MUL_BITS)).value)
    p = x * fl(MUL_BITS)
    m, e = math.frexp(p)
    y = math.ldexp(round(m * 2 ** 24), e - 24)
    return bits(ctypes.c_float(y).value)


def model(inp):
    sh = bytearray.fromhex(inp['pre'])
    obj = bytearray.fromhex(inp['object'])
    assert len(sh) == SHADOW_BYTES and len(obj) == OBJ_BYTES
    pl = inp['parent_label']
    put(obj, 4, pl)                                    # el banco registra el objeto con el puntero al padre sustituido por su etiqueta
    put(sh, 0, OWNER_LABEL)
    trace = []
    st = {}
    if not inp['obj_present']:
        return 0, sh, obj, trace, st
    t = obj[0]
    put(sh, 0x24, t)
    st['type'] = t
    if t == 0 or t > 2:
        trace.append([6, SHADOW_AT, 0, 0, 0, 0])
        return 0, sh, obj, trace, st
    if t == 1:
        flags = u32le(obj, 0x4C)
        st['flags_bit1'] = bool(flags & 2)
        if flags & 2:
            trace.append([1, OBJ_AT, flags, 0, 0, 0])
            for i in range(4):
                put(obj, 0x1C + 4 * i, u32le(obj, 0x1C + 4 * i) ^ ((0x13579BDF + i * 0x01010101) & M32))
        sh[0x2C:0x3C] = obj[0x1C:0x2C]
    else:
        trace.append([2, OBJ_AT, SHADOW_AT + 0x2C, 1, 0, 0])
        for i in range(4):
            put(sh, 0x2C + 4 * i, u32le(obj, 0x1C + 4 * i) ^ ((0x2468ACE0 + i * 0x00FF00FF) & M32))
    sh[0x48:0x4C] = sh[0x38:0x3C]
    if pl in (1, 2, 3):
        mat = FRAME_AT + (pl - 1) * FRAME_BYTES + 0x10
        mvals = [inp['frame_pos'][3 * (pl - 1) + k] & M32 for k in range(3)]
    else:
        mat = 0x10 if pl == 0 else (BOGUS_PARENT + 0x10) & M32
        mvals = [(mat + k * 0x1111) & M32 for k in range(3)]
    trace.append([3, SHADOW_AT + 0x3C, SHADOW_AT + 0x2C, 1, mat, 0])
    for k in range(3):
        put(sh, 0x3C + 4 * k, u32le(sh, 0x2C + 4 * k) ^ mvals[k] ^ 0x3C6EF372)
    radius = u32le(sh, 0x38)
    a, b = mul_single(radius), mul_x87(radius)
    if a != b:
        raise RuntimeError('Fixture inseguro: el radio %#x da %#x con SSE y %#x con x87 a 24 bits (doble redondeo)' % (radius, a, b))
    trace.append([4, SHADOW_AT + 8, a, 0, 0, 0])
    trace.append([5, SHADOW_AT + 8, SHADOW_AT + 0x3C, u32le(sh, 0x3C), u32le(sh, 0x40), u32le(sh, 0x44)])
    st['atomic'] = t == 1
    st['clump'] = t == 2
    st['parent_label'] = pl
    st['radius_class'] = classify(radius)
    st['in_radius_class'] = classify(u32le(bytes.fromhex(inp['object']), 0x28))
    st['frustum'] = a
    st['radius'] = radius
    return 1, sh, obj, trace, st


def classify(rb):
    e, m = (rb >> 23) & 0xFF, rb & 0x7FFFFF
    if e == 255:
        return 'nan' if m else ('neg_inf' if rb >> 31 else 'inf')
    if e == 0:
        return 'zero' if m == 0 else 'denormal'
    return 'neg' if rb >> 31 else 'pos'


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    ret, sh, obj, trace, st = model(inp)
    stats = {}
    g = inp['group']
    stats['g%d' % g] = True
    stats['fam%d' % inp['family']] = True
    stats['ret_true'] = ret == 1
    stats['ret_false'] = ret == 0
    stats['obj_null'] = not inp['obj_present']
    t = st.get('type')
    if inp['obj_present']:
        o0 = bytes.fromhex(inp['object'])[0]
        stats['type0'] = o0 == 0
        stats['type_other'] = o0 > 2
        stats['type_ge128'] = o0 >= 128 and o0 > 2
    stats['atomic'] = bool(st.get('atomic'))
    stats['clump'] = bool(st.get('clump'))
    if st.get('atomic'):
        stats['resync'] = st['flags_bit1']
        stats['no_resync'] = not st['flags_bit1']
        flags = u32le(bytes.fromhex(inp['object']), 0x4C)
        stats['flags_hi_bits_no_bit1'] = (flags & 2) == 0 and (flags & 0xFFFFFFFC) != 0
        stats['flags_only_bit1'] = flags == 2
    if 'parent_label' in st:
        pl = st['parent_label']
        stats['parent_null'] = pl == 0
        stats['parent_bogus'] = pl == 4
        stats['parent_frame'] = pl in (1, 2, 3)
        for k in (1, 2, 3):
            stats['parent_frame%d' % k] = pl == k
        stats['rad_' + st['radius_class']] = True
        stats['in_rad_' + st['in_radius_class']] = True
        fr = st['frustum']
        stats['fr_inf_from_finite'] = classify(st['radius']) in ('pos', 'neg', 'zero', 'denormal') and (fr & 0x7FFFFFFF) == 0x7F800000
        stats['fr_nan'] = (fr & 0x7F800000) == 0x7F800000 and (fr & 0x7FFFFF) != 0
        stats['fr_zero'] = (fr & 0x7FFFFFFF) == 0
        stats['fr_denormal'] = (fr & 0x7F800000) == 0 and (fr & 0x7FFFFF) != 0
        stats['fr_negative'] = bool(fr >> 31) and not stats['fr_nan']
    return {'ret': ret, 'shadow': bytes(sh).hex(), 'object': bytes(obj).hex(), 'trace': trace, 'guards_ok': True, 'owner_ok': True, 'frames_ok': True}, stats


KEYS = ('ret', 'shadow', 'object', 'trace', 'guards_ok', 'owner_ok', 'frames_ok')


def compare(observed, want):
    """Claves en las que lo observado difiere de lo esperado."""
    return [k for k in KEYS if observed.get(k) != want.get(k)]


if __name__ == '__main__':
    import random
    # 1) mul_single == mul_x87 en todo el rango normal y para los denormales vetados; y el contraejemplo conocido existe
    r = random.Random(7)
    for _ in range(100000):
        b = r.getrandbits(32)
        if (b >> 23) & 0xFF == 0:
            continue
        assert mul_single(b) == mul_x87(b), hex(b)
    for b in (1, 2, 3, 0x400000, 0x7FFFFF, 0x7FFFFE, 0x200000, 0x100001):
        for s in (0, 0x80000000):
            assert mul_single(b | s) == mul_x87(b | s)
    assert any(mul_single(b) != mul_x87(b) for b in range(0x300000, 0x300400)), 'el doble redondeo debe existir para algun denormal'
    assert mul_single(0x7F7FFFFF) == 0x7F800000 and mul_single(0x80000000) == 0x80000000 and mul_single(0x7FC00001) == 0x7FC00001 and mul_single(0x7FA00000) == 0x7FE00000
    assert mul_single(0x3F800000) == MUL_BITS
    # 2) camino de atomo con resincronizacion y marco 2
    obj = bytearray(OBJ_BYTES)
    obj[0] = 1
    put(obj, 0x4C, 0x102)
    for i in range(4):
        put(obj, 0x1C + 4 * i, 0x3F800000 + i)
    pos = [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC]
    inp = dict(group=4, family=0, obj_present=True, object=bytes(obj).hex(), parent_label=2, frame_pos=pos, pre=bytes(range(SHADOW_BYTES)).hex())
    want, st = expected(NAME, inp, 0)
    sh = bytes.fromhex(want['shadow'])
    assert want['ret'] == 1 and u32le(sh, 0) == OWNER_LABEL and u32le(sh, 0x24) == 1
    c0 = [0x3F800000 + i ^ ((0x13579BDF + i * 0x01010101) & M32) for i in range(4)]
    assert [u32le(sh, 0x2C + 4 * i) for i in range(4)] == c0 and u32le(sh, 0x48) == c0[3]
    assert [u32le(sh, 0x3C + 4 * k) for k in range(3)] == [c0[k] ^ pos[3 + k] ^ 0x3C6EF372 for k in range(3)]
    assert [e[0] for e in want['trace']] == [1, 3, 4, 5] and want['trace'][1][4] == FRAME_AT + FRAME_BYTES + 0x10
    assert want['trace'][2][2] == mul_single(c0[3])
    assert sh[4:0x24] == bytes(range(4, 0x24)) and sh[0x28:0x2C] == bytes(range(0x28, 0x2C))
    # 3) clump con padre nulo; tipo desconocido; sin objeto
    obj[0] = 2
    inp2 = dict(inp, object=bytes(obj).hex(), parent_label=0)
    w2, _ = expected(NAME, inp2, 0)
    assert [e[0] for e in w2['trace']] == [2, 3, 4, 5] and w2['trace'][1][4] == 0x10
    obj[0] = 0x80
    w3, _ = expected(NAME, dict(inp, object=bytes(obj).hex()), 0)
    assert w3['ret'] == 0 and w3['trace'] == [[6, SHADOW_AT, 0, 0, 0, 0]] and u32le(bytes.fromhex(w3['shadow']), 0x24) == 0x80
    w4, _ = expected(NAME, dict(inp, obj_present=False), 0)
    assert w4['ret'] == 0 and w4['trace'] == [] and u32le(bytes.fromhex(w4['shadow']), 0x24) == 0x27262524
    print('ok')
