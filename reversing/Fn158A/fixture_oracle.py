"""Oraculo independiente (Python) de Fn158A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 158A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   CEventLeaderEntryExit (vtable 0x859540, 16 ranuras)
   0  0x43E1E0  ranura 9  HasEditableResponse: xor al,al; ret
   1  0x43E1F0  ranura 10 GetSourceEntity: xor eax,eax; ret
   2  0x43E210  ranura 3  GetLifeTime: xor eax,eax; ret
   3  0x43E280  ranura 15 CanBeInterruptedBySameEvent: mov al,1; ret
   4  0x43E200  ranura 1  GetEventType: mov eax,4Dh; ret
   5  0x43E2C0  ranura 2  GetEventPriority: mov eax,37h; ret
   6  0x43E2B0  cuerpo del destructor: mov [ecx],859540h; jmp 4B2210 (cuerpo de ~CEventAreaCodes)
   7  0x43E290  ranura 0  destructor con borrado: push esi; mov esi,ecx; call 43E2B0; test byte [esp+8],1; je +; push esi; call 4B5630; add esp,4; mov eax,esi; pop esi; ret 4
   8  0x43E220  ranura 4  Clone (marco SEH): eax = 4B5620(10h) (cdecl); si no es nulo: ecx = eax; call 4B2190 con el dword [this+0Ch] (thiscall `ret 4`, su EAX se descarta); [esi] = 859540h; devuelve esi; si es nulo, devuelve 0
   CTaskSimpleUninterruptable (vtable 0x859580, 9 ranuras)
   9  0x43E300  ranura 2  GetSubTask: xor eax,eax; ret
  10  0x43E310  ranura 3  IsSimple: mov al,1; ret
  11  0x43E320  ranura 8  SetPedPosition: xor al,al; ret 4
  12  0x43E3A0  ranura 7  ProcessPed: xor al,al; ret 4
  13  0x43E380  ranura 4  GetTaskType: mov eax,0C9h; ret
  14  0x43E390  ranura 6  MakeAbortable: cmp dword [esp+8],2; sete al; ret 0Ch  (compara el dword completo de la prioridad)
  15  0x43E3D0  cuerpo del destructor: mov [ecx],859580h; jmp 61A3A0 (~CTask)
  16  0x43E3B0  ranura 0  destructor con borrado: cuerpo 43E3D0; CTask::operator delete 61A5B0
  17  0x43E330  ranura 1  Clone (marco SEH): eax = 61A5A0(8) (cdecl); si no es nulo: ecx = eax; call 61A390 (su EAX se descarta); [esi] = 859580h; devuelve esi; si es nulo, devuelve 0
   CTaskSimpleGoToPointFine (vtable 0x86F964)
  18  0x65EEE0  ranura 4  GetTaskType: mov eax,39Eh; ret
  19  0x65EEF0  cuerpo del destructor: mov [ecx],86F964h; jmp 667A00 (~CTaskSimpleGoTo)
   CTaskComplexTrackEntity (vtable 0x86F998)
  20  0x663620  ranura 0  destructor con borrado: cuerpo 65F460; CTask::operator delete 61A5B0
   CTaskGangHasslePed (vtable 0x86FA00)
  21  0x6620D0  ranura 1  Clone (marco SEH): eax = 61A5A0(2Ch); si no es nulo: ecx = eax; push [esi+18h], [esi+14h], [esi+10h], [esi+0Ch] (el ultimo, arriba: ped); call 65FED0; devuelve el EAX del constructor; si es nulo, devuelve 0
   CTaskSimpleCarForcePedOut (vtable 0x86EE94)
  22  0x647770  ranura 4  GetTaskType: mov eax,32Fh; ret

Los falsos de los colaboradores anotan: cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero, dword del puntero];
operator new [direccion, tamano]; constructor 4B2190 [direccion, ECX, dword de [ECX], ped]; constructor 61A390 [direccion, ECX, dword de [ECX]]; constructor 65FED0 [direccion, ECX, dword de [ECX], ped, a3, a4, a5].
Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CEventLeaderEntryExit::HasEditableResponse', 'CEventLeaderEntryExit::GetSourceEntity', 'CEventLeaderEntryExit::GetLifeTime', 'CEventLeaderEntryExit::CanBeInterruptedBySameEvent', 'CEventLeaderEntryExit::GetEventType', 'CEventLeaderEntryExit::GetEventPriority', 'CEventLeaderEntryExit::DestructorBody', 'CEventLeaderEntryExit::ScalarDeletingDestructor', 'CEventLeaderEntryExit::Clone', 'CTaskSimple::GetSubTask', 'CTaskSimple::IsSimple', 'CTaskSimple::SetPedPosition', 'CTaskSimpleUninterruptable::ProcessPed', 'CTaskSimpleUninterruptable::GetTaskType', 'CTaskSimpleUninterruptable::MakeAbortable', 'CTaskSimpleUninterruptable::DestructorBody', 'CTaskSimpleUninterruptable::ScalarDeletingDestructor', 'CTaskSimpleUninterruptable::Clone', 'CTaskSimpleGoToPointFine::GetTaskType', 'CTaskSimpleGoToPointFine::DestructorBody', 'CTaskComplexTrackEntity::ScalarDeletingDestructor', 'CTaskGangHasslePed::Clone', 'CTaskSimpleCarForcePedOut::GetTaskType']
ENTRIES = (0x43E1E0, 0x43E1F0, 0x43E210, 0x43E280, 0x43E200, 0x43E2C0, 0x43E2B0, 0x43E290, 0x43E220, 0x43E300, 0x43E310, 0x43E320, 0x43E3A0, 0x43E380, 0x43E390, 0x43E3D0, 0x43E3B0, 0x43E330,
           0x65EEE0, 0x65EEF0, 0x663620, 0x6620D0, 0x647770)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
NEWEV, NEWTASK, CTORAC, CTORSIMPLE, CTORGANG = 0x4B5620, 0x61A5A0, 0x4B2190, 0x61A390, 0x65FED0

CONST, BOOL, ABORT, SDD, EVBODY, EVCLONE, TASKCLONE, GANGCLONE = 'const bool abort sdd evbody evclone taskclone gangclone'.split()
KIND = [BOOL, CONST, CONST, BOOL, CONST, CONST, EVBODY, SDD, EVCLONE,
        CONST, BOOL, BOOL, BOOL, CONST, ABORT, EVBODY, SDD, TASKCLONE,
        CONST, EVBODY, SDD, GANGCLONE, CONST]
CONSTANT = {1: 0, 2: 0, 4: 0x4D, 5: 0x37, 9: 0, 13: 0xC9, 18: 0x39E, 22: 0x32F}
CONSTANT_AL = {0: 0, 3: 1, 10: 1, 11: 0, 12: 0}
NARGS = {11: 1, 12: 1}
# cuerpos de destructor: (direccion del EXE de la base a la que salta, vtable que deja la funcion antes de saltar)
EVBODIES = {6: (0x4B2210, 0x859540), 15: (0x61A3A0, 0x859580), 19: (0x667A00, 0x86F964)}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {7: 0x43E2B0, 16: 0x43E3D0, 20: 0x65F460}
DELETE = {7: 0x4B5630, 16: 0x61A5B0, 20: 0x61A5B0}
SPAN = {EVCLONE: 0x10, GANGCLONE: 0x2C}


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
for _fn in range(len(NAMES)):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CONST, BOOL, EVBODY):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    elif _kind == ABORT:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prio_2', 'prio_other', 'prio_low_byte_2_only', 'prio_negative']
    elif _kind == SDD:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == EVCLONE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'new_null', 'new_ok', 'player_in_arena', 'player_raw']
    elif _kind == TASKCLONE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'new_null', 'new_ok']
    elif _kind == GANGCLONE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'new_null', 'new_ok', 'player_in_arena', 'player_raw', 'ctor_ret_zero', 'ctor_ret_other']
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
    elif k == BOOL:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT_AL[fn]
    elif k == ABORT:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 1 if a[4] == 2 else 0                      # compara el dword completo
        marks.append('prio_2' if a[4] == 2 else 'prio_other')
        if a[4] != 2 and (a[4] & 0xFF) == 2:
            marks.append('prio_low_byte_2_only')
        if a[4] >> 31:
            marks.append('prio_negative')
    elif k == EVBODY:
        base, vt = EVBODIES[fn]
        A.wr(o, 4, a[3])
        W.snap()
        events.append([base, tag(0, o), vt])             # `mov [ecx],vtable` y `jmp base`: el cuerpo de la base ve la vtable de la clase
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
    elif k in (EVCLONE, GANGCLONE):
        player = W.raw(2, a[5]) if a[4] else a[3]
        A.wr(o + 0xC, 4, player)
        if k == GANGCLONE:
            A.wr(o + 0x10, 4, a[16])
            A.wr(o + 0x14, 4, a[17])
            A.wr(o + 0x18, 4, a[18])
        newp = W.raw(1, a[7]) if a[6] else 0
        W.snap()
        marks.append('player_in_arena' if a[4] else 'player_raw')
        if k == EVCLONE:
            events.append([NEWEV, 0x10])
            if newp:
                events.append([CTORAC, W.tagv(newp), W.rd(newp, 4), W.tagv(player)])
                W.wr(newp, 4, a[8])                      # el constructor falso deja su dword
                W.wr(newp, 4, 0x859540)                  # y la funcion deja la vtable de la clase
                marks.append('new_ok')
                ret = W.tagv(newp)
            else:
                marks.append('new_null')
                ret = 0
        else:
            events.append([NEWTASK, 0x2C])
            if newp:
                events.append([CTORGANG, W.tagv(newp), W.rd(newp, 4), W.tagv(player), a[16], a[17], a[18]])
                W.wr(newp, 4, a[8])
                marks.append('new_ok')
                marks.append('ctor_ret_zero' if a[9] == 0 else 'ctor_ret_other')
                ret = W.tagv(a[9])                       # devuelve lo que deja el constructor en EAX
            else:
                marks.append('new_null')
                ret = 0
    elif k == TASKCLONE:
        A.wr(o, 4, a[3])
        newp = W.raw(1, a[7]) if a[6] else 0
        W.snap()
        events.append([NEWTASK, 8])
        if newp:
            events.append([CTORSIMPLE, W.tagv(newp), W.rd(newp, 4)])
            W.wr(newp, 4, a[8])
            W.wr(newp, 4, 0x859580)
            marks.append('new_ok')
            ret = W.tagv(newp)
        else:
            marks.append('new_null')
            ret = 0
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
