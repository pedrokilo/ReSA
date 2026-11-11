"""Oraculo independiente (Python) de Fn159A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 159A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
  0..10  destructores con borrado (`push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`):
         0x64BEE0 (cuerpo 647490), 0x64BF50 (647790), 0x64BF70 (647950), 0x64BF90 (648070), 0x64D290 (64A580), 0x6375B0 (62FC70), 0x6375D0 (62FCF0), 0x67CA00 (678160), 0x67CA20 (678310),
         0x67D550 (6790B0), 0x67D570 (679AF0); la liberacion es CTask::operator delete 61A5B0
  11..13 constantes virtuales (ranura 4): 0x62FA50 `mov eax,0D4h`, 0x677F70 `mov eax,0CDh`, 0x678ED0 `mov eax,0F2h`
  14..16 cuerpos de destructor con marco SEH: 0x62FB40 (vtable 86DDBCh, animacion en +24h), 0x677F80 (8703E0h, +0Ch), 0x678EE0 (8704FCh, +08h): [this] = vtable; ecx = animacion; si no es 0:
         push 0; push 4D1390h; call 4CEBE0; ecx = this; call 61A3A0
  17..19 0x537550 / 0x537560 / 0x537570: push esi; mov esi,ecx; call 40FAF0; mov eax,esi; pop esi; ret
  20     0x537A10: push esi; mov esi,ecx; call 403E00 (call 532A90; al = [+36h]; al = (al & 0F9h) | 1; [+36h] = al; or dword [+1Ch],1; [this] = 8585C8h); [this] = 8639B0h; eax = esi
  21     0x537290 (cdecl): esi = atomic; edi = call 749E60(atomic); eax = [esi+4]; ecx = [eax+0A0h]; call 74BF20(edi, ecx); call 74A490(clump, edi); eax = atomic
  22     0x537510: edx = [this+20h]; eax = 0; si edx == 0: ret 0; si edx <= 0 (con signo): eax = 1; si no: ecx = [this+2Ch]; repite edx veces: eax = max(eax, u16[ecx], u16[ecx+2], u16[ecx+4]) (sin signo);
         ecx += 8; al final eax + 1

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos de destructor, ~CTask, constructores de CBoundingBox y de CEntity); operator delete anota
[direccion, puntero (etiqueta), dword del puntero] (prueba que se borra DESPUES de destruir); SetFinishCallback [direccion, ECX, funcion (0x4D1390), datos]; RpAtomicClone [direccion, atomic];
RpAtomicSetFrame [direccion, atomic (el clon), raiz]; RpClumpAddAtomic [direccion, clump, atomic (el clon)]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskSimpleCarJumpOut::ScalarDeletingDestructor", "CTaskSimpleCarForcePedOut::ScalarDeletingDestructor", "CTaskSimpleCarSetPedOut::ScalarDeletingDestructor",
         "CTaskSimpleCarSlowDragPedOut::ScalarDeletingDestructor", "CTaskComplexGoToCarDoorAndStandStill::ScalarDeletingDestructor", "CTaskSimpleDieInCar::ScalarDeletingDestructor",
         "CTaskComplexDieInCar::ScalarDeletingDestructor", "CTaskComplexGetUpAndStandStill::ScalarDeletingDestructor", "CTaskSimpleFall::ScalarDeletingDestructor",
         "CTaskComplexFallToDeath::ScalarDeletingDestructor", "CTaskSimpleJump::ScalarDeletingDestructor", "CTaskSimpleDie::GetTaskType", "CTaskSimpleGetUp::GetTaskType",
         "CTaskSimpleLand::GetTaskType", "CTaskSimpleDie::DestructorBody", "CTaskSimpleGetUp::DestructorBody", "CTaskSimpleLand::DestructorBody", "CBoundingBox::ConstructorAlias537550",
         "CBoundingBox::ConstructorAlias537560", "CBoundingBox::ConstructorAlias537570", "CAnimatedBuilding::Constructor", "CloneAtomicToClumpCB", "CCollisionData::GetNoOfShdwVerts"]
ENTRIES = (0x64BEE0, 0x64BF50, 0x64BF70, 0x64BF90, 0x64D290, 0x6375B0, 0x6375D0, 0x67CA00, 0x67CA20, 0x67D550, 0x67D570, 0x62FA50, 0x677F70, 0x678ED0,
           0x62FB40, 0x677F80, 0x678EE0, 0x537550, 0x537560, 0x537570, 0x537A10, 0x537290, 0x537510)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SETFINISH, CTASKDTOR, CTASKDELETE, BBOXCTOR, ENTITYCTOR = 0x4CEBE0, 0x61A3A0, 0x61A5B0, 0x40FAF0, 0x532A90
RPCLONE, RPSETFRAME, RPADDATOM = 0x749E60, 0x74BF20, 0x74A490
DEFAULT_CB = 0x4D1390

# forma de cada funcion
SDD, CONST, ANIMBODY, BBOX, BUILDCTOR, CLONE, SHADOW = 'sdd const animbody bbox buildctor clone shadow'.split()
KIND = [SDD] * 11 + [CONST] * 3 + [ANIMBODY] * 3 + [BBOX] * 3 + [BUILDCTOR, CLONE, SHADOW]
CONSTS = {11: 0xD4, 12: 0xCD, 13: 0xF2}
# SDD: direccion (EXE) del cuerpo que llama
BODY = {0: 0x647490, 1: 0x647790, 2: 0x647950, 3: 0x648070, 4: 0x64A580, 5: 0x62FC70, 6: 0x62FCF0, 7: 0x678160, 8: 0x678310, 9: 0x6790B0, 10: 0x679AF0}
# cuerpos de destructor de tareas simples: (vtable, desplazamiento de la animacion)
ANIM_INFO = {14: (0x86DDBC, 0x24), 15: (0x8703E0, 0xC), 16: (0x8704FC, 0x8)}
BUILD_VT_FINAL = 0x8639B0


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
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == ANIMBODY:
        _k += ['anim_null', 'anim_set', 'anim_raw', 'prev_zero', 'prev_other']
    elif _kind == BBOX:
        _k += ['prev_zero', 'prev_other']
    elif _kind == BUILDCTOR:
        _k += ['prev_zero', 'prev_other', 'type_bits_were_set', 'type_bits_were_clear', 'bit0_was_set', 'bit0_was_clear', 'flags_bit0_was_set', 'flags_bit0_was_clear']
    elif _kind == CLONE:
        _k += ['frame_in_b', 'frame_in_c', 'frame_self', 'frame_in_a', 'root_raw', 'root_in_b', 'root_in_c', 'root_zero', 'clone_in_c', 'clone_raw', 'clone_zero']
    elif _kind == SHADOW:
        _k += ['mode_zero', 'mode_negative', 'mode_positive', 'tris_in_b', 'tris_in_c', 'count_one', 'count_many', 'ret_one', 'ret_65536', 'ptr_unaligned']
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
            events.append([CTASKDELETE, W.tagv(this), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = W.tagv(this)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ANIMBODY:
        vt, off = ANIM_INFO[fn]
        anim = 0 if a[6] == 0 else (W.raw(1, a[7]) if a[6] == 1 else a[8])
        A.wr(o, 4, a[3])
        A.wr(o + off, 4, anim)
        W.snap()
        A.wr(o, 4, vt)
        anim = A.rd(o + off, 4)                            # el EXE la lee de la memoria del objeto
        if anim:
            events.append([SETFINISH, W.tagv(anim), DEFAULT_CB, 0])
            marks.append('anim_set' if a[6] == 1 else 'anim_raw')
        else:
            marks.append('anim_null')
        events.append([CTASKDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BBOX:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([BBOXCTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        ret = W.tagv(this)                                # devuelve this, no el EAX del constructor
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BUILDCTOR:
        A.wr(o, 4, a[3]); A.wr(o + 0x1C, 4, a[6]); A.wr(o + 0x34, 4, a[7])
        W.snap()
        events.append([ENTITYCTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        b = A.rd(o + 0x36, 1)
        marks.append('type_bits_were_set' if b & 6 else 'type_bits_were_clear')
        marks.append('bit0_was_set' if b & 1 else 'bit0_was_clear')
        A.wr(o + 0x36, 1, (b & 0xF9) | 1)
        d = A.rd(o + 0x1C, 4)
        marks.append('flags_bit0_was_set' if d & 1 else 'flags_bit0_was_clear')
        A.wr(o + 0x1C, 4, d | 1)
        A.wr(o, 4, BUILD_VT_FINAL)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLONE:
        frame = {0: lambda: W.raw(1, a[4]), 1: lambda: W.raw(2, a[4]), 2: lambda: this, 3: lambda: W.raw(0, a[4])}[a[3]]()
        root = a[6] if a[5] == 0 else (W.raw(1, a[6]) if a[5] == 1 else (W.raw(2, a[6]) if a[5] == 2 else 0))
        clone = W.raw(2, a[8]) if a[7] == 0 else (a[8] if a[7] == 1 else 0)
        W.wr(frame + 0xA0, 4, root)                        # primero la raiz del marco (+A0h), despues el puntero al marco en el atomic (+4): el orden importa si se solapan
        W.wr(this + 4, 4, frame)
        W.snap()
        frame = W.rd(this + 4, 4)
        root_read = W.rd(frame + 0xA0, 4)
        events.append([RPCLONE, W.tagv(this)])
        events.append([RPSETFRAME, W.tagv(clone), W.tagv(root_read)])
        events.append([RPADDATOM, W.tagv(a[9]), W.tagv(clone)])
        ret = W.tagv(this)
        marks += [['frame_in_b', 'frame_in_c', 'frame_self', 'frame_in_a'][a[3]], ['root_raw', 'root_in_b', 'root_in_c', 'root_zero'][a[5]], ['clone_in_c', 'clone_raw', 'clone_zero'][a[7]]]
    elif k == SHADOW:
        mode = a[3]
        ptr = a[5]
        if mode >= 2:
            ptr = W.raw(1 if mode == 2 else 2, a[6])
            data = bytes.fromhex(c['s'])
            for i, byte in enumerate(data):
                W.wr(ptr + i, 1, byte)
        A.wr(o + 0x20, 4, a[4])
        A.wr(o + 0x2C, 4, ptr)
        W.snap()
        count = a[4] - (1 << 32) if a[4] & 0x80000000 else a[4]   # con signo
        if count == 0:
            ret = 0
            marks.append('mode_zero')
        elif count < 0:
            ret = 1
            marks.append('mode_negative')
        else:
            best = 0
            p = A.rd(o + 0x2C, 4)
            for t in range(count):
                for kk in range(3):
                    best = max(best, W.rd((p + 8 * t + 2 * kk) & M32, 2))
            ret = best + 1
            marks += ['mode_positive', 'tris_in_b' if mode == 2 else 'tris_in_c']
            if count == 1:
                marks.append('count_one')
            if count >= 32:
                marks.append('count_many')
            if ret == 1:
                marks.append('ret_one')
            if ret == 0x10000:
                marks.append('ret_65536')
            if a[6] % 2:
                marks.append('ptr_unaligned')
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
