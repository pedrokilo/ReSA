"""Oraculo independiente del banco PedWrappers60: CPed::ProcessControl (0x5E8CD0), CPed::SpecialEntityPreCollisionStuff (0x5E3C30) y CPed::SpecialEntityCalcCollisionSteps (0x5E3E90).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco: "pokes" (desplazamiento, tamano, valor, es_puntero) de cada
objeto falso, globales, resultados de los falsos y mutaciones. Los punteros son simbolicos (0xFD000000 | etiqueta). El oraculo lleva un mapa de bytes CONOCIDOS por objeto: leer un byte
que la fila no especifica o escribir sobre uno desconocido lanza Fail (la fixture esta mal generada). La x87 va con el FCW de 24 bits: cada operacion float/double del EXE se redondea a
24 bits de mantisa con exponente extendido (clase Fp mas abajo, con Fraction).

x87: "fcomp ; fnstsw ; test ah,41h ; je X" salta si a > b (ordenado) ; "test ah,5 ; jp X" salta salvo a < b (ordenado) ; "test ah,44h ; jp X" salta salvo a == b (ordenado) ;
"test dl(=1),ah" (C0) es verdadero si a < b o no ordenado.

=== CPed::SpecialEntityCalcCollisionSteps 0x5E3E90 (thiscall, ECX=this -> ESI, 2 argumentos de pila que no se leen, `ret 8`; devuelve AL) ===
  3E9A  si [esi+FCh] (adjunto) != 0 -> AL = 1.
  3EAA  si [esi+480h] (datos de jugador) == 0: v = ((x*x + y*y) + z*z) * paso * paso (todo a 24 bits) ; si !(v >= 0.09f) -> AL = 1.
  3EEF  d = fsqrt((x*x + y*y) + z*z) * paso ;
        sin datos de jugador: ceil(d * 5.0f) (fstp qword ; call ceil 0x823820 ; _ftol2) ;
        con datos y [esi+568h] (entidad bajo los pies) != 0: c = ceil(d * 6.6666665f) ; c < 4.0 -> 4, si no _ftol2(ceil(float(d * 6.6666665f))) ;
        con datos y sin entidad: c = ceil(d * 3.3333333f) ; c < 2.0 -> 2, si no igual.
  3FBE  sin datos de jugador: [esi+9Ch] = float(2 * [esi+9Ch]) (elasticidad) ; EAX = 32 bits bajos de _ftol2 ; la funcion devuelve AL.

=== CPed::SpecialEntityPreCollisionStuff 0x5E3C30 (thiscall, 6 argumentos de pila, `ret 18h`) ===
  Salidas: "OR" = [esi+40h] |= 0x1000 (5E3E61) y "COLA" = 5E3E64: si el tipo ([esi+598h]) es 0 o 1 y CPedIntelligence::GetTaskClimb (0x601180) != 0 -> [esi+40h] |= 0x1000.
  Ver expected_ps().

=== CPed::ProcessControl 0x5E8CD0 (thiscall, sin argumentos, `ret`; guarda EBX, EBP, ESI, EDI) ===
  Ver expected_pc(): sigue el desensamblado bloque a bloque con las direcciones del EXE.
"""
import math
import struct
from fractions import Fraction

M32 = 0xFFFFFFFF
NAME_PC = 'CPed::ProcessControl'
NAME_PS = 'CPed::SpecialEntityPreCollisionStuff'
NAME_CS = 'CPed::SpecialEntityCalcCollisionSteps'
NAMES = [NAME_CS, NAME_PS, NAME_PC]
CASES_BY_NAME = {NAME_PC: 6144, NAME_PS: 4096, NAME_CS: 4096}
CASES = sum(CASES_BY_NAME.values())
ADDR = {NAME_PC: 0x5E8CD0, NAME_PS: 0x5E3C30, NAME_CS: 0x5E3E90}

# tamanos de arena (suma a mano guarda + ventana + guarda; comprobados por fixture_mirror.py contra Fixtures.hpp)
SIZES = {'ped': (0x79C, 0x40, 0x81C), 'pdata': (0xAC, 0x20, 0xEC), 'pped': (0x488, 0x20, 0x4C8), 'ent': (0x598, 0x20, 0x5D8), 'veh': (0x28, 0x20, 0x68), 'mat': (0x48, 0x20, 0x88),
         'minfo': (0x18, 0x20, 0x58), 'tok': (0x10, 0x10, 0x30), 'matarr': (0x40, 0x20, 0x80), 'bools': (8, 8, 0x18), 'vtbl': (0x68, 0x10, 0x88)}

# etiquetas (mismo orden que Fixtures.hpp)
(L_NULL, L_PED, L_PDATA, L_PDATA2, L_PPED, L_ENT, L_VEH, L_MATPED, L_MATOTHER, L_MATCAM, L_MI0, L_MI1, L_OBJINFO, L_FX, L_HIER, L_CLUMP, L_INTEL, L_WOBJ, L_CM0, L_CM1, L_TEX, L_MATARR, L_VTBL,
 L_BOOLS, L_COUNT) = range(25)
TOK = 0xFD000000


def tok(label):
    return TOK | label


def is_tok(v):
    return (v & 0xFF000000) == TOK and (v & 0xFFFFFF) < L_COUNT


# tipos de evento de ProcessControl / PreCollision
(K_AE_SERVICE, K_SWIM, K_GETHIER, K_BONEIDX, K_MATARR, K_CREATEFX, K_FXLOCAL, K_FXCOPY, K_FXPLAY, K_FXDESTROY, K_COORS, K_PRUNE, K_GETALPHA, K_SETALPHA, K_COLMODELS, K_FINDPLAYER, K_ISMEMBER,
 K_BUOY, K_PFIRST, K_PPROC, K_SKIP, K_PHYSPC, K_DELW, K_FOOT, K_NEWVEL, K_UPDPOS, K_MOVEANIM, K_BLOOD, K_SHADOW, K_PEDCOUNT, K_SAY, K_WAE_ADD, K_WAE_SERVICE, K_BADVIRT) = range(1, 35)
K_CLIMB = 35
EVENT_WIDTH = 20
FX_NAME_HASH = None   # se calcula abajo (FNV-1a de "molotov_flame")

# ids de falso mutante (mismo orden que PStub de Fixtures.hpp)
(S_PRUNE, S_BUOY, S_PFIRST, S_SKIP, S_PHYSPC, S_DELW, S_FOOT, S_PPROC, S_NEWVEL, S_UPDPOS, S_MOVEANIM, S_COUNT) = range(12)


class Fail(Exception):
    """Fixture invalida: el EXE leeria o escribiria algo que la fila no especifica o que el banco no modela."""


# ---------------------------------------------------------------------------------------------------- enteros
def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fnv1a(data):
    h = 0xCBF29CE484222325
    for b in data:
        h = ((h ^ b) * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def fnv32(data):
    return fnv1a(data) & M32


FX_NAME_HASH = fnv32(b'molotov_flame')


# ---------------------------------------------------------------------------------------------------- x87 a 24 bits (exponente extendido)
class Fp:
    """Valor de la x87: k = 'f' finito (v Fraction, s = signo del cero), 'i' infinito (s = +-1), 'n' NaN."""
    __slots__ = ('k', 'v', 's')

    def __init__(self, k, v=None, s=1):
        self.k = k
        self.v = v
        self.s = s

    @staticmethod
    def bits(b):
        b &= M32
        sign = -1 if b >> 31 else 1
        e = (b >> 23) & 0xFF
        m = b & 0x7FFFFF
        if e == 0xFF:
            return Fp('n', b | 0x00400000) if m else Fp('i', None, sign)      # fld dword silencia los NaN (conserva la carga util)
        if e == 0:
            return Fp('f', Fraction(sign * m, 1 << 149), sign)
        return Fp('f', Fraction(sign * (m | 0x800000), 1 << 150) * (Fraction(2) ** e), sign)

    @staticmethod
    def of(x):
        if isinstance(x, Fp):
            return x
        return Fp('f', Fraction(x), 1)

    def is_zero(self):
        return self.k == 'f' and self.v == 0

    def neg(self):
        if self.k == 'n':
            return self
        if self.k == 'i':
            return Fp('i', None, -self.s)
        return Fp('f', -self.v, -self.s)

    def to_bits(self):
        """Redondeo a float32 (al mas cercano, pares) con desbordamiento a infinito y denormales."""
        if self.k == 'n':
            return self.v
        if self.k == 'i':
            return 0xFF800000 if self.s < 0 else 0x7F800000
        v = self.v
        if v == 0:
            return 0x80000000 if self.s < 0 else 0
        sign = 0x80000000 if v < 0 else 0
        a = abs(v)
        # exponente e tal que 2^e <= a < 2^(e+1)
        e = a.numerator.bit_length() - a.denominator.bit_length()
        if Fraction(2) ** e > a:
            e -= 1
        if e < -126:
            e = -126          # denormal: cuanto fijo 2^-149
        q = a / (Fraction(2) ** (e - 23))     # mantisa con 23 bits fraccionarios
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (n & 1)):
            n += 1
        if e < -126:
            pass
        if n >= (1 << 24):
            n >>= 1
            e += 1
        if e > 127:
            return sign | 0x7F800000
        if e == -126 and n < (1 << 23):
            return sign | n
        return sign | ((e + 127) << 23) | (n & 0x7FFFFF)


def r24(v):
    """Redondea una Fraction a 24 bits de mantisa (al mas cercano, pares), sin limite de exponente."""
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    q = a / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (n & 1)):
        n += 1
    r = Fraction(n) * (Fraction(2) ** (e - 23))
    return -r if v < 0 else r


def _nan():
    return Fp('n', 0xFFC00000)          # NaN indefinido de la x87 (operacion invalida)


def _pick_nan(a, b):
    """Si un operando es NaN, el resultado es ese NaN silenciado; con dos, el de mayor significando."""
    if a.k == 'n' and b.k == 'n':
        return a if (a.v & 0x7FFFFF) >= (b.v & 0x7FFFFF) else b
    return a if a.k == 'n' else b


def fadd(a, b):
    a, b = Fp.of(a), Fp.of(b)
    if a.k == 'n' or b.k == 'n':
        return _pick_nan(a, b)
    if a.k == 'i' or b.k == 'i':
        if a.k == 'i' and b.k == 'i':
            return a if a.s == b.s else _nan()
        return a if a.k == 'i' else b
    s = a.v + b.v
    if s == 0:
        if a.v == 0 and b.v == 0:
            return Fp('f', Fraction(0), a.s if a.s == b.s else 1)
        return Fp('f', Fraction(0), 1)
    return Fp('f', r24(s), 1)


def fsub(a, b):
    a, b = Fp.of(a), Fp.of(b)
    if a.k == 'n' or b.k == 'n':
        return _pick_nan(a, b)
    return fadd(a, b.neg())


def fmul(a, b):
    a, b = Fp.of(a), Fp.of(b)
    if a.k == 'n' or b.k == 'n':
        return _pick_nan(a, b)
    sa = a.s if a.k == 'i' else (a.s if a.v == 0 else (1 if a.v > 0 else -1))
    sb = b.s if b.k == 'i' else (b.s if b.v == 0 else (1 if b.v > 0 else -1))
    if a.k == 'i' or b.k == 'i':
        if a.is_zero() or b.is_zero():
            return _nan()
        return Fp('i', None, sa * sb)
    p = a.v * b.v
    if p == 0:
        return Fp('f', Fraction(0), sa * sb)
    return Fp('f', r24(p), 1)


def fsqrt(a):
    a = Fp.of(a)
    if a.k == 'n':
        return a
    if a.k == 'i':
        return a if a.s > 0 else _nan()
    if a.v == 0:
        return a
    if a.v < 0:
        return _nan()
    q = a.v
    # sqrt con unos 80 bits de precision (la escala se ajusta al exponente de q) y bit pegajoso
    e = q.numerator.bit_length() - q.denominator.bit_length()
    shift = (160 - e) // 2
    scaled = q * (Fraction(2) ** (2 * shift))
    n, d = scaled.numerator, scaled.denominator
    iv = n // d
    r = math.isqrt(iv)
    exact = (r * r == iv) and (iv * d == n)
    val = Fraction(r) / (Fraction(2) ** shift)
    if not exact:
        val += Fraction(1) / (Fraction(2) ** (shift + 40))      # pegajoso: muy por debajo del medio ulp de 24 bits
    return Fp('f', r24(val), 1)


def lt(a, b):
    a, b = Fp.of(a), Fp.of(b)
    return _cmp(a, b, lambda x, y: x < y)


def gt(a, b):
    a, b = Fp.of(a), Fp.of(b)
    return _cmp(a, b, lambda x, y: x > y)


def eq(a, b):
    a, b = Fp.of(a), Fp.of(b)
    return _cmp(a, b, lambda x, y: x == y)


def _val(a):
    if a.k == 'i':
        return a.s * math.inf
    return a.v


def _cmp(a, b, op):
    if a.k == 'n' or b.k == 'n':
        return False
    return op(_val(a), _val(b))


def ftol_low(a):
    """_ftol2: 32 bits bajos del entero de 64 bits truncado (NaN/infinito/|x| >= 2^63 -> 0x8000000000000000)."""
    a = Fp.of(a)
    if a.k != 'f':
        return 0
    t = int(a.v)            # trunca hacia cero
    if t >= (1 << 63) or t < -(1 << 63):
        return 0
    return t & M32


def fceil(a):
    a = Fp.of(a)
    if a.k != 'f':
        return a
    v = a.v
    c = -((-v.numerator) // v.denominator)
    return Fp('f', Fraction(c), 1)


def f32(a):
    """fstp dword + fld dword: valor redondeado a float32."""
    return Fp.bits(Fp.of(a).to_bits())


def fl(bits):
    return Fp.bits(bits)


def f32c(x):
    """Constante float32 del EXE a partir de un numero Python."""
    return Fp.bits(struct.unpack('<I', struct.pack('<f', x))[0])


def sumsq(x, y, z):
    """(x*x + y*y) + z*z a 24 bits sin guardar (CVector::SquaredMagnitude 0x406DA0 y la copia en linea de 0x5E3EB4)."""
    return fadd(fadd(fmul(x, x), fmul(y, y)), fmul(z, z))


# ---------------------------------------------------------------------------------------------------- memoria falsa con bytes conocidos
class Mem:
    """Memoria falsa: los punteros se guardan simbolicos (etiqueta); `changes` los informa resueltos con las direcciones reales de las arenas (`addrs`, que registra el banco)."""

    def __init__(self, size, pokes, name, addrs=None):
        self.name = name
        self.size = size
        self.addrs = addrs
        self.b = bytearray(size)
        self.known = bytearray(size)
        self.ptr_offs = set()
        self.init = None
        for off, sz, val, ptr in pokes:
            for i in range(sz):
                self.b[off + i] = (val >> (8 * i)) & 0xFF
                self.known[off + i] = 1
            if ptr:
                self.ptr_offs.add(off)
        self.init = bytes(self.b)
        self.initk = bytes(self.known)

    def resolved(self, image):
        """Imagen con los punteros simbolicos sustituidos por la direccion real de su arena."""
        out = bytearray(image)
        for off in self.ptr_offs:
            v = int.from_bytes(image[off:off + 4], 'little')
            if is_tok(v):
                label = v & 0xFFFFFF
                a = self.addrs[label] if self.addrs else 0x1000 * label
                out[off:off + 4] = a.to_bytes(4, 'little')
        return out

    def rd(self, off, sz):
        if off < 0 or off + sz > self.size:
            raise Fail('%s: lectura fuera de la ventana +%#x/%d' % (self.name, off, sz))
        for i in range(sz):
            if not self.known[off + i]:
                raise Fail('%s: lectura de un byte sin especificar +%#x' % (self.name, off + i))
        return int.from_bytes(self.b[off:off + sz], 'little')

    def wr(self, off, sz, val):
        if off < 0 or off + sz > self.size:
            raise Fail('%s: escritura fuera de la ventana +%#x/%d' % (self.name, off, sz))
        for i in range(sz):
            if not self.known[off + i]:
                raise Fail('%s: escritura sobre un byte sin especificar +%#x' % (self.name, off + i))
            self.b[off + i] = (val >> (8 * i)) & 0xFF
        if sz == 4 and (is_tok(val & M32) or off in self.ptr_offs):
            self.ptr_offs.add(off)

    def changes(self, skip=()):
        out = []
        now, init = self.resolved(self.b), self.resolved(self.init)
        for i in range(self.size):
            if now[i] != init[i] and i not in skip:
                out.append([i, now[i]])
        return out


def pokes_of(inp, key):
    return [tuple(p) for p in inp.get(key, [])]


# ---------------------------------------------------------------------------------------------------- CalcCollisionSteps
def expected_cs(inp, case):
    ped = Mem(SIZES['ped'][0], pokes_of(inp, 'ped'), 'ped', inp.get('addrs'))
    ts = fl(inp['timestep'])
    if ped.rd(0xFC, 4) != 0:
        return finish_cs(ped, 1)
    pdata = ped.rd(0x480, 4)
    speed = [fl(ped.rd(0x44 + 4 * i, 4)) for i in range(3)]
    sq = sumsq(*speed)
    slow = False
    if pdata == 0:
        v = fmul(fmul(sq, ts), ts)
        slow = not (gt(v, f32c(0.09)) or eq(v, f32c(0.09)))     # !(v >= 0.09f): el EXE sale con C0 (menor o no ordenado)
        if slow:
            return finish_cs(ped, 1, None, 'slow')
    d = fmul(fsqrt(sq), ts)
    if pdata == 0:
        steps = ftol_low(fceil(fmul(d, f32c(5.0))))
        mode = 0
    elif ped.rd(0x568, 4) != 0:
        sc = fmul(d, f32c(6.6666665))
        c1 = fceil(sc)
        steps = 4 if lt(c1, f32c(4.0)) else ftol_low(fceil(f32(sc)))
        mode = 1
    else:
        sc = fmul(d, f32c(3.3333333))
        c1 = fceil(sc)
        steps = 2 if lt(c1, f32c(2.0)) else ftol_low(fceil(f32(sc)))
        mode = 2
    if pdata == 0:
        eb = ped.rd(0x9C, 4)
        e = fl(eb)
        # fadd st(0),st(0): un NaN conserva su carga util (silenciado); el resto: 2 * e redondeado
        ped.wr(0x9C, 4, fadd(e, e).to_bits())
    return finish_cs(ped, steps & 0xFF, mode)


def finish_cs(ped, ret, mode=None, why='attached'):
    want = {'ret': ret & 0xFF, 'events': [], 'changes': [[0, c[0], c[1]] for c in ped.changes()]}
    return want, ({'mode%s' % mode: True} if mode is not None else {'early_' + why: True})


# ---------------------------------------------------------------------------------------------------- PreCollisionStuff
def expected_ps(inp, case):
    ped = Mem(SIZES['ped'][0], pokes_of(inp, 'ped'), 'ped', inp.get('addrs'))
    ent = Mem(SIZES['ent'][0], pokes_of(inp, 'ent'), 'ent', inp.get('addrs'))
    mat_ped = Mem(SIZES['mat'][0], pokes_of(inp, 'mat_ped'), 'mat_ped', inp.get('addrs'))
    mat_ent = Mem(SIZES['mat'][0], pokes_of(inp, 'mat_ent'), 'mat_ent', inp.get('addrs'))
    objinfo = Mem(SIZES['minfo'][0], pokes_of(inp, 'objinfo'), 'objinfo', inp.get('addrs'))
    bools = [inp['bools'][i] & 0xFF for i in range(4)]       # [0] bCollisionDisabled, [1] ...CollisionIgnored, [2] ...UnableToMove, [3] ...Stuck
    ev = []
    br = {}
    T_DIS, T_IGN, T_UNABLE, T_STUCK = 0, 1, 2, 3
    # [esp+18h] arg3 = bCollisionDisabled ; [esp+1Ch] arg4 = bCollidedEntityCollisionIgnored ; [esp+20h] arg5 = bCollidedEntityUnableToMove ; [esp+24h] arg6 = bThisOrCollidedEntityStuck
    ignore_check = (inp['ignore_arg'] & 0xFF) != 0           # mov al,[esp+14h] ; test al,al
    this_tok, col_tok = tok(L_PED), tok(L_ENT)

    def B(name):
        br[name] = True

    def climb():
        ev.append([K_CLIMB, L_INTEL])
        return inp['r_climb'] != 0

    def tail():
        if ped.rd(0x598, 4) in (0, 1):
            if climb():
                ped.wr(0x40, 4, ped.rd(0x40, 4) | 0x1000)
                B('tail_climb_flag')
        return finish_ps(ped, ent, ev, bools, br)

    def set_flag_then_tail():
        ped.wr(0x40, 4, ped.rd(0x40, 4) | 0x1000)
        B('flag_set')
        return tail()

    col_type = ent.rd(0x36, 1) & 7
    if col_type == 2:
        f470 = ped.rd(0x470, 4)
        if f470 & 0x80000000 and ped.rd(0x58C, 4) == col_tok:
            bools[T_DIS] = 1
            B('veh_knocked')
            return tail()
    if ped.rd(0x128, 4) == col_tok or ent.rd(0x128, 4) == this_tok:
        bools[T_IGN] = 1
        f470 = ped.rd(0x470, 4)
        B('ignored')
        if (f470 & 0x10) and not (f470 & 0x80000000):
            B('ignored_airborne')
            return tail()
        return set_flag_then_tail()
    my_att = ped.rd(0xFC, 4)
    if my_att == col_tok:
        bools[T_DIS] = 1
        B('attached_to_other')
        return tail()
    other_att = ent.rd(0xFC, 4)
    if other_att == this_tok:
        bools[T_DIS] = 1
        B('other_attached_to_me')
        return tail()
    if my_att != 0 and other_att != 0:
        bools[T_DIS] = 1
        B('both_attached')
        return tail()
    oflags = ent.rd(0x40, 4)
    my_stuck = (ped.rd(0x1C, 4) >> 4) & 1
    other_stuck = (ent.rd(0x1C, 4) >> 4) & 1
    if oflags & 0x20:
        B('move_force')
        if (oflags & 0x4) or (oflags & 0x40000000):
            bools[T_UNABLE] = 1
            B('mf_unable')
            return set_flag_then_tail()
        if ignore_check:
            bools[T_DIS] = 1
            B('mf_ignore')
            return set_flag_then_tail()
        if my_stuck or other_stuck:
            bools[T_STUCK] = 1
            B('mf_stuck')
        return set_flag_then_tail()
    if oflags & 0xC0:
        B('inf_mass')
        if ignore_check:
            bools[T_IGN] = 1
            B('im_ignore')
            return set_flag_then_tail()
        if my_stuck or other_stuck:
            bools[T_STUCK] = 1
            B('im_stuck')
        return set_flag_then_tail()
    if col_type == 4:
        B('object')
        objflags = ent.rd(0x140, 4)
        if objflags & 0x100:
            mat = mat_ent_of(ent, mat_ent)
            if lt(fl(mat.rd(0x28, 4)), f32c(0.66)):
                bools[T_IGN] = 1
                B('lamp_fallen')
                return set_flag_then_tail()
        if ent.rd(0x22, 2) == 0x156:
            omat = mat_ent_of(ent, mat_ent)
            pmat = mat_ped_of(ped, mat_ped)
            if lt(fl(omat.rd(0x38, 4)), fl(pmat.rd(0x38, 4))):
                bools[T_IGN] = 1
                B('model_156_below')
                return set_flag_then_tail()
        info_tok = ent.rd(0x160, 4)
        if info_tok != tok(L_OBJINFO):
            raise Fail('puntero a CObjectData inesperado')
        uproot = fl(objinfo.rd(0x14, 4))
        if gt(uproot, 0.0) or (oflags & 4):
            B('uproot_or_dcf')
            slow = all(lt(fabs(fl(ent.rd(0x44 + 4 * i, 4))), f32c(0.001)) for i in range(3))
            if slow:
                bools[T_UNABLE] = 1
                B('object_still')
                return tail()
        if other_stuck:
            bools[T_UNABLE] = 1
            B('object_stuck')
        return tail()
    B('not_object')
    model = ent.rd(0x22, 2)
    if model in (0x1B9, 0x234, 0x252):
        bools[T_IGN] = 1
        B('model_ignored')
        return set_flag_then_tail()
    if other_stuck:
        bools[T_UNABLE] = 1
        B('other_stuck')
    return tail()


def fabs(a):
    a = Fp.of(a)
    if a.k == 'n':
        return a
    if a.k == 'i':
        return Fp('i', None, 1)
    return Fp('f', abs(a.v), 1)


def mat_ent_of(ent, mat_ent):
    p = ent.rd(0x14, 4)
    if p != tok(L_MATOTHER):
        raise Fail('matriz de la entidad inesperada')
    return mat_ent


def mat_ped_of(ped, mat_ped):
    p = ped.rd(0x14, 4)
    if p != tok(L_MATPED):
        raise Fail('matriz del ped inesperada')
    return mat_ped


def finish_ps(ped, ent, ev, bools, br):
    changes = [[0, c[0], c[1]] for c in ped.changes()] + [[1, c[0], c[1]] for c in ent.changes()]
    want = {'ret': None, 'events': ev, 'changes': changes, 'bools': bools}
    return want, br


# ---------------------------------------------------------------------------------------------------- ProcessControl
class PC:
    """Estado y trazas de una ejecucion modelada de CPed::ProcessControl."""

    def __init__(self, inp):
        self.inp = inp
        self.ped = Mem(SIZES['ped'][0], pokes_of(inp, 'ped'), 'ped', inp.get('addrs'))
        self.pdata = Mem(SIZES['pdata'][0], pokes_of(inp, 'pdata'), 'pdata', inp.get('addrs'))
        self.pdata2 = Mem(SIZES['pdata'][0], pokes_of(inp, 'pdata2'), 'pdata2', inp.get('addrs'))
        self.pped = Mem(SIZES['pped'][0], pokes_of(inp, 'pped'), 'pped', inp.get('addrs'))
        self.ent = Mem(SIZES['ent'][0], pokes_of(inp, 'ent'), 'ent', inp.get('addrs'))
        self.veh = Mem(SIZES['veh'][0], pokes_of(inp, 'veh'), 'veh', inp.get('addrs'))
        self.mats = {L_MATPED: Mem(SIZES['mat'][0], pokes_of(inp, 'mat_ped'), 'mat_ped', inp.get('addrs')), L_MATOTHER: Mem(SIZES['mat'][0], pokes_of(inp, 'mat_veh'), 'mat_veh', inp.get('addrs')),
                     L_MATCAM: Mem(SIZES['mat'][0], pokes_of(inp, 'mat_cam'), 'mat_cam', inp.get('addrs'))}
        self.clump = Mem(SIZES['tok'][0], pokes_of(inp, 'clump'), 'clump', inp.get('addrs'))
        self.ev = []
        self.br = {}
        self.rng = inp['seed'] & M32
        self.rand_calls = 0
        self.muts = [tuple(m) for m in inp['muts']]
        self.frame = inp['frame'] & M32
        self.ts = fl(inp['timestep'])

    def B(self, name):
        self.br[name] = True

    def push(self, kind, *args):
        self.ev.append([kind] + [a & M32 for a in args])

    def mutate(self, stub):
        for s, off, mask in self.muts:
            if s == stub:
                self.ped.wr(off, 4, self.ped.rd(off, 4) ^ mask)
                self.B('mutated')

    def rand(self):
        self.rng = (self.rng * 0x343FD + 0x269EC3) & M32
        self.rand_calls += 1
        return (self.rng >> 16) & 0x7FFF

    def obj_of(self, ptr, expect=None):
        if not is_tok(ptr):
            raise Fail('puntero %#x no simbolico' % ptr)
        label = ptr & 0xFFFFFF
        if expect is not None and label not in expect:
            raise Fail('puntero a la etiqueta %d inesperado' % label)
        return label

    def mem_of(self, ptr):
        label = self.obj_of(ptr)
        return {L_PDATA: self.pdata, L_PDATA2: self.pdata2, L_PPED: self.pped, L_ENT: self.ent, L_VEH: self.veh, L_CLUMP: self.clump}.get(label) or self.mats.get(label)


def expected_pc(inp, case):
    S = PC(inp)
    ped = S.ped
    L_PEDAUDIO, L_PEDWEAPAUDIO = 0x100 + 0x138, 0x100 + 0x394

    # --- 5E8CD9
    S.push(K_AE_SERVICE, L_PEDAUDIO)
    ptype = ped.rd(0x598, 4)

    # --- 5E8CE4..5E8DE6 llama de molotov
    if ptype in (0, 1):
        slot = s8(ped.rd(0x718, 1))
        woff = 0x5A0 + slot * 0x1C
        if ped.rd(woff, 4) == 0x12:
            S.B('molotov')
            show = not ((ped.rd(0x474, 4) >> 1) & 1)
            if show:
                show = bool((ped.rd(0x1C, 4) >> 7) & 1)
            if show and (ped.rd(0x40, 4) >> 8) & 1:
                S.push(K_SWIM, L_INTEL)
                if inp['r_swim'] != 0:
                    show = False
                    S.B('molotov_swim')
            if show and ped.rd(0x4F4, 4) == 0:
                show = False
            fx = ped.rd(woff + 0x18, 4)
            if show:
                if fx == 0:
                    S.B('molotov_create')
                    S.push(K_GETHIER, L_CLUMP)
                    S.push(K_BONEIDX, L_HIER, 0x18)
                    S.push(K_MATARR, L_HIER)
                    idx = inp['r_bone'] & M32
                    S.push(K_CREATEFX, 0xA9AE80, FX_NAME_HASH, 0, 0, 0, (idx * 0x40) & M32, 0)
                    newfx = tok(L_FX) if inp['r_fx'] else 0
                    ped.wr(woff + 0x18, 4, newfx)
                    if newfx:
                        S.push(K_FXLOCAL, L_FX, 1)
                        S.push(K_FXCOPY, L_FX)
                        S.push(K_FXPLAY, L_FX)
                        S.B('molotov_fx_ok')
                    else:
                        S.B('molotov_fx_null')
                else:
                    S.B('molotov_keep')
            elif fx != 0:
                S.B('molotov_destroy')
                S.push(K_FXDESTROY, 0xA9AE80, L_FX)
                ped.wr(woff + 0x18, 4, 0)
    # --- 5E8DE6
    if ped.rd(0x478, 4) & 0x200000:
        S.B('replay_exit')
        return finish_pc(S)
    # --- 5E8DF6
    if (ped.rd(0x474, 4) >> 12) & 1:
        S.B('attack_wave')
        if inp['area'] != 0:
            S.B('attack_wave_area_exit')
            return finish_pc(S)
        S.push(K_COORS, 0xFFFFFFFF)
        if gt(fl(inp['pl_coors'][2]), f32c(950.0)):
            S.B('attack_wave_high_exit')
            return finish_pc(S)
    # --- 5E8E31
    if ((S.frame + (ped.rd(0x20, 1))) & 0x1F) == 0:
        S.B('prune')
        S.push(K_PRUNE, L_PED)
        S.mutate(S_PRUNE)
    # --- 5E8E49
    S.push(K_GETALPHA, L_CLUMP)
    alpha = s32(inp['r_alpha'])
    if (ped.rd(0x470, 4) >> 3) & 1:
        alpha = s32(alpha - 8)          # sub eax,8 ; jns: SF del resultado de 32 bits (INT_MIN - 8 da positivo)
        S.B('fade_out')
        if alpha < 0:
            alpha = 0
            S.B('fade_floor')
    elif alpha < 255:
        alpha = s32(alpha + 16)
        if alpha > 255:
            alpha = 255
            S.B('fade_in_cap')
        S.B('fade_in')
    S.push(K_SETALPHA, L_CLUMP, alpha)
    # --- 5E8E89
    flagsB = ped.rd(0x470, 4)
    if flagsB & 0x80000000:
        S.B('knocked_off')
        c1 = (ped.rd(0x46C, 4) & 1) or (flagsB & 0x80000)
        if c1 and ped.rd(0x568, 4) == 0 and not (flagsB & 0x200000) and ped.rd(0x588, 4) == 0x47C34FFF:
            S.B('knocked_off_cond')
            sp = [fl(ped.rd(0x44 + 4 * i, 4)) for i in range(3)]
            if lt(sumsq(*sp), f32c(0.01)):
                S.B('knocked_off_slow')
                veh = ped.rd(0x58C, 4)
                if veh != 0:
                    if S.obj_of(veh, (L_VEH,)) != L_VEH:
                        raise Fail('vehiculo')
                    vmodel = s16(S.veh.rd(0x22, 2))
                    pmodel = s16(ped.rd(0x22, 2))
                    mi = {int(i): (int(a), int(b)) for i, a, b in inp['mi']}
                    if vmodel not in mi or pmodel not in mi:
                        raise Fail('modelo fuera de la tabla del banco')
                    vcm = mi[vmodel][1]
                    pcm = mi[pmodel][1]
                    mveh = S.veh.rd(0x14, 4)
                    mped = ped.rd(0x14, 4)
                    S.push(K_COLMODELS, S.obj_of(mped), pcm, S.obj_of(mveh), vcm, 0xC092A8, 0, 0, 0)
                    if inp['r_col'] == 0:
                        S.B('knocked_off_cleared')
                        ped.wr(0x470, 4, ped.rd(0x470, 4) & 0x7FFFFFFF)
                        ped.wr(0x128, 4, 0)
                    else:
                        S.B('knocked_off_colliding')
                else:
                    S.B('knocked_off_noveh')
                    ped.wr(0x470, 4, flagsB & 0x7FFFFFFF)
    # --- 5E8F54
    now = ped.rd(0x470, 4)
    if (now & 0x200000) and (ped.rd(0x478, 4) >> 8) & 1:
        S.B('head_check')
        pos = ped_pos(S)
        if lt(fadd(pos[2], f32c(1.5)), fl(ped.rd(0x588, 4))):
            S.B('head_cleared')
            ped.wr(0x470, 4, now & 0xFFDFFFFF)
    # --- 5E8F9E
    p46C, p470, pphys, p474, p478 = ped.rd(0x46C, 4), ped.rd(0x470, 4), ped.rd(0x40, 4), ped.rd(0x474, 4), ped.rd(0x478, 4)
    clean470 = p470 & 0xFFF7FFFF
    ped.wr(0x46C, 4, p46C & 0xFFFEFFFD)
    ped.wr(0x470, 4, clean470)
    ped.wr(0x40, 4, pphys & 0xFFFFFEFF)
    ped.wr(0x474, 4, p474 & 0x7FFFFFFE)
    ped.wr(0x478, 4, p478 & 0xFFEFBEFF)
    ped.wr(0x588, 4, 0x47C34FFF)
    # --- 5E9007 / 5E9054
    for aoff, poff in ((0x504, 0x506), (0x508, 0x50A)):
        alpha_v = s16(ped.rd(aoff, 2))
        if alpha_v > 0:
            S.B('gunflash')
            prog = s16(ped.rd(poff, 2))
            ms = ftol_low(fmul(fmul(S.ts, f32c(0.02)), f32c(1000.0)))
            limit = (prog * ms) & M32
            if (alpha_v & M32) > limit:
                S.B('gunflash_sub')
                ped.wr(aoff, 2, (ped.rd(aoff, 2) - (limit & 0xFFFF)) & 0xFFFF)
            else:
                S.B('gunflash_zero')
                ped.wr(aoff, 2, 0)
    # --- 5E90AB
    if not (ped.rd(0x46C, 4) & 1):
        S.B('not_standing_clear')
        ped.wr(0x470, 4, clean470 & 0xFFDFFFFF)
    # --- 5E90C3
    if ped.rd(0x484, 1) == 2:
        S.B('mission')
        S.push(K_FINDPLAYER, 0xFFFFFFFF)
        mode = inp['r_find_player']
        player = {0: 0, 1: tok(L_PED), 2: tok(L_PPED)}[mode]
        if player != tok(L_PED):
            S.push(K_FINDPLAYER, 0xFFFFFFFF)
            if player == 0:
                raise Fail('FindPlayerPed nulo: el EXE desreferencia el resultado')
            pd = S.pped.rd(0x480, 4)
            if pd != tok(L_PDATA2):
                raise Fail('datos del otro jugador')
            group = S.pdata2.rd(0x38, 4)
            S.push(K_ISMEMBER, (group * 0x2D4) & M32, L_PED)
            if inp['r_is_member']:
                S.B('mission_member')
                ped.wr(0x478, 4, ped.rd(0x478, 4) & 0xFFFFFEFF)
    # --- 5E910D
    S.push(K_BUOY, L_PED)
    S.mutate(S_BUOY)
    # --- 5E9114
    pd = ped.rd(0x480, 4)
    if pd != 0:
        S.B('player_data')
        if (ped.rd(0x40, 4) >> 8) & 1 and S.pdata.rd(0x8E, 1) > 0x32:
            wet = s8(S.pdata.rd(0x8C, 1))
            if wet < 100:
                S.B('wet_inc')
                S.pdata.wr(0x8C, 1, (wet + 1) & 0xFF)
        else:
            wet = s8(S.pdata.rd(0x8C, 1))
            if wet > 0:
                S.B('wet_dec')
                S.pdata.wr(0x8C, 1, (wet - 1) & 0xFF)
    # --- 5E9152
    if ped.rd(0x46C, 4) & 1:
        if inp['force'] & 0xFF == 0:
            stand = ped.rd(0x568, 4)
            if stand != 0:
                S.B('standing_on')
                if (S.ent.rd(0x1C, 4) >> 5) & 1:
                    S.B('postponed_safe')
                    ped.wr(0x1C, 4, ped.rd(0x1C, 4) | 0x40)
                    return finish_pc(S)
                if (S.ent.rd(0x36, 1) & 7) == 2 and S.ent.rd(0x594, 4) == 6:
                    S.B('postponed_train')
                    ped.wr(0x1C, 4, ped.rd(0x1C, 4) | 0x40)
                    return finish_pc(S)
    # --- 5E919E
    S.push(K_PFIRST, L_INTEL)
    S.mutate(S_PFIRST)
    standing = ped.rd(0x46C, 4) & 1
    if not standing:
        sz = fl(ped.rd(0x4C, 4))
        if gt(sz, 0.25):
            S.B('fall_speed')
            if ped.rd(0x480, 4) != 0:
                S.B('fall_player')
                ped.wr(0x4C, 4, 0x3E800000)
            else:
                S.B('fall_decay')
                decay_speed(S, ped)
    # --- 5E920D
    t2 = ped.rd(0x598, 4)
    skip = False
    if t2 not in (0, 1) and standing:
        c = all(eq(fl(ped.rd(0x44 + 4 * i, 4)), 0.0) for i in range(3))
        if c:
            ms_ = ped.rd(0x534, 4)
            if ms_ in (0, 1):
                if eq(fl(ped.rd(0x4D8, 4)), 0.0) and eq(fl(ped.rd(0x4DC, 4)), 0.0):
                    if ped.rd(0x530, 4) != 0x29 and not ((ped.rd(0x46C, 4) >> 9) & 1) and ped.rd(0x568, 4) == 0:
                        skip = True
    if skip:
        S.B('skip_physics')
        S.push(K_SKIP, L_PED)
        S.mutate(S_SKIP)
    else:
        S.push(K_PHYSPC, L_PED)
        S.mutate(S_PHYSPC)
    # --- 5E92BF
    S.push(K_DELW, L_PED)
    S.mutate(S_DELW)
    S.push(K_FOOT, L_PED)
    S.mutate(S_FOOT)
    ped.wr(0x474, 4, ped.rd(0x474, 4) & 0xFFEF7FFF)
    S.push(K_PPROC, L_INTEL)
    S.mutate(S_PPROC)
    if ped.rd(0x530, 4) != 0x37:
        S.push(K_NEWVEL, L_PED)
        S.mutate(S_NEWVEL)
    S.push(K_UPDPOS, L_PED)
    S.mutate(S_UPDPOS)
    S.push(K_MOVEANIM, L_PED)
    S.mutate(S_MOVEANIM)
    # --- 5E9300
    ped.wr(0x474, 4, ped.rd(0x474, 4) & 0xFFF0FFFF)
    if (ped.rd(0x46C, 4) & 0x40000) or ped.rd(0x72F, 1) != 0:
        S.B('bleed_gate')
        S.push(K_BLOOD)
        if inp['r_blood'] & 0xFF:
            if not (ped.rd(0x46C, 4) & 0x100):
                S.B('bleed_outside')
                if ped.rd(0x72F, 1) != 0:
                    ped.wr(0x72F, 1, ped.rd(0x72F, 1) - 1)
                if (S.frame & 3) == 0:
                    S.B('bleed_frame')
                    cam = camera_pos(S)
                    pos = ped_pos(S)
                    dx, dy, dz = (f32(fsub(pos[i], cam[i])) for i in range(3))
                    if lt(sumsq(dx, dy, dz), f32c(2500.0)):
                        S.B('bleed_near')
                        r1, r2, r3 = S.rand(), S.rand(), S.rand()
                        mp = S.mats[L_MATPED]
                        if ped.rd(0x14, 4) != tok(L_MATPED):
                            raise Fail('gota de sangre sin matriz')
                        mpos = [fl(mp.rd(0x30 + 4 * i, 4)) for i in range(3)]
                        size = fadd(fmul(Fp.of(r1 & 0x7F), fl(0x3AC49BA6)), fl(0x3E19999A))
                        x = fadd(fmul(Fp.of((r2 & 0x7F) - 0x40), fl(0x3BE56042)), mpos[0])
                        y = fadd(fmul(Fp.of((r3 & 0x7F) - 0x40), fl(0x3BE56042)), mpos[1])
                        z = fadd(mpos[2], f32c(1.0))
                        r4 = S.rand()
                        sb = size.to_bits()
                        S.push(K_SHADOW, 1, L_TEX, x.to_bits(), y.to_bits(), z.to_bits(), sb, 0, 0, sb ^ 0x80000000, 0xFF, 0xC8, 0, 0, FBITS_4_0, (r4 & 0xFFF) + 0x7D0, FBITS_1_0)
        else:
            S.B('bleed_noblood')
    # --- 5E9473
    f46 = ped.rd(0x46C, 4)
    if f46 & 0x100:
        if ped.rd(0x58C, 4) != 0:
            S.B('pedcount_vehicle')
            S.push(K_PEDCOUNT, L_PED, 1)
        else:
            S.B('pedcount_clear_invehicle')
            ped.wr(0x46C, 4, f46 & 0xFFFFFEFF)
    else:
        S.push(K_PEDCOUNT, L_PED, 0)
    # --- 5E94A3
    if (((ped.rd(0x20, 2)) + S.frame) & 0x3FFF) == 0 and (ped.rd(0x478, 4) >> 10) & 1:
        S.B('say')
        S.push(K_SAY, L_PED, 0x51, 0, 0x3F800000, 0, 0, 0)
    # --- 5E94DA
    slot = s8(ped.rd(0x718, 1))
    if ped.rd(0x5A0 + slot * 0x1C, 4) == 9 and ped.rd(0x530, 4) != 0x10 and not (ped.rd(0x46C, 4) & 0x100):
        S.push(K_SWIM, L_INTEL)
        if inp['r_swim'] == 0:
            S.B('chainsaw')
            S.push(K_WAE_ADD, L_PEDWEAPAUDIO, 0x99)
    S.push(K_WAE_SERVICE, L_PEDWEAPAUDIO)
    return finish_pc(S)


FBITS_4_0 = 0x40800000
FBITS_1_0 = 0x3F800000


def ped_pos(S):
    ped = S.ped
    m = ped.rd(0x14, 4)
    if m == 0:
        return [fl(ped.rd(0x04 + 4 * i, 4)) for i in range(3)]
    if m != tok(L_MATPED):
        raise Fail('matriz del ped inesperada')
    mp = S.mats[L_MATPED]
    return [fl(mp.rd(0x30 + 4 * i, 4)) for i in range(3)]


def camera_pos(S):
    cm = S.inp['cam_mat']
    if cm == 0:
        return [fl(S.inp['cam_pos'][i]) for i in range(3)]
    mc = S.mats[L_MATCAM]
    return [fl(mc.rd(0x30 + 4 * i, 4)) for i in range(3)]


def decay_factor(ts):
    """float(pow(0.95f, paso)) (fstp dword del resultado de _CIpow). El pow de Python no es el del CRT del EXE al ulp: el banco compara con tolerancia de 2 ulp."""
    if ts.k == 'n':
        return ts
    if ts.k == 'i':
        return Fp.bits(0x7F800000) if ts.s < 0 else Fp.bits(0)
    try:
        pw = math.pow(float(f32c(0.95).v), float(ts.v))
    except (OverflowError, ValueError):
        pw = math.inf
    if pw != pw:
        return _nan()
    try:
        return Fp.bits(struct.unpack('<I', struct.pack('<f', pw))[0])
    except OverflowError:
        return Fp.bits(0x7F800000)


def decay_speed(S, ped):
    """0x5E91D7..0x5E920A: factor = float(pow(0.95f (qword 0x86C3C8), paso)) y v *= factor (cada producto a float, 40FEC0)."""
    factor = decay_factor(S.ts)
    for i in range(3):
        v = fl(ped.rd(0x44 + 4 * i, 4))
        ped.wr(0x44 + 4 * i, 4, fmul(v, factor).to_bits())


def finish_pc(S):
    ped = S.ped
    skip = set(range(0x44, 0x50))
    changes = [[0, c[0], c[1]] for c in ped.changes(skip)] + [[1, c[0], c[1]] for c in S.pdata.changes()]
    speed = [ped.rd(0x44 + 4 * i, 4) for i in range(3)]
    want = {'ret': None, 'events': S.ev, 'changes': changes, 'speed': speed, 'rand_calls': S.rand_calls}
    for k in ('ent', 'veh', 'pped', 'pdata2'):
        if getattr(S, k).changes():
            raise Fail('el oraculo modifico un objeto que no debia: ' + k)
    return want, S.br


def expected(function, inp, case):
    if function == NAME_PC:
        want, br = expected_pc(inp, case)
        stats = dict(br)
        stats['family%d' % inp['family']] = True
        stats['events_ge10'] = len(want['events']) >= 10
        stats['events_ge20'] = len(want['events']) >= 20
        return want, stats
    if function == NAME_PS:
        want, br = expected_ps(inp, case)
        stats = dict(br)
        stats['family%d' % inp['family']] = True
        stats['ig_garbage'] = (inp['ignore_arg'] & 0xFF) == 0 and inp['ignore_arg'] != 0
        return want, stats
    if function == NAME_CS:
        want, br = expected_cs(inp, case)
        stats = dict(br)
        stats['family%d' % inp['family']] = True
        stats['ret_ge2'] = want['ret'] >= 2
        return want, stats
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    print('ok (la autoprueba funcional se hace en fixture_mirror.py contra las fixtures reales)')
