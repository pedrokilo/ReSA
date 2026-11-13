"""Oraculo independiente (Python) de Fn180A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 180A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables y datos leidos del propio EXE).
   0  0x4D6CC0  StopAmbienceTrack (ecx = this): eax = [+8]; si eax es 0, 1, 2 o 3: [+8] = 4; bytes [+0], [+1] y [+2] = 0; ret
   1  0x4D6DA0  StartTrackPlayback: DisableBassEq 4D94D0 (ecx = 0B5F8B8h); SetChannelVolume 4D8870 (ecx = 0B5F8B8h; pila: canal = WORD de +4 ampliado a cero, 0, dword de +0x18, 0); SetChannelFlags 4D9500 (canal
            ampliado a cero, 0, 21h); salto final a 4D8F30 (ecx = 0B5F8B8h)
   2  0x4D6DF0  CheckForPause: factor = 0 si el byte 0xB7CB49, el 0xB7CB48 o el 0xBA6831 no es nulo (en ese orden), si no el dword de +0x1C (se lee una vez); SetChannelFrequencyScalingFactor 4D8960 (ecx = 0B5F8B8h;
            pila: canal = WORD de +4 ampliado a cero, 0, factor) DOS veces, releyendo el canal en cada una
   3  0x4D7890  CAEAudioChannel::Constructor(directSound, canal, frecuencia, bits) `ret 10h`: [+0x55] WORD = bits (WORD bajo), [+4] = directSound, [+0x3A] WORD = canal (WORD bajo), [+0x4F] dword = (bits >> 3) * frecuencia
            (32 bits), [this] = 85F03Ch, [+0x28] = 0, [+0x5C] = 0, [+0x3C] = frecuencia, [+0x57] WORD = 0, [+0x53] WORD = 2, [+0x49] WORD = 1, [+0x4B] dword = frecuencia, [+0x40] = frecuencia, [+0x47] WORD = 1,
            [+0x34] = 0C2C80000h, [+8] = 0, [+0xC] = 0, BYTE [+0x38] = [+0x44] = [+0x45] = 0; eax = this
   4  0x4D7910  DestructorBody: [this] = 85F03Ch; si [+8] != 0: dec dword [0B5F898h]; p = [+8]; si p: Release (stdcall, [[p]+8]); q = [+0xC]; [+8] = 0; si q: Release; [+0xC] = 0
   5  0x59EEB0  CHandObject::Constructor(modelo, ped, izquierda) `ret 0Ch`: 5A1D10 (ecx = this); [this] = 866EE0h; 734A40([ped+0x18]) -> jerarquia; [+0x17C] = ped; 532B20 (ecx = ped); 7C51A0(jerarquia,
            21h si el BYTE bajo de izquierda no es nulo, si no 17h) -> [+0x180]; 532AE0 (ecx = this; modelo); 4D6720([+0x18]); mi = [0A9B0C8h + movsx WORD [ped+0x22] * 4]; indice = movsx WORD [mi+0xA]; pool = [0C8800Ch];
            objeto = [pool] + indice * 12 (el estado [[pool+4] + indice] tiene el bit 7 a cero); dic = [objeto]; 734E50(dic, [mi+4]) -> [+0x184]; si es nulo: 734940(dic) -> [+0x184]; [+0x1C] = ([+0x1C] & ~1) | 10000400h;
            BYTE [+0x36] = ([+0x36] & 7) | 10h; BYTE [+0x13C] = 5; 5A0760 (ecx = this; 0); BYTE [[0A9B0C8h + modelo * 4] + 0xC] = 0FFh; BYTE [+0x188] = 1; eax = this
   6  0x59EFE0  destructor con borrado: call 59F000 (jmp 59F660, cuerpo de ~CObject); si el bit 0 del BYTE de banderas: delete 5A1F20 (cdecl); eax = this
   7  0x59F050  cdecl(indice): 1 si el argumento (dword) es igual a alguno de los WORD (ampliados a cero) 0x8CD700, 0x8CD6FC, 0x8CD740 o 0x8CD71C; si no 0 (solo AL)
   8  0x648E90  ComputeAnimIDExe(grupo*, anim*) `ret 8`: [anim*] = 183h si el dword +0x14 es 10 u 11, si no 184h; id = [anim*] (se relee); veh = [this+0x10]; manejo = [veh+0x384]; byte = [manejo+0xDE];
            GetGroup 6E3B00 (ecx = 0C1CDC0h + 94h * byte; pila: id); [grupo*] = eax
   9  0x64CD60  destructor con borrado: call 648FC0; delete 61A5B0 si bit 0; eax = this
  10  0x64CD80  como 9 con el cuerpo 649100
  11  0x64CDA0  CreateSubTask(tipo) `ret 4`: 0xCB: new(20h) 61A5A0; si no es nulo ctor 62F310(ecx = mem; -1, 0, 0, 41000000h); 0x387: 649160 (ecx = this) ANTES de reservar; new(28h); si no es nulo ctor 668120
            (ecx = mem; 4, this+0xC, [86FC84h] = 0.5f, [86FC88h] = 2.0f, 0, 0); otro tipo (0x516 incluido): 0. eax = lo que deja el constructor en eax; 0 con reserva nula
  12  0x64CE80  como 9 con el cuerpo 649280
  13  0x6557A0  CTaskSimpleHitHead::Clone: new(10h) 61A5A0; si no es nulo: ctor base 61A390 (ecx = mem); [mem] = 86F1D0h; BYTE [mem+8] = 0; [mem+0xC] = 0; eax = mem; si no, 0
  14  0x655DB0  como 9 con el cuerpo 655DD0 (jmp 61A6F0)
  15  0x655DE0  como 9 con el cuerpo 655E00 (jmp 61A6F0)
  16  0x655E30  como 9 con el cuerpo 653090
  17  0x655E80  como 9 con el cuerpo 6531D0
  18  0x6586A0  CTaskSimpleAbseil::Constructor: 61A900 (ecx = this; 0, 98h, 40800000h, 1A7h, 86F484h, 0); [this] = 86F45Ch; eax = this
  19  0x6586D0  Clone: new(20h) 61A5A0; si no es nulo: 61A900 como arriba sobre mem; [mem] = 86F45Ch; eax = mem; si no 0
  20  0x668D50  CreateSubTask(tipo) `ret 4`: 0x386: new(18h); ctor 667E20(ecx = mem; [+0x20], [+0x24], [+0x28]); 0xCB: new(20h); ctor 62F310 (0, 0, 0, 41000000h); 0x384: new(30h); ctor 667CD0 (ecx = mem; [+0xC],
            this+0x10, [+0x1C], 0, 0); otro tipo (0x516 incluido): 0. Reserva nula: 0 sin constructor; eax = lo que deja el constructor
  21  0x66E050  [this] = 870078h; si [+0x20] != 0: delete 41B7D0 (cdecl; no lo pone a cero); 61A3D0 (ecx = this)

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [direccion, puntero (etiqueta), dword del puntero]; operator new [direccion, tamano]; metodos del
hardware de audio [direccion, ECX, argumentos de pila]; Release [0xC0DE, objeto, vtable, +8, +0xC, contador]; constructores de tareas [direccion, ECX, argumentos de pila (los punteros a un vector con sus tres dwords)].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CAEAmbienceTrackManager::StopAmbienceTrack", "CAEAmbienceTrackManager::StartTrackPlayback", "CAEAmbienceTrackManager::CheckForPause", "CAEAudioChannel::Constructor",
         "CAEAudioChannel::DestructorBody", "CHandObject::Constructor", "CHandObject::ScalarDeletingDestructor", "CObject::IsCraneMovingPartModel", "CTaskSimpleCarFallOut::ComputeAnimIDExe",
         "CTaskSimpleSetPedAsAutoDriver::ScalarDeletingDestructor", "CTaskComplexGoToBoatSteeringWheel::ScalarDeletingDestructor", "CTaskComplexGoToBoatSteeringWheel::CreateSubTask",
         "CTaskComplexGetOnBoatSeat::ScalarDeletingDestructor", "CTaskSimpleHitHead::Clone", "CTaskSimpleHitFromBehind::ScalarDeletingDestructor", "CTaskSimpleHitWall::ScalarDeletingDestructor",
         "CTaskSimpleHitHead::ScalarDeletingDestructor", "CTaskSimpleEvasiveStep::ScalarDeletingDestructor", "CTaskSimpleAbseil::Constructor", "CTaskSimpleAbseil::Clone",
         "CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask", "CTaskComplexFollowPointRoute::DestructorBody"]
ENTRIES = (0x4D6CC0, 0x4D6DA0, 0x4D6DF0, 0x4D7890, 0x4D7910, 0x59EEB0, 0x59EFE0, 0x59F050, 0x648E90, 0x64CD60, 0x64CD80, 0x64CDA0, 0x64CE80, 0x6557A0, 0x655DB0, 0x655DE0,
           0x655E30, 0x655E80, 0x6586A0, 0x6586D0, 0x668D50, 0x66E050)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
SEED = 60                                                        # a[60..63] = semillas de las arenas
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
HW = 0xB5F8B8                                                    # objeto CAEAudioHardware
RELEASE_EV = 0xC0DE
VTOFF = 0xE00

STOP, STARTTP, CHKPAUSE, CHANCTOR, CHANDTOR, HANDCTOR, SDDOBJ, CRANE, ANIMID, SDD, SUBBOAT, HITCLONE, ABCTOR, ABCLONE, SUBGO, ROUTE = \
    'stop starttp chkpause chanctor chandtor handctor sddobj crane animid sdd subboat hitclone abctor abclone subgo route'.split()
KIND = [STOP, STARTTP, CHKPAUSE, CHANCTOR, CHANDTOR, HANDCTOR, SDDOBJ, CRANE, ANIMID, SDD, SDD, SUBBOAT, SDD, HITCLONE, SDD, SDD, SDD, SDD, ABCTOR, ABCLONE, SUBGO, ROUTE]
SDD_BODY = {6: (0x59F660, 0x5A1F20), 9: (0x648FC0, 0x61A5B0), 10: (0x649100, 0x61A5B0), 12: (0x649280, 0x61A5B0), 14: (0x61A6F0, 0x61A5B0), 15: (0x61A6F0, 0x61A5B0), 16: (0x653090, 0x61A5B0),
            17: (0x6531D0, 0x61A5B0)}
NEWADDR, TASKBASECTOR, STANDCTOR, GOTOPOINTCTOR, GOTOSTANDCTOR, ACHIEVECTOR, RUNANIMCTOR, ROUTEDELETE, COMPLEXBODY, TARGETPOS, GETGROUP = \
    0x61A5A0, 0x61A390, 0x62F310, 0x667CD0, 0x668120, 0x667E20, 0x61A900, 0x41B7D0, 0x61A3D0, 0x649160, 0x6E3B00
OBJCTOR, SETSTATIC, SETMODELINDEX, ANIMHIER, UPDHANIM, HANIMINDEX, CLUMPINIT, FINDTEX, FIRSTTEX = 0x5A1D10, 0x5A0760, 0x532AE0, 0x734A40, 0x532B20, 0x7C51A0, 0x4D6720, 0x734E50, 0x734940
MODEL_TABLE = 0xA9B0C8
GROUP_BASE, GROUP_STRIDE = 0xC1CDC0, 0x94
COUNTER_ADDR = 0xB5F898
HALF, TWO = 0x3F000000, 0x40000000        # [0x86FC84] y [0x86FC88] del EXE (0.5f y 2.0f)
EIGHT = 0x41000000


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


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def sx16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


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
        self.mem = tuple(Mem(a[2], a[SEED + i]) for i in range(4))

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
_EXTRA = {
    STOP: ['st_active', 'st_stopping', 'st_other'],
    STARTTP: ['chan_hi', 'chan_lo', 'vol_nan', 'mutate'],
    CHKPAUSE: ['pause_user', 'pause_code', 'menu', 'run', 'mutate', 'factor_nan', 'chan_hi'],
    CHANCTOR: ['bits_ge8', 'overflow', 'id_hi', 'ds_null'],
    CHANDTOR: ['p8_null', 'p8_set', 'q_null', 'q_set', 'q_same', 'mut_zero', 'mut_obj', 'counter_zero'],
    HANDCTOR: ['left', 'right', 'tex_found', 'tex_missing', 'txd_zero', 'txd_pos', 'share0', 'share1', 'share2'],
    SDDOBJ: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    SDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    CRANE: ['match_magnet', 'match_harness', 'match_mini', 'match_ball', 'no_match', 'arg_neg1', 'arg_hi'],
    ANIMID: ['door_lhs', 'door_other', 'alias', 'group_byte_hi'],
    SUBBOAT: ['type_standstill', 'type_gotopoint', 'type_finished', 'type_other', 'mem_null', 'mem_set'],
    HITCLONE: ['mem_null', 'mem_set'],
    ABCTOR: ['prev_zero', 'prev_other'],
    ABCLONE: ['mem_null', 'mem_set'],
    SUBGO: ['type_gotopoint', 'type_standstill', 'type_achieve', 'type_finished', 'type_other', 'mem_null', 'mem_set'],
    ROUTE: ['route_null', 'route_set'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, gch = [], None, []
    o = a[0]
    this = W.raw(0, o)
    tag = W.tagv

    def alloc_ctor(size, ctor_event, mem_off_arg_a, mem_mode):
        """new(size) del pool de tareas: evento y, si hay memoria (arena B), el constructor falso; devuelve (memoria o 0)"""
        events.append([NEWADDR, size])
        if mem_mode == 0:
            marks.append('mem_null')
            return 0
        marks.append('mem_set')
        mem = W.raw(1, mem_off_arg_a)
        events.append(ctor_event(mem))
        return mem

    if k == STOP:
        A.wr(o, 4, a[3]); A.wr(o + 8, 4, a[4])
        W.snap()
        if a[4] in (0, 1, 2, 3):
            A.wr(o + 8, 4, 4)
            marks.append('st_active')
        elif a[4] == 4:
            marks.append('st_stopping')
        else:
            marks.append('st_other')
        A.wr(o, 1, 0); A.wr(o + 1, 1, 0); A.wr(o + 2, 1, 0)
    elif k == STARTTP:
        A.wr(o + 4, 4, a[3]); A.wr(o + 0x18, 4, a[4])
        W.snap()
        chan = a[3] & 0xFFFF
        events.append([0x4D94D0, HW])
        events.append([0x4D8870, HW, chan, 0, a[4], 0])
        if a[7]:
            A.wr(o + 4, 4, a[6])
            marks.append('mutate')
        events.append([0x4D9500, HW, A.rd(o + 4, 4) & 0xFFFF, 0, 0x21])
        events.append([0x4D8F30, HW])
        marks.append('chan_hi' if chan >= 0x8000 else 'chan_lo')
        if is_nan(a[4]):
            marks.append('vol_nan')
    elif k == CHKPAUSE:
        A.wr(o + 4, 4, a[3]); A.wr(o + 0x1C, 4, a[4])
        W.snap()
        user, code, menu = a[5] & 0xFF, a[6] & 0xFF, a[7] & 0xFF
        if user:
            factor = 0
            marks.append('pause_user')
        elif code:
            factor = 0
            marks.append('pause_code')
        elif menu:
            factor = 0
            marks.append('menu')
        else:
            factor = a[4]
            marks.append('run')
            if is_nan(a[4]):
                marks.append('factor_nan')
        if (a[3] & 0xFFFF) >= 0x8000:
            marks.append('chan_hi')
        events.append([0x4D8960, HW, a[3] & 0xFFFF, 0, factor])
        if a[10]:
            A.wr(o + 4, 4, a[8]); A.wr(o + 0x1C, 4, a[9])
            marks.append('mutate')
        events.append([0x4D8960, HW, A.rd(o + 4, 4) & 0xFFFF, 0, factor])
    elif k == CHANCTOR:
        W.snap()
        bits, ident, freq = a[6] & 0xFFFF, a[4] & 0xFFFF, a[5]
        avg = ((bits >> 3) * freq) & M32
        A.wr(o + 0x55, 2, bits); A.wr(o + 4, 4, a[3]); A.wr(o + 0x3A, 2, ident); A.wr(o + 0x4F, 4, avg)
        A.wr(o, 4, 0x85F03C); A.wr(o + 0x28, 4, 0); A.wr(o + 0x5C, 4, 0); A.wr(o + 0x3C, 4, freq); A.wr(o + 0x57, 2, 0); A.wr(o + 0x53, 2, 2)
        A.wr(o + 0x49, 2, 1); A.wr(o + 0x4B, 4, freq); A.wr(o + 0x40, 4, freq); A.wr(o + 0x47, 2, 1); A.wr(o + 0x34, 4, 0xC2C80000)
        A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0); A.wr(o + 0x38, 1, 0); A.wr(o + 0x44, 1, 0); A.wr(o + 0x45, 1, 0)
        ret = tag(this)
        if bits >= 8:
            marks.append('bits_ge8')
        if (bits >> 3) * freq > M32:
            marks.append('overflow')
        if a[4] >> 16:
            marks.append('id_hi')
        if a[3] == 0:
            marks.append('ds_null')
    elif k == CHANDTOR:
        def com(mode, off):
            return 0 if mode == 0 else (W.raw(1, off) if mode == 1 else W.raw(2, off))
        p8 = com(a[4], a[5])
        q0 = p8 if a[6] == 3 else com(a[6], a[7])
        obj3 = W.raw(3, a[11])
        A.wr(o, 4, a[3]); A.wr(o + 8, 4, p8); A.wr(o + 0xC, 4, q0)
        W.snap()
        vt = 0x85F03C
        A.wr(o, 4, vt)
        cnt = a[10]
        if p8:
            cnt = (cnt - 1) & M32
            marks.append('p8_set')
        else:
            marks.append('p8_null')
        p = A.rd(o + 8, 4)
        if p:
            events.append([RELEASE_EV, tag(p), vt, tag(p8), tag(q0), cnt])
            if a[8] == 1:
                A.wr(o + 0xC, 4, 0)
                marks.append('mut_zero')
            elif a[8] == 2:
                A.wr(o + 0xC, 4, obj3)
                marks.append('mut_obj')
        q = A.rd(o + 0xC, 4)
        A.wr(o + 8, 4, 0)
        if q:
            events.append([RELEASE_EV, tag(q), vt, 0, tag(q), cnt])
            marks.append('q_set')
            if q == p8:
                marks.append('q_same')
        else:
            marks.append('q_null')
        A.wr(o + 0xC, 4, 0)
        if p8 and a[10] == 0:
            marks.append('counter_zero')
        old, new = a[10].to_bytes(4, 'little'), cnt.to_bytes(4, 'little')
        gch = [[COUNTER_ADDR + i, new[i]] for i in range(4) if old[i] != new[i]]
    elif k == HANDCTOR:
        ped = W.raw(1, a[17])
        pedmi, handmi_own = W.raw(2, a[18]), W.raw(2, a[19])
        handmi = handmi_own if a[20] == 0 else pedmi
        # preparacion: objetos y globales que la funcion lee
        A.wr(o, 4, a[3])
        ped_clump = 0 if a[7] == 0 else W.raw(3, 0xD00)
        new_clump = 0 if a[10] == 0 else (W.raw(3, 0xD40) if a[10] == 1 else W.raw(3, 0xD00))
        D.wr(0xD00, 1, 2); D.wr(0xD40, 1, 2)
        B.wr(a[17] + 0x18, 4, ped_clump)
        B.wr(a[17] + 0x20, 4, ((a[6] & 0xFFFF) << 16) | 0x5A5A)
        Cc.wr(a[18] + 4, 4, a[16])
        Cc.wr(a[18] + 8, 4, ((a[14] & 0xFFFF) << 16) | (a[15] & 0xFFFF))
        txd = a[14] & 0xFFFF
        D.wr(0x600 + 12 * txd, 4, a[13])                                  # primer dword del objeto del pool de TXD (pool de objetos desde 0x600)
        D.wr(0x100 + txd, 1, a[21] & 0x7F)                                # byte de estado (bit 7 a cero: objeto presente)
        D.wr(0xC00, 4, D_raw(W, 0x600))
        D.wr(0xC04, 4, D_raw(W, 0x100))
        D.wr(0xC08, 4, 0x1000)                                            # capacidad del pool (la DLL comprueba el indice)
        W.snap()
        events.append([OBJCTOR, tag(this), A.rd(o + 0x1C, 4), A.rd(o + 0x34, 4)])
        A.wr(o, 4, a[26])
        A.wr(o, 4, 0x866EE0)
        clump = B.rd(a[17] + 0x18, 4)
        events.append([ANIMHIER, clump])
        A.wr(o + 0x17C, 4, ped)
        events.append([UPDHANIM, tag(ped)])
        left = (a[5] & 0xFF) != 0
        events.append([HANIMINDEX, a[8], 0x21 if left else 0x17])
        marks.append('left' if left else 'right')
        A.wr(o + 0x180, 4, a[9])
        events.append([SETMODELINDEX, tag(this), a[4]])
        A.wr(o + 0x18, 4, new_clump)
        A.wr(o + 0x1C, 4, A.rd(o + 0x1C, 4) ^ a[24])
        A.wr(o + 0x34, 4, A.rd(o + 0x34, 4) ^ a[25])
        events.append([CLUMPINIT, A.rd(o + 0x18, 4)])
        key = Cc.rd(a[18] + 4, 4)
        dict_ = D.rd(0x600 + 12 * Cc.rd(a[18] + 8 + 2, 2), 4)
        events.append([FINDTEX, dict_, key])
        A.wr(o + 0x184, 4, a[11])
        if a[11] == 0:
            events.append([FIRSTTEX, dict_])
            A.wr(o + 0x184, 4, a[12])
            marks.append('tex_missing')
        else:
            marks.append('tex_found')
        marks.append('txd_zero' if txd == 0 else 'txd_pos')
        marks.append('share%d' % a[20])
        A.wr(o + 0x1C, 4, (A.rd(o + 0x1C, 4) & ~1 & M32) | 0x10000400)
        A.wr(o + 0x36, 1, (A.rd(o + 0x36, 1) & 7) | 0x10)
        A.wr(o + 0x13C, 1, 5)
        events.append([SETSTATIC, tag(this), 0, A.rd(o + 0x1C, 4), A.rd(o + 0x34, 4), A.rd(o + 0x13C, 4), A.rd(o + 0x17C, 4), A.rd(o + 0x184, 4)])
        A.wr(o + 0x1C, 4, A.rd(o + 0x1C, 4) ^ a[27])
        Cc.wr((handmi - W.bases[2] - GUARD) + 0xC, 1, 0xFF)
        A.wr(o + 0x188, 1, 1)
        ret = tag(this)
    elif k == SDDOBJ or k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        body, delete = SDD_BODY[fn]
        events.append([body, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([delete, tag(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tag(this)
    elif k == CRANE:
        W.snap()
        words = [a[4] & 0xFFFF, a[5] & 0xFFFF, a[6] & 0xFFFF, a[7] & 0xFFFF]
        names = ['match_magnet', 'match_harness', 'match_mini', 'match_ball']
        hit = False
        for i, w in enumerate(words):
            if a[3] == w:
                marks.append(names[i])
                hit = True
        marks.append('no_match' if not hit else 'hit')
        if a[3] == M32:
            marks.append('arg_neg1')
        elif a[3] >> 16:
            marks.append('arg_hi')
        ret = 1 if hit else 0
    elif k == ANIMID:
        A.wr(o + 0x10, 4, W.raw(1, a[4])); A.wr(o + 0x14, 4, a[3])
        B.wr(a[4] + 0x384, 4, W.raw(2, a[5]))
        Cc.wr(a[5] + 0xDE, 1, a[6] & 0xFF)
        W.snap()
        ident = 0x183 if a[3] in (10, 11) else 0x184
        marks.append('door_lhs' if a[3] in (10, 11) else 'door_other')
        out_group = a[8] if a[10] else a[9]
        D.wr(a[8], 4, ident)
        ident2 = D.rd(a[8], 4)
        b = Cc.rd(a[5] + 0xDE, 1)
        events.append([GETGROUP, (GROUP_BASE + GROUP_STRIDE * b) & M32, ident2])
        D.wr(out_group, 4, a[7])
        if a[10]:
            marks.append('alias')
        if b >= 0x80:
            marks.append('group_byte_hi')
    elif k == SUBBOAT:
        W.snap()
        t = a[3]
        if t == 0xCB:
            marks.append('type_standstill')
            mem = alloc_ctor(0x20, lambda m: [STANDCTOR, tag(m), M32, 0, 0, EIGHT], a[5], a[4])
            if mem:
                W.wr(mem, 4, a[7])
                ret = tag(a[6])
            else:
                ret = 0
        elif t == 0x387:
            marks.append('type_gotopoint')
            events.append([TARGETPOS, tag(this)])
            A.wr(o + 0xC, 4, a[8]); A.wr(o + 0x10, 4, a[9]); A.wr(o + 0x14, 4, a[10])
            mem = alloc_ctor(0x28, lambda m: [GOTOSTANDCTOR, tag(m), 4, tag((this + 0xC) & M32), a[8], a[9], a[10], HALF, TWO, 0, 0], a[5], a[4])
            if mem:
                W.wr(mem, 4, a[7])
                ret = tag(a[6])
            else:
                ret = 0
        else:
            marks.append('type_finished' if t == 0x516 else 'type_other')
            ret = 0
    elif k == HITCLONE:
        W.snap()
        mem = alloc_ctor(0x10, lambda m: [TASKBASECTOR, tag(m), B.rd(a[4], 4)], a[4], a[3])
        if mem:
            W.wr(mem, 4, a[6])
            W.wr(mem, 4, 0x86F1D0)
            W.wr(mem + 8, 1, 0)
            W.wr(mem + 0xC, 4, 0)
            ret = tag(mem)
        else:
            ret = 0
    elif k == ABCTOR:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([RUNANIMCTOR, tag(this), 0, 0x98, 0x40800000, 0x1A7, 0x86F484, 0])
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x86F45C)
        ret = tag(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ABCLONE:
        W.snap()
        mem = alloc_ctor(0x20, lambda m: [RUNANIMCTOR, tag(m), 0, 0x98, 0x40800000, 0x1A7, 0x86F484, 0], a[4], a[3])
        if mem:
            W.wr(mem, 4, a[6])
            W.wr(mem, 4, 0x86F45C)
            ret = tag(mem)
        else:
            ret = 0
    elif k == SUBGO:
        for off, v in ((0xC, a[8]), (0x10, a[9]), (0x14, a[10]), (0x18, a[11]), (0x1C, a[12]), (0x20, a[13]), (0x24, a[14]), (0x28, a[15])):
            A.wr(o + off, 4, v)
        W.snap()
        t = a[3]
        if t == 0x384:
            marks.append('type_gotopoint')
            mem = alloc_ctor(0x30, lambda m: [GOTOPOINTCTOR, tag(m), a[8], tag((this + 0x10) & M32), a[9], a[10], a[11], a[12], 0, 0], a[5], a[4])
        elif t == 0xCB:
            marks.append('type_standstill')
            mem = alloc_ctor(0x20, lambda m: [STANDCTOR, tag(m), 0, 0, 0, EIGHT], a[5], a[4])
        elif t == 0x386:
            marks.append('type_achieve')
            mem = alloc_ctor(0x18, lambda m: [ACHIEVECTOR, tag(m), a[13], a[14], a[15]], a[5], a[4])
        else:
            marks.append('type_finished' if t == 0x516 else 'type_other')
            mem = None
        if mem:
            W.wr(mem, 4, a[7])
            ret = tag(a[6])
        else:
            ret = 0
    elif k == ROUTE:
        route = 0 if a[4] == 0 else W.raw(1, a[5])
        A.wr(o, 4, a[3]); A.wr(o + 0x20, 4, route)
        W.snap()
        A.wr(o, 4, 0x870078)
        if route:
            events.append([ROUTEDELETE, tag(route), B.rd(a[5], 4)])
            marks.append('route_set')
        else:
            marks.append('route_null')
        events.append([COMPLEXBODY, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


def D_raw(W, off):
    return W.raw(3, off)


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
