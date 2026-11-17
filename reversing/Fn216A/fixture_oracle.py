"""Oraculo independiente (Python) de Fn216A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 33 funciones del lote 216A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp). Sin x87.

Desensamblado del EXE (sobre el gta-sa.exe canonico, exe_full_116.dis).
   0..18  constructores de FxInfo*_c (0x4A5980 ...): marco SEH; esi = ecx; [esi] = vtable derivada; ecx = esi+8; constructor del interp (REAL; ecx = interp): byte [interp+4] = 0, dword [interp+8] = 0, [interp] = vtable
        del interp, dword [interp+0xC] = 0 (o sea +0xC, +0x10, +8 y +0x14 del objeto); word [esi+4] = tipo; byte [esi+0Eh] = ranuras (el byte +0xF no se toca); con ranuras: ecx = 0xA9AF2C; push alineacion 4,
        push tamano; call 0x4A9CA0 (GetMem; falso); [esi+14h] = eax; eax = esi
  19..25  destructores con borrado (0x4A6A70 ...): push esi; esi = ecx; call cuerpo (falso); si byte [esp+8] & 1: operator delete 0x8214BD(esi) (falso); eax = esi; ret 4
  26..32  cuerpos de destructor (0x4A6A90 ...): marco SEH; ecx = esi+8; destructor del interp (REAL): [interp] = 0x85A968; [esi] = 0x85A7DC

Los falsos anotan: [0x4A9CA0, pool, tamano, alineacion, seis dwords del objeto (+0..+0x14) en el momento de la llamada, con las vtables (+0 y +8) como marca (la esperada del EXE o una del ASI)];
[cuerpo, this como etiqueta, seis dwords del objeto]; [0x8214BD, this como etiqueta]. Las vtables que deja cada lado (la del EXE o una del ASI) se normalizan a la esperada del EXE tras la llamada (`vtable_ok`).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
# (clase, entrada, vtable derivada, vtable del interp, constructor del interp, tipo (word +4), ranuras (byte +0xE), bytes pedidos a GetMem (0 = ninguno)), leidos del EXE
CTORS = (
    ('FxInfoGroundCollide_c', 0x4A5980, 0x85A884, 0x85A980, 0x4A8990, 0x2020, 3, 12),
    ('FxInfoWind_c', 0x4A5A10, 0x85A890, 0x85A980, 0x4A8990, 0x2040, 1, 4),
    ('FxInfoJitter_c', 0x4A5B20, 0x85A89C, 0x85A980, 0x4A8990, 0x2080, 1, 4),
    ('FxInfoRotSpeed_c', 0x4A5CB0, 0x85A8A8, 0x85A970, 0x4A8440, 0x2100, 4, 16),
    ('FxInfoFloat_c', 0x4A5DA0, 0x85A8B4, 0x85A970, 0x4A8440, 0x2200, 0, 0),
    ('FxInfoUnderwater_c', 0x4A5E20, 0x85A8C0, 0x85A978, 0x4A87D0, 0x2400, 0, 0),
    ('FxInfoColour_c', 0x4A5EA0, 0x85A8CC, 0x85A978, 0x4A87D0, 0x4001, 4, 16),
    ('FxInfoSize_c', 0x4A5FA0, 0x85A8D8, 0x85A980, 0x4A8990, 0x4002, 4, 16),
    ('FxInfoSpriteRect_c', 0x4A60B0, 0x85A8E4, 0x85A988, 0x4A8B50, 0x4004, 4, 16),
    ('FxInfoHeatHaze_c', 0x4A61A0, 0x85A8F0, 0x85A988, 0x4A8B50, 0x4008, 0, 0),
    ('FxInfoTrail_c', 0x4A6220, 0x85A8FC, 0x85A978, 0x4A87D0, 0x4010, 2, 8),
    ('FxInfoFlat_c', 0x4A6320, 0x85A908, 0x85A970, 0x4A8440, 0x4020, 9, 0x24),
    ('FxInfoDir_c', 0x4A6440, 0x85A914, 0x85A970, 0x4A8440, 0x4040, 3, 12),
    ('FxInfoAnimTexture_c', 0x4A6550, 0x85A920, 0x85A980, 0x4A8990, 0x4080, 1, 4),
    ('FxInfoColourRange_c', 0x4A6630, 0x85A92C, 0x85A978, 0x4A87D0, 0x4100, 7, 0x1C),
    ('FxInfoSelfLit_c', 0x4A6760, 0x85A938, 0x85A978, 0x4A87D0, 0x4200, 0, 0),
    ('FxInfoColourBright_c', 0x4A67D0, 0x85A944, 0x85A978, 0x4A87D0, 0x4400, 5, 0x14),
    ('FxInfoSmoke_c', 0x4A68E0, 0x85A950, 0x85A978, 0x4A87D0, 0x8001, 8, 0x20),
    ('FxInfoEmRate_c', 0x4A6A00, 0x85A95C, 0x85A970, 0x4A8440, 0x1001, 1, 4),
)
# destructores: (clase, destructor con borrado, cuerpo, destructor del interp)
DTORS = (
    ('FxInfoEmRate_c', 0x4A6A70, 0x4A6A90, 0x4A8460),
    ('FxInfoEmSize_c', 0x4A6AE0, 0x4A6B00, 0x4A89B0),
    ('FxInfoEmSpeed_c', 0x4A6B50, 0x4A6B70, 0x4A89B0),
    ('FxInfoEmDir_c', 0x4A6BC0, 0x4A6BE0, 0x4A89B0),
    ('FxInfoEmAngle_c', 0x4A6C30, 0x4A6C50, 0x4A8460),
    ('FxInfoEmLife_c', 0x4A6CA0, 0x4A6CC0, 0x4A87F0),
    ('FxInfoEmPos_c', 0x4A6D10, 0x4A6D30, 0x4A8460),
)
BASE_VT, INTERP_BASE_VT, POOL_THIS, VTMARK = 0x85A7DC, 0x85A968, 0xA9AF2C, 0xA51A51A5
NAMES = (['%s::Constructor' % c[0] for c in CTORS] + ['%s::ScalarDeletingDestructor' % d[0] for d in DTORS] + ['%s::Destructor' % d[0] for d in DTORS])
ENTRIES = tuple([c[1] for c in CTORS] + [d[1] for d in DTORS] + [d[2] for d in DTORS])
NCTOR, NSDD = len(CTORS), len(DTORS)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CTOR, SDD, BODY = range(3)
KIND = [CTOR] * NCTOR + [SDD] * NSDD + [BODY] * NSDD

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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


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

    def __init__(self, c, stl):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        # las funciones de la STL rellenan la arena B siempre con datos aleatorios (los demas rellenos harian invisibles las copias)
        self.mem = tuple(Mem(0 if (stl and i == 1) else a[2], a[92 + i]) for i in range(4))
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

    def getg(self, addr, n):
        return sum(self.glob[addr + k] << (8 * k) for k in range(n))

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


KEYS = {}
for _i in range(NCTOR):
    KEYS[_i] = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'ret_arena' if CTORS[_i][7] else 'nomem', 'ret_raw' if CTORS[_i][7] else 'nomem']
for _i in range(NCTOR, NCTOR + NSDD):
    KEYS[_i] = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'self_freed', 'self_kept']
for _i in range(NCTOR + NSDD, NCTOR + 2 * NSDD):
    KEYS[_i] = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = [KEYS[i] for i in range(len(NAMES))]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c, False)
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    for i in range(6):
        if a[40 + i]:
            A.wr(o + 4 * i, 4, a[4 + i])
    W.snap()
    if k == CTOR:
        _, _, vt, ivt, _, typ, count, size = CTORS[fn]
        A.wr(o, 4, vt)
        A.wr(o + 0xC, 1, 0); A.wr(o + 0x10, 4, 0); A.wr(o + 8, 4, ivt); A.wr(o + 0x14, 4, 0)
        A.wr(o + 4, 2, typ)
        A.wr(o + 0xE, 1, count)
        if size:
            snap = [A.rd(o + 4 * i, 4) for i in range(6)]
            snap[0] = VTMARK
            snap[2] = VTMARK
            events.append([0x4A9CA0, POOL_THIS, size, 4] + snap)
            r = W.raw(1, a[11]) if a[10] == 0 else a[12]
            A.wr(o + 0x14, 4, r)
            marks.append('ret_arena' if a[10] == 0 else 'ret_raw')
        else:
            marks.append('nomem')
        ret = W.tagv(this)
    elif k == SDD:
        body = DTORS[fn - NCTOR][2]
        events.append([body, W.tagv(this)] + [A.rd(o + 4 * i, 4) for i in range(6)])
        if a[13] & 1:
            events.append([0x8214BD, W.tagv(this)])
            marks.append('self_freed')
        else:
            marks.append('self_kept')
        ret = W.tagv(this)
    else:
        A.wr(o + 8, 4, INTERP_BASE_VT)
        A.wr(o, 4, BASE_VT)
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
    if KIND[c['fn']] != SDD and o.get('vtable_ok') is not True:
        return 'vtable_ok %s' % o.get('vtable_ok')
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
