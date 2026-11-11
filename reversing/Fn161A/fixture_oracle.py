"""Oraculo independiente (Python) de Fn161A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 12 funciones del lote 161A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x405900  CIplDefPool::Constructor = CPool<IplDef>(capacidad, nombre) (thiscall, `ret 8`): eax = capacidad * 34h (imul de 32 bits); [esi] = 821195(eax) (operator new[]); [esi+4] = 821195(capacidad); [esi+10h] = 1 (byte);
                [esi+8] = capacidad; [esi+0Ch] = -1; si capacidad > 0 (con signo): para i = 0..capacidad-1: ecx = [esi+4]; [ecx+i] = [ecx+i] | 80h; edx = [esi+4]; [edx+i] = [edx+i] & 80h (el puntero de estados se
                relee en cada paso); devuelve this
   1  0x405960  CIplDefPool::DestructorBody (thiscall, `ret`): si [this+8] (con signo) > 0: si byte [this+10h] != 0: 8213AE([this]) y 8213AE([this+4]) (operator delete[]); despues [this], [this+4], [this+8], [this+0Ch] = 0
   2  0x406360  GetGTA3ImgSize (cdecl, `ret`): eax = [8E4010h]; push 0; push eax; call [858074h] (GetFileSize, stdcall); ret
   3  0x59EA00  CDummyObject::Constructor (thiscall, `ret`): call 532540 (cuerpo del constructor de CDummy, ecx = this); [esi] = 866E78h; eax = this
   4  0x59EA20  CDummyObject(CObject*) (thiscall, `ret 4`, marco SEH): call 532540; [esi] = 866E78h; call 533700 (ecx = this, pila = movsx word [obj+22h]); si [obj+18h] != 0: call 533ED0 (ecx = this, pila = [obj+18h], 1);
                call 533FB0 (ecx = obj); cl = [obj+2Eh]; [this+2Eh] = cl; dl = [obj+2Fh]; [this+2Fh] = dl; call 404C90 (cdecl: movzx cl, this); eax = this
   5  0x59EAB0  CDummyObject::DestructorBody (thiscall, `ret`): [ecx] = 866E78h; jmp 532560
   6  0x54EB70  CMatrixLinkList::Destructor (thiscall, `ret`, marco SEH): call 59ACD0 con ecx = this+1A4h, +150h, +0FCh, +0A8h, +54h y this, en ese orden
   7  0x54F1E0  CPlaceable::Constructor (thiscall, `ret`): [eax] = 863C40h; [eax+4], [eax+8], [eax+0Ch], [eax+10h], [eax+14h] = 0; eax = this
   8  0x4C44D0  CAtomicModelInfo::CreateInstance(RwMatrix*) (thiscall, `ret 4`): si [this+1Ch] == 0 devuelve 0; call 4C4BA0 (AddRef, ecx = this); atomic = [this+1Ch]; clon = 749E60(atomic) (cdecl);
                frame = 7F0410() (cdecl); rep movsd de 16 dwords de la matriz a frame+10h (hacia delante); 74BF20(clon, frame) (cdecl); call 4C4BB0 (RemoveRef, ecx = this); devuelve el clon
   9  0x4C4530  CAtomicModelInfo::CreateInstance() (thiscall, `ret`): igual que 8 sin la copia de la matriz
  10  0x4C5590  CAtomicModelInfo::DestructorBody (thiscall, `ret`): [ecx] = 85BB9Ch
  11  0x4C55A0  CDamageAtomicModelInfo::ConstructorBody (thiscall, `ret`): call 4C4A60 (cuerpo del constructor de CBaseModelInfo, ecx = this); [esi] = 85BC30h; eax = this

Los falsos de los colaboradores anotan: cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator new[] [direccion, tamano]; operator delete[] [direccion, puntero (etiqueta)];
GetFileSize [casilla 858074h, manejador, puntero (crudo)]; AddRef / RemoveRef [direccion, ECX]; RpAtomicClone [direccion, atomic]; RwFrameCreate [direccion]; RpAtomicSetFrame [direccion, atomic, frame];
SetModelIndexNoCreate [direccion, ECX, indice]; AttachToRwObject [direccion, ECX, objeto, bandera]; DetachFromRwObject [direccion, ECX]; IncludeEntity [direccion, indice, entidad].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CIplDefPool::Constructor', 'CIplDefPool::DestructorBody', 'GetGTA3ImgSize', 'CDummyObject::Constructor', 'CDummyObject::ConstructorFromObject', 'CDummyObject::DestructorBody',
         'CMatrixLinkList::Destructor', 'CPlaceable::Constructor', 'CAtomicModelInfo::CreateInstance-rwmat', 'CAtomicModelInfo::CreateInstance-void', 'CAtomicModelInfo::DestructorBody',
         'CDamageAtomicModelInfo::ConstructorBody']
ENTRIES = (0x405900, 0x405960, 0x406360, 0x59EA00, 0x59EA20, 0x59EAB0, 0x54EB70, 0x54F1E0, 0x4C44D0, 0x4C4530, 0x4C5590, 0x4C55A0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
NEWARR, FREEARR, GFS = 0x821195, 0x8213AE, 0x858074
ADDREF, REMREF, CLONE, FRAMECREATE, SETFRAME = 0x4C4BA0, 0x4C4BB0, 0x749E60, 0x7F0410, 0x74BF20
SETMI, ATTACH, DETACH, INCLUDE = 0x533700, 0x533ED0, 0x533FB0, 0x404C90

IPLCTOR, IPLFLUSH, IMGSIZE, CTORBODY, DUMMYOBJ, EVBODY, MLL, PLCTOR, CREATE, CREATEM, VTONLY = 'iplctor iplflush imgsize ctorbody dummyobj evbody mll plctor create createm vtonly'.split()
KIND = [IPLCTOR, IPLFLUSH, IMGSIZE, CTORBODY, DUMMYOBJ, EVBODY, MLL, PLCTOR, CREATEM, CREATE, VTONLY, CTORBODY]
VT = {3: 0x866E78, 5: 0x866E78, 7: 0x863C40, 10: 0x85BB9C, 11: 0x85BC30}
BODY = {3: 0x532540, 5: 0x532560, 11: 0x4C4A60}
SPAN = {IPLCTOR: 0x14, IPLFLUSH: 0x14, IMGSIZE: 4, CTORBODY: 4, DUMMYOBJ: 0x38, EVBODY: 4, MLL: 0x1FC, PLCTOR: 0x18, CREATE: 0x20, CREATEM: 0x20, VTONLY: 4}


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
        addr &= M32
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        addr &= M32
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
for _fn in range(12):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CTORBODY, EVBODY, VTONLY, PLCTOR, MLL):
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other']
    elif _kind == IPLCTOR:
        _k += ['this_aligned', 'this_unaligned', 'size_zero', 'size_one', 'size_small', 'size_large', 'size_neg', 'ptr_mode0', 'ptr_mode1', 'ptr_mode2', 'ptr_mode3', 'ptr_mode4', 'ptr_mode5', 'ptr_mode6',
               'alias_states_header']
    elif _kind == IPLFLUSH:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'size_pos', 'size_zero', 'size_neg', 'owner_set', 'owner_clear', 'free_called', 'ptr_in_arena']
    elif _kind == IMGSIZE:
        _k = _k[:8] + ['ret_zero', 'ret_ffffffff', 'ret_other']
    elif _kind == DUMMYOBJ:
        _k += ['this_aligned', 'this_unaligned', 'obj_separate', 'obj_same', 'obj_overlap', 'rw_null', 'rw_set', 'rw_in_arena', 'mi_negative', 'mi_positive', 'ipl_zero', 'ipl_nonzero', 'obj_unaligned']
    elif _kind in (CREATE, CREATEM):
        _k += ['this_aligned', 'this_unaligned', 'atomic_null', 'atomic_set', 'atomic_in_arena', 'clone_in_arena', 'clone_raw']
        if _kind == CREATEM:
            _k += ['mat_separate', 'mat_same', 'mat_overlap_after', 'mat_overlap_before', 'mat_in_this_arena', 'mat_unaligned_overlap']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    k = KIND[fn]
    if k == IMGSIZE:
        return marks
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPAN.get(k, 4) > WIN:
        marks.append('spill')
    return marks


def sgn(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


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
    if k in (CTORBODY, EVBODY, VTONLY, PLCTOR, MLL):
        A.wr(o, 4, a[3])
        W.snap()
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        if k == CTORBODY:
            events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
            A.wr(o, 4, a[5])
            A.wr(o, 4, VT[fn])
            ret = tag(0, o)
        elif k == EVBODY:
            A.wr(o, 4, VT[fn])
            events.append([BODY[fn], tag(0, o), A.rd(o, 4)])
            A.wr(o, 4, a[5])
        elif k == VTONLY:
            A.wr(o, 4, VT[fn])
        elif k == PLCTOR:
            A.wr(o, 4, VT[fn])
            for j in range(1, 6):
                A.wr(o + 4 * j, 4, 0)
            ret = tag(0, o)
        else:
            for off in (0x1A4, 0x150, 0xFC, 0xA8, 0x54, 0):
                events.append([0x59ACD0, tag(0, o + off), A.rd(o + off, 4)])
                A.wr(o + off, 4, a[5])
    elif k == IPLCTOR:
        size = a[3]
        ssize = sgn(size)
        mode = a[5]
        W.snap()
        p1 = 0 if mode == 1 else (a[10] if mode == 4 else W.raw(1, a[7]))
        if mode == 2:
            p2 = (this + a[9]) & M32
        elif mode == 3:
            p2 = W.raw(0, a[11])
        elif mode == 5:
            p2 = a[10]
        elif mode == 6:
            p2 = 0
        else:
            p2 = W.raw(2, a[8])
        marks.append('ptr_mode%d' % mode)
        marks.append('size_zero' if ssize == 0 else 'size_one' if ssize == 1 else 'size_neg' if ssize < 0 else 'size_small' if ssize < 200 else 'size_large')
        events.append([NEWARR, (size * 0x34) & M32])
        W.wr(this, 4, p1)
        events.append([NEWARR, size])
        W.wr(this + 4, 4, p2)
        W.wr(this + 0x10, 1, 1)
        W.wr(this + 8, 4, size)
        W.wr(this + 0xC, 4, M32)
        for i in range(max(ssize, 0)):
            ptr = W.rd(this + 4, 4)
            W.wr(ptr + i, 1, W.rd(ptr + i, 1) | 0x80)
            ptr2 = W.rd(this + 4, 4)
            W.wr(ptr2 + i, 1, W.rd(ptr2 + i, 1) & 0x80)
        if mode == 2:
            marks.append('alias_states_header')
        ret = tag(0, o)
    elif k == IPLFLUSH:
        stor = W.raw(1, a[9]) if a[8] & 1 else a[3]
        sts = W.raw(2, a[10]) if a[8] & 2 else a[4]
        A.wr(o, 4, stor)
        A.wr(o + 4, 4, sts)
        A.wr(o + 8, 4, a[5])
        A.wr(o + 0xC, 4, a[6])
        A.wr(o + 0x10, 1, a[7] & 0xFF)
        W.snap()
        ssize = sgn(a[5])
        marks.append('size_zero' if ssize == 0 else 'size_neg' if ssize < 0 else 'size_pos')
        marks.append('owner_set' if a[7] & 0xFF else 'owner_clear')
        if a[8]:
            marks.append('ptr_in_arena')
        if ssize > 0:
            if a[7] & 0xFF:
                events.append([FREEARR, W.tagv(stor)])
                events.append([FREEARR, W.tagv(sts)])
                marks.append('free_called')
            A.wr(o, 4, 0)
            A.wr(o + 4, 4, 0)
            A.wr(o + 8, 4, 0)
            A.wr(o + 0xC, 4, 0)
    elif k == IMGSIZE:
        W.snap()
        handle = c.get('handle', 0) & M32
        events.append([GFS, handle, 0])
        ret = a[3]
        marks.append('ret_zero' if a[3] == 0 else 'ret_ffffffff' if a[3] == M32 else 'ret_other')
    elif k == DUMMYOBJ:
        mode = a[16]
        obj = W.raw(1, a[6]) if mode == 0 else (this if mode == 1 else (this + a[17]) & M32)
        rw = W.raw(2, a[19]) if a[18] else a[8]
        A.wr(o, 4, a[3])
        W.wr(obj + 0x22, 2, a[7] & 0xFFFF)
        W.wr(obj + 0x18, 4, rw)
        W.wr(obj + 0x2E, 1, a[9] & 0xFF)
        W.wr(obj + 0x2F, 1, a[10] & 0xFF)
        W.snap()
        marks.append('obj_separate' if mode == 0 else 'obj_same' if mode == 1 else 'obj_overlap')
        if obj % 4:
            marks.append('obj_unaligned')
        events.append([0x532540, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x866E78)
        mi = W.rd(obj + 0x22, 2)
        marks.append('mi_negative' if mi & 0x8000 else 'mi_positive')
        events.append([SETMI, tag(0, o), (mi | 0xFFFF0000) if mi & 0x8000 else mi])
        rwv = W.rd(obj + 0x18, 4)
        if rwv:
            events.append([ATTACH, tag(0, o), W.tagv(rwv), 1])
            marks.append('rw_set')
            if a[18]:
                marks.append('rw_in_arena')
        else:
            marks.append('rw_null')
        events.append([DETACH, W.tagv(obj)])
        cl = W.rd(obj + 0x2E, 1)
        W.wr(this + 0x2E, 1, cl)
        dl = W.rd(obj + 0x2F, 1)
        W.wr(this + 0x2F, 1, dl)
        marks.append('ipl_zero' if cl == 0 else 'ipl_nonzero')
        events.append([INCLUDE, cl, tag(0, o)])
        ret = tag(0, o)
    elif k in (CREATE, CREATEM):
        atomic = 0 if a[3] == 0 else (W.raw(1, a[4]) if a[3] == 1 else a[5])
        A.wr(o + 0x1C, 4, atomic)
        clone = W.raw(1, a[7]) if a[6] == 0 else a[10]
        frame = W.raw(2, a[8])
        matrix = None
        if k == CREATEM:
            m = a[16]
            matrix = {1: frame + 0x10, 2: frame + 0x10 + 4 * a[18], 3: frame + 0x10 - 4 * a[18], 4: W.raw(0, a[17]), 5: frame + 0x10 + a[18]}.get(m, W.raw(1, a[17])) & M32
            marks.append({0: 'mat_separate', 1: 'mat_same', 2: 'mat_overlap_after', 3: 'mat_overlap_before', 4: 'mat_in_this_arena', 5: 'mat_unaligned_overlap'}[m])
        W.snap()
        if atomic == 0:
            marks.append('atomic_null')
            ret = 0
        else:
            marks.append('atomic_set')
            if a[3] == 1:
                marks.append('atomic_in_arena')
            marks.append('clone_in_arena' if a[6] == 0 else 'clone_raw')
            events.append([ADDREF, tag(0, o)])
            events.append([CLONE, W.tagv(atomic)])
            events.append([FRAMECREATE])
            if k == CREATEM:
                for i in range(16):
                    W.wr(frame + 0x10 + 4 * i, 4, W.rd(matrix + 4 * i, 4))
            events.append([SETFRAME, W.tagv(clone), W.tagv(frame)])
            events.append([REMREF, tag(0, o)])
            ret = W.tagv(clone)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
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
