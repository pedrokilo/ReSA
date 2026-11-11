"""Oraculo independiente (Python) de Fn160A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 160A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
  1, 2, 7, 11, 12, 18, 22  destructores con borrado (`push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je ..; push esi; call <liberacion>; add esp,4; mov eax,esi; pop esi; ret 4`):
         0x68B0C0 (cuerpo 6857D0), 0x68B0E0 (688A00), 0x6705B0 (66B830), 0x67DBC0 (67A1D0), 0x662350 (65DF30), 0x5FEE20 (5FED80), 0x5FEF20 (5FEE80); la liberacion es CTask::operator delete 61A5B0
         (tareas) o CEvent::operator delete 4B5630 (eventos); solo cuenta el BIT 0 del BYTE bajo de la ranura de pila
  3, 8, 16, 20  constantes virtuales (ranura 4 o 1): 0x66B690 `mov eax,393h`, 0x67A050 `mov eax,0D3h`, 0x5FED90 `mov eax,3`, 0x5FEE90 `mov eax,4`
  4      0x66B6B0 (MakeAbortable, ranura 6): `mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]` con la pila intacta (ped, prioridad, evento): llama a la ranura 6 de la subtarea y devuelve su EAX
  5, 9   0x66B710 y 0x67A100 (ControlSubTask, ranura 10): `mov eax,[ecx+8]; ret 4`
  0      0x68B050 (CTaskSimpleSwim::Clone): eax = 61A5A0(64h) (cdecl); si es nulo, devuelve 0; si no: ecx = eax; call 688930(&[this+14h], [this+20h]) y devuelve su EAX (this)
  6      0x66D1E0 (CTaskComplexGoToPointAnyMeans::Clone): eax = 61A5A0(34h); null -> 0; call 66B790([+18h], &[+0Ch], [+20h], [+1Ch], [+24h]) y devuelve su EAX
  10     0x67C610 (CTaskSimpleClimb::Clone): eax = 61A5A0(30h); null -> 0; call 67A110([+24h], &[+18h], [+14h], byte [+0Fh], byte [+0Dh], byte [+0Bh]) (los bytes se empujan con cero extendido; el
         constructor solo lee el byte) y devuelve su EAX
  17, 21 0x5FEDA0 / 0x5FEEA0 (Clone de CEventPedCollisionWithPlayer / CEventPlayerCollisionWithPed, marco SEH): edi = 4B5620(34h); null -> 0; si no: call 4AC990([+0Ch] como palabra, [+10h] (fld/fstp), [+14h],
         &[+18h], &[+24h], movsx [+30h], movsx [+32h]) con ecx = edi (el valor devuelto se IGNORA); [edi] = 86C970h / 86C9B0h; devuelve edi
  13     0x667310: lee movsx [this+8], [this+0Ah], [this+0Ch] ANTES de escribir; escribe x, y, z = entero * 0.125 (0x858C48) en [destino], [destino+4], [destino+8] (en ese orden); void
  14     0x667460: al = (byte [ecx] == byte [0x858B54] (= 0)); eax = 0 o 1
  15, 19 0x5FED80 / 0x5FEE80: [ecx] = 86C970h / 86C9B0h; jmp 4ACA70 (cuerpo de ~CEventPedCollisionWithPed)

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos de destructor); operator delete anota [direccion, puntero (etiqueta), dword del puntero] (prueba que
se borra DESPUES de destruir); operator new [direccion, tamano]; los constructores [direccion, ECX (etiqueta), argumentos (etiquetas de arena para punteros; solo la palabra o el byte que lee el constructor)];
la subtarea de MakeAbortable [0xA001, ECX (etiqueta), ped, prioridad, evento].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskSimpleSwim::Clone", "CTaskSimplePlayerOnFoot::ScalarDeletingDestructor", "CTaskSimpleSwim::ScalarDeletingDestructor", "CTaskComplexGoToAttractor::GetTaskType",
         "CTaskComplexGoToAttractor::MakeAbortable", "CTaskComplexGoToAttractor::ControlSubTask", "CTaskComplexGoToPointAnyMeans::Clone", "CTaskComplexGoToPointAnyMeans::ScalarDeletingDestructor",
         "CTaskComplexJump::GetTaskType", "CTaskComplexJump::ControlSubTask", "CTaskSimpleClimb::Clone", "CTaskSimpleClimb::ScalarDeletingDestructor", "CTaskComplexGangLeader::ScalarDeletingDestructor",
         "CPathNode::GetNodeCoors", "CTaskSimpleRunNamedAnim::AnimNameIsEmpty", "CEventPedCollisionWithPlayer::DestructorBody", "CEventPedCollisionWithPlayer::GetEventType",
         "CEventPedCollisionWithPlayer::Clone", "CEventPedCollisionWithPlayer::ScalarDeletingDestructor", "CEventPlayerCollisionWithPed::DestructorBody", "CEventPlayerCollisionWithPed::GetEventType",
         "CEventPlayerCollisionWithPed::Clone", "CEventPlayerCollisionWithPed::ScalarDeletingDestructor"]
ENTRIES = (0x68B050, 0x68B0C0, 0x68B0E0, 0x66B690, 0x66B6B0, 0x66B710, 0x66D1E0, 0x6705B0, 0x67A050, 0x67A100, 0x67C610, 0x67DBC0,
           0x662350, 0x667310, 0x667460, 0x5FED80, 0x5FED90, 0x5FEDA0, 0x5FEE20, 0x5FEE80, 0x5FEE90, 0x5FEEA0, 0x5FEF20)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
TASK_NEW, TASK_DEL, EVENT_NEW, EVENT_DEL, EVENT_BODY_CALLEE, SUBTASK_EVENT = 0x61A5A0, 0x61A5B0, 0x4B5620, 0x4B5630, 0x4ACA70, 0xA001
CTOR_SWIM, CTOR_ANY, CTOR_CLIMB, CTOR_EVENT = 0x688930, 0x66B790, 0x67A110, 0x4AC990

# forma de cada funcion
SDD, CONST, RETFIELD, ABORT, CLONE, EVBODY, NODE, EMPTY = 'sdd const retfield abort clone evbody node empty'.split()
KIND = [CLONE, SDD, SDD, CONST, ABORT, RETFIELD, CLONE, SDD, CONST, RETFIELD, CLONE, SDD, SDD, NODE, EMPTY, EVBODY, CONST, CLONE, SDD, EVBODY, CONST, CLONE, SDD]
CONSTS = {3: 0x393, 8: 0xD3, 16: 3, 20: 4}
BODY = {1: 0x6857D0, 2: 0x688A00, 7: 0x66B830, 11: 0x67A1D0, 12: 0x65DF30, 18: 0x5FED80, 22: 0x5FEE80}
DELETE = {1: TASK_DEL, 2: TASK_DEL, 7: TASK_DEL, 11: TASK_DEL, 12: TASK_DEL, 18: EVENT_DEL, 22: EVENT_DEL}
EV_VTABLE = {15: 0x86C970, 17: 0x86C970, 19: 0x86C9B0, 21: 0x86C9B0}
# Clone: (palabras del objeto fuente, indice de la palabra puntero, direccion de operator new, tamano)
CLONE_INFO = {0: (7, 6, TASK_NEW, 0x64), 6: (8, 6, TASK_NEW, 0x34), 10: (8, 7, TASK_NEW, 0x30), 17: (11, 3, EVENT_NEW, 0x34), 21: (11, 3, EVENT_NEW, 0x34)}


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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


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


KEYS = {}
for _fn in range(len(NAMES)):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == EVBODY:
        _k += ['prev_zero', 'prev_other']
    elif _kind == RETFIELD:
        _k += ['value_raw', 'value_in_b', 'value_in_c', 'value_this']
    elif _kind == ABORT:
        _k += ['sub_in_b', 'sub_in_c', 'sub_this', 'prio_zero', 'prio_other']
    elif _kind == CLONE:
        _k += ['new_null', 'new_ok', 'ptr_raw', 'ptr_arena']
        if _fn == 10:
            _k += ['bytes_high', 'bytes_low']
        if _fn in (17, 21):
            _k += ['words_negative', 'words_positive']
    elif _kind == NODE:
        _k += ['out_in_b', 'out_in_c', 'out_overlap', 'out_in_a_else', 'coord_negative', 'coord_zero']
    elif _kind == EMPTY:
        _k += ['empty_string', 'nonempty_string']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    return ['kind%d' % kind, 'fill%d' % fill, 'this_unaligned' if a[0] % 4 else 'this_aligned']


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn]
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], W.tagv(this), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], W.tagv(this), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = W.tagv(this)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == EVBODY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, EV_VTABLE[fn])                         # deja su vtable y salta al cuerpo de ~CEventPedCollisionWithPed
        events.append([EVENT_BODY_CALLEE, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == RETFIELD:
        mode = a[3]
        val = a[4] if mode == 0 else (W.raw(1, a[4]) if mode == 1 else (W.raw(2, a[4]) if mode == 2 else this))
        A.wr(o, 4, a[6])
        A.wr(o + 8, 4, val)
        W.snap()
        ret = W.tagv(A.rd(o + 8, 4))
        marks.append(['value_raw', 'value_in_b', 'value_in_c', 'value_this'][mode])
    elif k == ABORT:
        mode = a[3]
        sub = W.raw(1, a[4]) if mode == 0 else (W.raw(2, a[4]) if mode == 1 else this)
        A.wr(o, 4, a[9])
        A.wr(o + 8, 4, sub)
        W.snap()                                          # el puntero a la vtable falsa que el banco escribe en la subtarea queda en la imagen de referencia
        sub = A.rd(o + 8, 4)
        events.append([SUBTASK_EVENT, W.tagv(sub), W.tagv(a[6]), a[7], W.tagv(a[8])])
        ret = a[5]
        marks.append(['sub_in_b', 'sub_in_c', 'sub_this'][mode])
        marks.append('prio_zero' if a[7] == 0 else 'prio_other')
    elif k == CLONE:
        nwords, pidx, newaddr, size = CLONE_INFO[fn]
        for i in range(nwords):
            A.wr(o + 8 + 4 * i, 4, a[16 + i])
        ptr = a[4] if a[3] == 0 else W.raw(2, a[4])
        A.wr(o + 8 + 4 * pidx, 4, ptr)
        W.snap()
        new = W.raw(1, a[6]) if a[5] else 0
        events.append([newaddr, size])
        word = lambda i: A.rd(o + 8 + 4 * i, 4)
        marks.append('new_ok' if new else 'new_null')
        marks.append('ptr_raw' if a[3] == 0 else 'ptr_arena')
        if new:
            if fn == 0:
                events.append([CTOR_SWIM, W.tagv(new), W.tagv(this + 0x14), W.tagv(word(6))])
            elif fn == 6:
                events.append([CTOR_ANY, W.tagv(new), word(4), W.tagv(this + 0xC), W.tagv(word(6)), word(5), word(7)])
            elif fn == 10:
                bytes3 = [A.rd(o + 0xF, 1), A.rd(o + 0xD, 1), A.rd(o + 0xB, 1)]
                events.append([CTOR_CLIMB, W.tagv(new), W.tagv(word(7)), W.tagv(this + 0x18), word(3)] + bytes3)
                marks.append('bytes_high' if any(b & 0x80 for b in bytes3) else 'bytes_low')
            else:
                words = [A.rd(o + 0xC, 2), A.rd(o + 0x30, 2), A.rd(o + 0x32, 2)]
                events.append([CTOR_EVENT, W.tagv(new), words[0], word(2), W.tagv(word(3)), W.tagv(this + 0x18), W.tagv(this + 0x24), words[1], words[2]])
                marks.append('words_negative' if any(w & 0x8000 for w in words) else 'words_positive')
            final = a[7] if fn in (0, 6, 10) else EV_VTABLE[fn]    # los constructores de tarea dejan lo que escribe el falso; en los eventos el Clone escribe la vtable despues
            W.wr(new, 4, a[7])
            W.wr(new, 4, final)
            ret = W.tagv(new)
        else:
            ret = 0
            if fn == 10:
                marks.append('bytes_high' if any(A.rd(o + off, 1) & 0x80 for off in (0xF, 0xD, 0xB)) else 'bytes_low')
            if fn in (17, 21):
                marks.append('words_negative' if any(w & 0x8000 for w in (A.rd(o + 0xC, 2), A.rd(o + 0x30, 2), A.rd(o + 0x32, 2))) else 'words_positive')
    elif k == NODE:
        A.wr(o + 8, 4, a[3])
        A.wr(o + 0xC, 4, a[4])
        mode = a[5]
        out = W.raw(1, a[6]) if mode == 0 else (W.raw(2, a[6]) if mode == 1 else ((this + 8 + s32(a[6])) & M32 if mode == 2 else W.raw(0, a[6])))
        W.snap()
        vals = [s16(A.rd(o + 8, 2)), s16(A.rd(o + 0xA, 2)), s16(A.rd(o + 0xC, 2))]   # los tres enteros se leen antes de escribir
        for i, v in enumerate(vals):
            W.wr((out + 4 * i) & M32, 4, struct.unpack('<I', struct.pack('<f', v * 0.125))[0])
        marks.append(['out_in_b', 'out_in_c', 'out_overlap', 'out_in_a_else'][mode])
        marks.append('coord_negative' if any(v < 0 for v in vals) else 'coord_zero')
    elif k == EMPTY:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 1 if (a[3] & 0xFF) == 0 else 0
        marks.append('empty_string' if ret else 'nonempty_string')
    out = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}
    return out


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
