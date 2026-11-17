"""Oraculo independiente (Python) de Fn215A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 215A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y la cadena de prueba (Fixtures.hpp).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`).
   0   0x4D5F20  CAnimBlendClumpData: destructor con borrado: `push esi; mov esi,ecx; call 0x4CF100; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`
   1   0x4D6240  callback cdecl (fd, dato): tag = [fd+0x14]; nombre = 0x4D56F0(tag); si nombre != 0: si _stricmp (0x8229B6)(dato, nombre) == 0 (EAX completo): [0xB5F87C] = fd
   2   0x4D6270  callback cdecl (fd, dato): nombre = 0x72FB30([fd+0x10]) (sin comprobar nulo); si _stricmp(dato, nombre) == 0: [0xB5F87C] = fd
   3   0x4D6310  callback cdecl (fd, dato): nombre = 0x4D56F0([fd+0x14]); si nombre != 0: si 0x53CF30(nombre) == dato (dword): [0xB5F87C] = fd
   4   0x4D6340  callback cdecl (fd, dato): nombre = 0x72FB30([fd+0x10]); si 0x53CF30(nombre) == dato: [0xB5F87C] = fd (sin comprobar nulo)
   5   0x4D6430  callback cdecl (fd, array): [array + 0x732A20([fd+0x10]) * 4] = fd (indice de 32 bits; el producto se trunca a 32 bits)
   6   0x46A900  thiscall (ID, estado) `ret 8`: ID = byte con signo; si ID < 0 nada; para los 70 registros de 0x14: si byte [r+3] == ID: byte [r+4] = byte de estado
   7   0x46A930  thiscall (ImgIndex, Model, priority, attachType, Type, Radius) `ret 0x18`: primer registro (70) con word [r] == 0xFFFF; sin libres no escribe. word [r] = ImgIndex, word [r+0xE] = priority,
             byte [r+3] = Type, word [r+0xC] = Model, byte [r+2] = attachType, byte [r+4] = 1; x87: fld Radius; fcomp 0.0 (0x858B50); fnstsw; test ah,41h; jp -> dword [r+8] = bits de Radius (Radius > 0 o NaN);
             si no [r+8] = dword 0x859D9C (5.0f). No toca +5..+7 ni +0x10.
   8   0x46A9C0  thiscall (word, char*, byte) `ret 0xC`: primer registro libre igual; word [r] = a2; copia byte a byte la cadena con su NUL a [r+0xC ...] SIN limite; byte [r+2] = attachtype, byte [r+3] = 0xFF,
             byte [r+4] = 1, dword [r+8] = dword 0x859D9C (5.0f)
   9-21 0x4A4C20 ... 0x4A57C0  constructores de FxInfoEmSize_c ... FxInfoAttractLine_c (thiscall, sin argumentos, devuelve this): [this] = vtable, interp en this+8: 0x4A8990 / 0x4A8440 / 0x4A87D0 (byte [i+4] = 0,
             dword [i+8] = 0, vtable [i], dword [i+0xC] = 0); word [this+4] = tipo; byte [this+0xE] = contador; 0x4A9CA0 (GetMem de 0xA9AF2C, tamano 4 * contador, alineacion 4); [this+0x14] = lo que devuelve.

Constantes del EXE: 0x858B50 = 0.0f, 0x859D9C = 5.0f (0x40A00000).

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
FX = ['EmSize', 'EmSpeed', 'EmDir', 'EmAngle', 'EmLife', 'EmPos', 'EmWeather', 'EmRotation', 'Noise', 'Force', 'Friction', 'AttractPt', 'AttractLine']
NAMES = ["CAnimBlendClumpData::ScalarDeletingDestructor", "RpAnimBlend::FrameFindByNameCBskin", "RpAnimBlend::FrameFindByNameCBnonskin", "RpAnimBlend::FrameFindByKeyCBskin",
         "RpAnimBlend::FrameFindByKeyCBnonskin", "RpAnimBlend::FillFrameArrayIndiciesNonSkinnedCB", "CScriptsForBrains::SwitchAllObjectBrainsWithThisID", "CScriptsForBrains::AddNewScriptBrain",
         "CScriptsForBrains::AddNewStreamedScriptBrainForCodeUse"] + ['FxInfo%s_c::Constructor' % n for n in FX]
ENTRIES = (0x4D5F20, 0x4D6240, 0x4D6270, 0x4D6310, 0x4D6340, 0x4D6430, 0x46A900, 0x46A930, 0x46A9C0,
           0x4A4C20, 0x4A4D00, 0x4A4DC0, 0x4A4E90, 0x4A4F50, 0x4A5010, 0x4A50E0, 0x4A51B0, 0x4A5270, 0x4A5450, 0x4A5550, 0x4A5650, 0x4A57C0)
FX_TYPE = (0x1004, 0x1008, 0x1010, 0x1020, 0x1040, 0x1080, 0x1100, 0x1200, 0x2001, 0x2002, 0x2004, 0x2008, 0x2010)
FX_COUNT = (7, 2, 3, 2, 2, 3, 4, 2, 1, 3, 1, 4, 7)
FX_VT = (0x85A7E8, 0x85A7F4, 0x85A800, 0x85A80C, 0x85A818, 0x85A824, 0x85A830, 0x85A83C, 0x85A848, 0x85A854, 0x85A860, 0x85A86C, 0x85A878)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(DEL, NAMESKIN, NAMENON, KEYSKIN, KEYNON, FILL, SWITCH, ADDBRAIN, ADDSTR, FXC) = 'del nameskin namenon keyskin keynon fill switch addbrain addstr fx'.split()
KIND = [DEL, NAMESKIN, NAMENON, KEYSKIN, KEYNON, FILL, SWITCH, ADDBRAIN, ADDSTR] + [FXC] * 13
RET_TAG = {DEL, FXC}
GLOBAL = 0xB5F87C
EV_DTOR, EV_DEL, EV_BONE, EV_NODE, EV_STRICMP, EV_KEY, EV_HIER, EV_GETMEM = 0x4CF100, 0x8214BD, 0x4D56F0, 0x72FB30, 0x8229B6, 0x53CF30, 0x732A20, 0x4A9CA0
FIVE = 0x40A00000


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


def strchar(seed, k):
    return 1 + rnd(seed, 0x500 + k) % 255


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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    DEL: ['flag_set', 'flag_clear'],
    NAMESKIN: ['name_null', 'name_set', 'cmp_zero', 'cmp_nonzero', 'global_set'],
    NAMENON: ['name_null', 'name_set', 'cmp_zero', 'cmp_nonzero', 'global_set'],
    KEYSKIN: ['name_null', 'name_set', 'key_match', 'key_differ', 'global_set'],
    KEYNON: ['name_null', 'name_set', 'key_match', 'key_differ', 'global_set'],
    FILL: ['idx_neg', 'idx_pos', 'idx_zero', 'idx_wrap'],
    SWITCH: ['id_neg', 'id_nonneg', 'hits', 'state_gt1'],
    ADDBRAIN: ['none_free', 'free_first', 'free_mid', 'free_last', 'rad_nan', 'rad_snan', 'rad_pos', 'rad_nonpos'],
    ADDSTR: ['none_free', 'free_first', 'free_mid', 'free_last', 'len0', 'len_long', 'src_in_a'],
    FXC: ['getmem_null', 'getmem_arena', 'getmem_raw'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def first_free(words):
    for i in range(70):
        if (words[i] & 0xFFFF) == 0xFFFF:
            return i
    return 70


def expected(c):
    a = padded(c)
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, big, gch = [], None, [], []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tg = W.tagv
    vt = None
    if k == DEL:
        W.snap()
        events.append([EV_DTOR, tg(this)])
        if a[4] & 1:
            events.append([EV_DEL, tg(this)])
            marks.append('flag_set')
        else:
            marks.append('flag_clear')
        ret = tg(this)
    elif k in (NAMESKIN, NAMENON, KEYSKIN, KEYNON, FILL):
        frame = W.ptr(a[5], a[6], a[7], 3)
        name = W.ptr(a[8], a[9], a[10], 2)
        data = W.ptr(a[11], a[12], a[13], 1)
        if k in (KEYSKIN, KEYNON) and a[16]:
            data = a[15]
        A.wr(o + 0x14, 4, a[4])
        A.wr(o + 0x10, 4, frame)
        W.snap()
        glob_init = a[17]
        hit = False
        if k in (NAMESKIN, NAMENON, KEYSKIN, KEYNON):
            marks.append('name_null' if name == 0 else 'name_set')
        if k in (NAMESKIN, KEYSKIN):
            events.append([EV_BONE, a[4]])
        elif k != FILL:
            events.append([EV_NODE, tg(frame)])
        if k == NAMESKIN:
            if name != 0:
                events.append([EV_STRICMP, tg(data), tg(name)])
                hit = a[14] == 0
                marks.append('cmp_zero' if hit else 'cmp_nonzero')
        elif k == NAMENON:
            events.append([EV_STRICMP, tg(data), tg(name)])
            hit = a[14] == 0
            marks.append('cmp_zero' if hit else 'cmp_nonzero')
        elif k == KEYSKIN:
            if name != 0:
                events.append([EV_KEY, tg(name)])
                hit = (a[15] & M32) == data
                marks.append('key_match' if hit else 'key_differ')
        elif k == KEYNON:
            events.append([EV_KEY, tg(name)])
            hit = (a[15] & M32) == data
            marks.append('key_match' if hit else 'key_differ')
        else:
            events.append([EV_HIER, tg(frame)])
            idx = a[14] & M32
            target = (data + ((idx * 4) & M32)) & M32
            arena = next(i for i, b in enumerate(W.bases) if b <= target and target + 4 <= b + N)
            W.mem[arena].wr(target - W.bases[arena] - GUARD, 4, this)
            marks.append('idx_neg' if sgn(idx) < 0 else ('idx_zero' if idx == 0 else 'idx_pos'))
            if sgn(idx) * 4 != sgn((idx * 4) & M32):
                marks.append('idx_wrap')
        if hit:
            marks.append('global_set')
            for i in range(4):
                nb = (this >> (8 * i)) & 0xFF
                if nb != (glob_init >> (8 * i)) & 0xFF:
                    gch.append([GLOBAL + i, nb])
    elif k == SWITCH:
        for i in range(min(a[6], 8)):
            A.wr(o + a[7 + i] * 0x14 + 3, 1, a[4] & 0xFF)
        W.snap()
        ident = a[4] & 0xFF
        marks.append('id_neg' if ident >= 0x80 else 'id_nonneg')
        if (a[5] & 0xFF) > 1:
            marks.append('state_gt1')
        if ident < 0x80:
            matched = False
            for i in range(70):
                if A.rd(o + i * 0x14 + 3, 1) == ident:
                    A.wr(o + i * 0x14 + 4, 1, a[5] & 0xFF)
                    matched = True
            if matched:
                marks.append('hits')
    elif k in (ADDBRAIN, ADDSTR):
        for i in range(70):
            A.wr(o + i * 0x14, 2, a[16 + i] & 0xFFFF)
        string_at = None
        if k == ADDSTR:
            n = a[7]
            if a[5] == 3:
                at = o + a[6]
                for j in range(n):
                    A.wr(at + j, 1, strchar(c['seed'], j))
                A.wr(at + n, 1, 0)
                string_at = (0, at)
                marks.append('src_in_a')
            else:
                for j in range(n):
                    B.wr(a[6] + j, 1, strchar(c['seed'], j))
                B.wr(a[6] + n, 1, 0)
                string_at = (1, a[6])
            marks.append('len0' if n == 0 else ('len_long' if n >= 8 else 'len_short'))
        W.snap()
        f = first_free([a[16 + i] for i in range(70)])
        marks.append('none_free' if f == 70 else ('free_first' if f == 0 else ('free_last' if f == 69 else 'free_mid')))
        if k == ADDBRAIN:
            bits = a[9]
            if is_nan(bits):
                marks.append('rad_nan')
                if not (bits & 0x400000):
                    marks.append('rad_snan')
            else:
                marks.append('rad_pos' if fval(bits) > 0.0 else 'rad_nonpos')
        if f < 70:
            r = o + f * 0x14
            if k == ADDBRAIN:
                A.wr(r, 2, a[4] & 0xFFFF)
                A.wr(r + 0xE, 2, a[6] & 0xFFFF)
                A.wr(r + 3, 1, a[8] & 0xFF)
                A.wr(r + 0xC, 2, a[5] & 0xFFFF)
                A.wr(r + 2, 1, a[7] & 0xFF)
                A.wr(r + 4, 1, 1)
                bits = a[9]
                A.wr(r + 8, 4, bits if (is_nan(bits) or fval(bits) > 0.0) else FIVE)
            else:
                A.wr(r, 2, a[4] & 0xFFFF)
                src_arena, src_off = string_at
                mem = W.mem[src_arena]
                j = 0
                while True:
                    ch = mem.rd(src_off + j, 1)
                    A.wr(r + 0xC + j, 1, ch)
                    j += 1
                    if ch == 0:
                        break
                A.wr(r + 2, 1, a[8] & 0xFF)
                A.wr(r + 3, 1, 0xFF)
                A.wr(r + 4, 1, 1)
                A.wr(r + 8, 4, FIVE)
    else:   # FXC
        fi = fn - 9
        mem_ret = W.ptr(a[4], a[5], a[6], 3)
        marks.append('getmem_null' if a[4] == 0 else ('getmem_raw' if a[4] == 1 else 'getmem_arena'))
        W.snap()
        events.append([EV_GETMEM, 0xA9AF2C, 4 * FX_COUNT[fi], 4])
        A.wr(o + 0xC, 1, 0)
        A.wr(o + 0x10, 4, 0)
        A.wr(o + 4, 2, FX_TYPE[fi])
        A.wr(o + 0xE, 1, FX_COUNT[fi])
        A.wr(o + 0x14, 4, mem_ret)
        vt = [1, 1]
        ret = tg(this)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'big': big, 'gchanges': gch, 'ret': ret, 'threw': False, 'marks': marks, 'vt': vt}


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
    if o.get('vt') != e['vt']:
        return 'vtables %s != %s' % (o.get('vt'), e['vt'])
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
