"""Oraculo independiente (Python) de Fn188A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 13 funciones del lote 188A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico; vtables leidas de .rdata del propio EXE).
   0  0x5FEF70  mov [ecx],86C9F0h; jmp 4AF400                        (cuerpo del destructor de CEventScriptedAttractor)
   1  0x5FEF80  mov eax,30h; ret
   2  0x5FEF90  CloneEditable: marco SEH; push 20h; call 4B5620; add esp,4; esi = eax; si es nulo: eax = 0; si no: ecx = [edi+14h] (efecto); eax = [edi+18h] (entidad); push 0; push eax; push ecx; ecx = esi; call 4AF350;
            [esi] = 86C9F0h; eax = esi; ret
   3  0x5FEFF0  destructor con borrado: push esi; esi = ecx; call 5FEF70; test byte [esp+8],1; je +; push esi; call 4B5630; add esp,4; eax = esi; pop esi; ret 4
   4  0x5FF030  mov [ecx],86CA38h; jmp 4AF890                        (CEventAcquaintancePedRespect)
   5  0x5FF050  CloneEditable: push 18h; call 4B5620; si no es nulo: eax = [edi+14h]; push eax; ecx = esi; call 4AF820; [esi] = 86CA38h; eax = esi; ret
   6  0x5FF0B0  destructor con borrado: cuerpo 5FF030, delete 4B5630
   7  0x5FF0F0  mov [ecx],86CA80h; jmp 4AF890                        (CEventAcquaintancePedLike)
   8  0x5FF110  CloneEditable: igual que 5 con vtable 86CA80h
   9  0x5FF170  destructor con borrado: cuerpo 5FF0F0, delete 4B5630
  10  0x5FF1B0  mov [ecx],86CAC8h; jmp 4AF890                        (CEventAcquaintancePedDislike)
  11  0x5FF1D0  CloneEditable: igual que 5 con vtable 86CAC8h
  12  0x5FF230  destructor con borrado: cuerpo 5FF1B0, delete 4B5630

Los falsos anotan: cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [direccion, puntero (etiqueta), dword del puntero]; operator new [direccion, tamano]; constructor de
CEventAcquaintancePed (un argumento) [direccion, ECX, ped (etiqueta), dword de [ECX]]; constructor de CEventAttractor [direccion, ECX, efecto (etiqueta), entidad (etiqueta), tercer argumento, dword de [ECX]].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CEventScriptedAttractor::DestructorBody", "CEventScriptedAttractor::GetEventType", "CEventScriptedAttractor::CloneEditable", "CEventScriptedAttractor::ScalarDeletingDestructor",
         "CEventAcquaintancePedRespect::DestructorBody", "CEventAcquaintancePedRespect::CloneEditable", "CEventAcquaintancePedRespect::ScalarDeletingDestructor",
         "CEventAcquaintancePedLike::DestructorBody", "CEventAcquaintancePedLike::CloneEditable", "CEventAcquaintancePedLike::ScalarDeletingDestructor",
         "CEventAcquaintancePedDislike::DestructorBody", "CEventAcquaintancePedDislike::CloneEditable", "CEventAcquaintancePedDislike::ScalarDeletingDestructor"]
ENTRIES = (0x5FEF70, 0x5FEF80, 0x5FEF90, 0x5FEFF0, 0x5FF030, 0x5FF050, 0x5FF0B0, 0x5FF0F0, 0x5FF110, 0x5FF170, 0x5FF1B0, 0x5FF1D0, 0x5FF230)
GUARD = 0x40
WINS = (0x1000, 0x1000, 0x1000, 0x1000)
SIZES = tuple(GUARD + w + GUARD for w in WINS)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

BODYJMP, CONSTTYPE, EVCLONE, SDD = 'bodyjmp consttype evclone sdd'.split()
KIND = [BODYJMP, CONSTTYPE, EVCLONE, SDD, BODYJMP, EVCLONE, SDD, BODYJMP, EVCLONE, SDD, BODYJMP, EVCLONE, SDD]
BODYJMP_VT = {0: (0x86C9F0, 0x4AF400), 4: (0x86CA38, 0x4AF890), 7: (0x86CA80, 0x4AF890), 10: (0x86CAC8, 0x4AF890)}
SDD_BODY = {3: 0x5FEF70, 6: 0x5FF030, 9: 0x5FF0F0, 12: 0x5FF1B0}
SDD_DEL = {3: 0x4B5630, 6: 0x4B5630, 9: 0x4B5630, 12: 0x4B5630}
# clones: (vtable, tamano de new, direccion del constructor, es el de CEventAttractor)
CLONE = {2: (0x86C9F0, 0x20, 0x4AF350, True), 5: (0x86CA38, 0x18, 0x4AF820, False), 8: (0x86CA80, 0x18, 0x4AF820, False), 11: (0x86CAC8, 0x18, 0x4AF820, False)}


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


def s8(v):
    v &= 0xFF
    return v - 0x100 if v >= 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 0x80000000 else v


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
        self.mem = tuple(Mem(a[2], a[60 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + SIZES[i]:
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
            if b <= v < b + SIZES[i]:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == EVCLONE:
        _k += ['mem_null', 'mem_set']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == CONSTTYPE:
        W.snap()
        ret = 0x30
    elif k == BODYJMP:
        W.snap()
        vt, target = BODYJMP_VT[fn]
        A.wr(o, 4, vt)
        events.append([target, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([SDD_DEL[fn], W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == EVCLONE:
        vt, size, ctor, attractor = CLONE[fn]
        A.wr(o + 0x14, 4, a[4]); A.wr(o + 0x18, 4, a[5])
        W.snap()
        events.append([0x4B5620, size])
        if a[6] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[7])
            if attractor:
                events.append([ctor, W.tagv(mem), W.tagv(a[4]), W.tagv(a[5]), 0, B.rd(a[7], 4)])
            else:
                events.append([ctor, W.tagv(mem), W.tagv(a[4]), B.rd(a[7], 4)])
            B.wr(a[7], 4, a[9])
            B.wr(a[7], 4, vt)
            ret = W.tagv(mem)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
