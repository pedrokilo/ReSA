"""Oraculo independiente (Python) de Fn218A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 218A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp). Sin x87.

Desensamblado del EXE (sobre el gta-sa.exe canonico, exe_full_116.dis).
   0..9   cuerpos de destructor de FxInfo*_c (0x4A76C0 ...): marco SEH; esi = ecx; ecx = esi+8; destructor del interp (REAL; ecx = interp): [interp] = 0x85A968; [esi] = 0x85A7DC
  10..18  destructores con borrado (0x4A7710 ...): push esi; esi = ecx; call cuerpo (falso); si byte [esp+8] & 1: operator delete 0x8214BD(esi) (falso); eax = esi; ret 4
  19..22  constructores de FxInterpInfo*_c (0x4A8440 ...): eax = ecx; ecx = 0; byte [eax+4] = 0; dword [eax+8] = 0; dword [eax] = vtable del interp; dword [eax+0xC] = 0 (no toca +5, +6, +7)
  23..26  cuerpos de destructor de los interp (0x4A8460 ...): dword [ecx] = 0x85A968
  27..30  destructores con borrado de los interp (0x4A8D30 ...): como 10..18 (cuerpo 0x4A8460 ... falso)
  31      ListItem_c::Constructor (0x4A8DB0): eax = ecx; dword [eax] = 0; dword [eax+4] = 0

Los falsos anotan: [cuerpo, this como etiqueta, seis dwords del objeto]; [0x8214BD, this como etiqueta]. Las vtables que deja cada lado (la del EXE o una del ASI) se normalizan a la esperada del EXE tras la llamada (`vtable_ok`).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
# cuerpos de destructor de FxInfo: (clase, entrada del cuerpo, destructor del interp REAL que llama)
FXBODIES = (
    ('FxInfoSpriteRect_c', 0x4A76C0, 0x4A8B70),
    ('FxInfoHeatHaze_c', 0x4A7730, 0x4A8B70),
    ('FxInfoTrail_c', 0x4A77A0, 0x4A87F0),
    ('FxInfoFlat_c', 0x4A7810, 0x4A8460),
    ('FxInfoDir_c', 0x4A7880, 0x4A8460),
    ('FxInfoAnimTexture_c', 0x4A78F0, 0x4A89B0),
    ('FxInfoColourRange_c', 0x4A7960, 0x4A87F0),
    ('FxInfoSelfLit_c', 0x4A79D0, 0x4A87F0),
    ('FxInfoColourBright_c', 0x4A7A40, 0x4A87F0),
    ('FxInfoSmoke_c', 0x4A7AB0, 0x4A87F0),
)
# destructores con borrado de FxInfo (los nueve sin SpriteRect, cuyo destructor con borrado 0x4A76A0 es del lote 217A): (clase, entrada); su cuerpo es FXBODIES[k + 1]
FXSDD = (
    ('FxInfoHeatHaze_c', 0x4A7710), ('FxInfoTrail_c', 0x4A7780), ('FxInfoFlat_c', 0x4A77F0), ('FxInfoDir_c', 0x4A7860), ('FxInfoAnimTexture_c', 0x4A78D0),
    ('FxInfoColourRange_c', 0x4A7940), ('FxInfoSelfLit_c', 0x4A79B0), ('FxInfoColourBright_c', 0x4A7A20), ('FxInfoSmoke_c', 0x4A7A90),
)
# interp: (clase, constructor, vtable que deja, cuerpo de destructor, destructor con borrado)
INTERPS = (
    ('FxInterpInfoFloat_c', 0x4A8440, 0x85A970, 0x4A8460, 0x4A8D30),
    ('FxInterpInfoU255_c', 0x4A87D0, 0x85A978, 0x4A87F0, 0x4A8D50),
    ('FxInterpInfo32_c', 0x4A8990, 0x85A980, 0x4A89B0, 0x4A8D70),
    ('FxInterpInfo255_c', 0x4A8B50, 0x85A988, 0x4A8B70, 0x4A8D90),
)
LI_ENTRY = 0x4A8DB0
BASE_VT, INTERP_BASE_VT, VTMARK = 0x85A7DC, 0x85A968, 0xA51A51A5
NAMES = (['%s::Destructor' % b[0] for b in FXBODIES] + ['%s::ScalarDeletingDestructor' % d[0] for d in FXSDD] + ['%s::Constructor' % i[0] for i in INTERPS]
         + ['%s::Destructor' % i[0] for i in INTERPS] + ['%s::ScalarDeletingDestructor' % i[0] for i in INTERPS] + ['ListItem_c::Constructor'])
ENTRIES = tuple([b[1] for b in FXBODIES] + [d[1] for d in FXSDD] + [i[1] for i in INTERPS] + [i[3] for i in INTERPS] + [i[4] for i in INTERPS] + [LI_ENTRY])
assert len(NAMES) == len(ENTRIES) == 32
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
BODY, SDD, ICTOR, IBODY, LI = range(5)
KIND = [BODY] * 10 + [SDD] * 9 + [ICTOR] * 4 + [IBODY] * 4 + [SDD] * 4 + [LI]

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


BASEKEYS = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = [BASEKEYS + (['self_freed', 'self_kept'] if KIND[i] == SDD else []) for i in range(len(NAMES))]


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
    if k == SDD:
        body = FXBODIES[fn - 10 + 1][1] if fn < 19 else INTERPS[fn - 27][3]
        events.append([body, W.tagv(this)] + [A.rd(o + 4 * i, 4) for i in range(6)])
        if a[13] & 1:
            events.append([0x8214BD, W.tagv(this)])
            marks.append('self_freed')
        else:
            marks.append('self_kept')
        ret = W.tagv(this)
    elif k == BODY:
        A.wr(o + 8, 4, INTERP_BASE_VT)
        A.wr(o, 4, BASE_VT)
    elif k == IBODY:
        A.wr(o, 4, INTERP_BASE_VT)
    elif k == ICTOR:
        A.wr(o + 4, 1, 0)
        A.wr(o + 8, 4, 0)
        A.wr(o, 4, INTERPS[fn - 19][2])
        A.wr(o + 0xC, 4, 0)
        ret = W.tagv(this)
    else:   # LI
        A.wr(o, 4, 0)
        A.wr(o + 4, 4, 0)
        ret = W.tagv(this)
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
    if KIND[c['fn']] in (BODY, IBODY, ICTOR) and o.get('vtable_ok') is not True:
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
