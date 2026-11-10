"""Oraculo independiente (Python) de Fn155A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 17 funciones del lote 155A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x4DEFE0  CAEGlobalWeaponAudioEntity::PickAmbientGunFire (thiscall sin usar this, `ret 0Ch`): r = 4D9C10(0,1); si r == 0: *p1 = 4D9C10(4,14); *p2 = 1Eh; *p3 = 4D9C10(90,350);
                si r == 1: *p1 = 4D9C10(2,4); *p2 = 16h; *p3 = 4D9C10(500,1200); otro valor: nada (las salidas se escriben en ese orden; la tercera llamada al RNG va antes de la escritura de *p3)
   1  0x625250  CTaskSimpleGunControl (vtable 0x86D76C) destructor con borrado: push esi; mov esi,ecx; call 61F4D0; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   2  0x61DF30  CTaskSimpleUseGun cuerpo del destructor (marco SEH): [esi]=86D724h; si [esi+1Ch]: call 571A00 (CleanUpOldReference, ecx = [esi+1Ch], arg = esi+1Ch); si [esi+2Ch]: call 4CEBC0
                (SetDeleteCallback, ecx = [esi+2Ch], args 4D1390h y 0); call 61A3A0 (~CTask); ret
   3  0x6091B0  CEventDontJoinPlayerGroup::GetLifeTime (vtable 0x86D0E0, ranura 3): xor eax,eax; ret
   4  0x609220  CEventDontJoinPlayerGroup::AffectsPed (ranura 5): mov al,1; ret 4
   5  0x6091A0  CEventDontJoinPlayerGroup::GetEventType (ranura 1): mov eax,5Eh; ret
   6  0x609230  CEventDontJoinPlayerGroup destructor con borrado: cuerpo 609140; CEvent::operator delete 4B5630
   7  0x6091C0  CEventDontJoinPlayerGroup::Clone (ranura 4, marco SEH): eax = 4B5620(10h) (cdecl); si no es nulo: ecx = eax; call 6090E0 con el dword [this+0Ch] (el constructor, thiscall `ret 4`) y devuelve su EAX; si es nulo, devuelve 0
   8  0x64C200  CTaskSimpleCarSetPedSlowDraggedOut (vtable 0x86EF30) destructor con borrado: cuerpo 648430; CTask::operator delete 61A5B0
   9  0x64E2A0  CTaskComplexCarSlowBeDraggedOut (vtable 0x86EF54) destructor con borrado: cuerpo 64C580; CTask::operator delete 61A5B0
  10  0x648330  CTaskSimpleCarSlowBeDraggedOut::ComputeAnimID (thiscall(&grupo, &animId) `ret 8`): eax = [this+14h]; edx = animId; si (eax - 8) <= 3 (sin signo): [edx] = 17Bh (puertas 8, 9) o 17Ah (10, 11);
                ecx = [[this+10h]+384h]; byte [ecx+0DEh] = g; eax = [edx]; eax = 6E3B00(ecx = C1CDC0h + 94h * g, eax) (thiscall `ret 4`); [&grupo] = eax
  11  0x4B8330  CEventLeaderExitedCarAsDriver::GetLifeTime (vtable 0x85B898, ranura 3): xor eax,eax; ret
  12  0x4B8390  CEventLeaderExitedCarAsDriver::AffectsPed (ranura 5): xor al,al; ret 4
  13  0x4B8320  CEventLeaderExitedCarAsDriver::GetEventType (ranura 1): mov eax,37h; ret
  14  0x4B84D0  CEventLeaderExitedCarAsDriver::GetEventPriority (ranura 2): mov eax,2Fh; ret
  15  0x4B83C0  CEventLeaderExitedCarAsDriver cuerpo del destructor: mov [ecx],85B898h; jmp 4AC480
  16  0x4B83A0  CEventLeaderExitedCarAsDriver destructor con borrado: cuerpo 4B83C0; CEvent::operator delete 4B5630

Los falsos de los colaboradores anotan: cuerpos de destructor [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero, dword del puntero];
RNG 4D9C10 [direccion, minimo, maximo]; CleanUpOldReference [direccion, ECX, argumento (etiquetas)]; SetDeleteCallback [direccion, ECX, funcion (el DefaultAnimCB de la DLL se normaliza al del EXE), dato];
CEvent::operator new [direccion, tamano]; constructor 6090E0 [direccion, ECX, argumento]; GetGroup [direccion, ECX, animId]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CAEGlobalWeaponAudioEntity::PickAmbientGunFire', 'CTaskSimpleGunControl::ScalarDeletingDestructor', 'CTaskSimpleUseGun::DestructorBody',
         'CEventDontJoinPlayerGroup::GetLifeTime', 'CEventDontJoinPlayerGroup::AffectsPed', 'CEventDontJoinPlayerGroup::GetEventType', 'CEventDontJoinPlayerGroup::ScalarDeletingDestructor',
         'CEventDontJoinPlayerGroup::Clone', 'CTaskSimpleCarSetPedSlowDraggedOut::ScalarDeletingDestructor', 'CTaskComplexCarSlowBeDraggedOut::ScalarDeletingDestructor',
         'CTaskSimpleCarSlowBeDraggedOut::ComputeAnimID', 'CEventLeaderExitedCarAsDriver::GetLifeTime', 'CEventLeaderExitedCarAsDriver::AffectsPed', 'CEventLeaderExitedCarAsDriver::GetEventType',
         'CEventLeaderExitedCarAsDriver::GetEventPriority', 'CEventLeaderExitedCarAsDriver::DestructorBody', 'CEventLeaderExitedCarAsDriver::ScalarDeletingDestructor']
ENTRIES = (0x4DEFE0, 0x625250, 0x61DF30, 0x6091B0, 0x609220, 0x6091A0, 0x609230, 0x6091C0, 0x64C200, 0x64E2A0, 0x648330, 0x4B8330, 0x4B8390, 0x4B8320, 0x4B84D0, 0x4B83C0, 0x4B83A0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
RAND, CLEANUP, SETDELCB, NEWEV, CTOREV, GETGROUP, TASKDTOR = 0x4D9C10, 0x571A00, 0x4CEBC0, 0x4B5620, 0x6090E0, 0x6E3B00, 0x61A3A0
DEFCB = 0x4D1390

PICK, SDD, USEGUN, CONST, CONSTAL1, CLONE, ANIMID, EVBODY = 'pick sdd usegun const constal1 clone animid evbody'.split()
KIND = [PICK, SDD, USEGUN, CONST, CONSTAL1, CONST, SDD, CLONE, SDD, SDD, ANIMID, CONST, CONSTAL1, CONST, CONST, EVBODY, SDD]
CONSTANT = {3: 0, 5: 0x5E, 11: 0, 13: 0x37, 14: 0x2F}
CONSTANT_AL = {4: 1, 12: 0}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {1: 0x61F4D0, 6: 0x609140, 8: 0x648430, 9: 0x64C580, 16: 0x4B83C0}
DELETE = {1: 0x61A5B0, 6: 0x4B5630, 8: 0x61A5B0, 9: 0x61A5B0, 16: 0x4B5630}
SPAN = {USEGUN: 0x3C, CLONE: 0x10, ANIMID: 0x18}


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
for _fn in range(17):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CONST, CONSTAL1, EVBODY):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    elif _kind == PICK:
        _k += ['this_aligned', 'this_unaligned', 'pick_case0', 'pick_case1', 'pick_other', 'pick_alias_p1p2', 'pick_alias_p2p3', 'pick_alias_p1p3', 'pick_alias_all', 'pick_overlap_plus1', 'pick_overlap_minus2']
    elif _kind == SDD:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == USEGUN:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'target_set', 'target_null', 'anim_set', 'anim_null', 'target_in_arena', 'anim_in_arena']
    elif _kind == CLONE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'new_null', 'new_ok', 'player_in_arena', 'player_raw']
    elif _kind == ANIMID:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'door_8', 'door_9', 'door_10', 'door_11', 'door_other', 'group_zero', 'group_high', 'alias_group_anim', 'alias_anim_door', 'alias_anim_handling', 'alias_group_this']
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
    elif k == CONSTAL1:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT_AL[fn]
    elif k == EVBODY:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([0x4AC480, tag(0, o), 0x85B898])       # `mov [ecx],85B898h` y `jmp 4AC480`: el cuerpo de la base ve la vtable de la clase
        A.wr(o, 4, a[5])
    elif k == PICK:
        A.wr(o, 4, a[3])
        W.snap()
        p = [W.raw(1, a[7]), W.raw(1, a[8]), W.raw(1, a[9])]
        mode = a[6]
        if mode == 1:
            p[1] = p[0]
            marks.append('pick_alias_p1p2')
        elif mode == 2:
            p[2] = p[1]
            marks.append('pick_alias_p2p3')
        elif mode == 3:
            p[2] = p[0]
            marks.append('pick_alias_p1p3')
        elif mode == 4:
            p[1] = p[2] = p[0]
            marks.append('pick_alias_all')
        elif mode == 5:
            p[1] = (p[0] + 1) & M32
            marks.append('pick_overlap_plus1')
        elif mode == 6:
            p[2] = (p[0] - 2) & M32
            marks.append('pick_overlap_minus2')
        r = [a[3], a[4], a[5]]
        events.append([RAND, 0, 1])
        if r[0] == 0:
            events.append([RAND, 4, 14])
            W.wr(p[0], 4, r[1])
            W.wr(p[1], 4, 30)
            events.append([RAND, 90, 350])
            W.wr(p[2], 4, r[2])
            marks.append('pick_case0')
        elif r[0] == 1:
            events.append([RAND, 2, 4])
            W.wr(p[0], 4, r[1])
            W.wr(p[1], 4, 22)
            events.append([RAND, 500, 1200])
            W.wr(p[2], 4, r[2])
            marks.append('pick_case1')
        else:
            marks.append('pick_other')
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
    elif k == USEGUN:
        target = {0: 0, 1: W.raw(1, a[7]), 2: a[8]}[a[6]]
        anim = {0: 0, 1: W.raw(1, a[10]), 2: a[11]}[a[9]]
        A.wr(o, 4, a[3])
        A.wr(o + 0x1C, 4, target)
        A.wr(o + 0x2C, 4, anim)
        W.snap()
        A.wr(o, 4, 0x86D724)
        if target:
            events.append([CLEANUP, W.tagv(target), tag(0, o + 0x1C)])
            marks.append('target_set')
            if a[6] == 1:
                marks.append('target_in_arena')
        else:
            marks.append('target_null')
        if anim:
            events.append([SETDELCB, W.tagv(anim), DEFCB, 0])
            marks.append('anim_set')
            if a[9] == 1:
                marks.append('anim_in_arena')
        else:
            marks.append('anim_null')
        events.append([TASKDTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLONE:
        player = W.raw(2, a[5]) if a[4] else a[3]
        A.wr(o + 0xC, 4, player)
        newp = W.raw(1, a[7]) if a[6] else 0
        W.snap()
        marks.append('player_in_arena' if a[4] else 'player_raw')
        events.append([NEWEV, 0x10])
        if newp:
            events.append([CTOREV, W.tagv(newp), W.tagv(player)])
            W.wr(newp, 4, a[8])
            marks.append('new_ok')
        else:
            marks.append('new_null')
        ret = W.tagv(newp)
    elif k == ANIMID:
        door, mode = a[3], a[7]
        veh = W.raw(1, a[8])
        hand = W.raw(2, a[9])
        group_slot = W.raw(1, a[10])
        anim_slot = W.raw(2, a[11])
        this = W.raw(0, o)
        A.wr(o + 0x10, 4, veh)
        A.wr(o + 0x14, 4, door)
        B.wr(a[8] + 0x384, 4, hand)
        Cc.wr(a[9] + 0xDC, 4, a[16])
        Cc.wr(a[9] + 0xDE, 1, a[4] & 0xFF)
        if mode == 2:
            anim_slot = this + 0x14
            marks.append('alias_anim_door')
        elif mode == 3:
            anim_slot = hand + 0xDC
            marks.append('alias_anim_handling')
        elif mode == 4:
            anim_slot = hand + 0xDE
            marks.append('alias_anim_handling')
        if mode in (0, 1):
            if mode == 1:
                group_slot = anim_slot
                marks.append('alias_group_anim')
            W.wr(anim_slot, 4, a[5])
        elif mode == 5:
            group_slot = this + 0x10
            marks.append('alias_group_this')
        elif mode == 6:
            group_slot = hand + 0xDE
            marks.append('alias_group_anim')
        W.snap()
        marks.append('door_%d' % door if door in (8, 9, 10, 11) else 'door_other')
        if ((door - 8) & M32) <= 3:
            W.wr(anim_slot, 4, 0x17B if door in (8, 9) else 0x17A)
        idx = W.rd(hand + 0xDE, 1)
        marks.append('group_zero' if idx == 0 else 'group_high' if idx >= 30 else 'group_mid')
        anim_id = W.rd(anim_slot, 4)
        events.append([GETGROUP, (0xC1CDC0 + 0x94 * idx) & M32, anim_id])
        W.wr(group_slot, 4, a[6])
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
