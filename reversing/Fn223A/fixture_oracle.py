"""Oraculo independiente (Python) de Fn223A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 223A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las regiones globales
(Fixtures.hpp) para conocer su imagen inicial, la aritmetica x87 a 24 bits (racionales exactos) y lee del propio gta-sa.exe canonico las tablas de saltos, la constante 2^32 y las escrituras de la tabla de formatos
de 0x4C9AB0 (que interpreta con un mini ejecutor x86 sobre los bytes del EXE).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`):
   0  0x542080  `jmp 0x536E10` (CEntryInfoList::Flush; el falso anota [0x536E10, ECX, dword de [ECX]] y deja un dword en [ECX]).
   1  0x465040  thiscall(ref) `ret 4`: ranura = ref sar 8; si el BYTE de banderas [[pool+4] + ranura] == byte bajo de ref: [pool] + ranura * 0x19C, si no 0.
   2  0x465070  thiscall(objeto) `ret 4`: dif = objeto - [pool]; ranura = dif / 412 con la secuencia de multiplicacion magica del EXE (imul 0x13E22CBD, sar 5, + signo); devuelve BYTE [[pool+4] + ranura] + (ranura << 8).
   3  0x4650A0  thiscall: 41 x CDecision::CDecision (0x6040C0, ECX = this + 0x3C i), 41 x SetDefault (0x600530); devuelve this.
   4  0x4654B0  thiscall(handle, tipo) `ret 8`: recorre las 75 entradas (tipo BYTE en +0, handle en +4, paso 8); coincide si BYTE tipo == byte bajo del tipo pedido y handle igual; con tipo 1, 2, 3: entidad del pool
             0xB74494 / 0xB74490 / 0xB7449C (comprueba el byte de banderas; tamanos 0xA18 / 0x7C4 / 0x19C); si tiene el bit 0x40000 de las banderas (+0x1C): lo apaga y, si el bit 2 esta apagado (0x4633E0), AddToMovingList
             (0x542800); vacia la entrada (handle, tipo) y decrementa el BYTE +0x258.
   5  0x46A7E0  cdecl(modelo): las 40 ranuras de 0xA44940 iguales al modelo pasan a -1.
   6  0x46B260  thiscall: CScriptsForBrains::Init (0x46A8C0, ECX = this); devuelve this.
   7  0x474870 / 8  0x4748A0  thiscall sin pila: fild [0xB7CB84]; si es negativo fadd 0x858C54 (2^32 como float, redondeo a 24 bits); fcomp [ECX + 0xC60 / 0xCC0]; `test ah,41h; jp` -> 1 si tiempo <= fin (ordenado), si no 0.
   9  0x4748D0  thiscall(banderas) `ret 4`: CTask::Destructor (0x4748F0 = jmp 0x61A3A0); si el BYTE de banderas tiene el bit 0: CTask::operator delete (0x61A5B0); devuelve this.
  10  0x486AE0  `ret`.
  11  0x493360  cdecl(ref, x1, y1, z1, x2, y2, z2, vel): GetActualScriptThingIndex(ref, 2) (0x4839A0); negativo -> nada; si no: foco = 0xA94D68 + i * 0x7C; x87 a 24 bits: a = X - x1, b = Y - y1 (X, Y = [foco+0x14], [foco+0x18]),
             primera = b*b + a*a, segunda = d*d + c*c con c = X - x2, d = Y - y2; `fcompp; test ah,41h; jp`: estado [foco+3] = 0x81 si primera <= segunda (ordenado), si no 0x82; copias crudas +0x28..+0x3C, +0x40 = vel;
             si [foco+0x48] != 0: CleanUpOldReference (0x571A00, ECX = anterior, arg = foco+0x48); [foco+0x48] = 0.
  12  0x493420  cdecl(ref, entidad, vel): estado 0x83, seis dwords a cero en +0x28..+0x3C, +0x40 = vel, CleanUp del anterior (si no nulo), [foco+0x48] = entidad, RegisterReference (0x571B70, ECX = entidad, arg = foco+0x48).
  13  0x493480  cdecl(ref, x, y, z, vel): +0x28..+0x30 = x, y, z; ceros +0x34..+0x3C; estado 0x84; +0x40 = vel; CleanUp del anterior; [foco+0x48] = 0.
  14  0x4C57A0  thiscall: CBaseModelInfo (0x4C4A60, ECX = this); [this] = 0x85BDC0; [this+0x34] = 0; devuelve this.
  15  0x4C9990  cdecl(cabecera): ver expected().
  16  0x4C9AB0  cdecl(): RwRasterRegisterPlugin(0x24, 0x40C, 0x4C9A60, 0x4C9A80, 0) (0x7FB0B0); [0xB4E9E0] = resultado; si < 0 devuelve 0; si no 0x80 dwords a cero desde 0xB4E7E0 y las escrituras de bytes y palabras del EXE.
  17  0x4C9EA0  cdecl(textura): raster = [textura]; nulo -> 0; si no BYTE [raster + [0xB4E9E0] + 8].
  18  0x4C9F70  cdecl(raster): BYTE [raster + [0xB4E9E0] + 9] & 0xF.
  19  0x4C9F90  cdecl(pixeles, raster, modo): ver raster_fn().
  20  0x4CA290  cdecl(sin uso, raster): ver raster_fn().
  21  0x4CA540  cdecl(sin uso, raster): ver raster_fn().

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CEntryInfoList::Destructor", "CPool<CObject>::GetAt", "CPool<CObject>::GetRef", "CDecisionMaker::Constructor", "CMissionCleanup::RemoveEntityFromList", "CTheScripts::RemoveFromSuppressedCarModels",
         "CScriptsForBrains::Constructor", "CCamera::VectorTrackRunning", "CCamera::VectorMoveRunning", "CTaskSimpleSetCharIgnoreWeaponRangeFlag::ScalarDeletingDestructor", "CTheScripts::ScriptDebugBoxStub",
         "CTheScripts::MoveSearchLightBetweenTwoPoints", "CTheScripts::MoveSearchLightToEntity", "CTheScripts::MoveSearchLightToPointAndStop", "CPedModelInfo::ConstructorBody",
         "RwCore::RxD3D9ResEntryHeaderDestroy", "RwCore::_rwD3D9RasterPluginAttach", "RwCore::_rwD3D9TextureHasAlpha", "RwCore::_rwD3D9RasterIsCubeRaster", "RwCore::_rwD3D9RasterLock",
         "RwCore::_rwD3D9RasterUnlock", "RwCore::_rwD3D9RasterPaletteUnlock"]
ENTRIES = (0x542080, 0x465040, 0x465070, 0x4650A0, 0x4654B0, 0x46A7E0, 0x46B260, 0x474870, 0x4748A0, 0x4748D0, 0x486AE0, 0x493360, 0x493420, 0x493480, 0x4C57A0, 0x4C9990, 0x4C9AB0, 0x4C9EA0, 0x4C9F70,
           0x4C9F90, 0x4CA290, 0x4CA540)
assert len(NAMES) == len(ENTRIES) == 22
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_FAKES = tuple(0x31000000 + 0x10 * i for i in range(11))
(ENTRYDTOR, POOLGETAT, POOLGETREF, DECISION, MISSIONRM, SUPPRESSED, BRAINS, CAMERA, SDD, BOXSTUB, SLBETWEEN, SLENTITY, SLPOINT, PEDMI, HEADER, ATTACH, TEXALPHA, ISCUBE, LOCK, UNLOCK, PALETTE) = range(21)
KIND = [ENTRYDTOR, POOLGETAT, POOLGETREF, DECISION, MISSIONRM, SUPPRESSED, BRAINS, CAMERA, CAMERA, SDD, BOXSTUB, SLBETWEEN, SLENTITY, SLPOINT, PEDMI, HEADER, ATTACH, TEXALPHA, ISCUBE, LOCK, UNLOCK, PALETTE]
EXE_PATH = 'D:/Grand Theft Auto San Andreas/gta-sa.exe'
STACK_TAG = 0x7F000000
# regiones globales (direccion, bytes): 0 tabla de formatos y desplazamiento de la extension, 1 dispositivo D3D9 (0xC97C28) y superficie de dibujo (0xC97C30), 2 coches suprimidos, 3 focos, 4 temporizador,
# 5 punteros de pools, 6 marca del banco
REGIONS = ((0xB4E7E0, 0x204), (0xC97C28, 12), (0xA44940, 0xA0), (0xA94D68, 8 * 0x7C), (0xB7CB84, 4), (0xB74490, 0x10), (0xA9B0B0, 1))
EXT_OFFSET = 0xB4E9E0
DEV_AT = 0xC97C28
RENDER_SURFACE_AT = 0xC97C30
# regiones que toca cada funcion (en el orden del banco, la marca siempre al final)
FN_REGIONS = {ATTACH: (0, 6), TEXALPHA: (0, 6), ISCUBE: (0, 6), LOCK: (0, 1, 6), UNLOCK: (0, 1, 6), PALETTE: (0, 1, 6), SUPPRESSED: (2, 6), SLBETWEEN: (3, 6), SLENTITY: (3, 6), SLPOINT: (3, 6), CAMERA: (4, 6),
              MISSIONRM: (5, 6)}
# ids de los objetos COM falsos
(C_DEVICE, C_TEXTURE, C_CUBE, C_RENDER, C_SWAP, C_L0, C_L1, C_L2, C_L3, C_O0, C_O1) = range(11)
EV_FLUSH, EV_DECCTOR, EV_DECSET, EV_MOVING, EV_BRAINSINIT, EV_TASKDTOR, EV_TASKDELETE = 0x536E10, 0x6040C0, 0x600530, 0x542800, 0x46A8C0, 0x61A3A0, 0x61A5B0
EV_THINGIDX, EV_CLEANUP, EV_REGREF, EV_BASEMODEL = 0x4839A0, 0x571A00, 0x571B70, 0x4C4A60
EV_DYNGEO, EV_INDEXBUF, EV_DECL, EV_VTXDESTROY, EV_DYNVTX, EV_REGPLUGIN, EV_MIPMAPS = 0x74E9E0, 0x730D30, 0x7FAC10, 0x7F56A0, 0x7F5AE0, 0x7FB0B0, 0x7F3CD0
EV_COM = 0xC0A00000
(COM_RELEASE, COM_GETDESC, COM_LOCKRECT, COM_UNLOCKRECT, COM_GETLEVEL, COM_GETFACE, COM_CREATE, COM_GETRTDATA, COM_UPDATE, COM_SETPAL) = range(1, 11)


# ---------------------------------------------------------------------------------------------- imagen del EXE canonico (solo lectura)
class Exe:
    def __init__(self, path=EXE_PATH):
        self.data = open(path, 'rb').read()
        e = struct.unpack_from('<I', self.data, 0x3C)[0]
        nsec = struct.unpack_from('<H', self.data, e + 6)[0]
        opt = struct.unpack_from('<H', self.data, e + 20)[0]
        self.base = struct.unpack_from('<I', self.data, e + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            _, vs, va, rs, ro = struct.unpack_from('<8sIIII', self.data, e + 24 + opt + i * 40)
            self.secs.append((va + self.base, vs, rs, ro))

    def rd(self, va, n):
        for v, vs, rs, ro in self.secs:
            if v <= va < v + max(vs, rs):
                return self.data[ro + va - v: ro + va - v + n] if va - v < rs else bytes(n)
        raise ValueError('direccion fuera de la imagen: %x' % va)

    def u32(self, va):
        return struct.unpack('<I', self.rd(va, 4))[0]


_TABLES = None


def tables():
    """tablas de saltos de 0x4C9F90 / 0x4CA290, constante 2^32 de 0x858C54 y escrituras de la tabla de formatos de 0x4C9AB0, leidas del EXE canonico"""
    global _TABLES
    if _TABLES is None:
        exe = Exe()
        lock_jt = [exe.u32(0x4CA278 + 4 * i) for i in range(6)]
        unlock_jt = [exe.u32(0x4CA4C0 + 4 * i) for i in range(6)]
        # destinos: 0x4C9FFE = textura (0 y 4), 0x4CA0AA = camara (2 y 5), 0x4CA26E = fallo (1 y 3)
        assert lock_jt == [0x4C9FFE, 0x4CA26E, 0x4CA0AA, 0x4CA26E, 0x4C9FFE, 0x4CA0AA], [hex(x) for x in lock_jt]
        assert unlock_jt == [0x4CA2CC, 0x4CA4B5, 0x4CA342, 0x4CA4B5, 0x4CA2CC, 0x4CA342], [hex(x) for x in unlock_jt]
        two32 = exe.u32(0x858C54)
        assert two32 == 0x4F800000
        _TABLES = (two32, attach_writes(exe))
    return _TABLES


def attach_writes(exe):
    """escrituras de _rwD3D9RasterPluginAttach tras comprobar el desplazamiento (0x4C9ADA..): mini ejecutor x86 sobre los bytes del EXE (mov a registros, mov de byte/palabra a una direccion absoluta, rep stosd)"""
    code = exe.rd(0x4C9ADA, 0x4C9E77 - 0x4C9ADA)
    regs = {'eax': 0, 'ebx': 0, 'ecx': 0, 'edx': 0, 'esi': 0, 'edi': 0}   # ebx = 0 (xor ebx,ebx al entrar)
    names8 = {0: 'eax', 1: 'ecx', 2: 'edx', 3: 'ebx'}
    names16 = {0: 'eax', 1: 'ecx', 2: 'edx', 3: 'ebx', 6: 'esi', 7: 'edi'}
    out = []   # (direccion, bytes)
    i = 0
    while True:
        op = code[i]
        if op in (0x56, 0x57, 0x5E, 0x5F):                      # push/pop esi/edi
            i += 1
        elif op == 0x5B:                                       # pop ebx
            i += 1
            assert code[i] == 0xC3, 'ret esperado'
            break
        elif op == 0x33 and code[i + 1] == 0xC0:               # xor eax,eax
            regs['eax'] = 0; i += 2
        elif op == 0xB8:
            regs['eax'] = struct.unpack_from('<I', code, i + 1)[0]; i += 5
        elif op == 0xB9:
            regs['ecx'] = struct.unpack_from('<I', code, i + 1)[0]; i += 5
        elif op == 0xBE:
            regs['esi'] = struct.unpack_from('<I', code, i + 1)[0]; i += 5
        elif op == 0xBF:
            regs['edi'] = struct.unpack_from('<I', code, i + 1)[0]; i += 5
        elif op in (0xB0, 0xB1, 0xB2, 0xB3):                   # mov r8, imm8 (al, cl, dl, bl)
            r = names8[op - 0xB0]
            regs[r] = (regs[r] & 0xFFFFFF00) | code[i + 1]; i += 2
        elif op == 0xF3 and code[i + 1] == 0xAB:               # rep stosd
            out.append((regs['edi'], struct.pack('<I', regs['eax']) * regs['ecx']))
            regs['edi'] += 4 * regs['ecx']; regs['ecx'] = 0; i += 2
        elif op == 0xC6 and code[i + 1] == 0x05:               # mov byte [abs], imm8
            out.append((struct.unpack_from('<I', code, i + 2)[0], bytes([code[i + 6]]))); i += 7
        elif op == 0x88 and (code[i + 1] & 0xC7) == 0x05:      # mov [abs], r8
            r = names8[(code[i + 1] >> 3) & 7]
            out.append((struct.unpack_from('<I', code, i + 2)[0], bytes([regs[r] & 0xFF]))); i += 6
        elif op == 0xA2:                                       # mov [abs], al
            out.append((struct.unpack_from('<I', code, i + 1)[0], bytes([regs['eax'] & 0xFF]))); i += 5
        elif op == 0x66 and code[i + 1] == 0xA3:               # mov [abs], ax
            out.append((struct.unpack_from('<I', code, i + 2)[0], struct.pack('<H', regs['eax'] & 0xFFFF))); i += 6
        elif op == 0x66 and code[i + 1] == 0xC7 and code[i + 2] == 0x05:   # mov word [abs], imm16
            out.append((struct.unpack_from('<I', code, i + 3)[0], code[i + 7:i + 9])); i += 9
        elif op == 0x66 and code[i + 1] == 0x89 and (code[i + 2] & 0xC7) == 0x05:   # mov word [abs], r16
            r = names16[(code[i + 2] >> 3) & 7]
            out.append((struct.unpack_from('<I', code, i + 3)[0], struct.pack('<H', regs[r] & 0xFFFF))); i += 7
        else:
            raise ValueError('instruccion no prevista en 0x%X: %s' % (0x4C9ADA + i, code[i:i + 8].hex()))
    return out


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
    return v - (1 << 32) if v >= 1 << 31 else v


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


class Mem:
    """arena (o region) dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, guard=GUARD):
        self.kind, self.seed, self.guard, self.m, self.r = kind, seed, guard, {}, {}
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
        return sum(self.get(self.guard + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[self.guard + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self, absolute=0):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - self.guard
            if self.m[i] != ref:
                out.append([rel + absolute, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba) y las regiones globales: lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.fakes = tuple(c.get('fakes') or DEFAULT_FAKES)
        self.mem = tuple(Mem(a[2], a[92 + i]) for i in range(4))
        self.reg = [Mem(a[2], (a[90] + r * 17) & M32, guard=0) for r in range(len(REGIONS))]

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return self.mem[i], addr - b - GUARD
        for r, (at, n) in enumerate(REGIONS):
            if at <= addr < at + n:
                return self.reg[r], addr - at
        raise ValueError('direccion fuera de las arenas y regiones: %x' % addr)

    def rd(self, addr, n=4):
        m, off = self.locate(addr & M32)
        return m.rd(off, n)

    def wr(self, addr, n, v):
        m, off = self.locate(addr & M32)
        m.wr(off, n, v)

    def bytes(self, addr, n):
        m, off = self.locate(addr & M32)
        return bytes(m.get(m.guard + off + j) for j in range(n))

    def tagv(self, v):
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def fake(self, i):
        return self.fakes[i]

    def com_id(self, ptr):
        ptr &= M32
        for i, f in enumerate(self.fakes):
            if f == ptr:
                return i
        return ptr

    def snap(self):
        for m in self.mem:
            m.snap()
        for m in self.reg:
            m.snap()


# ---------------------------------------------------------------------------------------------- x87 a 24 bits (racionales exactos)
def r24(x):
    """Redondeo exacto (al par) de un racional a 24 bits de mantisa, exponente sin limite (x87 con PC=24)."""
    x = Fraction(x)
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    m = a / ulp
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return sign * q * ulp


class X:
    """valor x87: ('f', Fraction) finito, ('i', +1/-1) infinito, ('n',) NaN"""
    __slots__ = ('k', 'v')

    def __init__(self, k, v=None):
        self.k, self.v = k, v


def fld(bits):
    bits &= M32
    sign = -1 if bits >> 31 else 1
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        return X('n') if man else X('i', sign)
    if exp == 0:
        return X('f', sign * Fraction(man, 1 << 149))
    return X('f', sign * Fraction(man | 0x800000, 1 << 23) * (Fraction(2) ** (exp - 127)))


def xneg(a):
    return a if a.k == 'n' else (X('i', -a.v) if a.k == 'i' else X('f', -a.v))


def xadd(a, b):
    if a.k == 'n' or b.k == 'n':
        return X('n')
    if a.k == 'i' or b.k == 'i':
        if a.k == 'i' and b.k == 'i':
            return a if a.v == b.v else X('n')
        return a if a.k == 'i' else b
    return X('f', r24(a.v + b.v))


def xsub(a, b):
    return xadd(a, xneg(b))


def xmul(a, b):
    if a.k == 'n' or b.k == 'n':
        return X('n')
    if a.k == 'i' or b.k == 'i':
        za = a.k == 'f' and a.v == 0
        zb = b.k == 'f' and b.v == 0
        if za or zb:
            return X('n')
        sa = a.v if a.k == 'i' else (1 if a.v > 0 else -1)
        sb = b.v if b.k == 'i' else (1 if b.v > 0 else -1)
        return X('i', sa * sb)
    return X('f', r24(a.v * b.v))


def xle(a, b):
    """a <= b ordenado (fcom + test ah,41h; jp: C0 o C3 exactamente uno); con NaN falso"""
    if a.k == 'n' or b.k == 'n':
        return False
    ka = (a.v, Fraction(0)) if a.k == 'i' else (0, a.v)
    kb = (b.v, Fraction(0)) if b.k == 'i' else (0, b.v)
    return ka <= kb


def sar32(v, n):
    return (signed(v) >> n) & M32


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    two32, attach = tables()
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, threw = [], None, False
    o = a[0]
    this = W.raw(0, o)
    tg = W.tagv
    W.reg[6].wr(0, 1, 0x5A)

    def ev(kind, *vals):
        events.append([kind] + [v & M32 for v in vals])

    if k not in (LOCK, UNLOCK, PALETTE):
        A.wr(o, 4, a[3])
    marks.append('this_unaligned' if o % 4 else 'this_aligned')
    if k == ENTRYDTOR:
        W.snap()
        ev(EV_FLUSH, tg(this), A.rd(o, 4))
        A.wr(o, 4, a[5])
        marks.append('write_zero' if a[5] == 0 else 'write_other')
    elif k == POOLGETAT:
        objects = W.raw(2, a[9]) if a[8] else a[7]
        slot = signed(a[4])
        A.wr(o, 4, objects); A.wr(o + 4, 4, (W.raw(1, a[6]) - slot) & M32)
        B.wr(a[6], 1, a[5])
        ref = ((slot << 8) | (a[5] if a[10] else a[11])) & M32
        W.snap()
        rs = signed(ref) >> 8
        flag = W.rd((A.rd(o + 4, 4) + rs) & M32, 1)
        if flag == (ref & 0xFF):
            ret = tg((A.rd(o, 4) + rs * 0x19C) & M32)
            marks.append('match')
        else:
            ret = 0
            marks.append('no_match')
        marks.append('slot_negative' if rs < 0 else 'slot_nonneg')
        if a[8]:
            marks.append('objects_arena')
    elif k == POOLGETREF:
        s = signed(a[4])
        diff = s * 412 + (-a[7] if s < 0 else a[7])
        obj = W.raw(2, a[9]) if a[8] else a[9]
        A.wr(o, 4, (obj - diff) & M32); A.wr(o + 4, 4, (W.raw(1, a[6]) - s) & M32)
        B.wr(a[6], 1, a[5])
        W.snap()
        # secuencia del EXE: sub edx,eax; imul edx (eax = 13E22CBDh); sar edx,5; eax = edx >> 31; add edx,eax
        d = (obj - A.rd(o, 4)) & M32
        hi = (signed(d) * 0x13E22CBD) >> 32
        hi = signed(hi & M32) >> 5
        slot = (hi + ((hi & M32) >> 31)) & M32
        flag = W.rd((A.rd(o + 4, 4) + slot) & M32, 1)
        ret = (flag + ((slot << 8) & M32)) & M32
        marks.append('slot_negative' if signed(slot) < 0 else ('slot_zero' if slot == 0 else 'slot_positive'))
        if a[8]:
            marks.append('obj_arena')
        q = abs(signed(d)) // 412      # la secuencia magica coincide con la division con signo exacta
        assert signed(slot) == (-q if signed(d) < 0 else q), (d, slot)
    elif k == DECISION:
        W.snap()
        count = 0
        for i in range(41):
            ev(EV_DECCTOR, tg(this + 0x3C * i))
            A.wr(o + 0x3C * i, 4, a[4] ^ count)
            count += 1
        for i in range(41):
            ev(EV_DECSET, tg(this + 0x3C * i))
            A.wr(o + 0x3C * i + 4, 4, a[5] ^ count)
            count += 1
        ret = tg(this)
    elif k == MISSIONRM:
        entries = bytes.fromhex(c['s'])
        for i in range(0x258):
            A.wr(o + i, 1, entries[i])
        A.wr(o + 0x258, 1, a[11])
        slot = a[6]
        sizes = (0xA18, 0x7C4, 0x19C)
        for i in range(3):
            B.wr(a[12 + i], 4, (W.raw(3, a[8]) - slot * sizes[i]) & M32)
            B.wr(a[12 + i] + 4, 4, (W.raw(2, a[9]) - slot) & M32)
        Cc.wr(a[9], 1, a[7])
        D.wr(a[8] + 0x1C, 4, a[10])
        W.wr(0xB74494, 4, W.raw(1, a[12])); W.wr(0xB74490, 4, W.raw(1, a[13])); W.wr(0xB7449C, 4, W.raw(1, a[14]))
        W.snap()
        handle, tpe = a[4], a[5] & 0xFF
        poolsel = {1: (0xB74494, 0xA18), 2: (0xB74490, 0x7C4), 3: (0xB7449C, 0x19C)}
        matches = 0
        for i in range(75):
            et = A.rd(o + 8 * i, 1)
            eh = A.rd(o + 8 * i + 4, 4)
            if et != tpe or eh != handle:
                continue
            matches += 1
            if et in poolsel:
                pool = W.rd(poolsel[et][0], 4)
                flags = W.rd((pool + 4) & M32, 4)
                slot_e = signed(handle) >> 8
                if W.rd((flags + slot_e) & M32, 1) == (handle & 0xFF):
                    ent = (slot_e * poolsel[et][1] + W.rd(pool, 4)) & M32
                    marks.append('pool_hit')
                    if ent:
                        fl = W.rd((ent + 0x1C) & M32, 4)
                        if fl & 0x40000:
                            fl &= 0xFFFBFFFF
                            W.wr(ent + 0x1C, 4, fl)
                            marks.append('wait_bit')
                            if not (fl & 4):
                                ev(EV_MOVING, tg(ent))
                                marks.append('moving_list')
                            else:
                                marks.append('static')
                else:
                    marks.append('pool_miss')
            else:
                marks.append('no_pool_type')
            A.wr(o + 8 * i + 4, 4, 0)
            A.wr(o + 8 * i, 1, 0)
            A.wr(o + 0x258, 1, (A.rd(o + 0x258, 1) - 1) & 0xFF)
        marks.append('matches%d' % min(matches, 3))
        marks.append('type%d' % (tpe if tpe < 4 else 9))
    elif k == SUPPRESSED:
        for i in range(40):
            W.wr(0xA44940 + 4 * i, 4, a[8 + i])
        W.snap()
        n = 0
        for i in range(40):
            if W.rd(0xA44940 + 4 * i, 4) == a[4]:
                W.wr(0xA44940 + 4 * i, 4, 0xFFFFFFFF)
                n += 1
        marks.append('replaced%d' % min(n, 3))
    elif k == BRAINS:
        W.snap()
        ev(EV_BRAINSINIT, tg(this))
        A.wr(o, 4, a[4])
        ret = tg(this)
    elif k == CAMERA:
        W.wr(0xB7CB84, 4, a[4])
        A.wr(o + 0xC60, 4, a[5]); A.wr(o + 0xCC0, 4, a[6])
        W.snap()
        t = signed(W.rd(0xB7CB84, 4))
        if t < 0:
            v = xadd(X('f', Fraction(t)), fld(two32))
            marks.append('time_negative')
        else:
            v = X('f', Fraction(t))
            marks.append('time_nonneg')
        end = fld(A.rd(o + (0xC60 if fn == 7 else 0xCC0), 4))
        r = xle(v, end)
        ret = 1 if r else 0
        marks.append('ret_true' if r else 'ret_false')
        if end.k == 'n':
            marks.append('end_nan')
        elif end.k == 'i':
            marks.append('end_inf')
        elif v.k == 'f' and end.k == 'f' and v.v == end.v:
            marks.append('equal')
    elif k == SDD:
        W.snap()
        ev(EV_TASKDTOR, tg(this))
        A.wr(o, 4, a[5])
        if a[4] & 1:
            ev(EV_TASKDELETE, tg(this), A.rd(o, 4))
            marks.append('flag_bit0_set')
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tg(this)
    elif k == BOXSTUB:
        W.snap()
    elif k in (SLBETWEEN, SLENTITY, SLPOINT):
        idx = signed(a[5])
        if 0 <= idx < 8:
            L = 0xA94D68 + 0x7C * idx
            W.wr(L + 0x14, 4, a[7]); W.wr(L + 0x18, 4, a[8]); W.wr(L + 0x48, 4, a[6])
        W.snap()
        ev(EV_THINGIDX, a[4], 2)
        if idx < 0:
            marks.append('index_negative')
        else:
            marks.append('index_ok')
            L = 0xA94D68 + 0x7C * idx
            ref = L + 0x48
            prev = W.rd(ref, 4)

            def cleanup():
                if prev:
                    ev(EV_CLEANUP, prev, ref)
                    marks.append('cleanup')
                else:
                    marks.append('no_cleanup')
            if k == SLBETWEEN:
                x1, y1, z1, x2, y2, z2, speed = a[9:16]
                X0, Y0 = fld(W.rd(L + 0x14, 4)), fld(W.rd(L + 0x18, 4))
                aa = xsub(X0, fld(x1)); bb = xsub(Y0, fld(y1))
                first = xadd(xmul(bb, bb), xmul(aa, aa))
                cc = xsub(X0, fld(x2)); dd = xsub(Y0, fld(y2))
                second = xadd(xmul(dd, dd), xmul(cc, cc))
                near_first = xle(first, second)
                for off, v in ((0x28, x1), (0x2C, y1), (0x30, z1), (0x34, x2), (0x38, y2), (0x3C, z2)):
                    W.wr(L + off, 4, v)
                W.wr(L + 3, 1, 0x81 if near_first else 0x82)
                W.wr(L + 0x40, 4, speed)
                marks.append('first_near' if near_first else 'first_far')
                if first.k == 'n' or second.k == 'n':
                    marks.append('unordered')
                elif first.k == 'f' and second.k == 'f' and first.v == second.v:
                    marks.append('tie')
                cleanup()
                W.wr(ref, 4, 0)
            elif k == SLENTITY:
                entity, speed = a[9], a[10]
                W.wr(L + 3, 1, 0x83)
                for off in (0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C):
                    W.wr(L + off, 4, 0)
                W.wr(L + 0x40, 4, speed)
                cleanup()
                W.wr(ref, 4, entity)
                ev(EV_REGREF, entity, ref)
                marks.append('entity_null' if entity == 0 else 'entity_set')
            else:
                x, y, z, speed = a[9:13]
                for off, v in ((0x28, x), (0x2C, y), (0x30, z), (0x34, 0), (0x38, 0), (0x3C, 0)):
                    W.wr(L + off, 4, v)
                W.wr(L + 3, 1, 0x84)
                W.wr(L + 0x40, 4, speed)
                cleanup()
                W.wr(ref, 4, 0)
    elif k == PEDMI:
        W.snap()
        ev(EV_BASEMODEL, tg(this))
        A.wr(o, 4, a[4]); A.wr(o + 0x34, 4, a[5])
        A.wr(o, 4, 0x85BDC0); A.wr(o + 0x34, 4, 0)
        ret = tg(this)
    elif k == HEADER:
        A.wr(o + 0xC, 4, W.raw(1, a[5]) if a[4] else 0)
        if a[4]:
            B.wr(a[5], 1, a[6]); B.wr(a[5] + 0x18, 4, a[7])
        A.wr(o + 0x20, 4, a[8]); A.wr(o + 0x4C, 4, a[9]); A.wr(o + 0x54, 4, a[10])
        for i in range(2):
            b = 11 + 6 * i
            base = o + 0x28 + 0x10 * i
            A.wr(base, 4, a[b]); A.wr(base + 4, 4, a[b + 1]); A.wr(base + 8, 4, a[b + 2]); A.wr(base + 0xC, 2, a[b + 3]); A.wr(base + 0xE, 1, a[b + 4]); A.wr(base + 0xF, 1, a[b + 5])
        W.snap()
        owner = A.rd(o + 0xC, 4)
        if owner:
            t = W.rd(owner, 1)
            if t != 0xFF:
                for i in range(2):
                    if A.rd(o + 0x36 + 0x10 * i, 1) == 0:
                        if t == 8:
                            ev(EV_DYNGEO, tg(owner)); marks.append('dyn_owner')
                        elif t == 1:
                            ev(EV_DYNGEO, tg(W.rd(owner + 0x18, 4))); marks.append('dyn_clump')
                        else:
                            marks.append('dyn_other')
                        break
            else:
                marks.append('owner_ff')
        else:
            marks.append('owner_null')
        ib = A.rd(o + 0x20, 4)
        if ib:
            ev(EV_INDEXBUF, tg(ib)); A.wr(o + 0x20, 4, 0); marks.append('index_buffer')
        dc = A.rd(o + 0x4C, 4)
        if dc:
            ev(EV_DECL, tg(dc)); A.wr(o + 0x4C, 4, 0); marks.append('declaration')
        for i in range(2):
            s = o + 0x28 + 0x10 * i
            buf = A.rd(s, 4)
            if buf:
                if A.rd(s + 0xE, 1) != 0:
                    stride = A.rd(s + 8, 4)
                    ev(EV_VTXDESTROY, stride, (A.rd(o + 0x54, 4) * stride) & M32, tg(buf), A.rd(s + 4, 4))
                    marks.append('stream_managed')
                elif A.rd(s + 4, 4) == 0 and A.rd(s + 0xF, 1) == 0:
                    ev(EV_DYNVTX, tg(buf))
                    marks.append('stream_dynamic')
                else:
                    marks.append('stream_kept')
                A.wr(s, 4, 0)
            A.wr(s + 4, 4, 0); A.wr(s + 8, 4, 0); A.wr(s + 0xC, 2, 0); A.wr(s + 0xE, 1, 0); A.wr(s + 0xF, 1, 0)
    elif k == ATTACH:
        W.snap()
        ev(EV_REGPLUGIN, 0x24, 0x40C, 0x4C9A60, 0x4C9A80, 0)
        W.wr(EXT_OFFSET, 4, a[4])
        if signed(a[4]) < 0:
            ret = 0
            marks.append('offset_negative')
        else:
            for addr, data in attach:
                for j, byte in enumerate(data):
                    W.wr(addr + j, 1, byte)
            ret = 1
            marks.append('offset_ok')
    elif k == TEXALPHA:
        W.wr(EXT_OFFSET, 4, a[6])
        A.wr(o, 4, W.raw(1, a[5]) if a[4] else 0)
        if a[4]:
            B.wr(a[5] + a[6] + 8, 1, a[7]); B.wr(a[5] + a[6] + 9, 1, a[8])
        W.snap()
        raster = A.rd(o, 4)
        if raster == 0:
            ret = 0
            marks.append('raster_null')
        else:
            ret = W.rd((raster + W.rd(EXT_OFFSET, 4) + 8) & M32, 1)
            marks.append('alpha_nonzero' if ret else 'alpha_zero')
    elif k == ISCUBE:
        W.wr(EXT_OFFSET, 4, a[6])
        B.wr(a[5] + a[6] + 9, 1, a[7])
        W.snap()
        ret = W.rd((W.raw(1, a[5]) + W.rd(EXT_OFFSET, 4) + 9) & M32, 1) & 0xF
        marks.append('cube' if ret else 'not_cube')
    elif k in (LOCK, UNLOCK, PALETTE):
        ret = raster_fn(a, k, W, ev, marks, o, this)
    gch = []
    for r in FN_REGIONS.get(k, (6,)):
        gch += W.reg[r].changes(absolute=REGIONS[r][0])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'vptrs': [], 'gchanges': gch, 'ret': ret, 'threw': threw, 'marks': marks}


# ---------------------------------------------------------------------------------------------- rasters D3D9
def raster_fn(a, k, W, ev, marks, o, this):
    A, B, Cc, D = W.mem
    ar = (A, B, Cc, D)
    off = (a[0], a[5], a[6], a[7])
    X0 = a[8]
    depth = a[4] if k != PALETTE else 0
    for kk in range(depth + 1):
        ar[kk].wr(off[kk], 4, W.raw(kk + 1, off[kk + 1]) if kk < depth else W.raw(kk, off[kk]))
    A.wr(o + 4, 4, a[9]); A.wr(o + 8, 4, a[46]); A.wr(o + 0xC, 4, a[13]); A.wr(o + 0x10, 4, a[14]); A.wr(o + 0x18, 4, a[19])
    A.wr(o + 0x1C, 2, a[15]); A.wr(o + 0x1E, 2, a[16]); A.wr(o + 0x20, 1, a[10]); A.wr(o + 0x22, 1, a[11]); A.wr(o + 0x23, 1, a[12])
    A.wr(o + 0x28, 4, a[17]); A.wr(o + 0x2C, 4, a[18])
    e0 = o + X0
    A.wr(e0 + 9, 1, a[23]); A.wr(e0 + 0xA, 1, a[24]); A.wr(e0 + 0xB, 1, a[25])
    A.wr(e0 + 0xC, 4, W.fake(C_L0 + a[26]))
    A.wr(e0 + 4, 4, W.raw(2, a[43]) if a[45] else 0)
    et = off[depth] + X0
    cube = (a[21] & 0xF) != 0
    texture = W.fake(C_CUBE) if cube else W.fake(C_TEXTURE)
    if (a[10] & 7) == 2:
        texture = W.fake(C_SWAP) if a[22] != 0 else 0xDEADC0DE
    ar[depth].wr(et + 0, 4, texture); ar[depth].wr(et + 9, 1, a[21]); ar[depth].wr(et + 0x1C, 4, a[22])
    W.wr(EXT_OFFSET, 4, X0)
    W.wr(DEV_AT, 4, W.fake(C_DEVICE)); W.wr(RENDER_SURFACE_AT, 4, W.fake(C_RENDER))
    hr_gsl, gsl_sel, hr_lock, pitch, bits, hr_create, create_sel = a[27], a[28] & 3, a[29], a[30], a[31], a[32], a[33] & 1
    desc = a[34:42]
    if k == PALETTE:
        Cc.wr(a[43] + 0x400, 4, a[44])
    W.snap()
    ok = lambda hr: signed(hr) >= 0
    tg = W.tagv
    STK = STACK_TAG
    raster = this
    T = a[10] & 7                                    # tipo (cType & 7)
    xo = signed(W.rd(EXT_OFFSET, 4))
    ext = (raster + xo) & M32
    marks.append('type%d' % T)

    def parent_top(start):
        t = start
        for _ in range(8):
            nxt = W.rd(t, 4)
            if nxt == t:
                return t
            t = nxt
        raise ValueError('cadena de padres sin fin')

    def get_surface(texture, cubeface, level, outptr):
        """GetSurfaceLevel / GetCubeMapSurface (el mismo hueco 18 de la tabla virtual) sobre el objeto COM falso; devuelve (HRESULT, superficie que escribe en la salida o None)"""
        if cubeface & 0xF:
            ev(EV_COM + COM_GETFACE, W.com_id(texture), cubeface >> 4, level, outptr)
        else:
            ev(EV_COM + COM_GETLEVEL, W.com_id(texture), level, outptr)
        return hr_gsl, (W.fake(C_L0 + gsl_sel) if ok(hr_gsl) else None)

    def lock_rect(surface, top_is_raster):
        rect = None
        if not top_is_raster:
            left, top = sext16(W.rd(raster + 0x1E - 2, 2)), sext16(W.rd(raster + 0x1E, 2))
            rect = (left & M32, top & M32, (W.rd(raster + 0xC, 4) + left) & M32, (W.rd(raster + 0x10, 4) + top) & M32)
        lockflags = 0x800 if (a[20] & 1) else 0x8810
        ev(EV_COM + COM_LOCKRECT, W.com_id(surface), tg(ext + 0x10), 0 if rect is None else 1, *(rect if rect else (0, 0, 0, 0)), lockflags)
        if ok(hr_lock):
            W.wr(ext + 0x10, 4, pitch); W.wr(ext + 0x14, 4, bits)
        return hr_lock

    if k == PALETTE:
        if T not in (0, 4, 5):
            marks.append('bad_type')
            return 0
        priv = W.rd(raster + 0x22, 1)
        if priv & 0x10:
            pal = W.rd(ext + 4, 4)
            number = W.rd((pal + 0x400) & M32, 4)
            ev(EV_COM + COM_SETPAL, W.com_id(W.rd(DEV_AT, 4)), number, tg(pal), fnv(W.bytes(pal, 0x400)))
            marks.append('set_palette')
        else:
            marks.append('no_palette')
        W.wr(raster + 0x22, 1, priv & 0xE7)
        W.wr(raster + 8, 4, 0)
        return 1

    if k == LOCK:
        pixels_out = W.raw(2, a[42])
        mode = a[20]
        if W.rd(raster + 4, 4) != 0:
            marks.append('already_locked')
            return 0
        top = parent_top(W.rd(raster, 4))
        mip = (mode >> 8) & 0xFF
        write = mode & 1
        read = (mode >> 1) & 1
        topext = (top + xo) & M32
        if T in (1, 3) or T > 5:
            marks.append('bad_type')
            return 0
        top_is_raster = top == raster
        marks.append('top_is_raster' if top_is_raster else 'sub_raster')
        marks.append('write' if write else 'read_only')
        if T in (0, 4):
            tex = W.rd(topext, 4)
            hr, surf = get_surface(tex, W.rd(topext + 9, 1), mip, tg(ext + 0xC))
            if surf is not None:
                W.wr(ext + 0xC, 4, surf)
            if not ok(hr):
                marks.append('gsl_fail')
                return 0
            hr = lock_rect(W.rd(ext + 0xC, 4), top_is_raster)
            if not ok(hr):
                marks.append('lock_fail')
                return 0
            if mode & 2:
                W.wr(raster + 0x22, 1, W.rd(raster + 0x22, 1) | 2)
            if write:
                W.wr(raster + 0x22, 1, W.rd(raster + 0x22, 1) | 4)
        else:
            if T == 5:
                tex = W.rd(topext, 4)
                hr, source = get_surface(tex, W.rd(topext + 9, 1), mip, STK)
                if not ok(hr):
                    marks.append('gsl_fail')
                    return 0
            else:
                source = W.rd(topext, 4) if W.rd(topext + 0x1C, 4) != 0 else W.rd(RENDER_SURFACE_AT, 4)
            ev(EV_COM + COM_GETDESC, W.com_id(source), STK)
            fmt, width, height = desc[0], desc[6], desc[7]
            dev = W.rd(DEV_AT, 4)
            ev(EV_COM + COM_CREATE, W.com_id(dev), width, height, fmt, 2, tg(ext + 0xC), 0)
            result = hr_create
            if ok(result):
                W.wr(ext + 0xC, 4, W.fake(C_O0 + create_sel))
                if read:
                    ev(EV_COM + COM_GETRTDATA, W.com_id(dev), W.com_id(source), W.com_id(W.rd(ext + 0xC, 4)))
                result = lock_rect(W.rd(ext + 0xC, 4), top_is_raster)
                if ok(result):
                    if read:
                        W.wr(raster + 0x22, 1, W.rd(raster + 0x22, 1) | 2)
                    if write:
                        W.wr(raster + 0x22, 1, W.rd(raster + 0x22, 1) | 4)
                else:
                    ev(EV_COM + COM_RELEASE, W.com_id(W.rd(ext + 0xC, 4)))
                    marks.append('lock_fail')
            else:
                marks.append('create_fail')
            if T == 5:
                ev(EV_COM + COM_RELEASE, W.com_id(source))
            if not ok(result):
                return 0
        # exito
        W.wr(raster + 4, 4, W.rd(ext + 0x14, 4))
        width0 = W.rd(raster + 0xC, 4)
        height0 = W.rd(raster + 0x10, 4)
        W.wr(raster + 0x28, 4, width0)
        W.wr(raster + 0x2C, 4, height0)
        sh = mip & 31
        nw, nh = sar32(width0, sh), sar32(height0, sh)
        W.wr(raster + 0xC, 4, nw); W.wr(raster + 0x10, 4, nh)
        if nw == 0:
            W.wr(raster + 0xC, 4, 1)
        if nh == 0:
            W.wr(raster + 0x10, 4, 1)
        W.wr(raster + 0x18, 4, W.rd(ext + 0x10, 4))
        W.wr(ext + 0xB, 1, mip)
        W.wr(pixels_out, 4, W.rd(raster + 4, 4))
        marks.append('locked_ok')
        return 1

    # UNLOCK
    if W.rd(raster + 4, 4) == 0:
        marks.append('not_locked')
        return 0
    if T > 5 or T in (1, 3):
        marks.append('bad_type')
        return 0

    def mips(top):
        ev(EV_MIPMAPS, tg(top), 0)

    if T in (0, 4):
        ev(EV_COM + COM_UNLOCKRECT, W.com_id(W.rd(ext + 0xC, 4)))
        ev(EV_COM + COM_RELEASE, W.com_id(W.rd(ext + 0xC, 4)))
        W.wr(raster + 0xC, 4, W.rd(raster + 0x28, 4)); W.wr(raster + 0x10, 4, W.rd(raster + 0x2C, 4))
        priv = W.rd(raster + 0x22, 1)
        W.wr(raster + 0x18, 4, 0); W.wr(raster + 4, 4, 0)
        if (priv & 4) and (W.rd(raster + 0x23, 1) & 0x10) and W.rd(ext + 0xB, 1) == 0 and (W.rd(ext + 0xA, 1) & 0xF) == 0:
            W.wr(raster + 0x22, 1, priv & 0xF9)
            W.wr(ext + 0xB, 1, 0xFF)
            mips(parent_top(W.rd(raster, 4)))
            marks.append('mipmaps')
            return 1
        W.wr(raster + 0x22, 1, priv & 0xF9)
        marks.append('no_mipmaps')
        return 1
    # tipos 2 y 5
    W.wr(raster + 0xC, 4, W.rd(raster + 0x28, 4)); W.wr(raster + 0x10, 4, W.rd(raster + 0x2C, 4))
    W.wr(raster + 0x18, 4, 0); W.wr(raster + 4, 4, 0)
    ev(EV_COM + COM_UNLOCKRECT, W.com_id(W.rd(ext + 0xC, 4)))
    if not (W.rd(raster + 0x22, 1) & 4):
        ev(EV_COM + COM_RELEASE, W.com_id(W.rd(ext + 0xC, 4)))
        W.wr(raster + 0x22, 1, W.rd(raster + 0x22, 1) & 0xF9)
        marks.append('read_only_release')
        return 1
    top = parent_top(W.rd(raster, 4))
    topext = (top + xo) & M32
    result = 0
    if T == 5:
        result, dest = get_surface(W.rd(topext, 4), W.rd(topext + 9, 1), W.rd(ext + 0xB, 1), STK)
    else:
        dest = W.rd(topext, 4) if W.rd(topext + 0x1C, 4) != 0 else W.rd(RENDER_SURFACE_AT, 4)
    if ok(result):
        dev = W.rd(DEV_AT, 4)
        src = W.rd(ext + 0xC, 4)
        if top == raster:
            ev(EV_COM + COM_UPDATE, W.com_id(dev), W.com_id(src), 0, 0, 0, 0, 0, W.com_id(dest), 0, 0, 0)
        else:
            left, topy = sext16(W.rd(raster + 0x1C, 2)), sext16(W.rd(raster + 0x1E, 2))
            right, bottom = (W.rd(raster + 0xC, 4) + left) & M32, (W.rd(raster + 0x10, 4) + topy) & M32
            ev(EV_COM + COM_UPDATE, W.com_id(dev), W.com_id(src), 1, left & M32, topy & M32, right, bottom, W.com_id(dest), 1, left & M32, topy & M32)
        if T == 5:
            ev(EV_COM + COM_RELEASE, W.com_id(dest))
        marks.append('copied')
    else:
        marks.append('dest_fail')
    ev(EV_COM + COM_RELEASE, W.com_id(W.rd(ext + 0xC, 4)))
    W.wr(raster + 0x22, 1, W.rd(raster + 0x22, 1) & 0xF9)
    if T == 5 and (W.rd(raster + 0x23, 1) & 0x10) and W.rd(ext + 0xB, 1) == 0:
        W.wr(ext + 0xB, 1, 0xFF)
        mips(top)
        marks.append('mipmaps')
    return 1


KEYS = [[] for _ in NAMES]
KEYS[0] = ['write_zero', 'write_other']
KEYS[1] = ['match', 'no_match', 'slot_negative', 'slot_nonneg', 'objects_arena']
KEYS[2] = ['slot_negative', 'slot_zero', 'slot_positive', 'obj_arena']
KEYS[4] = ['pool_hit', 'pool_miss', 'no_pool_type', 'wait_bit', 'moving_list', 'static', 'matches0', 'matches1', 'matches2', 'matches3', 'type0', 'type1', 'type2', 'type3', 'type9']
KEYS[5] = ['replaced0', 'replaced1', 'replaced2', 'replaced3']
KEYS[7] = KEYS[8] = ['time_negative', 'time_nonneg', 'ret_true', 'ret_false', 'end_nan', 'end_inf', 'equal']
KEYS[9] = ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits']
KEYS[11] = ['index_negative', 'index_ok', 'first_near', 'first_far', 'unordered', 'tie', 'cleanup', 'no_cleanup']
KEYS[12] = ['index_negative', 'index_ok', 'entity_null', 'entity_set', 'cleanup', 'no_cleanup']
KEYS[13] = ['index_negative', 'index_ok', 'cleanup', 'no_cleanup']
KEYS[15] = ['owner_null', 'owner_ff', 'dyn_owner', 'dyn_clump', 'dyn_other', 'index_buffer', 'declaration', 'stream_managed', 'stream_dynamic', 'stream_kept']
KEYS[16] = ['offset_negative', 'offset_ok']
KEYS[17] = ['raster_null', 'alpha_zero', 'alpha_nonzero']
KEYS[18] = ['cube', 'not_cube']
KEYS[19] = ['already_locked', 'bad_type', 'top_is_raster', 'sub_raster', 'write', 'read_only', 'gsl_fail', 'lock_fail', 'create_fail', 'locked_ok', 'type0', 'type2', 'type4', 'type5']
KEYS[20] = ['not_locked', 'bad_type', 'mipmaps', 'no_mipmaps', 'read_only_release', 'copied', 'dest_fail', 'type0', 'type2', 'type4', 'type5']
KEYS[21] = ['bad_type', 'set_palette', 'no_palette', 'type0', 'type4', 'type5']


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
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
