"""Oraculo independiente (Python) de Fn193A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 34 funciones del lote 193A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 (precision de 24 bits, exponente sin limite, redondeo al par; almacenamiento a float de 32 bits) con fracciones exactas.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico; vtables y constantes leidas del propio EXE).
   0  0x625BA0  CTaskSimpleBeHitWhileMoving destructor con borrado: push esi; mov esi,ecx; call 61FE70; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   1  0x61FE70  cuerpo del destructor: [ecx]=86D7FCh; eax=[ecx+14h]; si no es nulo: [eax+1Ch]=0C0800000h; ecx=[esi+14h]; push 0; push 4D1390h; call 4CEBE0 (SetFinishCallback); ecx=esi; call 61A3A0 ([ecx]=86D48Ch)
   2  0x625BC0  CTaskSimpleChoking destructor con borrado: como 0 con el cuerpo 620370
   3  0x625BE0  CTaskSimpleBeHit destructor con borrado: como 0 con el cuerpo 620810
   4  0x625C00  CTaskComplexReactToAttack destructor con borrado: como 0 con el cuerpo 620B90
   5  0x620B90  cuerpo del destructor: [esi]=86D868h; ecx=[esi+14h]; si no es nulo: push esi+14h; call 571A00 (CleanUpOldReference, ecx = la entidad); ecx=esi; call 61A3D0 (~CTaskComplex)
   6  0x625C20  CreateNextSubTask(ped): si [esi+0Dh] != 0: 0; tipo = [[this+8]] ranura 4 (+10h); si es C8h, D0h o 3F0h: push 516h; call 620C30 (ecx = this) y devuelve su EAX; si no 0
   7  0x625C70  CreateFirstSubTask(ped): [this+0Ch]=0; IsPlayer 5DF8F0 (ecx=ped); si AL y [this+10h] > 9 (con signo): GetPad 53FB70(0) (cdecl); si word [pad+10Eh]==0: GetTarget 540670 (ecx=pad); si AL o alguna palabra
            [pad+0], [pad+2], [pad+10h], [pad+12h], [pad+14h], [pad+16h] != 0: CreateSubTask 620C30(0C8h). Si [this+20h]==3: si [ped+470h] & 4000000h: arma [this+10h] == 1Bh o 19h, o 18h con IsPlayer(ped) == 0: dx = [agresor pos x] - [ped pos x],
            dy = igual en y (cada uno guardado en float tras la resta), Normalise 44E480 (ecx = &(dx,dy)), edx = [ped+46Ch] & ~1, x * (-5.0) y y * (-5.0) (cada uno guardado en float), z = 5.0f, [ped+46Ch] = edx, ApplyMoveForce 5429F0
            (ecx = ped; pila x, y, z) y CreateSubTask(0D0h). Si no (arma < 9 con signo y salud < 30.0f ordenada): CreateSubTask(0D0h). Sin salto: CreateSubTask(3F0h). Posicion = matriz[+14h] ? matriz+30h : objeto+4.
   8  0x625E20  CTaskComplexKillPedOnFoot destructor con borrado: como 0 con el cuerpo 620EF0
   9  0x626A80  CTaskComplexKillPedOnFootStealth destructor con borrado: como 0 con el cuerpo 620FF0
  10  0x626AA0  CTaskComplexKillPedOnFootMelee destructor con borrado: como 0 con el cuerpo 621090
  11  0x6597A0  CTaskComplexUseWaterCannon::GetTaskType: mov eax,6Dh; ret
  12  0x6597B0  cuerpo del destructor: mov [ecx],86F5D0h; jmp 61A3D0 (base: [esi]=86D4CCh; si [esi+8]: push 1; call [[esi+8]] (destructor con borrado de la subtarea); [esi+8]=0; [esi]=86D48Ch)
  13  0x659870  CTaskComplexExtinguishFireOnFoot constructor: push esi; mov esi,ecx; call 61A3B0 (base: [ecx+4]=0, [ecx]=86D4CCh, [ecx+8]=0); ecx=[esp+8] (puntero a tres dwords); [esi]=86F5FCh; copia los tres dwords a [esi+0Ch]; eax=esi; ret 4
  14  0x6598A0  GetTaskType: mov eax,6Eh; ret
  15  0x6598B0  cuerpo del destructor: mov [ecx],86F5FCh; jmp 61A3D0
  16  0x6598C0  CreateNextSubTask(ped): tipo = [[this+8]] ranura 4 (+10h); 387h: new(3Ch) (61A5A0); nulo -> 0; ctor 61F3F0 (ecx = mem; pila: 0, this+0Ch, 0, 4, 1, -1) y devuelve su EAX; 3FCh: llamada virtual [[this]+24h] (ranura 9)
            con ecx = this y el ped, devuelve su EAX; otro tipo: 0
  17  0x659980  ControlSubTask: mov eax,[ecx+8]; ret 4
  18  0x659990  FindNearestFire(ped): posicion = [ped+14h] ? +30h : ped+4; 538F40 (ecx = 0B71F80h; pila: posicion, 0, 1) -> fuego (nulo -> 0); posicion del ped releida; ST: dx = ped.x - fuego.x, dy, dz (cada resta redondeada
            a 24 bits, sin guardar en memoria); dz*dz, dx*dx, suma, dy*dy, suma; comparacion ordenada con 100.0f (0x858628): menor -> el fuego, si no (NaN incluido) 0
  19  0x659D70  Clone: new(18h) (61A5A0); nulo -> 0; si no base 61A3B0, [esi]=86F5FCh, copia [this+0Ch..14h] a [esi+0Ch..14h]; devuelve la memoria
  20  0x659DE0  CTaskSimpleAbseil destructor con borrado: como 0 con el cuerpo 659E00
  21  0x659E00  cuerpo del destructor: jmp 61A6F0 (sin escribir ninguna vtable)
  22  0x659E10  CTaskSimpleGiveCPR destructor con borrado: como 0 con el cuerpo 6588A0
  23  0x659E70  CTaskComplexTreatAccident destructor con borrado: como 0 con el cuerpo 658AE0
  24  0x65A700  CTaskComplexExtinguishFireOnFoot destructor con borrado: como 0 con el cuerpo 6598B0
  25  0x65A720  CreateFirstSubTask(ped): FindNearestFire 659990 (ecx = this; pila: ped); nulo -> 0; copia [fuego+4..0Ch] a [this+0Ch..14h]; new(28h); nulo -> 0; ctor 668120 (ecx = mem; pila: 6, fuego+4, 40000000h, [86FC88h]=2.0f, 0, 0)
            y devuelve su EAX
  26  0x49E620  Fx_c constructor: lista de +44h por 4A8DD0 (tres dwords a cero); eax = this
  27  0x49E640  Fx_c::Update: mov ecx,0A9AE80h; jmp 4A9A80 (pila: camara, tiempo; ret 8)
  28  0x49E650  Fx_c::Render: mov ecx,0A9AE80h; jmp 4A92A0 (pila: camara, bandera; ret 8)
  29  0x49E660  InitStaticSystems: 17 llamadas a 4A9BE0 (ecx = 0A9AE80h; pila: nombre, &(0,0,0), 0, 1) con los nombres de 85A6C8h, 85A6B8h, 85A6ACh, 85A69Ch, 85A688h, 85A678h, 85A66Ch, 85A660h, 85A650h, 85A638h, 85A62Ch,
            85A620h, 85A614h, 85A608h, 85A5F8h, 85A5E8h y 85A5DCh; el resultado de la llamada k va a [this+4*k]
  30  0x49E850  ExitStaticSystems: 17 llamadas a 4A9810 (ecx = 0A9AE80h; pila: [this+4*k], k = 0..16)
  31  0x6833C0  CTaskComplexWaitForPed::GetTaskType: mov eax,4BDh; ret
  32  0x6833D0  cuerpo del destructor: [esi]=8706F8h; ecx=[esi+0Ch]; si no es nulo: push esi+0Ch; call 571A00; ecx=esi; call 61A3D0
  33  0x683430  MakeAbortable: mov al,1; ret 0Ch

Los falsos anotan: cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [direccion, puntero (etiqueta), dword del puntero]; operator new [direccion, tamano]; SetFinishCallback
[direccion, ECX, funcion, datos, dword de +1Ch]; CleanUpOldReference [direccion, ECX crudo, argumento]; CreateSubTask [direccion, ECX, tipo]; ranura 9 [0xD1000109, ECX, ped]; constructores [direccion, ECX, argumentos];
FindNearestFire propio [direccion, ECX, ped]; FindNearestFire del gestor [direccion, ECX crudo, posicion, x, y, z, banderas]; IsPlayer, GetTarget [direccion, ECX]; GetPad [direccion, 0]; Normalise [direccion, ECX, x, y];
ApplyMoveForce [direccion, ECX, x, y, z, dword de +46Ch]; Fx [direccion, ECX crudo, argumentos]; subtarea sintetica: destructor con borrado [0xD1000000, ECX, banderas], GetTaskType [0xD1000004, ECX].
La vtable que deja un constructor / destructor real se clasifica: 1 = la de la clase, 2 = la base final, 3 = sin cambios.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = json.loads('["CTaskSimpleBeHitWhileMoving::ScalarDeletingDestructor", "CTaskSimpleBeHitWhileMoving::DestructorBody", "CTaskSimpleChoking::ScalarDeletingDestructor", "CTaskSimpleBeHit::ScalarDeletingDestructor", "CTaskComplexReactToAttack::ScalarDeletingDestructor", "CTaskComplexReactToAttack::DestructorBody", "CTaskComplexReactToAttack::CreateNextSubTask", "CTaskComplexReactToAttack::CreateFirstSubTask", "CTaskComplexKillPedOnFoot::ScalarDeletingDestructor", "CTaskComplexKillPedOnFootStealth::ScalarDeletingDestructor", "CTaskComplexKillPedOnFootMelee::ScalarDeletingDestructor", "CTaskComplexUseWaterCannon::GetTaskType", "CTaskComplexUseWaterCannon::DestructorBody", "CTaskComplexExtinguishFireOnFoot::Constructor", "CTaskComplexExtinguishFireOnFoot::GetTaskType", "CTaskComplexExtinguishFireOnFoot::DestructorBody", "CTaskComplexExtinguishFireOnFoot::CreateNextSubTask", "CTaskComplexExtinguishFireOnFoot::ControlSubTask", "CTaskComplexExtinguishFireOnFoot::FindNearestFire", "CTaskComplexExtinguishFireOnFoot::Clone", "CTaskSimpleAbseil::ScalarDeletingDestructor", "CTaskSimpleAbseil::DestructorBody", "CTaskSimpleGiveCPR::ScalarDeletingDestructor", "CTaskComplexTreatAccident::ScalarDeletingDestructor", "CTaskComplexExtinguishFireOnFoot::ScalarDeletingDestructor", "CTaskComplexExtinguishFireOnFoot::CreateFirstSubTask", "Fx_c::Constructor", "Fx_c::Update", "Fx_c::Render", "Fx_c::InitStaticSystems", "Fx_c::ExitStaticSystems", "CTaskComplexWaitForPed::GetTaskType", "CTaskComplexWaitForPed::DestructorBody", "CTaskComplexWaitForPed::MakeAbortable"]')
ENTRIES = (0x625BA0, 0x61FE70, 0x625BC0, 0x625BE0, 0x625C00, 0x620B90, 0x625C20, 0x625C70, 0x625E20, 0x626A80, 0x626AA0, 0x6597A0, 0x6597B0, 0x659870, 0x6598A0, 0x6598B0, 0x6598C0, 0x659980,
           0x659990, 0x659D70, 0x659DE0, 0x659E00, 0x659E10, 0x659E70, 0x65A700, 0x65A720, 0x49E620, 0x49E640, 0x49E650, 0x49E660, 0x49E850, 0x6833C0, 0x6833D0, 0x683430)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000

(CONST, MAKEAB, SDD, BODYREF, BODYVT, BODYJMP, BHWM, WCBODY, EXTCTOR, EXTCLONE, EXTNEXT, CTRLSUB, EXTFIND, EXTFIRST, RTANEXT, RTAFIRST, FXCTOR, FXUPD, FXINIT, FXEXIT) = \
    'const makeab sdd bodyref bodyvt bodyjmp bhwm wcbody extctor extclone extnext ctrlsub extfind extfirst rtanext rtafirst fxctor fxupd fxinit fxexit'.split()
KIND = [SDD, BHWM, SDD, SDD, SDD, BODYREF, RTANEXT, RTAFIRST, SDD, SDD, SDD, CONST, WCBODY, EXTCTOR, CONST, BODYVT, EXTNEXT, CTRLSUB, EXTFIND, EXTCLONE, SDD, BODYJMP,
        SDD, SDD, SDD, EXTFIRST, FXCTOR, FXUPD, FXUPD, FXINIT, FXEXIT, CONST, BODYREF, MAKEAB]
CONSTS = {11: 0x6D, 14: 0x6E, 31: 0x4BD}
# destructores con borrado: direccion del cuerpo que llaman
SDD_BODY = {0: 0x61FE70, 2: 0x620370, 3: 0x620810, 4: 0x620B90, 8: 0x620EF0, 9: 0x620FF0, 10: 0x621090, 20: 0x659E00, 22: 0x6588A0, 23: 0x658AE0, 24: 0x6598B0}
SDD_DELETE = 0x61A5B0
# cuerpos con referencia: (desplazamiento del puntero, vtable que escribe)
BODYREF_INFO = {5: (0x14, 0x86D868), 32: (0xC, 0x8706F8)}
CLEANUP, BASE_DTOR, ANIM_DTOR, TASK_DTOR = 0x571A00, 0x61A3D0, 0x61A6F0, 0x61A3A0
SFC = 0x4CEBE0
NEWADDR_TASK, SUB_DTOR, SUB_TYPE, VIRT9 = 0x61A5A0, 0xD1000000, 0xD1000004, 0xD1000109
GUNCTOR, GOTOCTOR, FINDFIRE, FMFIND, CREATESUB = 0x61F3F0, 0x668120, 0x659990, 0x538F40, 0x620C30
ISPLAYER, GETPAD, GETTARGET, NORMALISE, MOVEFORCE = 0x5DF8F0, 0x53FB70, 0x540670, 0x44E480, 0x5429F0
FXUPDATE, FXRENDER, FXCREATE, FXDESTROY, FXMAN = 0x4A9A80, 0x4A92A0, 0x4A9BE0, 0x4A9810, 0xA9AE80
FIREMGR = 0xB71F80
FX_NAMES = ['prt_blood', 'prt_boatsplash', 'prt_bubble', 'prt_cardebris', 'prt_collisionsmoke', 'prt_gunshell', 'prt_sand', 'prt_sand2', 'prt_smoke_huge', 'prt_smokeII_3_expand', 'prt_spark', 'prt_spark_2',
            'prt_splash', 'prt_wake', 'prt_watersplash', 'prt_wheeldirt', 'prt_glass']
EXT_VT = 0x86F5FC
RTA_SUBTYPES = (0xC8, 0xD0, 0x3F0)


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


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 0x80000000 else v


def fnv(s):
    h = 2166136261
    for b in s.encode('ascii'):
        h ^= b
        h = (h * 16777619) & M32
    return h


# ------------------------------------------------------------------------------------------------ aritmetica x87 (PC = 24 bits, exponente sin limite) y float de 32 bits, con fracciones exactas
# Un valor es ('f', signo, Fraction >= 0) | ('i', signo) | ('n', bits de un NaN silencioso)
def parse(bits):
    bits &= M32
    sign, exp, man = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if exp == 0xFF:
        return ('i', sign) if man == 0 else ('n', bits | 0x400000)   # fld de un NaN de senal lo deja silencioso
    if exp == 0:
        return ('f', sign, Fraction(man, 1 << 149))
    return ('f', sign, Fraction(man | 0x800000) * Fraction(2) ** (exp - 150))


def floor_log2(q):
    e = q.numerator.bit_length() - q.denominator.bit_length()
    if Fraction(2) ** e > q:
        e -= 1
    return e


def rne(q):
    """redondeo al entero mas cercano, empates al par, de una fraccion >= 0"""
    f = q.numerator // q.denominator
    r = q - f
    if r > Fraction(1, 2) or (r == Fraction(1, 2) and f & 1):
        f += 1
    return f


def round24(q):
    if q == 0:
        return q
    e = floor_log2(q)
    step = Fraction(2) ** (e - 23)
    return rne(q / step) * step


def store32(sign, q):
    """bits de float de la magnitud q (sin signo) con denormales y desbordamiento"""
    if q == 0:
        return sign << 31
    if q >= Fraction(2) ** 128 - Fraction(2) ** 103:
        return (sign << 31) | 0x7F800000
    if q < Fraction(2) ** -126:
        m = rne(q / Fraction(2) ** -149)
        return (sign << 31) | m
    e = floor_log2(q)
    m = rne(q / Fraction(2) ** (e - 23))
    if m == 1 << 24:
        m >>= 1
        e += 1
    if e > 127:
        return (sign << 31) | 0x7F800000
    return (sign << 31) | ((e + 127) << 23) | (m - (1 << 23))


def nan_pick(a, b):
    na, nb = a[0] == 'n', b[0] == 'n'
    if na and nb:
        return a if (a[1] & 0x7FFFFF) >= (b[1] & 0x7FFFFF) else b
    return a if na else b


INDEFINITE = ('n', 0xFFC00000)


def sub(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return nan_pick(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return INDEFINITE if a[1] == b[1] else ('i', a[1])
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return ('i', 1 - b[1])
    va = (-1 if a[1] else 1) * a[2]
    vb = (-1 if b[1] else 1) * b[2]
    d = va - vb
    if d == 0:
        sign = 1 if (a[2] == 0 and b[2] == 0 and a[1] and not b[1]) else 0
        return ('f', sign, Fraction(0))
    return ('f', 1 if d < 0 else 0, round24(abs(d)))


def add(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return nan_pick(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return INDEFINITE if a[1] != b[1] else a
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    va = (-1 if a[1] else 1) * a[2]
    vb = (-1 if b[1] else 1) * b[2]
    s = va + vb
    if s == 0:
        sign = 1 if (a[2] == 0 and b[2] == 0 and a[1] and b[1]) else 0
        return ('f', sign, Fraction(0))
    return ('f', 1 if s < 0 else 0, round24(abs(s)))


def mul(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return nan_pick(a, b)
    sign = (a[1] ^ b[1])
    za = a[0] == 'f' and a[2] == 0
    zb = b[0] == 'f' and b[2] == 0
    if (a[0] == 'i' and zb) or (b[0] == 'i' and za):
        return INDEFINITE
    if a[0] == 'i' or b[0] == 'i':
        return ('i', sign)
    return ('f', sign, round24(a[2] * b[2]))


def to_bits(v):
    """el valor guardado con fstp dword (redondeo a float)"""
    if v[0] == 'n':
        return v[1] & M32
    if v[0] == 'i':
        return (v[1] << 31) | 0x7F800000
    return store32(v[1], v[2])


def less(v, bits):
    """comparacion ordenada v < constante (la constante es un float finito positivo)"""
    if v[0] == 'n':
        return False
    if v[0] == 'i':
        return v[1] == 1
    c = parse(bits)
    sv = -v[2] if v[1] else v[2]
    return sv < c[2]


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


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == BODYREF:
        _k += ['ref_null', 'ref_set']
    elif _kind == BHWM:
        _k += ['anim_null', 'anim_set']
    elif _kind == WCBODY:
        _k += ['sub_null', 'sub_set']
    elif _kind == EXTCLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == EXTNEXT:
        _k += ['type_387', 'type_3fc', 'type_other', 'mem_null', 'mem_set']
    elif _kind == EXTFIND:
        _k += ['fire_null', 'in_range', 'out_range', 'mutate', 'matrix', 'no_matrix']
    elif _kind == EXTFIRST:
        _k += ['fire_null', 'fire_set', 'mem_null', 'mem_set']
    elif _kind == RTANEXT:
        _k += ['done_flag', 'type_c8', 'type_d0', 'type_3f0', 'type_other']
    elif _kind == RTAFIRST:
        _k += ['not_player', 'player_low_weapon', 'pad_disabled', 'pad_target', 'pad_input', 'pad_idle', 'state3', 'flag_set', 'weapon_1b', 'weapon_19', 'weapon_18_player', 'weapon_18_npc', 'knockback',
               'health_low', 'health_high', 'weapon_below_9', 'default_be_hit', 'mask_applied']
    KEYS[_fn] = _k


def pos_of(W, matrix_ptr_mem, base_obj_arena, base_obj_off, mat_arena, mat_off, use_matrix):
    """no se usa: las posiciones se leen directamente en expected()"""
    return None


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, vt = [], None, None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == MAKEAB:
        W.snap()
        ret = 1
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([SDD_DELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == BODYREF:
        off, vtable = BODYREF_INFO[fn]
        A.wr(o + off, 4, a[4])
        W.snap()
        A.wr(o, 4, vtable)
        if a[4] != 0:
            events.append([CLEANUP, a[4], W.tagv(this + off)])
            marks.append('ref_set')
        else:
            marks.append('ref_null')
        events.append([BASE_DTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
    elif k == BODYVT:
        W.snap()
        events.append([BASE_DTOR, W.tagv(this), EXT_VT])
        A.wr(o, 4, a[5])
    elif k == BODYJMP:
        W.snap()
        events.append([ANIM_DTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == BHWM:
        anim = 0 if a[4] == 0 else W.raw(1, a[5])
        A.wr(o + 0x14, 4, anim)
        W.snap()
        A.wr(o, 4, 0x86D7FC)
        if anim:
            B.wr(a[5] + 0x1C, 4, 0xC0800000)
            events.append([SFC, W.tagv(anim), 0x4D1390, 0, 0xC0800000])
            marks.append('anim_set')
        else:
            marks.append('anim_null')
        events.append([TASK_DTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
    elif k == WCBODY:
        sub_ = 0
        if a[4] != 0:
            sub_ = W.raw(1, a[5])
            A.wr(o + 8, 4, sub_)
            B.wr(a[5], 4, W.raw(3, a[6]))
        else:
            A.wr(o + 8, 4, 0)
        W.snap()
        if sub_:
            events.append([SUB_DTOR, W.tagv(sub_), 1])
            marks.append('sub_set')
        else:
            marks.append('sub_null')
        A.wr(o + 8, 4, 0)
        vt = 2
    elif k == EXTCTOR:
        for i in range(3):
            B.wr(a[4] + 4 * i, 4, a[5 + i])
        W.snap()
        A.wr(o + 4, 4, 0)
        A.wr(o + 8, 4, 0)
        for i in range(3):
            A.wr(o + 0xC + 4 * i, 4, a[5 + i])
        vt = 1
        ret = W.tagv(this)
    elif k == EXTCLONE:
        for i in range(3):
            A.wr(o + 0xC + 4 * i, 4, a[5 + i])
        W.snap()
        events.append([NEWADDR_TASK, 0x18])
        if a[4] == 0:
            ret = 0
            vt = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[8])
            B.wr(a[8] + 4, 4, 0)
            B.wr(a[8] + 8, 4, 0)
            for i in range(3):
                B.wr(a[8] + 0xC + 4 * i, 4, a[5 + i])
            vt = 1
            ret = W.tagv(mem)
    elif k == EXTNEXT:
        A.wr(o, 4, W.raw(3, a[4]))
        sub_ = W.raw(1, a[5])
        A.wr(o + 8, 4, sub_)
        B.wr(a[5], 4, W.raw(3, a[6]))
        for i in range(3):
            A.wr(o + 0xC + 4 * i, 4, a[14 + i])
        W.snap()
        events.append([SUB_TYPE, W.tagv(sub_)])
        ret = 0
        t = a[7]
        if t == 0x387:
            marks.append('type_387')
            events.append([NEWADDR_TASK, 0x3C])
            if a[8] == 0:
                marks.append('mem_null')
            else:
                marks.append('mem_set')
                mem = W.raw(1, a[9])
                events.append([GUNCTOR, W.tagv(mem), 0, W.tagv(this + 0xC), 0, 4, 1, 0xFFFFFFFF])
                B.wr(a[9], 4, a[11])
                ret = W.tagv(a[10])
        elif t == 0x3FC:
            marks.append('type_3fc')
            events.append([VIRT9, W.tagv(this), W.tagv(a[13])])
            ret = W.tagv(a[12])
        else:
            marks.append('type_other')
    elif k == CTRLSUB:
        A.wr(o + 8, 4, a[4])
        W.snap()
        ret = W.tagv(a[4])
    elif k == EXTFIND:
        use_mat = a[5] != 0
        if use_mat:
            B.wr(a[4] + 0x14, 4, W.raw(2, a[6]))
            for i in range(3):
                Cc.wr(a[6] + 0x30 + 4 * i, 4, a[7 + i])
            marks.append('matrix')
            pos_arena, pos_off = Cc, a[6] + 0x30
            pos_raw = W.raw(2, a[6] + 0x30)
        else:
            B.wr(a[4] + 0x14, 4, 0)
            for i in range(3):
                B.wr(a[4] + 4 + 4 * i, 4, a[7 + i])
            marks.append('no_matrix')
            pos_arena, pos_off = B, a[4] + 4
            pos_raw = W.raw(1, a[4] + 4)
        fire = 0
        if a[10] != 0:
            for i in range(3):
                D.wr(a[11] + 4 + 4 * i, 4, a[12 + i])
            fire = W.raw(3, a[11])
        W.snap()
        events.append([FMFIND, FIREMGR, W.tagv(pos_raw), a[7], a[8], a[9], 0, 1])
        if a[15] != 0:
            marks.append('mutate')
            for i in range(3):
                pos_arena.wr(pos_off + 4 * i, 4, a[16 + i])
        if fire == 0:
            ret = 0
            marks.append('fire_null')
        else:
            ped = [parse(pos_arena.rd(pos_off + 4 * i, 4)) for i in range(3)]
            fp = [parse(a[12 + i]) for i in range(3)]
            dx, dy, dz = sub(ped[0], fp[0]), sub(ped[1], fp[1]), sub(ped[2], fp[2])
            s = add(add(mul(dz, dz), mul(dx, dx)), mul(dy, dy))
            near = less(s, 0x42C80000)
            ret = W.tagv(fire) if near else 0
            marks.append('in_range' if near else 'out_range')
    elif k == EXTFIRST:
        fire = 0
        if a[4] != 0:
            for i in range(3):
                B.wr(a[5] + 4 + 4 * i, 4, a[6 + i])
            fire = W.raw(1, a[5])
        W.snap()
        events.append([FINDFIRE, W.tagv(this), W.tagv(a[13])])
        ret = 0
        if fire == 0:
            marks.append('fire_null')
        else:
            marks.append('fire_set')
            for i in range(3):
                A.wr(o + 0xC + 4 * i, 4, a[6 + i])
            events.append([NEWADDR_TASK, 0x28])
            if a[9] == 0:
                marks.append('mem_null')
            else:
                marks.append('mem_set')
                mem = W.raw(2, a[10])
                events.append([GOTOCTOR, W.tagv(mem), 6, W.tagv(fire + 4), 0x40000000, 0x40000000, 0, 0])
                Cc.wr(a[10], 4, a[12])
                ret = W.tagv(a[11])
    elif k == RTANEXT:
        A.wr(o + 0xD, 1, a[4] & 0xFF)
        sub_ = W.raw(1, a[5])
        A.wr(o + 8, 4, sub_)
        B.wr(a[5], 4, W.raw(3, a[6]))
        W.snap()
        ret = 0
        if (a[4] & 0xFF) != 0:
            marks.append('done_flag')
        else:
            events.append([SUB_TYPE, W.tagv(sub_)])
            t = a[7]
            marks.append({0xC8: 'type_c8', 0xD0: 'type_d0', 0x3F0: 'type_3f0'}.get(t, 'type_other'))
            if t in RTA_SUBTYPES:
                events.append([CREATESUB, W.tagv(this), 0x516])
                ret = W.tagv(a[8])
    elif k == RTAFIRST:
        ped = W.raw(1, a[4])
        att = W.raw(2, a[7])
        A.wr(o + 0x10, 4, a[5])
        A.wr(o + 0x14, 4, att)
        A.wr(o + 0x20, 4, a[6])
        if a[8] != 0:
            Cc.wr(a[7] + 0x14, 4, W.raw(2, a[9]))
            for i in range(3):
                Cc.wr(a[9] + 0x30 + 4 * i, 4, a[10 + i])
            att_arena, att_off = Cc, a[9] + 0x30
        else:
            Cc.wr(a[7] + 0x14, 4, 0)
            for i in range(3):
                Cc.wr(a[7] + 4 + 4 * i, 4, a[10 + i])
            att_arena, att_off = Cc, a[7] + 4
        if a[13] != 0:
            B.wr(a[4] + 0x14, 4, W.raw(3, a[14]))
            for i in range(3):
                D.wr(a[14] + 0x30 + 4 * i, 4, a[15 + i])
            ped_arena, ped_off = D, a[14] + 0x30
        else:
            B.wr(a[4] + 0x14, 4, 0)
            for i in range(3):
                B.wr(a[4] + 4 + 4 * i, 4, a[15 + i])
            ped_arena, ped_off = B, a[4] + 4
        B.wr(a[4] + 0x46C, 4, a[18])
        B.wr(a[4] + 0x470, 4, a[19])
        B.wr(a[4] + 0x540, 4, a[20])
        pad = W.raw(3, a[28])
        D.wr(a[28] + 0x10E, 2, a[21])
        for off, v in ((0, a[22]), (2, a[23]), (0x10, a[24]), (0x12, a[25]), (0x14, a[26]), (0x16, a[27])):
            D.wr(a[28] + off, 2, v & 0xFFFF)
        W.snap()
        A.wr(o + 0xC, 1, 0)
        weapon = signed(a[5])
        state = a[6]

        def create(t):
            events.append([CREATESUB, W.tagv(this), t])
            return W.tagv(a[32])
        events.append([ISPLAYER, W.tagv(ped)])
        player = (a[30] & 0xFF) != 0
        marks.append('not_player' if not player else 'player')
        done = False
        if player and weapon > 9:
            events.append([GETPAD, 0])
            if (a[21] & 0xFFFF) != 0:
                marks.append('pad_disabled')
            else:
                events.append([GETTARGET, W.tagv(pad)])
                words = [v & 0xFFFF for v in (a[22], a[23], a[24], a[25], a[26], a[27])]
                if (a[29] & 0xFF) != 0:
                    marks.append('pad_target')
                    ret = create(0xC8)
                    done = True
                elif any(words):
                    marks.append('pad_input')
                    ret = create(0xC8)
                    done = True
                else:
                    marks.append('pad_idle')
        elif player:
            marks.append('player_low_weapon')
        if not done:
            result = None
            if state == 3:
                marks.append('state3')
                knock = False
                if a[19] & 0x4000000:
                    marks.append('flag_set')
                    if weapon == 0x1B:
                        marks.append('weapon_1b')
                        knock = True
                    elif weapon == 0x19:
                        marks.append('weapon_19')
                        knock = True
                    elif weapon == 0x18:
                        events.append([ISPLAYER, W.tagv(ped)])
                        p2 = (a[31] & 0xFF) != 0
                        marks.append('weapon_18_player' if p2 else 'weapon_18_npc')
                        knock = not p2
                if knock:
                    marks.append('knockback')
                    ax = [parse(att_arena.rd(att_off + 4 * i, 4)) for i in range(2)]
                    px = [parse(ped_arena.rd(ped_off + 4 * i, 4)) for i in range(2)]
                    dxb = to_bits(sub(ax[0], px[0]))
                    dyb = to_bits(sub(ax[1], px[1]))
                    events.append([NORMALISE, STACK_TAG, dxb, dyb])
                    flags = (a[18] ^ a[35]) & M32
                    if a[35]:
                        marks.append('mask_applied')
                    cleared = flags & 0xFFFFFFFE
                    m5 = parse(0xC0A00000)
                    fx = to_bits(mul(parse(a[33]), m5))
                    fy = to_bits(mul(parse(a[34]), m5))
                    B.wr(a[4] + 0x46C, 4, cleared)
                    events.append([MOVEFORCE, W.tagv(ped), fx, fy, 0x40A00000, cleared])
                    result = create(0xD0)
                if result is None and weapon < 9:
                    marks.append('weapon_below_9')
                    h = parse(a[20])
                    if less(h, 0x41F00000):
                        marks.append('health_low')
                        result = create(0xD0)
                    else:
                        marks.append('health_high')
            if result is None:
                if True:
                    marks.append('default_be_hit')
                    result = create(0x3F0)
            ret = result
    elif k == FXCTOR:
        W.snap()
        for off in (0x44, 0x48, 0x4C):
            A.wr(o + off, 4, 0)
        ret = W.tagv(this)
    elif k == FXUPD:
        W.snap()
        if fn == 27:
            events.append([FXUPDATE, FXMAN, a[4], a[5]])
        else:
            events.append([FXRENDER, FXMAN, a[4], a[5] & 0xFF])
    elif k == FXINIT:
        W.snap()
        for i, name in enumerate(FX_NAMES):
            events.append([FXCREATE, FXMAN, fnv(name), 0, 0, 0, 0, 1])
            A.wr(o + 4 * i, 4, mix((a[4] + i * 0x9E3779B1) & M32))
    elif k == FXEXIT:
        for i in range(17):
            A.wr(o + 4 * i, 4, a[4 + i])
        W.snap()
        for i in range(17):
            events.append([FXDESTROY, FXMAN, a[4 + i]])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'vt': vt, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if o.get('vt') != e['vt']:
        return 'vtable %s != %s' % (o.get('vt'), e['vt'])
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
