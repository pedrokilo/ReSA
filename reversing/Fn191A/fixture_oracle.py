"""Oraculo independiente (Python) de Fn191A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 33 funciones del lote 191A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y la generacion de cadenas (Fixtures.hpp)
para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; dumpbin /disasm).
   0  0x66BB90  CTaskComplexGotoDoorAndOpen::GetTaskType: mov eax,3A4h
   1  0x66BC80  GotoDoorAndOpen::MakeAbortable: si [esp+8] (prioridad) == 2: ecx = [ecx+8] (subtarea, sin guarda); [esp+8] = 2; jmp [[ecx]+18h] (ranura 6: ped, 2, evento); si no: xor al,al; ret 0Ch
   2  0x66C280  funcion muerta: eax = [this+0Ch] (CEntryExit*); x = ([eax+10h] + [eax+8]) * 0.5 (fld; fadd; fmul 0.5f; fstp: x87 de 24 bits); y = ([eax+14h] + [eax+0Ch]) * 0.5; z = dword [eax+18h]; CTask::operator new(30h) (0x61A5A0);
            si no es nulo: CTaskSimpleGoToPoint 0x667CD0 (6, &centro, 0.5f, 0, 0); devuelve el objeto (o nulo); ret 4
   3  0x66C3E0  CTaskSimpleSlideToCoord(pos*, rot, vel): 0x61A6C0(0); [+44h]=[+48h]=0; bytes [+4Ch]=[+4Dh]=0; vtable 86FFECh; [+64h..] = pos (3 dwords); [+74h] = vel; byte [+78h] = (b & 0FDh) | 1; [+70h] = rot; devuelve this; ret 0Ch
   4  0x66C450  CTaskSimpleSlideToCoord(pos*, rot, vel, nombre*, grupo*, banderas, mezcla, enSecuencia, tiempo): 0x61A990(nombre, grupo, banderas, mezcla, tiempo, 0, enSecuencia, 0, 0); vtable; pos; [+74h] = vel; byte [+78h] |= 3;
            [+70h] = rot; [+7Ch] = -1; ret 24h
            0x61A990: base 0x61A390; [+8]=0; byte [+0Ch] = (hold<<2) | (b & F8h); [+38h] = mezcla; [+40h] = tiempo; vtable 86D54Ch; [+44h]=[+48h]=0; bytes [+4Ch]=[+4Dh]=0; [+50h]=[+54h]=[+58h]=0; [+5Ch] = banderas;
            byte [+0Ch] = ((((fuera << 1) | enSecuencia) << 3 | noInterrumpir) << 1) | (byte[+0Ch] & 8Dh); strcpy([+10h], nombre); strcpy([+28h], grupo); bloque = 4D3940([+28h]); [+3Ch] = bloque ? 4D42F0([+10h], bloque) : 0
   5  0x66C4C0  SlideToCoord cuerpo del destructor: [this]=86FFECh; jmp 0x61A6F0: [esi]=86D504h; si [esi+8]: 4CEBE0 (SetFinishCallback(4D1390, 0)); word [anim+2Eh] |= 4; si el bit 2 del byte [esi+0Ch] es 0 y float [anim+18h] > 0 y [anim+1Ch] >= 0
            (ordenado): [anim+1Ch] = C0800000h; [esi+8] = 0; ~CTask (vtable 86D48Ch)
   6  0x619440  RsCameraShowRaster: jmp 0x745240
   7  0x619780  RsSetPresetView(camara, vista): camara nula, contador 0, vista >= contador o < 0 (con signo) -> 0; si no: [8D2E30] = vista; avanza (contador - vista - 1) nodos por [nodo+20h]; frame = [camara+4];
            7F10B0(frame); 7F1010(frame, 8D2E00 (1,0,0), -rotX (x87: fld; fchs; fstp), 0); 7F1010(frame, 8D2E0C (0,1,0), rotY, 2); 7F0E30(frame, nodo, 2); 7F0910(frame); 7EE1D0(camara, cercano [+14h]); 7EE2A0(camara, lejano [+18h]); devuelve 1
   8  0x6198C0  RsDestroyPresetViews: por cada nodo de la lista [C1707C] (siguiente +20h): si [nodo+1Ch]: 72F430(descripcion); 72F430(nodo); lista y contador a 0
   9  0x43EEB0, 14 0x43F780  cuerpo del destructor de CPool<CEntryExit>: si [this+8] (con signo) > 0: si byte [this+10h] != 0: 8213AE([this]); 8213AE([this+4]); [this] = [this+4] = [this+8] = [this+0Ch] = 0
  10  0x43EF00  GetInSlot(slot): pool = [96A7D8]; si byte [[pool+4] + slot] tiene el bit 7: 0; si no [pool] + slot * 3Ch (32 bits)
  11  0x43EF20  SetEntryExitFlagWithName(nombre, mascara, activar): cap = [pool+8] (una vez, con signo); para cada slot < cap: si el byte de estado tiene el bit 7: sigue; si _strnicmp(entrada, nombre, 8) == 0: activar (byte bajo != 0)
            ? word [entrada+30h] |= mascara (16 bits bajos) : word [entrada+30h] &= ~mascara
  12  0x43EF90  SetEntryExitFlagWithIndex(indice, mascara, activar): igual para el slot dado (con slot libre: puntero nulo; el banco no lo ejercita)
  13  0x43F720  CPool<CEntryExit> constructor(capacidad, nombre): operator new[] (821195) de capacidad * 3Ch -> [this]; operator new[] de capacidad -> [this+4]; byte [+10h] = 1; [+8] = capacidad; [+0Ch] = -1;
            si capacidad (con signo) > 0: byte [[this+4] + i] = 80h para i < capacidad; devuelve this; ret 8
  15  0x43F7D0  CPool<CEntryExit>::New: cursor = [this+0Ch] + 1 (escrito); si == capacidad ([this+8], una vez): cursor = 0 y, si ya habia dado la vuelta, devuelve nulo; hueco libre = bit 7 del byte de estado del cursor;
            al elegirlo: byte = (byte + 1) & 7Fh (con el bit 7 borrado); devuelve [this] + cursor * 3Ch
  16  0x43F950  CPool<CEntryExit> destructor con borrado(banderas): igual que 9; con el bit 0 del byte de banderas: 8214BD(this); devuelve this; ret 4
  17  0x43F9B0  SetEnabledByName(nombre, activar): 43EF20(nombre, 4000h, activar)
  18  0x4410E0  CPad::IsDPadRightPressed: si word [ecx+16h] != 0 y word [ecx+46h] == 0: 1; si no 0 (EAX completo)
  19  0x441130  CPhysical::SetVelocity(x, y, z): [ecx+44h] = x; [+48h] = y; [+4Ch] = z (dwords)
  20  0x6786F0  mov eax,0D0h
  21  0x678790  [ecx] = 870454h; jmp 61A3D0 (cuerpo de ~CTaskComplex: vtable 86D4CCh; si [this+8]: delete virtual (push 1; call [[sub]]); [this+8] = 0; vtable 86D48Ch)
  22  0x6789D0  CTaskComplexFallAndStayDown(anim, grupo): base 61A3B0 ([+4] = 0, [+8] = 0); [+0Ch] = anim; vtable 870480h; [+10h] = grupo; devuelve this; ret 8
  23  0x678A00  mov eax,0D1h
  24  0x678A10  constructor(direccion): base 61A3B0; vtable; si direccion (sin signo) <= 3: [+0Ch] = {18h, 19h, 1Ah, 1Bh}[direccion]; [+10h] = 0 siempre; ret 4
  25  0x678A90  [ecx] = 870480h; jmp 61A3D0
  26  0x678AA0  MakeAbortable(ped, prio, evento): prio 2: jmp [[sub]+18h] con prio 2; prio 1: bl = 0; si [[sub]+10h]() != 0CFh: bl = AL de la ranura 6(ped, prio, evento); devuelve bl; otra: si ranura 4() != 0CFh: ranura 6 (resultado ignorado);
            devuelve 0
  27  0x678B20  CreateFirstSubTask(ped): 61A5A0(20h); si no es nulo: base 61A390 ([+4] = 0); [+10h] = [this+10h]; [+0Ch] = [this+0Ch]; [+18h] = [+1Ch] = 5F5E0FFh; vtable 870430h; byte [+8] = 0; [+14h] = 0; devuelve el objeto o nulo; ret 4
  28  0x678BC0  mov eax,[ecx+8]; ret 4
  29  0x678BD0  CreateSubTask(tipo): solo 0CFh crea el objeto como 27; 516h u otro: 0; ret 4
  30  0x67CBC0  destructor con borrado(banderas): 678A90; con el bit 0: 61A5B0(this); devuelve this; ret 4
  31  0x67C270  Clone: 61A5A0(14h); si no es nulo: base 61A3B0; [+10h] = [this+10h]; vtable 870480h; [+0Ch] = [this+0Ch]; devuelve el objeto o nulo
  32  0x67CBE0  CreateNextSubTask(ped): si la subtarea ([this+8]) responde ranura 4() == 0CFh: 678BD0(516h) (ECX = this); si no: 0; ret 4

Los falsos anotan: operator new [0x61A5A0 / 0x821195, tamano]; operator delete [0x61A5B0 / 0x8214BD, puntero (etiqueta), dword de [puntero]]; delete[] [0x8213AE, puntero]; 0x678A90 [direccion, ECX, dword de [ECX]];
GetAnimationBlock [4D3940, puntero, hash FNV de la cadena]; GetAnimation [4D42F0, puntero, hash, bloque]; SetFinishCallback [4CEBE0, anim, funcion (DefaultAnimCB normalizado), dato]; psCameraShowRaster [745240, camara]; Free [72F430, puntero];
funciones RW [direccion, marco / camara, ...]; 43EF20 [direccion, nombre, mascara, byte de activar]; 678BD0 [direccion, ECX, tipo]; las ranuras virtuales falsas [7E1EA5E0 destructor: ECX, banderas], [7E1EA5E1 GetTaskType: ECX],
[7E1EA5E2 MakeAbortable: ECX, ped, prioridad, evento]. Las vtables que escriben constructores y destructores se omiten de los cambios y se anotan como clase ("vt": la del EXE o una de la DLL).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = json.loads(r'''["CTaskComplexGotoDoorAndOpen::GetTaskType", "CTaskComplexGotoDoorAndOpen::MakeAbortable", "CTaskComplexGotoDoorAndOpen::GoToEntryExitCenterTask", "CTaskSimpleSlideToCoord::Constructor-NoAnim", "CTaskSimpleSlideToCoord::Constructor-Anim", "CTaskSimpleSlideToCoord::Destructor", "Rs::RsCameraShowRaster", "Rs::RsSetPresetView", "Rs::RsDestroyPresetViews", "CEntryExitManager::PoolDestructorBody", "CEntryExitManager::GetInSlot", "CEntryExitManager::SetEntryExitFlagWithName", "CEntryExitManager::SetEntryExitFlagWithIndex", "CEntryExitManager::PoolConstructor", "CEntryExitManager::PoolDestructorBodyCopy", "CEntryExitManager::PoolNew", "CEntryExitManager::PoolScalarDeletingDestructor", "CEntryExitManager::SetEnabledByName", "CPad::IsDPadRightPressed", "CPhysical::SetVelocity", "CTaskComplexFallAndGetUp::GetTaskType", "CTaskComplexFallAndGetUp::DestructorBody", "CTaskComplexFallAndStayDown::Constructor", "CTaskComplexFallAndStayDown::GetTaskType", "CTaskComplexFallAndStayDown::Constructor2", "CTaskComplexFallAndStayDown::Destructor", "CTaskComplexFallAndStayDown::MakeAbortable", "CTaskComplexFallAndStayDown::CreateFirstSubTask", "CTaskComplexFallAndStayDown::ControlSubTask", "CTaskComplexFallAndStayDown::CreateSubTask", "CTaskComplexFallAndStayDown::ScalarDeletingDestructor", "CTaskComplexFallAndStayDown::Clone", "CTaskComplexFallAndStayDown::CreateNextSubTask"]''')
ENTRIES = (0x66BB90, 0x66BC80, 0x66C280, 0x66C3E0, 0x66C450, 0x66C4C0, 0x619440, 0x619780, 0x6198C0, 0x43EEB0, 0x43EF00, 0x43EF20, 0x43EF90, 0x43F720, 0x43F780, 0x43F7D0,
           0x43F950, 0x43F9B0, 0x4410E0, 0x441130, 0x6786F0, 0x678790, 0x6789D0, 0x678A00, 0x678A10, 0x678A90, 0x678AA0, 0x678B20, 0x678BC0, 0x678BD0, 0x67CBC0, 0x67C270, 0x67CBE0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(CONST, GOTOABORT, CENTER, SLIDE0, SLIDE1, SLIDEDTOR, RSSHOW, RSSET, RSDESTROY, POOLDTOR, GETSLOT, FLAGNAME, FLAGINDEX, POOLCTOR, POOLNEW, POOLSDD, SETENABLED, PADRIGHT, SETVEL, COMPLEXDTOR,
 SDCTOR1, SDCTOR2, SDABORT, SDFIRST, SDCONTROL, SDCREATESUB, SDSDD, SDCLONE, SDNEXT) = \
    ('const gotoabort center slide0 slide1 slidedtor rsshow rsset rsdestroy pooldtor getslot flagname flagindex poolctor poolnew poolsdd setenabled padright setvel complexdtor '
     'sdctor1 sdctor2 sdabort sdfirst sdcontrol sdcreatesub sdsdd sdclone sdnext').split()
KIND = [CONST, GOTOABORT, CENTER, SLIDE0, SLIDE1, SLIDEDTOR, RSSHOW, RSSET, RSDESTROY, POOLDTOR, GETSLOT, FLAGNAME, FLAGINDEX, POOLCTOR, POOLDTOR, POOLNEW, POOLSDD, SETENABLED, PADRIGHT, SETVEL, CONST, COMPLEXDTOR,
        SDCTOR1, CONST, SDCTOR2, COMPLEXDTOR, SDABORT, SDFIRST, SDCONTROL, SDCREATESUB, SDSDD, SDCLONE, SDNEXT]
CONSTS = {0: 0x3A4, 20: 0xD0, 23: 0xD1}
POOL_PTR, VIEW_LIST, VIEW_COUNT, VIEW_CURRENT = 0x96A7D8, 0xC1707C, 0xC17080, 0x8D2E30
NEWADDR, DELADDR, NEWARR, DELARR, DELSCALAR = 0x61A5A0, 0x61A5B0, 0x821195, 0x8213AE, 0x8214BD
GETBLOCK, GETANIM, SETFINISH, SHOWRASTER, FREE = 0x4D3940, 0x4D42F0, 0x4CEBE0, 0x745240, 0x72F430
FLAGNAMEADDR, CREATESUBADDR, BODYADDR = 0x43EF20, 0x678BD0, 0x678A90
EV_VDTOR, EV_VTYPE, EV_VABORT = 0x7E1EA5E0, 0x7E1EA5E1, 0x7E1EA5E2
DEFCB = 0x4D1390
ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_'


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


def avg_x87(abits, bbits):
    """fld a; fadd b; fmul 0.5; fstp dword con la x87 de 24 bits (exponente ampliado): suma redondeada a 24 bits, mitad exacta, redondeo final a float. Sin dos NaN."""
    na, nb = is_nan(abits), is_nan(bbits)
    if na and nb:
        raise ValueError('dos NaN')
    if na:
        return quiet(abits)
    if nb:
        return quiet(bbits)
    ia, ib = (abits & 0x7FFFFFFF) == 0x7F800000, (bbits & 0x7FFFFFFF) == 0x7F800000
    sa, sb = abits >> 31, bbits >> 31
    if ia and ib:
        return abits if sa == sb else 0xFFC00000
    if ia:
        return abits
    if ib:
        return bbits
    ma, ea = dec(abits)
    mb, eb = dec(bbits)
    s = Fraction(ma) * Fraction(2) ** ea + Fraction(mb) * Fraction(2) ** eb
    if s == 0:
        return 0x80000000 if (sa and sb) else 0
    s = round_sig(s, 24) / 2
    return to_float_bits(s)


def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


def str_char(seed, i):
    return ALPHABET[rnd(seed, 100 + i) % 63]


def entry_name(seed, i, name_seed, name_len):
    out = [0] * 8
    variant = rnd(seed, 10 + i) % 6

    def flip(j, c):
        lowerable = ('A' <= c <= 'Z') or ('a' <= c <= 'z')
        if lowerable and (rnd(seed, 100 + i * 8 + j) & 1):
            return ord(c) ^ 0x20
        return ord(c)
    m = min(name_len, 8)
    if name_len == 0 or variant == 4:
        return [ord(str_char((seed + 7 + i) & M32, j)) for j in range(8)]
    for j in range(8):
        out[j] = flip(j, str_char(name_seed, j)) if j < m else 0
    if variant == 2:
        p = rnd(seed, 40 + i) % m
        out[p] = ord('y' if str_char(name_seed, p) == 'z' else 'z')
    elif variant == 3:
        k = rnd(seed, 50 + i) % 8
        base = list(out)
        for j in range(8):
            out[j] = base[j] if j < k else (0 if j == k else ord(str_char((seed + 99) & M32, j)))
        if k >= m:
            for j in range(m, k):
                out[j] = ord(str_char((seed + 13) & M32, j))
    return out


def lower(b):
    return b + 32 if 65 <= b <= 90 else b


def strnicmp8(entry8, name):
    """_strnicmp(entrada, nombre, 8): minusculas ASCII, para al encontrar NUL en los dos"""
    for i in range(8):
        c1 = lower(entry8[i])
        c2 = lower(name[i]) if i < len(name) else 0
        if c1 != c2:
            return 1
        if c1 == 0:
            return 0
    return 0


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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind in (GOTOABORT, SDABORT):
        _k += ['prio2', 'prio_other', 'abort_true', 'abort_false']
        if _kind == SDABORT:
            _k += ['prio1', 'type_fall', 'type_other']
    elif _kind == CENTER:
        _k += ['mem_null', 'mem_set', 'sum_nan', 'sum_inf', 'denormal_result', 'overflow_sum']
    elif _kind in (SLIDE0, SLIDE1):
        _k += ['neg_zero_in', 'nan_in']
        if _kind == SLIDE1:
            _k += ['name_short', 'name_overflow', 'group_overflow', 'block_null', 'block_set', 'seq_set', 'seq_clear', 'reaches_hierarchy']
    elif _kind == SLIDEDTOR:
        _k += ['anim_null', 'anim_set', 'hold_set', 'hold_clear', 'blend_positive', 'delta_set', 'delta_kept']
    elif _kind == RSSHOW:
        _k += ['ret_zero', 'ret_set']
    elif _kind == RSSET:
        _k += ['camera_null', 'valid', 'invalid_view', 'count_zero', 'view_first', 'view_last', 'rotx_nan', 'view_unchanged']
    elif _kind == RSDESTROY:
        _k += ['nodes0', 'nodes_many', 'desc_some', 'desc_none']
    elif _kind in (POOLDTOR, POOLSDD):
        _k += ['cap_positive', 'cap_nonpositive', 'owner', 'not_owner']
        if _kind == POOLSDD:
            _k += ['delete_called', 'delete_skipped']
    elif _kind == GETSLOT:
        _k += ['slot_free', 'slot_used', 'slot_negative']
    elif _kind in (FLAGNAME, FLAGINDEX):
        _k += ['enable', 'disable']
        if _kind == FLAGNAME:
            _k += ['matches', 'no_match', 'cap_zero', 'skips_free']
    elif _kind == POOLCTOR:
        _k += ['cap_positive', 'cap_nonpositive', 'storage_arena', 'storage_raw']
    elif _kind == POOLNEW:
        _k += ['found', 'full', 'wraps', 'cursor_last']
    elif _kind == SETENABLED:
        _k += ['enable', 'disable']
    elif _kind == PADRIGHT:
        _k += ['pressed', 'not_pressed', 'old_down', 'new_up']
    elif _kind == SETVEL:
        _k += ['nan_in']
    elif _kind == COMPLEXDTOR:
        _k += ['sub_null', 'sub_set']
    elif _kind == SDCTOR1:
        _k += ['anim_zero', 'group_zero']
    elif _kind == SDCTOR2:
        _k += ['dir0', 'dir1', 'dir2', 'dir3', 'dir_other']
    elif _kind in (SDFIRST, SDCLONE):
        _k += ['mem_null', 'mem_set']
    elif _kind == SDCREATESUB:
        _k += ['type_fall', 'type_finished', 'type_other', 'mem_null', 'mem_set']
    elif _kind == SDSDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits']
    elif _kind == SDNEXT:
        _k += ['type_fall', 'type_other']
    elif _kind == SDCONTROL:
        _k += ['sub_null', 'sub_set']
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
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k in (GOTOABORT, SDABORT):
        mode = a[11]
        sub = 0 if mode == 0 else (W.raw(1, a[8]) if mode == 1 else a[12])
        A.wr(o + 8, 4, sub)
        W.snap()
        prio, ped, evt = a[5], a[4], a[6]
        abort = lambda p: events.append([EV_VABORT, tag(sub), tag(ped), p, tag(evt)])
        if prio == 2:
            marks.append('prio2')
            abort(2)
            ret = a[7] & 0xFF
        elif fn == 1:
            marks.append('prio_other')
            ret = 0
        elif prio == 1:
            marks.append('prio1')
            events.append([EV_VTYPE, tag(sub)])
            if a[10] == 0xCF:
                marks.append('type_fall')
                ret = 0
            else:
                marks.append('type_other')
                abort(1)
                ret = a[7] & 0xFF
        else:
            marks.append('prio_other')
            events.append([EV_VTYPE, tag(sub)])
            if a[10] != 0xCF:
                marks.append('type_other')
                abort(prio)
            else:
                marks.append('type_fall')
            ret = 0
        marks.append('abort_true' if ret else 'abort_false')
    elif k == CENTER:
        for i in range(5):
            B.wr(a[4] + 8 + 4 * i, 4, a[5 + i])
        A.wr(o + 0xC, 4, W.raw(1, a[4]))
        mem = W.raw(2, a[11]) if a[10] else 0
        W.snap()
        x = avg_x87(a[7], a[5])
        y = avg_x87(a[8], a[6])
        z = a[9]
        events.append([NEWADDR, 0x30])
        if any(is_nan(v) for v in a[5:9]):
            marks.append('sum_nan')
        if any((v & 0x7FFFFFFF) == 0x7F800000 for v in a[5:9]):
            marks.append('sum_inf')
        if any(0 < (v & 0x7FFFFFFF) < 0x800000 for v in (x, y)):
            marks.append('denormal_result')
        if any(((v >> 23) & 0xFF) >= 0xFE and not is_nan(v) for v in a[5:9]):
            marks.append('overflow_sum')
        if a[10]:
            marks.append('mem_set')
            m = a[11]
            Cc.wr(m + 4, 4, 0)
            Cc.wr(m + 8, 4, 6)
            Cc.wr(m + 0xC, 4, x); Cc.wr(m + 0x10, 4, y); Cc.wr(m + 0x14, 4, z)
            Cc.wr(m + 0x18, 4, 0x3F000000)
            Cc.wr(m + 0x1C, 1, Cc.rd(m + 0x1C, 1) & 0xC0)
            Cc.wr(m + 0x20, 1, Cc.rd(m + 0x20, 1) & 0xE0)
            vptrs.append([2, m, 'vt'])
            ret = tag(mem)
        else:
            marks.append('mem_null')
            ret = 0
    elif k in (SLIDE0, SLIDE1):
        for i in range(3):
            B.wr(a[4] + 4 * i, 4, a[5 + i])
        W.snap()
        if any(v == 0x80000000 for v in a[5:10]):
            marks.append('neg_zero_in')
        if any(is_nan(v) for v in a[5:10]):
            marks.append('nan_in')
        old0c = A.rd(o + 0xC, 1)
        A.wr(o + 4, 4, 0)                                              # base CTaskSimple (0x61A390)
        A.wr(o + 8, 4, 0)
        b1 = old0c & 0xF8                                              # 0x61A6C0 / 0x61A990 con hold = 0
        A.wr(o + 0xC, 1, b1)
        if k == SLIDE1:
            seq = a[18] & 1
            A.wr(o + 0x38, 4, a[17]); A.wr(o + 0x40, 4, a[19])
            A.wr(o + 0x44, 4, 0); A.wr(o + 0x48, 4, 0); A.wr(o + 0x4C, 1, 0); A.wr(o + 0x4D, 1, 0)
            A.wr(o + 0x50, 4, 0); A.wr(o + 0x54, 4, 0); A.wr(o + 0x58, 4, 0)
            A.wr(o + 0x5C, 4, a[16])
            A.wr(o + 0xC, 1, (seq << 4) | (b1 & 0x8D))
            name = [ord(str_char(a[10], i)) for i in range(a[11])] + [0]
            group = [ord(str_char(a[13], i)) for i in range(a[14])] + [0]
            for i, ch in enumerate(name):
                A.wr(o + 0x10 + i, 1, ch)
            for i, ch in enumerate(group):
                A.wr(o + 0x28 + i, 1, ch)

            def cstr(off):
                out = []
                while True:
                    ch = A.rd(off + len(out), 1)
                    if ch == 0:
                        return out
                    out.append(ch)
            events.append([GETBLOCK, tag(this + 0x28), fnv(cstr(o + 0x28))])
            if a[20]:
                events.append([GETANIM, tag(this + 0x10), fnv(cstr(o + 0x10)), tag(a[20])])
                A.wr(o + 0x3C, 4, a[21])
                marks.append('block_set')
            else:
                A.wr(o + 0x3C, 4, 0)
                marks.append('block_null')
            marks.append('seq_set' if seq else 'seq_clear')
            marks.append('name_short' if a[11] < 0x14 else 'name_overflow')
            if a[14] >= 0x14:
                marks.append('group_overflow')
            if a[14] >= 0x14 or a[11] >= 0x2C:
                marks.append('reaches_hierarchy')
        else:
            A.wr(o + 0x44, 4, 0); A.wr(o + 0x48, 4, 0); A.wr(o + 0x4C, 1, 0); A.wr(o + 0x4D, 1, 0)
        for i in range(3):
            A.wr(o + 0x64 + 4 * i, 4, a[5 + i])
        A.wr(o + 0x74, 4, a[9])
        A.wr(o + 0x70, 4, a[8])
        if k == SLIDE0:
            A.wr(o + 0x78, 1, (A.rd(o + 0x78, 1) & 0xFD) | 1)
        else:
            A.wr(o + 0x78, 1, A.rd(o + 0x78, 1) | 3)
            A.wr(o + 0x7C, 4, 0xFFFFFFFF)
        vptrs.append([0, o, 'vt'])
        ret = tag(this)
    elif k == SLIDEDTOR:
        A.wr(o + 0xC, 4, a[4])
        anim = W.raw(1, a[6]) if a[5] else 0
        A.wr(o + 8, 4, anim)
        if a[5]:
            B.wr(a[6] + 0x18, 4, a[7]); B.wr(a[6] + 0x1C, 4, a[8]); B.wr(a[6] + 0x2C, 4, a[9])
        W.snap()
        hold = bool(a[4] & 4)
        marks.append('hold_set' if hold else 'hold_clear')
        if anim:
            marks.append('anim_set')
            events.append([SETFINISH, tag(anim), DEFCB, 0])
            B.wr(a[6] + 0x2E, 2, B.rd(a[6] + 0x2E, 2) | 4)
            if not hold:
                if f32(a[7]) > 0.0:
                    marks.append('blend_positive')
                    if f32(a[8]) >= 0.0:
                        B.wr(a[6] + 0x1C, 4, 0xC0800000)
                        marks.append('delta_set')
                    else:
                        marks.append('delta_kept')
            A.wr(o + 8, 4, 0)
        else:
            marks.append('anim_null')
        vptrs.append([0, o, 'vt'])
    elif k == RSSHOW:
        W.snap()
        events.append([SHOWRASTER, tag(a[4])])
        ret = tag(a[5])
        marks.append('ret_zero' if a[5] == 0 else 'ret_set')
    elif k == RSSET:
        camera = W.raw(2, a[5]) if a[4] else 0
        frame = W.raw(3, a[7]) if a[6] == 0 else a[6]
        if a[4]:
            Cc.wr(a[5] + 4, 4, frame)
        n_views = signed(a[8])
        count = n_views if n_views > 0 else 1
        for j in range(count):
            B.wr(a[10] + 0x30 * j + 0x20, 4, W.raw(1, a[10] + 0x30 * (j + 1)) if j + 1 < count else 0)
            B.wr(a[10] + 0x30 * j + 0x1C, 4, a[18])
        valid = bool(camera) and n_views > 0 and 0 <= signed(a[9]) < n_views
        if valid:
            kk = n_views - a[9] - 1
            for i in range(7):
                B.wr(a[10] + 0x30 * kk + 4 * i, 4, a[11 + i])
        W.setg(VIEW_COUNT, 4, a[8]); W.setg(VIEW_LIST, 4, W.raw(1, a[10])); W.setg(VIEW_CURRENT, 4, a[19])
        gorder = [(VIEW_LIST, 4), (VIEW_COUNT, 4), (VIEW_CURRENT, 4)]
        W.snap()
        if not a[4]:
            marks.append('camera_null')
        if n_views == 0:
            marks.append('count_zero')
        if valid:
            marks.append('valid')
            W.setg(VIEW_CURRENT, 4, a[9])
            if a[9] == a[19]:
                marks.append('view_unchanged')
            if a[9] == 0:
                marks.append('view_first')
            if a[9] == n_views - 1:
                marks.append('view_last')
            if is_nan(a[14]):
                marks.append('rotx_nan')
            node = W.raw(1, a[10] + 0x30 * kk)
            events.append([0x7F10B0, tag(frame)])
            events.append([0x7F1010, tag(frame), 0x3F800000, 0, 0, neg_x87(a[14]), 0])
            events.append([0x7F1010, tag(frame), 0, 0x3F800000, 0, a[15], 2])
            events.append([0x7F0E30, tag(frame), tag(node), a[11], a[12], a[13], 2])
            events.append([0x7F0910, tag(frame)])
            events.append([0x7EE1D0, tag(camera), a[16]])
            events.append([0x7EE2A0, tag(camera), a[17]])
            ret = 1
        else:
            if a[4] and n_views != 0:
                marks.append('invalid_view')
            ret = 0
    elif k == RSDESTROY:
        count = a[4]
        for i in range(count):
            B.wr(a[5] + 0x30 * i + 0x20, 4, W.raw(1, a[5] + 0x30 * (i + 1)) if i + 1 < count else 0)
            B.wr(a[5] + 0x30 * i + 0x1C, 4, a[8 + i] if (a[6] >> i) & 1 else 0)
        W.setg(VIEW_LIST, 4, W.raw(1, a[5]) if count else 0); W.setg(VIEW_COUNT, 4, a[7])
        gorder = [(VIEW_LIST, 4), (VIEW_COUNT, 4)]
        W.snap()
        marks.append('nodes0' if count == 0 else ('nodes_many' if count >= 2 else 'nodes1'))
        anyd = False
        for i in range(count):
            d = a[8 + i] if (a[6] >> i) & 1 else 0
            if d:
                events.append([FREE, tag(d)])
                anyd = True
            events.append([FREE, tag(W.raw(1, a[5] + 0x30 * i))])
        if count:
            marks.append('desc_some' if anyd else 'desc_none')
        W.setg(VIEW_LIST, 4, 0); W.setg(VIEW_COUNT, 4, 0)
    elif k in (POOLDTOR, POOLSDD):
        cap = signed(a[4])
        A.wr(o + 0, 4, a[6]); A.wr(o + 4, 4, a[7]); A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, a[8]); A.wr(o + 0x10, 4, a[5])
        W.snap()
        if cap > 0:
            marks.append('cap_positive')
            if (a[5] & 0xFF) != 0:
                marks.append('owner')
                events.append([DELARR, tag(a[6])])
                events.append([DELARR, tag(a[7])])
            else:
                marks.append('not_owner')
            A.wr(o + 0, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        else:
            marks.append('cap_nonpositive')
        if k == POOLSDD:
            if a[9] & 1:
                marks.append('delete_called')
                events.append([DELSCALAR, tag(this), A.rd(o, 4)])
            else:
                marks.append('delete_skipped')
            ret = tag(this)
    elif k == GETSLOT:
        storage = W.raw(2, a[6]) if a[5] == 0 else a[7]
        B.wr(a[4] + 0, 4, storage); B.wr(a[4] + 4, 4, W.raw(3, a[8])); B.wr(a[4] + 8, 4, a[10])
        pos = a[8] + signed(a[9])
        D.wr(pos, 1, a[12] & 0xFF)
        W.setg(POOL_PTR, 4, W.raw(1, a[4]))
        gorder = [(POOL_PTR, 4)]
        W.snap()
        if signed(a[9]) < 0:
            marks.append('slot_negative')
        if D.rd(pos, 1) & 0x80:
            marks.append('slot_free')
            ret = 0
        else:
            marks.append('slot_used')
            ret = tag((storage + a[9] * 0x3C) & M32)
    elif k in (FLAGNAME, FLAGINDEX):
        cap = a[7]
        n = min(cap, 12)
        B.wr(a[4] + 0, 4, W.raw(2, a[5])); B.wr(a[4] + 4, 4, W.raw(3, a[6])); B.wr(a[4] + 8, 4, cap)
        for i in range(n):
            nm = entry_name(a[10], i, a[8], a[9])
            for j in range(8):
                Cc.wr(a[5] + 0x3C * i + j, 1, nm[j])
            Cc.wr(a[5] + 0x3C * i + 0x30, 2, rnd(a[10], 300 + i) & 0xFFFF)
            D.wr(a[6] + i, 1, (rnd(a[12], i) & 0x7F) | (0x80 if rnd(a[12], 500 + i) % 100 < 25 else 0))
        namestr = [ord(str_char(a[8], i)) for i in range(a[9])] + [0]
        for i, ch in enumerate(namestr):
            B.wr(a[11] + i, 1, ch)
        if k == FLAGINDEX:
            D.wr(a[6] + a[13], 1, D.rd(a[6] + a[13], 1) & 0x7F)
        W.setg(POOL_PTR, 4, W.raw(1, a[4]))
        gorder = [(POOL_PTR, 4)]
        W.snap()
        mask16 = a[14] & 0xFFFF
        enable = (a[15] & 0xFF) != 0
        marks.append('enable' if enable else 'disable')

        def apply(entry_off):
            flags = Cc.rd(entry_off + 0x30, 2)
            Cc.wr(entry_off + 0x30, 2, (flags | mask16) if enable else (flags & (~mask16 & 0xFFFF)))
        if k == FLAGNAME:
            if cap == 0:
                marks.append('cap_zero')
            matched = False
            for i in range(cap):
                if D.rd(a[6] + i, 1) & 0x80:
                    marks.append('skips_free')
                    continue
                e8 = [Cc.rd(a[5] + 0x3C * i + j, 1) for j in range(8)]
                if strnicmp8(e8, namestr) == 0:
                    matched = True
                    apply(a[5] + 0x3C * i)
            marks.append('matches' if matched else 'no_match')
        else:
            apply(a[5] + 0x3C * a[13])
    elif k == POOLCTOR:
        cap = signed(a[4])
        g0 = W.raw(2, a[7]) if a[6] == 0 else a[8]
        g1 = W.raw(3, a[9])
        W.snap()
        events.append([NEWARR, (a[4] * 0x3C) & M32])
        events.append([NEWARR, a[4]])
        A.wr(o + 0, 4, g0); A.wr(o + 4, 4, g1); A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, 0xFFFFFFFF)
        if cap > 0:
            marks.append('cap_positive')
            for i in range(cap):
                D.wr(a[9] + i, 1, 0x80)
        else:
            marks.append('cap_nonpositive')
        marks.append('storage_arena' if a[6] == 0 else 'storage_raw')
        ret = tag(this)
    elif k == POOLNEW:
        cap = a[4]
        storage = W.raw(2, a[10]) if a[9] == 0 else a[11]
        A.wr(o + 0, 4, storage); A.wr(o + 4, 4, W.raw(3, a[6])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[5])
        for i in range(cap):
            D.wr(a[6] + i, 1, (rnd(a[7], i) & 0x7F) | (0x80 if rnd(a[7], 500 + i) % 100 < a[8] else 0))
        W.snap()
        cursor = signed(a[5])
        wrapped = False
        while True:
            cursor += 1
            A.wr(o + 0xC, 4, cursor & M32)
            if cursor == cap:
                A.wr(o + 0xC, 4, 0)
                cursor = 0
                if wrapped:
                    ret = 0
                    marks.append('full')
                    break
                wrapped = True
                marks.append('wraps')
            idx = A.rd(o + 0xC, 4)
            if D.rd(a[6] + idx, 1) & 0x80:
                old = D.rd(a[6] + idx, 1)
                D.wr(a[6] + idx, 1, ((old & 0x7F) + 1) & 0x7F)
                ret = tag((storage + idx * 0x3C) & M32)
                marks.append('found')
                if idx == cap - 1:
                    marks.append('cursor_last')
                break
    elif k == SETENABLED:
        W.snap()
        name = W.raw(1, a[5]) if a[4] == 0 else a[6]
        events.append([FLAGNAMEADDR, tag(name), 0x4000, a[7] & 0xFF])
        marks.append('enable' if (a[7] & 0xFF) else 'disable')
    elif k == PADRIGHT:
        A.wr(o + 0x14, 4, a[4]); A.wr(o + 0x44, 4, a[5])
        W.snap()
        new_down, old_down = (a[4] >> 16) != 0, (a[5] >> 16) != 0
        ret = 1 if (new_down and not old_down) else 0
        marks.append('pressed' if ret else 'not_pressed')
        if old_down:
            marks.append('old_down')
        if not new_down:
            marks.append('new_up')
    elif k == SETVEL:
        W.snap()
        A.wr(o + 0x44, 4, a[4]); A.wr(o + 0x48, 4, a[5]); A.wr(o + 0x4C, 4, a[6])
        if any(is_nan(v) for v in a[4:7]):
            marks.append('nan_in')
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
    elif k == SDCTOR1:
        W.snap()
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5])
        vptrs.append([0, o, 'vt'])
        ret = tag(this)
        if a[4] == 0:
            marks.append('anim_zero')
        if a[5] == 0:
            marks.append('group_zero')
    elif k == SDCTOR2:
        W.snap()
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0)
        d = a[4]
        if d <= 3:
            A.wr(o + 0xC, 4, (0x18, 0x19, 0x1A, 0x1B)[d])
            marks.append('dir%d' % d)
        else:
            marks.append('dir_other')
        A.wr(o + 0x10, 4, 0)
        vptrs.append([0, o, 'vt'])
        ret = tag(this)
    elif k in (SDFIRST, SDCREATESUB, SDCLONE):
        if k == SDCLONE:
            A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5])
            anim, group, memflag, memoff = a[4], a[5], a[6], a[7]
        else:
            A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[6])
            anim, group, memflag, memoff = a[5], a[6], a[7], a[8]
        W.snap()
        creates = k != SDCREATESUB or a[4] == 0xCF
        if k == SDCREATESUB:
            marks.append('type_fall' if a[4] == 0xCF else ('type_finished' if a[4] == 0x516 else 'type_other'))
        if creates:
            events.append([NEWADDR, 0x14 if k == SDCLONE else 0x20])
        if creates and memflag:
            marks.append('mem_set')
            mem = W.raw(1, memoff)
            if k == SDCLONE:
                B.wr(memoff + 4, 4, 0); B.wr(memoff + 8, 4, 0); B.wr(memoff + 0xC, 4, anim); B.wr(memoff + 0x10, 4, group)
            else:
                B.wr(memoff + 4, 4, 0); B.wr(memoff + 0xC, 4, anim); B.wr(memoff + 0x10, 4, group)
                B.wr(memoff + 0x18, 4, 0x5F5E0FF); B.wr(memoff + 0x1C, 4, 0x5F5E0FF); B.wr(memoff + 8, 1, 0); B.wr(memoff + 0x14, 4, 0)
            vptrs.append([1, memoff, 'vt'])
            ret = tag(mem)
        else:
            if creates:
                marks.append('mem_null')
            ret = 0
    elif k == SDCONTROL:
        A.wr(o + 8, 4, a[5])
        W.snap()
        ret = tag(a[5])
        marks.append('sub_null' if a[5] == 0 else 'sub_set')
    elif k == SDSDD:
        W.snap()
        events.append([BODYADDR, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
        if a[4] & 1:
            marks.append('flag_bit0_set')
            events.append([DELADDR, tag(this), A.rd(o, 4)])
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tag(this)
    elif k == SDNEXT:
        sub = W.raw(1, a[5])
        A.wr(o + 8, 4, sub)
        W.snap()
        events.append([EV_VTYPE, tag(sub)])
        if a[7] == 0xCF:
            marks.append('type_fall')
            events.append([CREATESUBADDR, tag(this), 0x516])
            ret = tag(a[8])
        else:
            marks.append('type_other')
            ret = 0
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
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
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
