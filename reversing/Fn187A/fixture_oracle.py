"""Oraculo independiente (Python) de Fn187A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 19 funciones del lote 187A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las regiones globales (Fixtures.hpp)
para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables y datos leidos del propio EXE).
   0  0x4AF3D0  CEventAttractor::GetEventType: mov eax,1Bh; ret
   1  0x4AF3F0  GetEventPriority: xor eax,eax; ret
   2  0x4AF3E0  GetLifeTime: xor eax,eax; ret
   3  0x4B74B0  CEventVehicleToSteal::Clone: ecx = [0B744ACh]; call 4B5570 (pool de eventos); si nulo: 0; si no: ctor 4AF670 (ecx = mem; pila: [this+0Ch]); eax = lo que deja el constructor
   4  0x4B7520  CEventVehicleThreat::CloneEditable: idem con ctor 4AFB50 y [this+14h]
   5  0x4B7590  CEventKnockOffBike::Clone: idem con ctor 4AFCF0 (11 argumentos: [this+38h], this+0Ch, this+18h, [+24h], [+28h], movsx BYTE [+36h], BYTE [+34h], [+2Ch], [+30h], bit 0 y bit 1 de BYTE [+35h])
   6  0x4B7630  CEventGunAimedAt::CloneEditable: idem con ctor 4B0700 y [this+14h]
   7  0x4B76A0  CEventInAir::Clone: ecx = [0B744ACh]; call 4B5570; si nulo: 0; si no: [mem+4] = 0; BYTE [mem+8] = 0; [mem] = 85B3C0h; eax = mem
   8  0x60A9A0  CPlayerPed destructor con borrado: call 6093B0; si el bit 0 del BYTE de banderas: CPed::operator delete 5E4760 (cdecl); eax = this
   9  0x60D5B0  CPlayerPed::CPlayerPed(id, grupoCreado) `ret 8`: CPed 5E8030 (ecx = this; pila: 0); [this] = 86D168h; [this+480h] = 0B7CD9Ch + id*190h; AllocateData 571880 (ecx = datos); SetModelIndex 5E4880 (ecx = this; 0);
             SetInitialState 60CD20 (ecx = this; el dword de la pila); si [this+71Ch] != 0: 571A00 (ecx = ese valor; pila: this+71Ch); [this+71Ch] = 0; SetPedState 5E4500 (ecx = this; 1); [0C0BC10h] = 4D3990("playidles");
             si el BYTE de la bandera es 0: g = AddGroup 5FB800; [datos+38h] = g; SetDefaultTaskAllocatorType 5FBB70 (ecx = 0C09950h + g*2D4h; 5); BYTE [0C09BF0h + g*2D4h] = 1; SetLeader 5FB9C0 (ecx = 0C09928h + g*2D4h; this);
             Process 5FC7E0 (ecx = 0C09920h + g*2D4h); [datos+34h] &= ~100h, ~200h, ~400h; salud = GetFatAndMuscleModifier 559AF0 (9): [this+544h] = [this+540h] = ST0 (24 bits); BYTE [+72Dh] = BYTE [+72Eh] = 15;
             [+79Ch] = [+7A0h] = 0; Initialise 4E68D0 (ecx = this+294h; pila: this); [[this+47Ch]+0C8h] = 41F00000h; [[this+47Ch]+0C4h] = 2; bit 21 de [this+478h] = bit 0 del BYTE de la bandera; eax = this
  10  0x60F8C0  funcion vacia: ret
  11  0x692680  CTaskSimpleDuckWhileShotsWhizzing ctor(WORD) `ret 4`: 61A390 ([this+4] = 0, [this] = 86D4A8h); [+18h] = [+19h] = 0; [+1Ah] = [+1Bh] = 1; [+1Ch] = [+20h] = 0; BYTE [+24h] = 0; BYTE [+25h] = 0FFh; [+10h] = [+14h] = 0;
             [+8] = [0B7CB84h]; WORD [+0Ch] = argumento; WORD [+0Eh] = 0FFFFh; [this] = 870BECh; eax = this
  12  0x6926E0  GetTaskType: mov eax,137h; ret
  13  0x6926F0  cuerpo del destructor: [ecx] = 870BECh; salto a 692030 (ecx = this)
  14  0x692700  MakeAbortable(ped, prioridad, evento) `ret 0Ch`: si prioridad == 2: 692100 (ecx = this; ped, prioridad, evento); si no, si evento != 0: GetEventType ([[evento]+4], ecx = evento); si es 31h: sin animacion (+10h) -> AL = 0;
             si no: SetFinishCallback 4CEBE0 (ecx = +10h; 4D1390, 0); si +14h != 0: 4CEBE0 (ecx = +14h; 4D1390, 0); [+10h] = [+14h] = 0; [ped+46Ch] &= 0FBFFFFFFh; AL = 1; resto: 692100 (ped, prioridad, evento)
  15  0x692D60  Clone: new(28h) 61A5A0; si no es nulo: ctor 692680 (ecx = mem; pila: WORD [this+0Ch] ampliada a cero); eax = lo que deja el constructor; reserva nula: 0
  16  0x692DD0  CTaskSimpleShakeFist destructor con borrado: call 690BB0; si el bit 0: delete 61A5B0 (cdecl); eax = this
  17  0x693B80  CTaskSimpleDuckWhileShotsWhizzing destructor con borrado: call 6926F0; si el bit 0: delete 61A5B0; eax = this
  18  0x694640  ProcessPed(ped) `ret 4`: si [this+10h] == 0: 4D4610 (clump [ped+18h], 0, 8Dh, 40800000h); [this+10h] = eax; 4CEBC0 (ecx = eax; 692550, this); 694390 (ecx = this; ped); AL = su resultado

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar o argumentos de pila]; operator new [direccion, tamano]; metodos [direccion, ECX, argumentos de pila].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CEventAttractor::GetEventType", "CEventAttractor::GetEventPriority", "CEventAttractor::GetLifeTime", "CEventVehicleToSteal::Clone", "CEventVehicleThreat::CloneEditable", "CEventKnockOffBike::Clone",
         "CEventGunAimedAt::CloneEditable", "CEventInAir::Clone", "CPlayerPed::ScalarDeletingDestructor", "CPlayerPed::Constructor", "CPlayerPed::EmptyStub60F8C0", "CTaskSimpleDuckWhileShotsWhizzing::Constructor",
         "CTaskSimpleDuckWhileShotsWhizzing::GetTaskType", "CTaskSimpleDuckWhileShotsWhizzing::DestructorBody", "CTaskSimpleDuckWhileShotsWhizzing::MakeAbortable", "CTaskSimpleDuckWhileShotsWhizzing::Clone",
         "CTaskSimpleShakeFist::ScalarDeletingDestructor", "CTaskSimpleDuckWhileShotsWhizzing::ScalarDeletingDestructor", "CTaskSimpleDuckWhileShotsWhizzing::ProcessPed"]
ENTRIES = (0x4AF3D0, 0x4AF3F0, 0x4AF3E0, 0x4B74B0, 0x4B7520, 0x4B7590, 0x4B7630, 0x4B76A0, 0x60A9A0, 0x60D5B0, 0x60F8C0, 0x692680, 0x6926E0, 0x6926F0, 0x692700, 0x692D60, 0x692DD0, 0x693B80, 0x694640)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
SEED = 60                                                        # a[60..63] = semillas de las arenas
GSEED = 56                                                       # a[56], a[57] = semillas de las regiones globales
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
VTOFF = 0xE00
EV_GETTYPE = 0xE7A1                                              # evento del falso de GetEventType del evento de MakeAbortable

CONST, VOID, EVCLONE1, EVKNOCK, EVINAIR, SDD, PLAYERCTOR, DUCKCTOR, DUCKBODY, MAKEABORT, TASKCLONE, PROCESSPED = \
    'const void evclone1 evknock evinair sdd playerctor duckctor duckbody makeabort taskclone processped'.split()
KIND = [CONST, CONST, CONST, EVCLONE1, EVCLONE1, EVKNOCK, EVCLONE1, EVINAIR, SDD, PLAYERCTOR, VOID, DUCKCTOR, CONST, DUCKBODY, MAKEABORT, TASKCLONE, SDD, SDD, PROCESSPED]
CONSTS = {0: 0x1B, 1: 0, 2: 0, 12: 0x137}
SDD_BODY = {8: (0x6093B0, 0x5E4760), 16: (0x690BB0, 0x61A5B0), 17: (0x6926F0, 0x61A5B0)}
CLONE1 = {3: (0xC, 0x4AF670), 4: (0x14, 0x4AFB50), 6: (0x14, 0x4B0700)}     # desplazamiento del dato y constructor
POOL_NEW, KNOCK_CTOR, TASK_NEW, DUCK_CTOR = 0x4B5570, 0x4AFCF0, 0x61A5A0, 0x692680
PLAYERS, GROUPS_REGION, PLAYIDLES = 0xB7CD9C, 0xC09BF0, 0xC0BC10
PLAYERS_BYTES, GROUPS_BYTES = 0x190 * 4, 0x2D4 * 7 + 4


def signed32(v):
    v &= M32
    return v - 0x100000000 if v & 0x80000000 else v


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


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


def sx8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


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


class GMem(Mem):
    """region global del juego (sin guardas): direcciones absolutas; el relleno inicial es el de FillKind; los cambios salen como [direccion, byte]"""

    def __init__(self, base, size, kind, seed):
        super().__init__(kind, seed)
        self.at, self.size = base, size

    def rdg(self, addr, n):
        return sum(self.get(addr - self.at + k) << (8 * k) for k in range(n))

    def wrg(self, addr, n, v):
        for k in range(n):
            self.m[addr - self.at + k] = (v >> (8 * k)) & 0xFF

    def gchanges(self):
        out = []
        for i in sorted(self.m):
            ref = self.r[i] if i in self.r else self.base(i)
            if self.m[i] != ref:
                out.append([self.at + i, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[SEED + i]) for i in range(4))

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

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_FLAGS = ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
_POOL = ['mem_null', 'mem_set', 'pool_raw', 'pool_arena']
_EXTRA = {
    CONST: [],
    VOID: [],
    EVCLONE1: list(_POOL),
    EVKNOCK: _POOL + ['type_negative', 'type_positive', 'driver_set', 'driver_clear', 'force_set', 'force_clear'],
    EVINAIR: list(_POOL),
    SDD: _FLAGS,
    PLAYERCTOR: ['group_created', 'group_new', 'target_set', 'target_null', 'health_nan', 'health_zero', 'player_id_0', 'player_id_3', 'group_0', 'group_7'],
    DUCKCTOR: ['arg_hi_bits', 'arg_word_only'],
    DUCKBODY: [],
    MAKEABORT: ['prio_immediate', 'prio_other', 'event_null', 'event_set', 'type_whizzed', 'type_other', 'duck_null', 'duck_set', 'move_null', 'move_set'],
    TASKCLONE: ['mem_null', 'mem_set'],
    PROCESSPED: ['duck_null', 'duck_set'],
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

    def pool_ptr(mode, val):
        marks.append('pool_arena' if mode else 'pool_raw')
        return W.raw(3, val) if mode else val

    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn]
    elif k == VOID:
        A.wr(o, 4, a[3])
        W.snap()
    elif k == EVCLONE1:
        off, ctor = CLONE1[fn]
        A.wr(o + off, 4, a[3])
        pool = pool_ptr(a[4], a[5])
        W.snap()
        events.append([POOL_NEW, tag(pool)])
        if a[6] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[7])
            events.append([ctor, tag(mem), a[3]])
            B.wr(a[7], 4, a[9])
            ret = tag(a[8])
    elif k == EVKNOCK:
        A.wr(o + 0x24, 4, a[3]); A.wr(o + 0x28, 4, a[4]); A.wr(o + 0x2C, 4, a[5]); A.wr(o + 0x30, 4, a[6]); A.wr(o + 0x38, 4, a[7]); A.wr(o + 0x34, 4, a[8])
        for i in range(3):
            A.wr(o + 0xC + 4 * i, 4, a[9 + i]); A.wr(o + 0x18 + 4 * i, 4, a[12 + i])
        pool = pool_ptr(a[15], a[16])
        W.snap()
        events.append([POOL_NEW, tag(pool)])
        if a[17] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[18])
            flags = (a[8] >> 8) & 0xFF
            typ = sx8(a[8] >> 16) & M32
            events.append([KNOCK_CTOR, tag(mem), a[7], tag(this + 0xC), tag(this + 0x18), a[3], a[4], typ, a[8] & 0xFF, a[5], a[6], flags & 1, (flags >> 1) & 1])
            B.wr(a[18], 4, a[20])
            ret = tag(a[19])
            marks.append('type_negative' if typ & 0x80000000 else 'type_positive')
            marks.append('driver_set' if flags & 1 else 'driver_clear')
            marks.append('force_set' if flags & 2 else 'force_clear')
    elif k == EVINAIR:
        pool = pool_ptr(a[3], a[4])
        W.snap()
        events.append([POOL_NEW, tag(pool)])
        if a[5] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            B.wr(a[6] + 4, 4, 0)
            B.wr(a[6] + 8, 1, 0)
            B.wr(a[6], 4, 0x85B3C0)
            ret = tag(W.raw(1, a[6]))
    elif k == SDD:
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
    elif k == PLAYERCTOR:
        players = GMem(PLAYERS, PLAYERS_BYTES, a[2], a[GSEED])
        groups = GMem(GROUPS_REGION, GROUPS_BYTES, a[2], a[GSEED + 1])
        idles = GMem(PLAYIDLES, 4, 1, 0)           # valor inicial fijo: ceros en la region de 4 bytes
        intel = W.raw(2, a[9])
        A.wr(o + 0x47C, 4, intel)
        A.wr(o + 0x71C, 4, a[5])
        W.snap()
        for r in (players, groups, idles):
            r.snap()
        data = (PLAYERS + a[3] * 0x190) & M32
        flag = a[4]
        events.append([0x5E8030, tag(this), 0])
        A.wr(o, 4, a[11]); A.wr(o + 4, 4, a[12])      # el falso de CPed deja dos dwords
        A.wr(o, 4, 0x86D168)
        A.wr(o + 0x480, 4, data)
        events.append([0x571880, data])
        events.append([0x5E4880, tag(this), 0])
        events.append([0x60CD20, tag(this), flag])
        if a[5]:
            marks.append('target_set')
            events.append([0x571A00, tag(a[5]), tag(this + 0x71C)])
        else:
            marks.append('target_null')
        A.wr(o + 0x71C, 4, 0)
        events.append([0x5E4500, tag(this), 1])
        events.append([0x4D3990, fnv(b'playidles'), 9])
        idles.wrg(PLAYIDLES, 4, a[7])
        if (flag & 0xFF) == 0:
            marks.append('group_new')
            g = a[6]
            events.append([0x5FB800])
            players.wrg(data + 0x38, 4, g)
            events.append([0x5FBB70, (0xC09950 + g * 0x2D4) & M32, 5])
            groups.wrg(0xC09BF0 + g * 0x2D4, 1, 1)
            events.append([0x5FB9C0, (0xC09928 + g * 0x2D4) & M32, tag(this)])
            events.append([0x5FC7E0, (0xC09920 + g * 0x2D4) & M32])
            for mask in (0xFFFFFEFF, 0xFFFFFDFF, 0xFFFFFBFF):
                players.wrg(data + 0x34, 4, players.rdg(data + 0x34, 4) & mask)
            marks.append('group_0' if g == 0 else ('group_7' if g == 7 else 'group_mid'))
        else:
            marks.append('group_created')
        events.append([0x559AF0, 9])
        A.wr(o + 0x544, 4, a[8]); A.wr(o + 0x540, 4, a[8])
        h = a[8]
        if (h >> 23) & 0xFF == 0xFF and h & 0x7FFFFF:
            marks.append('health_nan')
        if h & 0x7FFFFFFF == 0:
            marks.append('health_zero')
        A.wr(o + 0x72D, 1, 15); A.wr(o + 0x72E, 1, 15)
        A.wr(o + 0x79C, 4, 0); A.wr(o + 0x7A0, 4, 0)
        events.append([0x4E68D0, tag(this + 0x294), tag(this)])
        Cc.wr(a[9] + 0xC8, 4, 0x41F00000)
        Cc.wr(a[9] + 0xC4, 4, 2)
        old = A.rd(o + 0x478, 4)
        A.wr(o + 0x478, 4, (old & ~0x200000) | ((flag & 1) << 21))
        ret = tag(this)
        marks.append('player_id_0' if a[3] == 0 else ('player_id_3' if a[3] == 3 else 'player_id_mid'))
        gch = players.gchanges() + groups.gchanges() + idles.gchanges()
    elif k == DUCKCTOR:
        W.snap()
        A.wr(o + 4, 4, 0)
        A.wr(o + 0x18, 1, 0); A.wr(o + 0x19, 1, 0); A.wr(o + 0x1A, 1, 1); A.wr(o + 0x1B, 1, 1)
        A.wr(o + 0x1C, 4, 0); A.wr(o + 0x20, 4, 0)
        A.wr(o + 0x24, 1, 0); A.wr(o + 0x25, 1, 0xFF)
        A.wr(o + 0x10, 4, 0); A.wr(o + 0x14, 4, 0)
        A.wr(o + 8, 4, a[4])
        A.wr(o + 0xC, 2, a[3] & 0xFFFF)
        A.wr(o + 0xE, 2, 0xFFFF)
        A.wr(o, 4, 0x870BEC)
        ret = tag(this)
        marks.append('arg_hi_bits' if a[3] >> 16 else 'arg_word_only')
    elif k == DUCKBODY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x870BEC)
        events.append([0x692030, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == MAKEABORT:
        ped = W.raw(1, a[3])
        event = W.raw(2, a[6]) if a[5] else 0
        B.wr(a[3] + 0x46C, 4, a[11])
        A.wr(o + 0x10, 4, a[8]); A.wr(o + 0x14, 4, a[9])
        if a[5]:
            Cc.wr(a[6], 4, W.raw(2, VTOFF))
        W.snap()
        prio = a[4]

        def delegate():
            events.append([0x692100, tag(this), tag(ped), prio, tag(event)])
            return a[10] & 0xFF

        marks.append('event_set' if a[5] else 'event_null')
        marks.append('duck_null' if a[8] == 0 else 'duck_set')
        marks.append('move_null' if a[9] == 0 else 'move_set')
        if prio == 2:
            marks.append('prio_immediate')
            ret = delegate()
        else:
            marks.append('prio_other')
            if a[5]:
                events.append([EV_GETTYPE, tag(event)])
                if a[7] == 0x31:
                    marks.append('type_whizzed')
                    if a[8] == 0:
                        ret = 0
                    else:
                        events.append([0x4CEBE0, tag(a[8]), 0x4D1390, 0])
                        if a[9]:
                            events.append([0x4CEBE0, tag(a[9]), 0x4D1390, 0])
                        A.wr(o + 0x10, 4, 0); A.wr(o + 0x14, 4, 0)
                        B.wr(a[3] + 0x46C, 4, a[11] & 0xFBFFFFFF)
                        ret = 1
                else:
                    marks.append('type_other')
                    ret = delegate()
            else:
                ret = delegate()
    elif k == TASKCLONE:
        A.wr(o + 0xC, 4, a[3])
        W.snap()
        events.append([TASK_NEW, 0x28])
        if a[5] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[6])
            events.append([DUCK_CTOR, tag(mem), a[3] & 0xFFFF])
            B.wr(a[6], 4, a[8])
            ret = tag(a[7])
    elif k == PROCESSPED:
        ped = W.raw(1, a[3])
        clump = W.raw(3, a[8]) if a[4] else 0
        if a[4]:
            D.wr(a[8], 1, 2)
        B.wr(a[3] + 0x18, 4, clump)
        A.wr(o + 0x10, 4, a[5])
        W.snap()
        if a[5] == 0:
            marks.append('duck_null')
            events.append([0x4D4610, tag(clump), 0, 0x8D, 0x40800000])
            A.wr(o + 0x10, 4, a[6])
            events.append([0x4CEBC0, tag(a[6]), 0x692550, tag(this)])
        else:
            marks.append('duck_set')
        events.append([0x694390, tag(this), tag(ped)])
        ret = a[7] & 0xFF
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


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
