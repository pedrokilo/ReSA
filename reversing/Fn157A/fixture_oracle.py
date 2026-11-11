"""Oraculo independiente (Python) de Fn157A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 24 funciones del lote 157A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
 0..14 y 22  destructores con borrado de tareas (thiscall con banderas en la pila, `ret 4`, 30 bytes): push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
       0 0x65D240 CTaskComplexFleeEntity (cuerpo 65B9D0)         1 0x65D260 CTaskComplexSmartFleePoint (65BDB0)      2 0x65D280 CTaskComplexSmartFleeEntity (65C4D0)
       3 0x65D2A0 CTaskComplexFleeShooting (65C9A0)              4 0x65D2C0 CTaskComplexFleeAnyMeans (65CD10)         5 0x637F60 CTaskComplexObserveTrafficLights (6317C0)
       6 0x637F80 CTaskComplexObserveTrafficLightsAndAchieveHeading (631980)   7 0x637FA0 CTaskComplexCrossRoadLookAndAchieveHeading (631B50)   8 0x637FC0 CTaskSimpleTurn180 (631D00)
       9 0x638020 CTaskComplexHitResponse (631DA0)              10 0x639490 CTaskSimpleIKLookAt (633EF0)            11 0x6394B0 CTaskSimpleIKPointArm (634240)
      12 0x6394D0 CTaskSimpleTriggerLookAt (634500)             13 0x6394F0 CTaskSimpleClearLookAt (6346A0)          14 0x639510 CTaskComplexUseClosestFreeScriptedAttractor (634720)
      22 0x6801D0 CTaskSimpleJetPack (67E600)
  15  0x5F6360  CEventPlayerCommandToGroupAttack::GetEventType (vtable 0x86C710, ranura 1): mov eax,47h; ret
  16  0x5F6400  CEventPlayerCommandToGroupAttack::GetEventPriority (ranura 2): mov eax,2Ch; ret
  17  0x5F6450  CPedGroupDefaultTaskAllocatorFollowAnyMeans::Constructor (vtable 0x86C75C): mov eax,ecx; mov [eax],86C75Ch; ret
  18  0x5F63F0  CEventPlayerCommandToGroupAttack cuerpo del destructor: mov [ecx],86C710h; jmp 4B2470 (cuerpo de ~CEventPlayerCommandToGroup, thiscall sin argumentos)
  19  0x5F63D0  CEventPlayerCommandToGroupAttack destructor con borrado: cuerpo 5F63F0; CEvent::operator delete 4B5630
  20  0x5F6370  CEventPlayerCommandToGroupAttack::CloneEditable (ranura 16, marco SEH): edi = this; esi = eax = 4B5620(1Ch) (cdecl); si no es nulo: push [edi+18h]; push 0; ecx = esi; call 4B23D0 (thiscall `ret 8`);
            [esi] = 86C710h; eax = esi; si es nulo: eax = 0
  21  0x67A1C0  CTaskSimpleClimb::GetTaskType (vtable 0x87059C, ranura 4): mov eax,0FEh; ret
  23  0x67A1D0  CTaskSimpleClimb cuerpo del destructor (marco SEH): [esi]=87059Ch; si [esi+24h]: call 571A00 (CleanUpOldReference, ecx = [esi+24h], arg = esi+24h); si [esi+2Ch]: call 4CEBC0 (SetDeleteCallback,
            ecx = [esi+2Ch], args 4D1390h y 0) y [esi+2Ch] = 0; call 61A3A0 (~CTask, ecx = esi); ret

Los falsos de los colaboradores anotan: cuerpos de destructor [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero, dword del puntero];
CleanUpOldReference [direccion, ECX, argumento (etiquetas)]; SetDeleteCallback [direccion, ECX, funcion (el DefaultAnimCB de la DLL se normaliza al del EXE), dato]; CEvent::operator new [direccion, tamano];
constructor 4B23D0 [direccion, ECX, orden, objetivo (etiqueta)]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexFleeEntity::ScalarDeletingDestructor", "CTaskComplexSmartFleePoint::ScalarDeletingDestructor", "CTaskComplexSmartFleeEntity::ScalarDeletingDestructor", "CTaskComplexFleeShooting::ScalarDeletingDestructor", "CTaskComplexFleeAnyMeans::ScalarDeletingDestructor", "CTaskComplexObserveTrafficLights::ScalarDeletingDestructor", "CTaskComplexObserveTrafficLightsAndAchieveHeading::ScalarDeletingDestructor", "CTaskComplexCrossRoadLookAndAchieveHeading::ScalarDeletingDestructor", "CTaskSimpleTurn180::ScalarDeletingDestructor", "CTaskComplexHitResponse::ScalarDeletingDestructor", "CTaskSimpleIKLookAt::ScalarDeletingDestructor", "CTaskSimpleIKPointArm::ScalarDeletingDestructor", "CTaskSimpleTriggerLookAt::ScalarDeletingDestructor", "CTaskSimpleClearLookAt::ScalarDeletingDestructor", "CTaskComplexUseClosestFreeScriptedAttractor::ScalarDeletingDestructor", "CEventPlayerCommandToGroupAttack::GetEventType", "CEventPlayerCommandToGroupAttack::GetEventPriority", "CPedGroupDefaultTaskAllocatorFollowAnyMeans::Constructor", "CEventPlayerCommandToGroupAttack::DestructorBody", "CEventPlayerCommandToGroupAttack::ScalarDeletingDestructor", "CEventPlayerCommandToGroupAttack::CloneEditable", "CTaskSimpleClimb::GetTaskType", "CTaskSimpleJetPack::ScalarDeletingDestructor", "CTaskSimpleClimb::DestructorBody"]
ENTRIES = (0x65D240, 0x65D260, 0x65D280, 0x65D2A0, 0x65D2C0, 0x637F60, 0x637F80, 0x637FA0, 0x637FC0, 0x638020, 0x639490, 0x6394B0, 0x6394D0, 0x6394F0, 0x639510,
           0x5F6360, 0x5F6400, 0x5F6450, 0x5F63F0, 0x5F63D0, 0x5F6370, 0x67A1C0, 0x6801D0, 0x67A1D0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CLEANUP, SETDELCB, NEWEV, CTORCMD, TASKDTOR = 0x571A00, 0x4CEBC0, 0x4B5620, 0x4B23D0, 0x61A3A0
DEFCB = 0x4D1390

SDD, CONST, CTOR, EVBODY, CLONE, CLIMBBODY = 'sdd const ctor evbody clone climbbody'.split()
KIND = [SDD] * 15 + [CONST, CONST, CTOR, EVBODY, SDD, CLONE, CONST, SDD, CLIMBBODY]
CONSTANT = {15: 0x47, 16: 0x2C, 21: 0xFE}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {0: 0x65B9D0, 1: 0x65BDB0, 2: 0x65C4D0, 3: 0x65C9A0, 4: 0x65CD10, 5: 0x6317C0, 6: 0x631980, 7: 0x631B50, 8: 0x631D00, 9: 0x631DA0, 10: 0x633EF0, 11: 0x634240, 12: 0x634500, 13: 0x6346A0, 14: 0x634720,
        19: 0x5F63F0, 22: 0x67E600}
DELETE = {fn: 0x61A5B0 for fn in list(range(15)) + [22]}
DELETE[19] = 0x4B5630
SPAN = {CLONE: 0x1C, CLIMBBODY: 0x30}
CTOR_VTABLE = 0x86C75C
ATTACK_VTABLE = 0x86C710
CLIMB_VTABLE = 0x87059C
PARENT_DTOR_BODY = 0x4B2470


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


class World:
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

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
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


KEYS = {}
for _fn in range(24):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CONST, CTOR, EVBODY):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    elif _kind == SDD:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == CLIMBBODY:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'target_set', 'target_null', 'anim_set', 'anim_null', 'target_in_arena', 'anim_in_arena']
    elif _kind == CLONE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'new_null', 'new_ok', 'target_in_arena', 'target_raw']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    k = KIND[fn]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPAN.get(k, 4) > WIN:
        marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT[fn]
    elif k == CTOR:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, CTOR_VTABLE)                              # `mov [eax],86C75Ch` con eax = this
        ret = tag(0, o)
    elif k == EVBODY:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([PARENT_DTOR_BODY, tag(0, o), ATTACK_VTABLE])       # `mov [ecx],86C710h` y `jmp 4B2470`: el cuerpo de la base ve la vtable de la clase
        A.wr(o, 4, a[5])
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], tag(0, o), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLIMBBODY:
        target = {0: 0, 1: W.raw(1, a[7]), 2: a[8]}[a[6]]
        anim = {0: 0, 1: W.raw(1, a[10]), 2: a[11]}[a[9]]
        A.wr(o, 4, a[3])
        A.wr(o + 0x24, 4, target)
        A.wr(o + 0x2C, 4, anim)
        W.snap()
        A.wr(o, 4, CLIMB_VTABLE)
        if target:
            events.append([CLEANUP, W.tagv(target), tag(0, o + 0x24)])
            marks.append('target_set')
            if a[6] == 1:
                marks.append('target_in_arena')
        else:
            marks.append('target_null')
        if anim:
            events.append([SETDELCB, W.tagv(anim), DEFCB, 0])
            A.wr(o + 0x2C, 4, 0)                                  # [esi+2Ch] = 0 tras SetDeleteCallback
            marks.append('anim_set')
            if a[9] == 1:
                marks.append('anim_in_arena')
        else:
            marks.append('anim_null')
        events.append([TASKDTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLONE:
        target = W.raw(2, a[5]) if a[4] else a[3]
        A.wr(o + 0x18, 4, target)
        newp = W.raw(1, a[7]) if a[6] else 0
        W.snap()
        marks.append('target_in_arena' if a[4] else 'target_raw')
        events.append([NEWEV, 0x1C])
        if newp:
            events.append([CTORCMD, W.tagv(newp), 0, W.tagv(target)])    # push [edi+18h]; push 0; ecx = nuevo
            W.wr(newp, 4, a[8])                                          # el falso del constructor deja un dword del caso
            W.wr(newp, 4, ATTACK_VTABLE)                                 # mov [esi],86C710h
            marks.append('new_ok')
        else:
            marks.append('new_null')
        ret = W.tagv(newp)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
