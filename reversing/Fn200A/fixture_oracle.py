"""Oraculo independiente (Python) de Fn200A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 31 funciones del lote 200A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen
inicial y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico; dumpbin /disasm).
   0  0x5B2AB0  alias del constructor de CBoundingBox: push esi; esi = ecx; call 0x40FAF0; eax = esi; pop esi; ret
   1  0x67BA70  cuerpo del destructor de CTaskComplexStuckInAir: [ecx] = 870608h; jmp 0x61A3D0 (cuerpo de ~CTaskComplex: vtable 86D4CCh; si [this+8]: delete virtual (push 1; call [[sub]]); [this+8] = 0; vtable 86D48Ch)
   2  0x67C0C0  CTaskSimpleGetUp::Clone: 0x61A5A0(10h); si no es nulo: base 0x61A390 ([+4] = 0; vtable 86D4A8h); byte [+8] = byte [+9] = 0; dword [+0Ch] = 0; vtable 8703E0h; devuelve el objeto o nulo
   3  0x67C130  CTaskComplexGetUpAndStandStill::Clone: 0x61A5A0(0Ch); base 0x61A3B0 ([+4] = 0, [+8] = 0; vtable 86D4CCh); vtable 870404h
   4  0x67C180  CTaskSimpleFall::Clone: 0x61A5A0(20h); lee grupo [this+10h], tiempo total [this+18h] y animacion [this+0Ch]; base 0x61A390; [+18h] = [+1Ch] = tiempo total; [+10h] = grupo; vtable 870430h; byte [+8] = 0; [+0Ch] = animacion; [+14h] = 0
   5  0x67C200  CTaskComplexFallAndGetUp::Clone: 0x61A5A0(18h); lee [this+14h], [this+10h], [this+0Ch]; base 0x61A3B0; [+10h], [+14h]; vtable 870454h; [+0Ch]
   6  0x67C2E0  CTaskComplexInAirAndLand::Clone: 0x61A5A0(10h); bytes [this+0Dh] y [this+0Ch] normalizados a 0/1 (test; setne); base 0x61A3B0; byte [+0Ch], byte [+0Dh]; vtable 8704ACh; byte [+0Eh] = 0
   7  0x67C360  CTaskSimpleInAir::Clone: 0x61A5A0(3Ch); dl = byte [this+24h]; constructor 0x678CD0(dl & 1, (dl >> 1) & 1, (dl >> 2) & 1)
            0x678CD0: base 0x61A390; [+1Ch] = [+20h] = [+28h] = 0; byte [+24h] = (viejo & F8h) | (arg3 << 2) | (arg2 << 1) | arg1 (cada uno & 1); vtable 8704D8h; [+2Ch] = [0B7CB84h]; [+30h] = 1F4h; byte [+34h] = 1; byte [+35h] = 0; dword [+38h] = 0
   8  0x67C3F0  CTaskSimpleLand::Clone: 0x61A5A0(14h); anim = [this+0Ch]; base 0x61A390; byte [+10h] = (viejo & FCh) | (anim == -1 ? 6 : 4); [+0Ch] = anim; vtable 8704FCh; dword [+8] = 0
   9  0x67C480  CTaskComplexFallToDeath::Clone: 0x61A5A0(24h); constructor 0x679040(byte [this+20h], &[this+0Ch], (byte [this+21h] >> 3) & 1, (byte [this+21h] >> 4) & 1)
            0x679040: base 0x61A3B0; vtable 870520h; [+0Ch..+14h] = los tres dwords de la posicion; [+18h] = [+1Ch] = FFFFFFFFh; byte [+20h] = direccion; byte [+21h] = ((a4 & 1) << 1 | (a3 & 1)) << 3 | (viejo & E0h)
  10  0x67C750  CTaskSimpleGetUp destructor con borrado(banderas): 0x677F80 (cuerpo); con el bit 0 del byte de banderas: 0x61A5B0(this); devuelve this; ret 4
  11  0x6F6300  CTrain destructor con borrado(banderas): 0x6F54D0 (cuerpo); con el bit 0: 0x6E2D90(this) (pool de vehiculos [0B74494h]: idx = (this - [pool]) / 0A18h (imul por 6572EC3h; sar 6; + signo); byte [[pool+4] + idx] |= 80h;
            si idx < [pool+0Ch] (con signo): [pool+0Ch] = idx); devuelve this; ret 4
  12  0x6F5930  GetDooorAngleOpenRatioU32(codigo): fld 0.0f; si codigo - 8 (sin signo) > 10 -> 0.0f; tabla de bytes {0, 1, 2, 3, 5, 5, 5, 5, 5, 5, 4}: 8 -> puerta 3, 9 -> 5, 10 -> 2, 11 -> 4 (fstp st0; [esp+4] = puerta; jmp [[this]+78h]);
            el resto (12..17 y 18) -> 0.0f en ST0
  13  0x6F59C0  GetDooorAngleOpenRatio(puerta): ecx = this + 5D8h + 18h * puerta (dword sin comprobar); call 0x6F47E0 (CDoor::GetAngleOpenRatio, resultado en ST0); ret 4
  14  0x6F5EE0  IsRoomForPedToLeaveCar: mov al,1; ret 8
  15  0x6F6370  OpenDoor(ped, componente, puerta, razon, sonido): puerta 2 -> componente 1; puerta 3 -> componente 2; otra: no hace nada; marco = [this + 668h + 4 * componente]; nulo: nada;
            CMatrix temporal ligada a marco + 10h (0x59C050, 0x59BD10: copia 12 dwords); posicion guardada = (+30h, +34h, +38h) de la CMatrix; puerta = this + 5D8h + 18h * puerta (+0 abierta, +4 cerrada, +0Ah eje (byte), +0Ch angulo);
            cerrada = 0x6F4800 (angulo == cerrada, con NaN falso); si cerrada: RwFrameForAllObjects(marco, 0x4C7BB0, 400h); 0x6F4790(razon) (Open); si estaba cerrada y ahora cerrada != angulo (fld; fcomp; test ah,44h; jnp): RwFrameForAllObjects(marco, 0x4C7B90, 400h)
            y, con ped, CEventSoundQuiet(ped, 60.0f, -1, (0,0,0)) (0x5E05B0) + 0x4ABA50 + 0x4AB420(evento, 0) + 0x5DEA00; si NO estaba cerrada y razon == 0.0f (fcomp; test ah,44h; jp salta): lo mismo con 120.0f; modelo (word +22h) == 23Ah (570):
            y = ((abierta - cerrada) * razon) + cerrada (x87 sin redondeos intermedios a float) y 0x59AF40(xGuardada, y, zGuardada); si no: orientacion (0,0,0) con orientacion[eje] = angulo (fld; fstp), 0x59B120(ox, oy, oz), y posicion += guardada (fld; fadd; fstp);
            siempre 0x59BBB0 (UpdateRW: copia 12 dwords a la RwMatrix del marco y llama a 0x7F18A0) y 0x59ACD0 (destructor); ret 14h
  16  0x6F7190  Teleport(x, y, z, orientacion): 0x563280(this); si [this+14h]: posicion en [matriz+30h..38h]; si no: [this+4], [this+8], [this+0Ch]; si el byte bajo de orientacion: 0x439A80(0, 0, 0) (ecx = this); [+44h..+4Ch] = 0; [+50h..+58h] = 0; 0x563220(this); ret 10h
  17  0x411A30  operator/=(d): r = 1.0 / d (fld 1.0; fdiv: queda en el registro, 24 bits); x = r * x (fld st0; fmul; fstp), y = r * y, z = r * z (la ultima consume el registro); eax = this; ret 4
  18  0x411BA0  lea eax,[ecx+10h]; ret
  19  0x411BF0  mov al,[esp+4]; mov [ecx+20h],al; ret 4
  20  0x43E1A0  jmp 0x552400
  21  0x54F440  eax = ecx; 13 dwords a cero en el orden +40h, +44h, +94h, +98h, +E8h, +ECh, +13Ch, +140h, +190h, +194h, +1E4h, +1E8h, +1F8h; ret (eax = this)
  22  0x54F490  cuerpo del destructor de CPlaceable: [this] = 863C40h; si [this+14h] != 0: 0x54F3B0 (RemoveMatrix, ecx = this); dec [0B74238h]; [this+14h] = 0B74240h
  23  0x54F5D0  destructor con borrado: igual; con el bit 0 del byte de banderas: 0x8214BD(this); devuelve this; ret 4
  24-30 0x54FA10, 0x54FA60, 0x54FAB0, 0x54FB00, 0x54FB50, 0x54FBA0, 0x54FBF0: si [this+8] (con signo) > 0: si byte [this+10h] != 0: 0x8213AE([this]); 0x8213AE([this+4]); [this] = [this+4] = [this+8] = [this+0Ch] = 0

Los falsos anotan: operator new [0x61A5A0, tamano]; operator delete [0x61A5B0 / 0x8214BD, puntero (etiqueta), dword de [puntero]]; delete[] [0x8213AE, puntero]; cuerpos y constructores falsos [direccion, ECX (etiqueta), dword de [ECX]];
RemoveMatrix / Flush [direccion, ECX]; ranura 30 falsa [7E1EA5F1, ECX, puerta]; GetAngleOpenRatio [6F47E0, ECX]; CDoor::Open [6F4790, ECX, razon]; RwFrameForAllObjects [7F1200, marco, funcion (normalizada), dato];
SetTranslate / SetRotate [direccion, x, y, z]; RwMatrixUpdate [7F18A0, matriz]; RegisterReference / CleanUpOldReference [571B70 / 571A00, ped, destino]; GetEventGlobalGroup [4ABA50]; CEventGroup::Add [4AB420, grupo, evento, bandera,
entidad, nivel, tiempo, x, y, z]; CWorld::Remove / Add [563280 / 563220, entidad]; SetOrientation [439A80, ECX, x, y, z]; las ranuras virtuales falsas [7E1EA5E0 destructor: ECX, banderas]. Las vtables que escriben
constructores y destructores se omiten de los cambios y se anotan como clase ("vt": la del EXE o una de la DLL).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = json.loads(r'''["CBoundingBox::ConstructorAlias5B2AB0", "CTaskComplexStuckInAir::DestructorBody", "CTaskSimpleGetUp::Clone", "CTaskComplexGetUpAndStandStill::Clone", "CTaskSimpleFall::Clone", "CTaskComplexFallAndGetUp::Clone", "CTaskComplexInAirAndLand::Clone", "CTaskSimpleInAir::Clone", "CTaskSimpleLand::Clone", "CTaskComplexFallToDeath::Clone", "CTaskSimpleGetUp::ScalarDeletingDestructor", "CTrain::ScalarDeletingDestructor", "CTrain::GetDooorAngleOpenRatioU32", "CTrain::GetDooorAngleOpenRatio", "CTrain::IsRoomForPedToLeaveCar", "CTrain::OpenDoor", "CTrain::Teleport", "CVector::DivideAssignExe", "CColPoint::GetNormalAddress", "CColPoint::SetSurfaceTypeA", "CPtrNodeSingleLinkPool::FlushJump", "CMatrixLinkList::Constructor", "CPlaceable::DestructorBody", "CPlaceable::ScalarDeletingDestructor", "CPlaceable::ArrayPairReset54FA10", "CPlaceable::ArrayPairReset54FA60", "CPlaceable::ArrayPairReset54FAB0", "CPlaceable::ArrayPairReset54FB00", "CPlaceable::ArrayPairReset54FB50", "CPlaceable::ArrayPairReset54FBA0", "CPlaceable::ArrayPairReset54FBF0"]''')
ENTRIES = (0x5B2AB0, 0x67BA70, 0x67C0C0, 0x67C130, 0x67C180, 0x67C200, 0x67C2E0, 0x67C360, 0x67C3F0, 0x67C480, 0x67C750, 0x6F6300, 0x6F5930, 0x6F59C0, 0x6F5EE0, 0x6F6370,
           0x6F7190, 0x411A30, 0x411BA0, 0x411BF0, 0x43E1A0, 0x54F440, 0x54F490, 0x54F5D0, 0x54FA10, 0x54FA60, 0x54FAB0, 0x54FB00, 0x54FB50, 0x54FBA0, 0x54FBF0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(BBOX, COMPLEXDTOR, CLONEGETUP, CLONESTAND, CLONEFALL, CLONEFALLGETUP, CLONEAIRLAND, CLONEINAIR, CLONELAND, CLONEDEATH, TASKSDD, TRAINSDD, DOORU32, DOORRATIO, ISROOM, OPENDOOR, TELEPORT, DIVIDE,
 NORMALADDR, SETSURFACE, FLUSHJUMP, MLLCTOR, PLDTOR, PLSDD, ARRRESET) = \
    ('bbox complexdtor clonegetup clonestand clonefall clonefallgetup cloneairland cloneinair cloneland clonedeath tasksdd trainsdd dooru32 doorratio isroom opendoor teleport divide '
     'normaladdr setsurface flushjump mllctor pldtor plsdd arrreset').split()
KIND = [BBOX, COMPLEXDTOR, CLONEGETUP, CLONESTAND, CLONEFALL, CLONEFALLGETUP, CLONEAIRLAND, CLONEINAIR, CLONELAND, CLONEDEATH, TASKSDD, TRAINSDD, DOORU32, DOORRATIO, ISROOM, OPENDOOR, TELEPORT, DIVIDE,
        NORMALADDR, SETSURFACE, FLUSHJUMP, MLLCTOR, PLDTOR, PLSDD] + [ARRRESET] * 7
CLOCK, NUMMATRICES, VEHPOOL = 0xB7CB84, 0xB74238, 0xB74494
NEWADDR, DELADDR, DELSCALAR, DELARR = 0x61A5A0, 0x61A5B0, 0x8214BD, 0x8213AE
EV_VDTOR = 0x7E1EA5E0
EV_SLOT30 = 0x7E1EA5F1
STACK_TAG = 0x7F000000
CLONE_SIZE = {CLONEGETUP: 0x10, CLONESTAND: 0xC, CLONEFALL: 0x20, CLONEFALLGETUP: 0x18, CLONEAIRLAND: 0x10, CLONEINAIR: 0x3C, CLONELAND: 0x14, CLONEDEATH: 0x24}
VEHICLE_SIZE_MAGIC = 0x6572EC3


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


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    """un float que pasa por la x87 (fld / fstp): un NaN senalizante sale silencioso"""
    return (b | 0x400000) & M32 if is_nan(b) else b & M32


def neg_x87(b):
    """fld; fchs; fstp: un NaN sale silencioso y con el signo cambiado; el resto cambia el signo"""
    return (quiet(b) ^ 0x80000000) & M32


def dec(b):
    s, e, f = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
    return (-1 if s else 1) * (f if e == 0 else (f | 0x800000)), (-149 if e == 0 else e - 150)


def round_sig(x, bits):
    """redondea la fraccion x a `bits` bits de mantisa (al par, exponente sin limite)"""
    if x == 0:
        return x
    sign = -1 if x < 0 else 1
    mag = abs(x)
    e = mag.numerator.bit_length() - mag.denominator.bit_length()
    if Fraction(2) ** e > mag:
        e -= 1
    step = Fraction(2) ** (e - bits + 1)
    q = mag / step
    fl = q.numerator // q.denominator
    rem = q - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl & 1):
        fl += 1
    return sign * fl * step


def to_float_bits(x, neg_zero=False):
    """fstp dword: fraccion exacta a float (al par, con denormales y desbordamiento a infinito)"""
    if x == 0:
        return 0x80000000 if neg_zero else 0
    sign = 0x80000000 if x < 0 else 0
    mag = abs(x)
    e = mag.numerator.bit_length() - mag.denominator.bit_length()
    if Fraction(2) ** e > mag:
        e -= 1
    step = Fraction(2) ** max(e - 23, -149)
    q = mag / step
    fl = q.numerator // q.denominator
    rem = q - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl & 1):
        fl += 1
    val = fl * step
    if val >= Fraction(2) ** 128:
        return sign | 0x7F800000
    # reconstruir los bits
    if val < Fraction(2) ** -126:
        return sign | int(val / Fraction(2) ** -149)
    e2 = val.numerator.bit_length() - val.denominator.bit_length()
    if Fraction(2) ** e2 > val:
        e2 -= 1
    frac = int((val / Fraction(2) ** e2 - 1) * (1 << 23))
    return sign | ((e2 + 127) << 23) | frac




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

    def changes(self, skip=()):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - GUARD
            if self.m[i] != ref and not any(s <= rel < s + 4 for s in skip):
                out.append([rel, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out




# ------------------------------------------------------------------------------------------------ aritmetica x87 de 24 bits (exponente ampliado), valores exactos
# un valor es ('n', Fraction, negativo_si_cero) | ('i', signo) | ('q', bits_de_NaN_silencioso)
DEFAULT_NAN = 0xFFC00000


def ld(bits):
    bits &= M32
    if is_nan(bits):
        return ('q', quiet(bits))
    if (bits & 0x7FFFFFFF) == 0x7F800000:
        return ('i', bits >> 31)
    m, e = dec(bits)
    return ('n', Fraction(m) * Fraction(2) ** e, bool(bits >> 31) and m == 0)


def _nan_of(a, b):
    na, nb = a[0] == 'q', b[0] == 'q'
    if na and nb:
        if a[1] == b[1]:
            return a                                  # el mismo NaN (p. ej. la misma entrada cargada dos veces)
        ma, mb = a[1] & 0x7FFFFF, b[1] & 0x7FFFFF
        if ma == mb:
            raise ValueError('dos NaN')
        return a if ma > mb else b                    # x87: gana el de mayor significando
    return a if na else (b if nb else None)


def _isneg(v):
    return v[2] if v[1] == 0 else v[1] < 0


def fadd(a, b):
    n = _nan_of(a, b)
    if n:
        return n
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else ('q', DEFAULT_NAN)
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('n', Fraction(0), a[1] == 0 and b[1] == 0 and a[2] and b[2])
    return ('n', round_sig(s, 24), False)


def fneg(a):
    if a[0] == 'q':
        return a
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('n', -a[1], (not a[2]) if a[1] == 0 else False)


def fsub(a, b):
    n = _nan_of(a, b)
    return n if n else fadd(a, fneg(b))


def fmul(a, b):
    n = _nan_of(a, b)
    if n:
        return n
    neg = _isneg(a) != _isneg(b) if a[0] != 'i' and b[0] != 'i' else None
    if a[0] == 'i' or b[0] == 'i':
        other = b if a[0] == 'i' else a
        if other[0] == 'n' and other[1] == 0:
            return ('q', DEFAULT_NAN)
        sign = (a[1] if a[0] == 'i' else (1 if _isneg(a) else 0)) ^ (b[1] if b[0] == 'i' else (1 if _isneg(b) else 0))
        return ('i', sign)
    p = a[1] * b[1]
    if p == 0:
        return ('n', Fraction(0), neg)
    return ('n', round_sig(p, 24), False)


def fdiv(a, b):
    n = _nan_of(a, b)
    if n:
        return n
    sa = (a[1] if a[0] == 'i' else (1 if _isneg(a) else 0))
    sb = (b[1] if b[0] == 'i' else (1 if _isneg(b) else 0))
    if a[0] == 'i' and b[0] == 'i':
        return ('q', DEFAULT_NAN)
    if a[0] == 'i':
        return ('i', sa ^ sb)
    if b[0] == 'i':
        return ('n', Fraction(0), bool(sa ^ sb))
    if b[1] == 0:
        if a[1] == 0:
            return ('q', DEFAULT_NAN)
        return ('i', sa ^ sb)
    q = a[1] / b[1]
    if q == 0:
        return ('n', Fraction(0), bool(sa ^ sb))
    return ('n', round_sig(q, 24), False)


def fst(v):
    """fstp dword: bits del float"""
    if v[0] == 'q':
        return v[1]
    if v[0] == 'i':
        return 0x7F800000 | (v[1] << 31)
    if v[1] == 0:
        return 0x80000000 if v[2] else 0
    return to_float_bits(v[1])


def feq(abits, bbits):
    """comparacion x87 (fcompp / fcomp con test ah,44h): igual solo si no hay NaN y los valores son iguales (+0 == -0)"""
    if is_nan(abits) or is_nan(bbits):
        return False
    return f32(abits) == f32(bbits)


def pool_index(diff):
    """CVehicle::operator delete (0x6E2D90): (this - [pool]) / 0xA18 con la constante magica: imul 6572EC3h; sar edx,6; + bit de signo"""
    d = signed(diff)
    hi = (d * VEHICLE_SIZE_MAGIC) >> 32            # edx de imul (con signo)
    q = hi >> 6                                    # sar edx,6
    q += 1 if q < 0 else 0                         # shr eax,1Fh; add edx,eax
    return q


""" ------------------------------------------------------------------------------------------------ marcas de cobertura """
COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == BBOX:
        _k += ['ret_zero', 'ret_set']
    elif _kind == COMPLEXDTOR:
        _k += ['sub_null', 'sub_set']
    elif _kind in CLONE_SIZE:
        _k += ['mem_null', 'mem_set']
        if _kind == CLONEAIRLAND:
            _k += ['byte0_norm', 'byte1_norm', 'bytes_clear']
        elif _kind == CLONEINAIR:
            _k += ['bits_all', 'bits_none', 'flags_high']
        elif _kind == CLONELAND:
            _k += ['anim_minus1', 'anim_other']
        elif _kind == CLONEDEATH:
            _k += ['bit3', 'bit4', 'dir_zero']
        elif _kind == CLONEFALL:
            _k += ['total_nonzero']
    elif _kind in (TASKSDD, TRAINSDD, PLSDD):
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits']
        if _kind == TRAINSDD:
            _k += ['idx_zero', 'idx_positive', 'lowers_first_free', 'keeps_first_free', 'rem_nonzero']
    elif _kind == DOORU32:
        _k += ['code_8', 'code_9', 'code_10', 'code_11', 'code_default', 'code_below', 'code_above', 'nan_result']
    elif _kind == DOORRATIO:
        _k += ['door_0', 'door_5', 'door_high', 'nan_result']
    elif _kind == ISROOM:
        pass
    elif _kind == OPENDOOR:
        _k += ['door_left', 'door_right', 'door_other', 'frame_null', 'ped_null', 'ped_set', 'was_closed', 'was_open', 'opens', 'stays_closed', 'event60', 'event120', 'ratio_zero', 'ratio_nan',
               'streak', 'other_model', 'axis0', 'axis1', 'axis2', 'streak_nan', 'streak_inf', 'saved_nan', 'saved_negzero']
    elif _kind == TELEPORT:
        _k += ['matrix_null', 'matrix_set', 'orient_clear', 'orient_keep', 'orient_high_bits', 'nan_in']
    elif _kind == DIVIDE:
        _k += ['div_zero', 'div_nan', 'div_inf', 'div_denormal', 'x_nan', 'result_inf', 'result_denormal', 'rounded_differs']
    elif _kind == SETSURFACE:
        _k += ['high_bits']
    elif _kind == MLLCTOR:
        pass
    elif _kind in (PLDTOR, PLSDD):
        _k += ['matrix_null', 'matrix_set', 'counter_wraps']
    elif _kind == ARRRESET:
        _k += ['count_positive', 'count_zero', 'count_negative', 'owner', 'not_owner', 'owner_high_bits']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    vptrs = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tag = W.tagv
    if k == BBOX:
        W.snap()
        events.append([0x40FAF0, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        ret = tag(this)
        marks.append('ret_zero' if a[4] == 0 else 'ret_set')
    elif k == COMPLEXDTOR:
        sub = W.raw(1, a[5]) if a[4] else 0
        A.wr(o + 8, 4, sub)
        W.snap()
        if sub:
            marks.append('sub_set')
            events.append([EV_VDTOR, tag(sub), 1])
        else:
            marks.append('sub_null')
        A.wr(o + 8, 4, 0)
        vptrs.append([0, o, 'vt'])
    elif k in CLONE_SIZE:
        # campos del objeto origen (+...), memoria que devuelve CTask::operator new y resultado del constructor del EXE
        if k == CLONEGETUP or k == CLONESTAND:
            memflag, memoff = a[4], a[5]
        elif k == CLONEFALL:
            A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x18, 4, a[5]); A.wr(o + 0xC, 4, a[6]); memflag, memoff = a[7], a[8]
        elif k == CLONEFALLGETUP:
            A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5]); A.wr(o + 0x14, 4, a[6]); memflag, memoff = a[7], a[8]
        elif k == CLONEAIRLAND:
            A.wr(o + 0xC, 1, a[4]); A.wr(o + 0xD, 1, a[5]); memflag, memoff = a[6], a[7]
        elif k == CLONEINAIR:
            A.wr(o + 0x24, 1, a[4]); W.setg(CLOCK, 4, a[5]); gorder = [(CLOCK, 4)]; memflag, memoff = a[6], a[7]
        elif k == CLONELAND:
            A.wr(o + 0xC, 4, a[4]); memflag, memoff = a[5], a[6]
        else:
            A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5]); A.wr(o + 0x14, 4, a[6]); A.wr(o + 0x20, 1, a[7]); A.wr(o + 0x21, 1, a[8]); memflag, memoff = a[9], a[10]
        W.snap()
        events.append([NEWADDR, CLONE_SIZE[k]])
        if not memflag:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            m = memoff
            if k == CLONEGETUP:
                B.wr(m + 4, 4, 0); B.wr(m + 8, 1, 0); B.wr(m + 9, 1, 0); B.wr(m + 0xC, 4, 0)
                vptrs.append([1, m, 'vt'])
            elif k == CLONESTAND:
                B.wr(m + 4, 4, 0); B.wr(m + 8, 4, 0)
                vptrs.append([1, m, 'vt'])
            elif k == CLONEFALL:
                B.wr(m + 4, 4, 0)
                B.wr(m + 0x18, 4, a[5]); B.wr(m + 0x1C, 4, a[5]); B.wr(m + 0x10, 4, a[4]); B.wr(m + 8, 1, 0); B.wr(m + 0xC, 4, a[6]); B.wr(m + 0x14, 4, 0)
                vptrs.append([1, m, 'vt'])
                if a[5]:
                    marks.append('total_nonzero')
            elif k == CLONEFALLGETUP:
                B.wr(m + 4, 4, 0); B.wr(m + 8, 4, 0)
                B.wr(m + 0x10, 4, a[5]); B.wr(m + 0x14, 4, a[6]); B.wr(m + 0xC, 4, a[4])
                vptrs.append([1, m, 'vt'])
            elif k == CLONEAIRLAND:
                b0, b1 = int((a[4] & 0xFF) != 0), int((a[5] & 0xFF) != 0)
                B.wr(m + 4, 4, 0); B.wr(m + 8, 4, 0)
                B.wr(m + 0xC, 1, b0); B.wr(m + 0xD, 1, b1); B.wr(m + 0xE, 1, 0)
                vptrs.append([1, m, 'vt'])
                if (a[4] & 0xFF) > 1:
                    marks.append('byte0_norm')
                if (a[5] & 0xFF) > 1:
                    marks.append('byte1_norm')
                if not b0 and not b1:
                    marks.append('bytes_clear')
            elif k == CLONEINAIR:
                bits = a[4] & 7
                old = B.rd(m + 0x24, 1)
                B.wr(m + 4, 4, 0)
                B.wr(m + 0x1C, 4, 0); B.wr(m + 0x20, 4, 0); B.wr(m + 0x28, 4, 0)
                B.wr(m + 0x24, 1, (old & 0xF8) | bits)
                B.wr(m + 0x2C, 4, a[5]); B.wr(m + 0x30, 4, 0x1F4); B.wr(m + 0x34, 1, 1); B.wr(m + 0x35, 1, 0); B.wr(m + 0x38, 4, 0)
                vptrs.append([1, m, 'vt'])
                if bits == 7:
                    marks.append('bits_all')
                if bits == 0:
                    marks.append('bits_none')
                if a[4] & 0xF8:
                    marks.append('flags_high')
            elif k == CLONELAND:
                old = B.rd(m + 0x10, 1)
                B.wr(m + 4, 4, 0)
                B.wr(m + 0x10, 1, (old & 0xFC) | (6 if a[4] == M32 else 4))
                B.wr(m + 0xC, 4, a[4]); B.wr(m + 8, 4, 0)
                vptrs.append([1, m, 'vt'])
                marks.append('anim_minus1' if a[4] == M32 else 'anim_other')
            else:
                old = B.rd(m + 0x21, 1)
                bit3, bit4 = (a[8] >> 3) & 1, (a[8] >> 4) & 1
                B.wr(m + 4, 4, 0); B.wr(m + 8, 4, 0)
                B.wr(m + 0xC, 4, a[4]); B.wr(m + 0x10, 4, a[5]); B.wr(m + 0x14, 4, a[6])
                B.wr(m + 0x18, 4, M32); B.wr(m + 0x1C, 4, M32)
                B.wr(m + 0x20, 1, a[7] & 0xFF)
                B.wr(m + 0x21, 1, ((bit4 << 1 | bit3) << 3) | (old & 0xE0))
                vptrs.append([1, m, 'vt'])
                if bit3:
                    marks.append('bit3')
                if bit4:
                    marks.append('bit4')
                if (a[7] & 0xFF) == 0:
                    marks.append('dir_zero')
            ret = tag(W.raw(1, m))
    elif k == TASKSDD:
        W.snap()
        events.append([0x677F80, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
        if a[4] & 1:
            marks.append('flag_bit0_set')
            events.append([DELADDR, tag(this), A.rd(o, 4)])
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tag(this)
    elif k == TRAINSDD:
        # pool de vehiculos sintetico en B: objetos = this - diff, estados en D, primer hueco libre en +0Ch
        idx, rem = signed(a[9]), a[10]
        diff = idx * 0xA18 + rem if idx >= 0 else idx * 0xA18 - rem
        objects = (this - diff) & M32
        pool = a[7]
        B.wr(pool + 0, 4, objects); B.wr(pool + 4, 4, W.raw(3, a[8])); B.wr(pool + 8, 4, 0x40); B.wr(pool + 0xC, 4, a[11])
        D.wr(a[8] + idx, 1, D.rd(a[8] + idx, 1) & 0x7F)       # el hueco esta usado (la DLL asserta contra un doble borrado)
        W.setg(VEHPOOL, 4, W.raw(1, pool))
        W.snap()
        events.append([0x6F54D0, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
        if a[4] & 1:
            marks.append('flag_bit0_set')
            index = pool_index((this - B.rd(pool, 4)) & M32)
            addr = (W.raw(3, a[8]) + index) & M32
            state_off = a[8] + index
            D.wr(state_off, 1, D.rd(state_off, 1) | 0x80)
            first = signed(B.rd(pool + 0xC, 4))
            if index < first:
                B.wr(pool + 0xC, 4, index & M32)
                marks.append('lowers_first_free')
            else:
                marks.append('keeps_first_free')
            marks.append('idx_negative' if index < 0 else ('idx_zero' if index == 0 else 'idx_positive'))
            if rem:
                marks.append('rem_nonzero')
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tag(this)
    elif k == DOORU32:
        vt = W.raw(2, a[6])
        A.wr(o, 4, vt)
        W.snap()
        t = (a[4] - 8) & M32
        mapped = {0: 3, 1: 5, 2: 2, 3: 4}.get(t)
        if mapped is not None:
            events.append([EV_SLOT30, tag(this), mapped])
            ret = quiet(a[5])
            marks.append(['code_8', 'code_9', 'code_10', 'code_11'][t])
            if is_nan(a[5]):
                marks.append('nan_result')
        else:
            ret = 0
            marks.append('code_default')
            if t > 10:
                marks.append('code_below' if a[4] < 8 or a[4] >= 0x80000000 else 'code_above')
            else:
                marks.append('code_above')
    elif k == DOORRATIO:
        W.snap()
        events.append([0x6F47E0, tag((this + 0x5D8 + 0x18 * a[4]) & M32)])
        ret = quiet(a[5])
        if a[4] == 0:
            marks.append('door_0')
        if a[4] == 5:
            marks.append('door_5')
        if a[4] > 5:
            marks.append('door_high')
        if is_nan(a[5]):
            marks.append('nan_result')
    elif k == ISROOM:
        W.snap()
        ret = 1
    elif k == OPENDOOR:
        ped = W.raw(2, a[5]) if a[4] else 0
        A.wr(o + 0x22, 2, a[10] & 0xFFFF)
        LF = W.raw(1, a[13]) if a[11] else 0
        RF = W.raw(1, a[14]) if a[12] else 0
        A.wr(o + 0x668 + 4, 4, LF); A.wr(o + 0x668 + 8, 4, RF)
        POSMODES = {1: [0x80000000] * 3, 2: [0x80000000, 0, 0x3F800000], 3: [0x7F800001, 0x7FA00000, 0xFF800001], 4: [0x7F800000, 0xFF800000, 0x00000001]}
        for present, off in ((a[11], a[13]), (a[12], a[14])):
            if present and a[25] in POSMODES:
                for i in range(3):
                    B.wr(off + 0x40 + 4 * i, 4, POSMODES[a[25]][i])
        door = a[7]
        dbase = o + 0x5D8 + 0x18 * door
        if door in (2, 3):
            A.wr(dbase + 0, 4, a[17]); A.wr(dbase + 4, 4, a[18]); A.wr(dbase + 0xA, 1, a[21]); A.wr(dbase + 0xC, 4, a[18] if a[19] == 0 else a[20])
        if a[4]:
            Cc.wr(a[5] + 0x14, 4, W.raw(3, a[15]) if a[4] == 2 else 0)
        W.setg(CLOCK, 4, a[23])
        gorder = [(CLOCK, 4)]
        frame = 0
        if door in (2, 3):
            frame = LF if door == 2 else RF
        W.snap()
        marks.append('door_left' if door == 2 else ('door_right' if door == 3 else 'door_other'))
        marks.append('ped_set' if ped else 'ped_null')
        if door in (2, 3) and not frame:
            marks.append('frame_null')
        if door in (2, 3) and frame:
            closed, openang = a[18], a[17]
            angle = a[18] if a[19] == 0 else a[20]
            ratio = a[8]
            fo = frame - W.bases[1] - GUARD            # desplazamiento de la ventana de B
            saved = [B.rd(fo + 0x10 + 0x30 + 4 * i, 4) for i in range(3)]
            was_closed = feq(closed, angle)
            marks.append('was_closed' if was_closed else 'was_open')
            if was_closed:
                events.append([0x7F1200, tag(frame), 0x4C7BB0, 0x400])
            events.append([0x6F4790, tag((this + 0x5D8 + 0x18 * door) & M32), ratio])
            if a[22] == 0:
                newang = closed
            elif a[22] == 1:
                newang = angle
            else:
                newang = a[24]
            A.wr(dbase + 0xC, 4, newang)

            def quiet_event(level):
                if ped:
                    events.append([0x571B70, tag(ped), STACK_TAG])
                    events.append([0x4ABA50])
                    pos = (Cc.rd(a[5] + 4 + 4 * i, 4) for i in range(3)) if a[4] == 1 else (D.rd(a[15] + 0x30 + 4 * i, 4) for i in range(3))
                    group = W.raw(3, a[16])
                    events.append([0x4AB420, tag(group), STACK_TAG, 0, tag(ped), level, a[23]] + list(pos))
                    events.append([0x571A00, tag(ped), STACK_TAG])
            if was_closed:
                if not feq(closed, newang):
                    marks.append('opens')
                    events.append([0x7F1200, tag(frame), 0x4C7B90, 0x400])
                    if ped:
                        marks.append('event60')
                    quiet_event(0x42700000)
                else:
                    marks.append('stays_closed')
            else:
                if feq(ratio, 0):
                    marks.append('ratio_zero')
                    if ped:
                        marks.append('event120')
                    quiet_event(0x42F00000)
            if is_nan(ratio):
                marks.append('ratio_nan')
            marks.append('axis%d' % a[21])
            rwm = fo + 0x10
            if (a[10] & 0xFFFF) == 0x23A:
                marks.append('streak')
                y = fst(fadd(fmul(fsub(ld(openang), ld(closed)), ld(ratio)), ld(closed)))
                if is_nan(y):
                    marks.append('streak_nan')
                if (y & 0x7FFFFFFF) == 0x7F800000:
                    marks.append('streak_inf')
                events.append([0x59AF40, saved[0], y, saved[2]])
                rows = [0x3F800000, 0, 0, 0, 0x3F800000, 0, 0, 0, 0x3F800000]
                pos = [saved[0], y, saved[2]]
            else:
                marks.append('other_model')
                orient = [0, 0, 0]
                orient[a[21]] = quiet(newang)
                events.append([0x59B120] + orient)
                rows = a[26:35]
                pos = [fst(fadd(ld(0), ld(s))) for s in saved]
                if any(is_nan(s) for s in saved):
                    marks.append('saved_nan')
                if any(s == 0x80000000 for s in saved):
                    marks.append('saved_negzero')
            # UpdateRW: 12 dwords a la RwMatrix del marco (derecha +0, adelante +10h, arriba +20h, posicion +30h) y RwMatrixUpdate
            for i in range(3):
                B.wr(rwm + 0x00 + 4 * i, 4, rows[i]); B.wr(rwm + 0x10 + 4 * i, 4, rows[3 + i]); B.wr(rwm + 0x20 + 4 * i, 4, rows[6 + i]); B.wr(rwm + 0x30 + 4 * i, 4, pos[i])
            events.append([0x7F18A0, tag(W.raw(1, rwm))])
    elif k == TELEPORT:
        matrix = W.raw(1, a[9]) if a[8] else 0
        A.wr(o + 0x14, 4, matrix)
        W.snap()
        events.append([0x563280, tag(this)])
        if matrix:
            for i in range(3):
                B.wr(a[9] + 0x30 + 4 * i, 4, a[4 + i])
            marks.append('matrix_set')
        else:
            for i in range(3):
                A.wr(o + 4 + 4 * i, 4, a[4 + i])
            marks.append('matrix_null')
        if a[7] & 0xFF:
            events.append([0x439A80, tag(this), 0, 0, 0])
            marks.append('orient_clear')
        else:
            marks.append('orient_keep')
        if a[7] >> 8:
            marks.append('orient_high_bits')
        if any(is_nan(v) for v in a[4:7]):
            marks.append('nan_in')
        for off in (0x44, 0x48, 0x4C, 0x50, 0x54, 0x58):
            A.wr(o + off, 4, 0)
        events.append([0x563220, tag(this)])
    elif k == DIVIDE:
        for i in range(3):
            A.wr(o + 4 * i, 4, a[5 + i])
        W.snap()
        d = ld(a[4])
        r = fdiv(ld(0x3F800000), d)
        out = []
        for i in range(3):
            if i < 2:
                p = fmul(r, ld(a[5 + i]))
            else:
                p = fmul(r, ld(a[5 + i]))
            out.append(fst(p))
            A.wr(o + 4 * i, 4, out[i])
        ret = tag(this)
        if (a[4] & 0x7FFFFFFF) == 0:
            marks.append('div_zero')
        if is_nan(a[4]):
            marks.append('div_nan')
        if (a[4] & 0x7FFFFFFF) == 0x7F800000:
            marks.append('div_inf')
        if 0 < (a[4] & 0x7FFFFFFF) < 0x800000:
            marks.append('div_denormal')
        if any(is_nan(v) for v in a[5:8]):
            marks.append('x_nan')
        if any((v & 0x7FFFFFFF) == 0x7F800000 for v in out) and not any((v & 0x7FFFFFFF) == 0x7F800000 for v in a[5:8]):
            marks.append('result_inf')
        if any(0 < (v & 0x7FFFFFFF) < 0x800000 for v in out):
            marks.append('result_denormal')
        if r[0] == 'n' and r[1] != 0 and ld(fst(r))[1] != r[1]:
            marks.append('rounded_differs')
    elif k == NORMALADDR:
        W.snap()
        ret = tag((this + 0x10) & M32)
    elif k == SETSURFACE:
        W.snap()
        A.wr(o + 0x20, 1, a[4] & 0xFF)
        if a[4] >> 8:
            marks.append('high_bits')
    elif k == FLUSHJUMP:
        W.snap()
        events.append([0x552400, tag(this)])
    elif k == MLLCTOR:
        W.snap()
        for off in (0x40, 0x44, 0x94, 0x98, 0xE8, 0xEC, 0x13C, 0x140, 0x190, 0x194, 0x1E4, 0x1E8, 0x1F8):
            A.wr(o + off, 4, 0)
        ret = tag(this)
    elif k in (PLDTOR, PLSDD):
        A.wr(o + 0x14, 4, a[5])
        W.setg(NUMMATRICES, 4, a[4])
        gorder = [(NUMMATRICES, 4)]
        W.snap()
        A.wr(o, 4, 0x863C40)
        if a[5]:
            marks.append('matrix_set')
            events.append([0x54F3B0, tag(this)])
            n = (a[4] - 1) & M32
            W.setg(NUMMATRICES, 4, n)
            if a[4] == 0:
                marks.append('counter_wraps')
        else:
            marks.append('matrix_null')
        A.wr(o + 0x14, 4, 0xB74240)
        if k == PLSDD:
            if a[6] & 1:
                marks.append('flag_bit0_set')
                events.append([DELSCALAR, tag(this), A.rd(o, 4)])
            else:
                marks.append('flag_bit0_clear')
            if a[6] >> 8:
                marks.append('flag_high_bits')
            ret = tag(this)
    elif k == ARRRESET:
        A.wr(o + 0, 4, a[4]); A.wr(o + 4, 4, a[5]); A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 1, a[8])
        W.snap()
        count = signed(a[6])
        if count > 0:
            marks.append('count_positive')
            if a[8] & 0xFF:
                marks.append('owner')
                events.append([DELARR, tag(a[4])])
                events.append([DELARR, tag(a[5])])
                if a[8] >> 8 or (a[8] & 0xFF) > 1:
                    marks.append('owner_high_bits')
            else:
                marks.append('not_owner')
            A.wr(o + 0, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        else:
            marks.append('count_zero' if count == 0 else 'count_negative')
    skip = {0: [], 1: [], 2: [], 3: []}
    for ar, off, _ in vptrs:
        skip[ar].append(off)
    return {'events': events, 'changes': {'A': A.changes(skip[0]), 'B': B.changes(skip[1]), 'C': Cc.changes(skip[2]), 'D': D.changes(skip[3])}, 'vptrs': vptrs,
            'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado)"""
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
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
