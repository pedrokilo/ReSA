"""Oraculo independiente (Python) de Fn196A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 196A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.
La unica dato que no puede calcular es la CPUID de la maquina: el banco la mide con el intrinseco __cpuid y la anota en la entrada de cada caso (campo `cpuid`).

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x6D63D0  CVehicle::GetNumContactWheels: mov eax,4; ret
   1  0x6D63E0  CVehicle::VehicleDamage: ret 18h
   2  0x6E2D90  CVehicle::operator delete(p): esi = [0B74494h]; ecx = p - [esi]; idx = ecx / 0A18h (imul 6572EC3h; sar 6; + bit de signo: division entera con signo, truncada hacia cero);
             [[esi+4] + idx] |= 80h; si idx < [esi+0Ch] (con signo): [esi+0Ch] = idx
   3  0x6E2DD0  CVehicle::operator delete(p, ref): igual (el segundo argumento no se lee)
   4  0x6E38A0  CVehicle destructor con borrado: call 6E2B40 (ecx = this); si el BYTE [esp+0Ch] & 1: el mismo borrado en linea con this; eax = this; ret 4
   5  0x5D24B0  CPlayerInfoSaveStructure::Construct(info): [ecx+0]=[info+B8h]; [ecx+4](w)=[info+130h](w); [ecx+6](b)=[info+DCh](b); [ecx+8]=[info+134h]; [ecx+0Ch]=[info+BCh];
             [ecx+10h](b)=[info+144h](b); [ecx+14h]=[info+C0h]; [ecx+18h]=[info+C4h]; [ecx+1Ch..24h](9 b)=[info+14Ch..154h]; [ecx+26h](w)=[info+156h](w)
   6  0x5D2560  CPlayerInfoSaveStructure::Extract(info): inversa; [info+144h](w) = movzx(byte [ecx+10h]); [info+130h](w) se escribe dos veces
   7  0x5D2620  CGangWarsSaveStructure::Construct: estaticos 0x96AB44..0x96ABD0 -> estructura (ver CONSTRUCT_COPIES); indices de zona por bucle (contadores WORD sin signo)
   8  0x5D2740  CGangWarsSaveStructure::Extract: inversa; indice -1 -> puntero nulo; si no base + indice * 11h (zona) o * 20h (navegacion)
   9  0x7457C0  Ps::GetCpuExtendedFeatures: CPUID(80000000h); si eax > 80000000h (sin signo): edx de CPUID(80000001h); si no 0; devuelve eso en EAX
  10  0x7457F0  Ps::GetCpuFeaturesAndVendor: CPUID 1, 80000000h, [80000001h], 0; guarda ebx, edx, ecx de la hoja 0 en 8D6238h / 8D623Ch / 8D6240h
  11  0x745A70  VideoMode::GetNumVideoModes: jmp 7F2CC0h (RwEngineGetNumVideoModes): devuelve su EAX
  12  0x53F910  CPad::SetDrunkInputDelay(delay): [ecx+74h] = delay
  13  0x5404F0  CPad::sub_5404F0: si WORD [ecx+10Ah] == 1 (tabla de saltos hasta 3; mayor: 0): devuelve ([ecx+12h] != 0 y [ecx+42h] == 0) (words); AL
  14  0x541D70  CPad::StopPadsShaking: ret
  15  0x541E50  CPad::UpdatePadsThunk: jmp 541DD0
  16  0x541E90  CMatrix::Rotate(vec*): llama a 59B460h (ecx = this) con x, y, z del vector (empujados z, y, x)
  17  0x541F40  CTimer::UpdateTimeStep(float): fld arg; fcomp [858C14h] (9.99999975e-06f); si C3|C0 son iguales (mayor o no ordenado): [0B7CB5Ch] = arg (bits); si no 3727C5ACh
  18  0x747000  WinInput::InputShutdown: eax = [[0C17054h] + 18h]; si no es nulo: Release (ranura 2) y [[0C17054h] + 18h] = 0 (psGlobal se relee)
  19  0x5FF3A0  CEventSeenCop cuerpo del destructor: [ecx] = 86CB58h; jmp 4AF890
  20  0x5FF3B0  CEventSeenCop::GetEventType: mov eax,48h
  21  0x5FF3C0  CEventSeenCop::CloneEditable: new 4B5620(18h); si nulo: 0; si no: 4AF820 (ecx = mem, argumento [this+14h]); [mem] = 86CB58h; devuelve mem
  22  0x5FF420  CEventSeenCop destructor con borrado: call 5FF3A0; si el BYTE [esp+8] & 1: 4B5630(this); eax = this
  23  0x5FF440  CEventHealthLow::Constructor: call 4AC450; [esi] = 86CBA0h; eax = this
  24  0x5FF460  CEventHealthLow cuerpo del destructor: [ecx] = 86CBA0h; jmp 4AC480

Los falsos anotan: [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos 0x4AF890, 0x4AC450, 0x4AC480, 0x5FF3A0); [0x6E2B40, ECX]; [0x4B5620, tamano]; [0x4B5630, argumento, dword];
[0x4AF820, ECX, argumento]; [0x7F2CC0]; [0x541DD0]; [0x59B460, ECX, x, y, z]; [0xD1, objeto COM] (Release).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CVehicle::GetNumContactWheels", "CVehicle::VehicleDamage", "CVehicle::operator delete-anon", "CVehicle::operator delete-poolRef", "CVehicle::ScalarDeletingDestructor",
         "CPlayerInfoSaveStructure::Construct", "CPlayerInfoSaveStructure::Extract", "CGangWarsSaveStructure::Construct", "CGangWarsSaveStructure::Extract", "Ps::GetCpuExtendedFeatures",
         "Ps::GetCpuFeaturesAndVendor", "VideoMode::GetNumVideoModes", "CPad::SetDrunkInputDelay", "CPad::sub_5404F0", "CPad::StopPadsShaking", "CPad::UpdatePadsThunk", "CMatrix::RotateByPointer",
         "CTimer::UpdateTimeStep", "Input::InputShutdown", "CEventSeenCop::DestructorBody", "CEventSeenCop::GetEventType", "CEventSeenCop::CloneEditable", "CEventSeenCop::ScalarDeletingDestructor",
         "CEventHealthLow::Constructor", "CEventHealthLow::DestructorBody"]
ENTRIES = (0x6D63D0, 0x6D63E0, 0x6E2D90, 0x6E2DD0, 0x6E38A0, 0x5D24B0, 0x5D2560, 0x5D2620, 0x5D2740, 0x7457C0, 0x7457F0, 0x745A70, 0x53F910, 0x5404F0, 0x541D70, 0x541E50, 0x541E90, 0x541F40,
           0x747000, 0x5FF3A0, 0x5FF3B0, 0x5FF3C0, 0x5FF420, 0x5FF440, 0x5FF460)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(CONST, VDMG, VDEL, VSDD, PICON, PIEXT, GWCON, GWEXT, CPUEXT, CPUVEND, VID, PADDRUNK, PADSUB, NOP, THUNK, ROTPTR, TIME, INSHUT, BODY, CLONESC, SDD, CTOR) = \
    'const vdmg vdel vsdd picon piext gwcon gwext cpuext cpuvend vid paddrunk padsub nop thunk rotptr time inshut body clonesc sdd ctor'.split()
KIND = [CONST, VDMG, VDEL, VDEL, VSDD, PICON, PIEXT, GWCON, GWEXT, CPUEXT, CPUVEND, VID, PADDRUNK, PADSUB, NOP, THUNK, ROTPTR, TIME, INSHUT, BODY, CONST, CLONESC, SDD, CTOR, BODY]
CONSTS = {0: 4, 20: 0x48}
# vtable que deja la funcion y la funcion base a la que llama (cuerpos)
BODY_INFO = {19: (0x4AF890,), 24: (0x4AC480,)}
BODY_VT = {19: 0x86CB58, 24: 0x86CBA0}
VEH_POOL, GW_BASE, GW_BYTES, ZONE_COUNT1, ZONE_COUNT2, ZONE_BASE1, ZONE_BASE2 = 0xB74494, 0x96AB44, 0x90, 0xBA1DE8, 0xBA3794, 0xBA1DF0, 0xBA3798
CPU_VENDOR, FLAG, TIMESTEP, PSGLOBAL = 0x8D6238, 0xA9B0B0, 0xB7CB5C, 0xC17054
VEH_SIZE = 0xA18
TIME_CONST = 0x3727C5AC

# (destino en la estructura, direccion del estatico, bytes) de CGangWarsSaveStructure::Construct, en el orden del EXE (los indices de zona van aparte: +0xC y +0x10)
GW_COPIES = [(0x00, 0x96AB90, 1), (0x04, 0x96AB64, 4), (0x08, 0x96AB60, 4), (0x14, 0x96ABBC, 4), (0x18, 0x96ABC0, 4), (0x1C, 0x96ABC4, 4), (0x20, 0x96AB84, 4), (0x24, 0x96AB50, 4),
             (0x28, 0x96AB58, 4), (0x2C, 0x96AB54, 4), (0x30, 0x96AB4C, 4), (0x34, 0x96AB48, 4), (0x38, 0x96ABC8, 4), (0x3C, 0x96ABCC, 4), (0x40, 0x96ABD0, 4), (0x44, 0x96AB44, 4),
             (0x48, 0x96AB98, 4), (0x4C, 0x96AB92, 1), (0x50, 0x96AB9C, 4), (0x54, 0x96AB80, 4)]
# CPlayerInfoSaveStructure::Construct: (destino, origen en CPlayerInfo, bytes)
PI_COPIES = [(0x00, 0xB8, 4), (0x04, 0x130, 2), (0x06, 0xDC, 1), (0x08, 0x134, 4), (0x0C, 0xBC, 4), (0x10, 0x144, 1), (0x14, 0xC0, 4), (0x18, 0xC4, 4)] + \
            [(0x1C + i, 0x14C + i, 1) for i in range(9)] + [(0x26, 0x156, 2)]


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def tdiv(a, b):
    """division entera con signo truncada hacia cero"""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


def pat(kind, seed, i):
    """byte i del relleno de una region (FillKind de Fixtures.hpp)"""
    if kind:
        return 0x00 if kind == 1 else (0xFF if kind == 2 else 0xCD)
    return (rnd(seed, i >> 2) >> (8 * (i & 3))) & 0xFF


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}

    def base(self, i):
        return pat(self.kind, self.seed, i)

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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta; mas la memoria global"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[60 + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte (el valor inicial es el que prepara el caso)
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
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

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def g(self, addr, n):
        return sum(self.glob[addr + k] << (8 * k) for k in range(n))

    def gpattern(self, addr, n, kind, seed):
        for i in range(n):
            self.glob[addr + i] = pat(kind, seed, i)

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        """cambios de las regiones de la funcion respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
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
    if _kind in (VDEL, VSDD):
        _k += ['idx_zero', 'idx_positive', 'idx_big', 'lowers_cursor', 'keeps_cursor', 'was_used']
        if _kind == VSDD:
            _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == PICON:
        _k += ['hours_over_255', 'hours_le_255']
    elif _kind == PIEXT:
        _k += ['hours_byte_high', 'hours_dword_high']
    elif _kind == GWCON:
        _k += ['ptr1_null', 'ptr1_found', 'ptr1_missing', 'ptr2_null', 'ptr2_found', 'ptr2_missing', 'count1_zero', 'count1_unsigned_big', 'count2_zero', 'count2_unsigned_big']
    elif _kind == GWEXT:
        _k += ['idx1_minus1', 'idx1_set', 'idx2_minus1', 'idx2_set', 'idx1_big']
    elif _kind == CPUEXT:
        _k += ['ext_leaf']
    elif _kind == VID:
        _k += ['zero', 'nonzero']
    elif _kind == PADDRUNK:
        _k += ['negative', 'zero']
    elif _kind == PADSUB:
        _k += ['mode1_pressed', 'mode1_not_pressed', 'mode_other', 'mode_high']
    elif _kind == ROTPTR:
        _k += ['nan', 'inf', 'denormal']
    elif _kind == TIME:
        _k += ['above', 'below', 'equal', 'nan', 'denormal', 'negative']
    elif _kind == INSHUT:
        _k += ['mode0', 'mode1', 'mode2']
    elif _kind in (BODY, CTOR):
        _k += ['prev_zero', 'prev_other']
    elif _kind == CLONESC:
        _k += ['mem_null', 'mem_set']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    KEYS[_fn] = _k


def vehicle_delete(W, B, D, a, ptr):
    """borrado en linea del almacen de vehiculos: marca el hueco y baja el cursor; devuelve el indice"""
    pool = W.raw(1, a[4])
    storage = B.rd(a[4], 4)
    diff = s32((ptr - storage) & M32)
    index = tdiv(diff, VEH_SIZE)
    state = B.rd(a[4] + 4, 4)
    addr = (state + index) & M32
    W.wr(addr, 1, W.rd(addr, 1) | 0x80)
    cursor = s32(B.rd(a[4] + 0xC, 4))
    lowers = index < cursor
    if lowers:
        B.wr(a[4] + 0xC, 4, index & M32)
    return index, lowers


MARKER = {CPUEXT, CPUVEND, VID, NOP, THUNK, TIME, INSHUT}     # funciones con la marca 0x5A en 0xA9B0B0 (region vigilada, sin cambios en las funciones sin mutar)


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k in MARKER:
        W.setg(FLAG, 1, 0x5A)
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == VDMG:
        W.snap()
    elif k in (VDEL, VSDD):
        base, idx = a[6], s32(a[5])
        ptr = (base + idx * VEH_SIZE) & M32
        mapp = (W.raw(3, a[7]) - idx) & M32
        B.wr(a[4] + 0, 4, base); B.wr(a[4] + 4, 4, mapp); B.wr(a[4] + 8, 4, a[9]); B.wr(a[4] + 0xC, 4, a[8])
        D.wr(a[7], 1, a[10] & 0xFF)
        W.setg(VEH_POOL, 4, W.raw(1, a[4]))
        gorder = [(VEH_POOL, 4)]
        W.snap()
        was_free = (a[10] & 0x80) != 0
        marks.append('idx_zero' if idx == 0 else ('idx_negative' if idx < 0 else 'idx_positive'))
        if abs(idx) > 1000:
            marks.append('idx_big')
        if k == VSDD:
            events.append([0x6E2B40, W.tagv(ptr)])
            flags = a[11] & 0xFF
            marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[11] >> 8 else 'flag_byte_only')
            ret = W.tagv(ptr)
            if flags & 1:
                lowers = vehicle_delete(W, B, D, a, ptr)[1]
                marks.append('lowers_cursor' if lowers else 'keeps_cursor')
                marks.append('was_free' if was_free else 'was_used')
        else:
            lowers = vehicle_delete(W, B, D, a, ptr)[1]
            marks.append('lowers_cursor' if lowers else 'keeps_cursor')
            marks.append('was_free' if was_free else 'was_used')
    elif k == PICON:
        B.wr(a[4] + 0x144, 2, a[5]); B.wr(a[4] + 0x130, 2, a[6]); B.wr(a[4] + 0x156, 2, a[7])
        W.snap()
        for dst, src, n in PI_COPIES:
            A.wr(o + dst, n, B.rd(a[4] + src, n))
        marks.append('hours_over_255' if (a[5] & 0xFFFF) > 0xFF else 'hours_le_255')
    elif k == PIEXT:
        A.wr(o + 0x10, 4, a[5]); A.wr(o + 4, 2, a[6]); A.wr(o + 0x26, 2, a[7])
        W.snap()
        for dst, src, n in PI_COPIES:
            if (dst, src) == (0x10, 0x144):
                B.wr(a[4] + 0x144, 2, A.rd(o + 0x10, 1))      # movzx dx, byte [ecx+10h]; mov [eax+144h], dx
            else:
                B.wr(a[4] + src, n, A.rd(o + dst, n))
            if (dst, src) == (0x06, 0xDC):
                B.wr(a[4] + 0x130, 2, A.rd(o + 4, 2))         # el EXE vuelve a escribir el WORD de densidad
        marks.append('hours_byte_high' if (a[5] & 0xFF) >= 0x80 else 'hours_byte_low')
        if a[5] >> 8:
            marks.append('hours_dword_high')
    elif k == GWCON:
        W.gpattern(GW_BASE, GW_BYTES, a[2], a[63])
        W.setg(0x96AB8C, 4, a[10]); W.setg(0x96AB88, 4, a[11]); W.setg(ZONE_COUNT1, 2, a[6] & 0xFFFF); W.setg(ZONE_COUNT2, 2, a[9] & 0xFFFF)
        gorder = [(GW_BASE, GW_BYTES), (ZONE_COUNT1, 2), (ZONE_COUNT2, 2)]
        W.snap()
        for dst, src, n in GW_COPIES:
            A.wr(o + dst, n, W.g(src, n))

        def zone_index(ptr, count, base, step):
            if ptr == 0 or count == 0:
                return M32
            d = (ptr - base) & M32
            if d % step == 0 and d // step < count:
                return d // step
            return M32
        i1 = zone_index(W.g(0x96AB8C, 4), W.g(ZONE_COUNT1, 2), ZONE_BASE1, 0x11)
        i2 = zone_index(W.g(0x96AB88, 4), W.g(ZONE_COUNT2, 2), ZONE_BASE2, 0x20)
        A.wr(o + 0xC, 4, i1); A.wr(o + 0x10, 4, i2)
        for nm, ptr, idx in (('ptr1', a[10], i1), ('ptr2', a[11], i2)):
            marks.append(nm + ('_null' if ptr == 0 else ('_found' if idx != M32 else '_missing')))
        marks.append('count1_zero' if a[6] & 0xFFFF == 0 else 'count1_nonzero')
        marks.append('count2_zero' if a[9] & 0xFFFF == 0 else 'count2_nonzero')
        if a[6] & 0xFFFF >= 0x8000:
            marks.append('count1_unsigned_big')
        if a[9] & 0xFFFF >= 0x8000:
            marks.append('count2_unsigned_big')
    elif k == GWEXT:
        W.gpattern(GW_BASE, GW_BYTES, a[2], a[63])
        A.wr(o + 0xC, 4, a[10]); A.wr(o + 0x10, 4, a[11])
        gorder = [(GW_BASE, GW_BYTES), (ZONE_COUNT1, 2), (ZONE_COUNT2, 2)]
        W.snap()
        for dst, src, n in GW_COPIES:
            W.setg(src, n, A.rd(o + dst, n))
        i1, i2 = A.rd(o + 0xC, 4), A.rd(o + 0x10, 4)
        W.setg(0x96AB8C, 4, 0 if i1 == M32 else (i1 * 0x11 + ZONE_BASE1) & M32)
        W.setg(0x96AB88, 4, 0 if i2 == M32 else ((i2 << 5) + ZONE_BASE2) & M32)
        marks.append('idx1_minus1' if i1 == M32 else 'idx1_set')
        marks.append('idx2_minus1' if i2 == M32 else 'idx2_set')
        if i1 >= 0x80000000 and i1 != M32:
            marks.append('idx1_big')
    elif k == CPUEXT:
        W.snap()
        max_ext, ext_edx = c['cpuid'][0], c['cpuid'][1]
        ret = ext_edx if max_ext > 0x80000000 else 0
        marks.append('ext_leaf')
    elif k == CPUVEND:
        W.gpattern(CPU_VENDOR, 12, a[2], a[63])
        gorder = [(CPU_VENDOR, 12)]
        W.snap()
        vebx, vedx, vecx = c['cpuid'][2], c['cpuid'][3], c['cpuid'][4]
        W.setg(CPU_VENDOR, 4, vebx); W.setg(CPU_VENDOR + 4, 4, vedx); W.setg(CPU_VENDOR + 8, 4, vecx)
    elif k == VID:
        W.snap()
        events.append([0x7F2CC0])
        ret = a[4]
        marks.append('zero' if a[4] == 0 else 'nonzero')
    elif k == PADDRUNK:
        W.snap()
        A.wr(o + 0x74, 4, a[4])
        marks.append('negative' if a[4] >= 0x80000000 else ('zero' if a[4] == 0 else 'positive'))
    elif k == PADSUB:
        A.wr(o + 0x10A, 2, a[4]); A.wr(o + 0x12, 2, a[5]); A.wr(o + 0x42, 2, a[6])
        W.snap()
        mode, new, old = a[4] & 0xFFFF, a[5] & 0xFFFF, a[6] & 0xFFFF
        pressed = mode == 1 and new != 0 and old == 0
        ret = 1 if pressed else 0
        if mode == 1:
            marks.append('mode1_pressed' if pressed else 'mode1_not_pressed')
        else:
            marks.append('mode_other')
        if mode > 3:
            marks.append('mode_high')
    elif k == NOP:
        W.setg(FLAG, 1, 0x5A)
        gorder = [(FLAG, 1)]
        W.snap()
    elif k == THUNK:
        W.snap()
        events.append([0x541DD0])
    elif k == ROTPTR:
        B.wr(a[7], 4, a[4]); B.wr(a[7] + 4, 4, a[5]); B.wr(a[7] + 8, 4, a[6])
        W.snap()
        events.append([0x59B460, W.tagv(this), a[4], a[5], a[6]])
        comps = [f32(a[4]), f32(a[5]), f32(a[6])]
        if any(math.isnan(v) for v in comps):
            marks.append('nan')
        if any(math.isinf(v) for v in comps):
            marks.append('inf')
        if any(0 < (x & 0x7FFFFFFF) < 0x800000 for x in a[4:7]):
            marks.append('denormal')
    elif k == TIME:
        W.setg(TIMESTEP, 4, a[5])
        gorder = [(TIMESTEP, 4)]
        W.snap()
        x = f32(a[4])
        if math.isnan(x):
            W.setg(TIMESTEP, 4, a[4])
            marks.append('nan')
        elif x > f32(TIME_CONST):
            W.setg(TIMESTEP, 4, a[4])
            marks.append('above')
        else:
            W.setg(TIMESTEP, 4, TIME_CONST)
            marks.append('equal' if x == f32(TIME_CONST) else 'below')
        if 0 < (a[4] & 0x7FFFFFFF) < 0x800000:
            marks.append('denormal')
        if a[4] >> 31:
            marks.append('negative')
    elif k == INSHUT:
        mode = a[4]
        field = 0 if mode == 0 else W.raw(2, a[6])
        B.wr(a[5] + 0x18, 4, field)
        D.wr(a[7] + 0x18, 4, a[9])
        W.setg(PSGLOBAL, 4, W.raw(1, a[5]))
        gorder = [(PSGLOBAL, 4)]
        W.snap()
        marks.append('mode%d' % mode)
        if field:
            events.append([0xD1, W.tagv(field)])
            cur = W.raw(1, a[5])
            if mode == 2:
                cur = W.raw(3, a[7])
                W.setg(PSGLOBAL, 4, cur)           # el falso de Release cambia psGlobal
            W.wr((cur + 0x18) & M32, 4, 0)          # psGlobal se relee tras Release
    elif k == BODY:
        W.snap()
        target = BODY_INFO[fn][0]
        A.wr(o, 4, BODY_VT[fn])
        events.append([target, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CTOR:
        W.snap()
        events.append([0x4AC450, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x86CBA0)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLONESC:
        A.wr(o + 0x14, 4, a[8])
        W.snap()
        events.append([0x4B5620, 0x18])
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([0x4AF820, W.tagv(mem), a[8]])
            W.wr(mem, 4, a[7])
            W.wr(mem, 4, 0x86CB58)
            ret = W.tagv(mem)
    elif k == SDD:
        W.snap()
        events.append([0x5FF3A0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([0x4B5630, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    if k in MARKER and (FLAG, 1) not in gorder:
        gorder = gorder + [(FLAG, 1)]
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
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
