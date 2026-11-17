"""Oraculo independiente (Python) de Fn217A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 31 funciones del lote 217A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, volcado exe_full_116.dis).
   0-14   cuerpos de destructor de FxInfoEmWeather_c, EmRotation, Noise, Force, Friction, AttractPt, AttractLine, GroundCollide, Wind, Jitter, RotSpeed, Float, Underwater, Colour, Size (67 B):
          `push -1; push <handler SEH>; mov eax,fs:[0]; push eax; mov fs:[0],esp; push ecx; push esi; mov esi,ecx; mov [esp+4],esi; lea ecx,[esi+8]; mov [esp+10h],0; call <destructor del interp>;
          mov ecx,[esp+8]; mov [esi],85A7DCh; pop esi; mov fs:[0],ecx; add esp,10h; ret`. El destructor del interp (0x4A8460, 0x4A89B0 o 0x4A87F0) es `mov [ecx],85A968h; ret`.
          Efecto neto: dword [this+8] = 0x85A968 y dword [this] = 0x85A7DC; nada mas cambia; sin eventos (el destructor del interp corre real).
   15-30  destructores con borrado (27 B) de las mismas clases y FxInfoSpriteRect_c (0x4A76A0): `push esi; mov esi,ecx; call <cuerpo = entrada + 0x20>; test byte [esp+8],1; je ..; push esi; call 0x8214BD;
          add esp,4; mov eax,esi; pop esi; ret 4`: evento [cuerpo, this]; con el bit 0 del BYTE bajo de las banderas, evento [0x8214BD, this]; devuelve this. Sin cambios de memoria (los falsos no escriben).

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = [
    "FxInfoEmWeather_c::DestructorBody",
    "FxInfoEmRotation_c::DestructorBody",
    "FxInfoNoise_c::DestructorBody",
    "FxInfoForce_c::DestructorBody",
    "FxInfoFriction_c::DestructorBody",
    "FxInfoAttractPt_c::DestructorBody",
    "FxInfoAttractLine_c::DestructorBody",
    "FxInfoGroundCollide_c::DestructorBody",
    "FxInfoWind_c::DestructorBody",
    "FxInfoJitter_c::DestructorBody",
    "FxInfoRotSpeed_c::DestructorBody",
    "FxInfoFloat_c::DestructorBody",
    "FxInfoUnderwater_c::DestructorBody",
    "FxInfoColour_c::DestructorBody",
    "FxInfoSize_c::DestructorBody",
    "FxInfoEmWeather_c::ScalarDeletingDestructor",
    "FxInfoEmRotation_c::ScalarDeletingDestructor",
    "FxInfoNoise_c::ScalarDeletingDestructor",
    "FxInfoForce_c::ScalarDeletingDestructor",
    "FxInfoFriction_c::ScalarDeletingDestructor",
    "FxInfoAttractPt_c::ScalarDeletingDestructor",
    "FxInfoAttractLine_c::ScalarDeletingDestructor",
    "FxInfoGroundCollide_c::ScalarDeletingDestructor",
    "FxInfoWind_c::ScalarDeletingDestructor",
    "FxInfoJitter_c::ScalarDeletingDestructor",
    "FxInfoRotSpeed_c::ScalarDeletingDestructor",
    "FxInfoFloat_c::ScalarDeletingDestructor",
    "FxInfoUnderwater_c::ScalarDeletingDestructor",
    "FxInfoColour_c::ScalarDeletingDestructor",
    "FxInfoSize_c::ScalarDeletingDestructor",
    "FxInfoSpriteRect_c::ScalarDeletingDestructor",
]
ENTRIES = (0x4A6DA0, 0x4A6E10, 0x4A6E80, 0x4A6EF0, 0x4A6F60, 0x4A6FD0, 0x4A7040, 0x4A70B0, 0x4A73B0, 0x4A7420, 0x4A7490, 0x4A7500, 0x4A7570, 0x4A75E0, 0x4A7650,
           0x4A6D80, 0x4A6DF0, 0x4A6E60, 0x4A6ED0, 0x4A6F40, 0x4A6FB0, 0x4A7020, 0x4A7090, 0x4A7390, 0x4A7400, 0x4A7470, 0x4A74E0, 0x4A7550, 0x4A75C0, 0x4A7630, 0x4A76A0)
NBODY = 15
VT_INTERP, VT_BASE = 0x85A968, 0x85A7DC
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(BODY, DEL) = 'body del'.split()
KIND = [BODY] * NBODY + [DEL] * 16
EV_DELETE = 0x8214BD


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




def sgn(v):
    v &= M32
    return v - (1 << 32) if v >= (1 << 31) else v


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def fval(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


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
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        sd = c['sd']
        fill = padded(c)[2]
        self.mem = tuple(Mem(fill, sd[i]) for i in range(4))

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

    def ptr(self, mode, off, raw, arena):
        return 0 if mode == 0 else (raw & M32 if mode == 1 else self.raw(arena, sgn(off)))


def padded(c):
    a = [x & M32 for x in c['a']]
    return a + [0] * (128 - len(a))


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned',
          'prev_base_already', 'prev_interp_already', 'prev_other_zero']
_EXTRA = {BODY: [], DEL: ['flag_set', 'flag_clear', 'flag_noise']}
KEYS = {fn: list(COMMON) + _EXTRA[KIND[fn]] for fn in range(len(NAMES))}


def expected(c):
    a = padded(c)
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    A.wr(o + 8, 4, a[4])
    for off, v in ((4, a[5]), (0xC, a[6]), (0x10, a[7]), (0x14, a[8])):
        A.wr(o + off, 4, v)
    if a[3] == VT_BASE:
        marks.append('prev_base_already')
    if a[4] == VT_INTERP:
        marks.append('prev_interp_already')
    if a[5] == 0 and a[6] == 0:
        marks.append('prev_other_zero')
    W.snap()
    tg = W.tagv
    if k == BODY:
        A.wr(o + 8, 4, VT_INTERP)
        A.wr(o, 4, VT_BASE)
    else:
        events.append([ENTRIES[fn] + 0x20, tg(this)])
        if a[9] & 1:
            events.append([EV_DELETE, tg(this)])
            marks.append('flag_set')
        else:
            marks.append('flag_clear')
        if a[9] & 0xFFFFFFFE:
            marks.append('flag_noise')
        ret = tg(this)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'threw': False, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
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
