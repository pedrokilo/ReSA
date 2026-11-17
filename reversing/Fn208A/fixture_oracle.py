"""Oraculo independiente (Python) de Fn208A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 15 funciones del lote 208A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su
imagen inicial y la geometria de los montajes que prepara LiveComparison.hpp (Setup).

Desensamblado del EXE (sobre el gta-sa.exe canonico; `dumpbin /disasm`).
   0  0x453F40  PhoneAnimFinishCallback(asoc, ped) cdecl: byte [asoc+2Eh] |= 4; [asoc+1Ch] = C47A0000h; call 53FB70 (GetPad, push 0); byte [pad+10Eh] &= 0BFh; fld [asoc+18h]; fcomp 0.5f; test ah,41h; jne (no entra con
            blend <= 0.5 ni NaN); entra: [ped+46Ch] |= 4000h; cmp [ped+530h],13h; si es igual: push 1; call 5E4500 (SetPedState, thiscall ecx = ped)
   1  0x4541E0  constructor de CPhoneInfo: llama al iterador del CRT 821D30 sobre this+8 (50 elementos de 34h, constructor 453FA0 = ret) y devuelve this: no escribe memoria
   2  0x454920  [esp+4] == 3, 4 o 5 -> eax = 1; si no 0 (comparaciones de 32 bits)
   3  0x55EB90  LoadSurfaceInfos: OpenFile("data\\surfinfo.dat","r") (538900), LoadLine(file) (536F80) hasta nulo; salta lineas vacias o que empiezan por '#'; sscanf de 36 campos; GetSurfaceIdFromName (55D220, ecx = this) y
            escritura de los campos de la entrada de 0Ch bytes que empieza en this+90h: byte tyreGrip = low8(_ftol2(TyreGrip*10)), byte wetGrip = low8(_ftol2(WetGrip*100)), dword +4 y +8 con los bits; CloseFile (5389D0)
   4  0x55F2B0  LoadSurfaceAudioInfos: igual con "data\\surfaud.dat", `%s %d x9` y los bits 10..18 del segundo dword
   5  0x72D850  CZoneDef::IsPointWithin(x, y, z) thiscall `ret 0Ch`: words con signo: +0 esquinaX, +2 esquinaY, +4 v1X, +6 v1Y, +8 v2X, +0Ah v2Y, +0Ch minZ, +0Eh maxZ; minZ < z y maxZ > z (NaN falla); dx = x - esquinaX y dy = y - esquinaY
            en x87 (24 bits) guardados como float; dot1 = v1X*dx + v1Y*dy (cada producto y la suma redondeados a 24 bits con exponente ampliado), dot1 >= 0 y dot1 <= len1 (entero de 32 bits con envoltura v1X*v1X + v1Y*v1Y);
            igual con el segundo vector
   6  0x72E3A0  RpClumpGtaStreamReadAtOnce(stream): FindChunk(1,&len,&ver); Read(cabecera de 12 bytes); RpClumpCreate; FindChunk(0Eh); _rwFrameListStreamRead; FindChunk(1Ah); lista de geometrias en una fase (0x72E080);
            clump+4 = frames[0]; por cada atomico (contador de la cabecera): FindChunk(14h) y 0x72E270; RpClumpAddAtomic; destruccion de la lista (0x72E360) y de los frames
   7  0x72E870  JoinAdjacentFreeBlocks(bloque) `ret 4`: une con el siguiente fisico libre y con el anterior libre; escribe tamano y siguiente->prev; devuelve el bloque resultante
   8  0x72EC60  Shutdown: push [this]; call 82413F (free); [this] = 0
   9  0x72EC80  FillInBlockData(desc, sig, tam) `ret 0Ch`
  10  0x72ED80  Malloc(tam) `ret 4`
  11  0x72F180  Init(mem, tam, initSizes) `ret 0Ch`
  12  0x72F3B0  CMemoryMgr::Init: ret
  13  0x4F3940  uninitialized_copy de punteros: con primero == ultimo devuelve el destino; si no copia `*dest = *src` solo si dest != 0 y avanza ambos; devuelve el destino final
  14  0x4F39C0  UserTracksLegacyString::Reallocate(tam, longitud) `ret 8` (std::string::_Copy con SEH)

Los falsos anotan: [0x53FB70, indice]; [0x5E4500, ECX (etiqueta), estado]; [0x538900, hash(ruta), hash(modo)]; [0x536F80, manejador]; [0x55D220, ECX, hash(nombre)]; [0x5389D0, manejador]; los de la lectura
de clump (ver Model); [0x82413F, puntero]; [0x72E870, ECX, bloque]; [0x72EC80, ECX, desc, sig, tam]; [0x72ED80, ECX, tam, dword de [this+38h]]; [0x82119A, tamano]; [0x8214BD, puntero].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CPhoneInfo::PhoneAnimFinishCallback", "CPhoneInfo::Constructor", "CPickup::IsWeaponSlotExchangeable", "SurfaceInfos_c::LoadSurfaceInfos", "SurfaceInfos_c::LoadSurfaceAudioInfos",
         "CZoneDef::IsPointWithin", "RpClumpGtaStreamReadAtOnce", "CMemoryHeap::JoinAdjacentFreeBlocks", "CMemoryHeap::Shutdown", "CMemoryHeap::FillInBlockData", "CMemoryHeap::Malloc",
         "CMemoryHeap::Init", "CMemoryMgr::Init", "UserTracksUninitCopy", "UserTracksLegacyString::Reallocate"]
ENTRIES = (0x453F40, 0x4541E0, 0x454920, 0x55EB90, 0x55F2B0, 0x72D850, 0x72E3A0, 0x72E870, 0x72EC60, 0x72EC80, 0x72ED80, 0x72F180, 0x72F3B0, 0x4F3940, 0x4F39C0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(PHONECB, PHONECTOR, SLOT, SURFINFO, SURFAUD, POINT, CLUMP, JOIN, SHUT, FILL, MALLOC, INIT, MGRINIT, UCOPY, REALLOC) = \
    'phonecb phonector slot surfinfo surfaud point clump join shut fill malloc init mgrinit ucopy realloc'.split()
KIND = [PHONECB, PHONECTOR, SLOT, SURFINFO, SURFAUD, POINT, CLUMP, JOIN, SHUT, FILL, MALLOC, INIT, MGRINIT, UCOPY, REALLOC]
KFC, KRD, KGEO, KATOMS, KGA, KFA = 32, 8, 24, 8, 32, 16
STACK = 0x7F000000
MARKER = 0xA9B0B0


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 208001) & M32)


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


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
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))
        self.glob = {}
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        addr &= M32
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

    def snap(self):
        for m in self.mem:
            m.snap()

    def gchanges(self, order):
        return []


def tag(arena, off=0):
    return 0x10000000 + (arena << 20) + GUARD + off


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
EXTRA = {
    PHONECB: ['blend_gt', 'blend_le', 'blend_eq', 'blend_nan', 'state13', 'state_other', 'mode_blend', 'mode_state', 'flag_bit_was_set', 'pad_bit_was_set'],
    PHONECTOR: [],
    SLOT: ['slot3', 'slot4', 'slot5', 'slot_below', 'slot_above', 'slot_negative'],
    SURFINFO: ['lines0', 'lines_many', 'comment', 'empty', 'data', 'unknown_adhesion', 'tyre_wraps', 'wet_wraps', 'id_high', 'extra_token', 'sand_skid', 'mud_skid'],
    SURFAUD: ['lines0', 'lines_many', 'comment', 'empty', 'data', 'id_high', 'extra_token', 'tab_sep'],
    POINT: ['z_nan', 'z_below', 'z_above', 'nan_xy', 'dot1_neg', 'dot1_over', 'dot2_neg', 'dot2_over', 'inside', 'huge', 'len_wrap', 'dot1_eq_len', 'dot_zero', 'dot2_eq_len'],
    CLUMP: ['ok', 'fc0_fail', 'read_short', 'create_null', 'fc1_fail', 'flread_fail', 'fc2_fail', 'list_struct_fail', 'list_count_short', 'malloc_null', 'geo_fail', 'atom_fail', 'count_zero', 'count_neg',
            'atoms_many', 'atoms_zero', 'setgeom_list', 'setgeom_stream', 'setframe', 'flags_changed'],
    JOIN: ['no_prev', 'prev_free', 'prev_used', 'next_free', 'next_used', 'both_free', 'neither', 'prev_elsewhere'],
    SHUT: ['first_null', 'first_arena', 'first_raw'],
    FILL: ['remainder', 'no_remainder', 'extend', 'exact', 'cs_list', 'free_list', 'size_zero', 'cs_idx_neg', 'nd_below_next'],
    MALLOC: ['cs_hit', 'cs_miss_all', 'free_hit', 'free_miss', 'blk_zero', 'blk_big', 'no_cs', 'rounded', 'wrap'],
    INIT: ['sizes', 'no_sizes', 'size_small', 'size_big'],
    MGRINIT: [],
    UCOPY: ['n0', 'n_many', 'dest_null', 'dest_set'],
    REALLOC: ['new_ok', 'first_throws', 'both_throw', 'cap_small', 'cap_big', 'len0', 'len_big', 'free_old', 'huge_size'],
}
KEYS = {i: COMMON + EXTRA[KIND[i]] for i in range(len(NAMES))}


# ------------------------------------------------------------------------------------------------ aritmetica x87 (24 bits, exponente ampliado)
def r24(x):
    """Fraction -> Fraction redondeada a 24 bits de significando (al par), exponente sin limite (palabra de control de 24 bits de la x87)"""
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    x = abs(x)
    e = x.numerator.bit_length() - x.denominator.bit_length()
    two_e = Fraction(2) ** e
    while x < two_e:
        e -= 1
        two_e /= 2
    while x >= two_e * 2:
        e += 1
        two_e *= 2
    ulp = Fraction(2) ** (e - 23)
    q = x / ulp
    fl = q.numerator // q.denominator
    rem = q - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    return sign * fl * ulp


def to_f32(fr):
    """Fraction (ya con 24 bits) -> float32 (como Fraction o float inf): desbordamiento a infinito, subnormales al par"""
    try:
        return Fraction(struct.unpack('<f', struct.pack('<f', float(fr)))[0])
    except OverflowError:
        return math.inf if fr > 0 else -math.inf


def ftol_low8(x):
    """byte bajo de _ftol2(x): truncamiento a entero de 64 bits; fuera de rango o NaN: el entero indefinido 0x8000000000000000 (byte 0)"""
    if isinstance(x, float) and (math.isnan(x) or math.isinf(x)):
        return 0
    t = int(x)
    if not (-(1 << 63) <= t < (1 << 63)):
        return 0
    return t & 0xFF


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, threw = [], None, False
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == PHONECB:
        A.wr(o + 0x2C, 4, a[4]); A.wr(o + 0x18, 4, a[5]); A.wr(o + 0x1C, 4, a[6])
        B.wr(a[7] + 0x530, 4, a[8]); B.wr(a[7] + 0x46C, 4, a[9])
        Cc.wr(a[10] + 0x10C, 4, a[11])
        W.snap()
        if (A.rd(o + 0x2E, 1) & 4):
            marks.append('flag_bit_was_set')
        if (Cc.rd(a[10] + 0x10E, 1) & 0x40):
            marks.append('pad_bit_was_set')
        A.wr(o + 0x2E, 1, A.rd(o + 0x2E, 1) | 4)
        A.wr(o + 0x1C, 4, 0xC47A0000)
        events.append([0x53FB70, 0])
        mode = a[12]
        if mode & 1:
            A.wr(o + 0x18, 4, a[13])
            marks.append('mode_blend')
        if mode & 2:
            B.wr(a[7] + 0x530, 4, a[14])
            marks.append('mode_state')
        Cc.wr(a[10] + 0x10E, 1, Cc.rd(a[10] + 0x10E, 1) & 0xBF)
        blend = f32(A.rd(o + 0x18, 4))
        if math.isnan(blend):
            marks.append('blend_nan')
        elif blend > 0.5:
            marks.append('blend_gt')
        elif blend == 0.5:
            marks.append('blend_eq')
        else:
            marks.append('blend_le')
        if blend > 0.5:
            B.wr(a[7] + 0x46C, 4, B.rd(a[7] + 0x46C, 4) | 0x4000)
        if B.rd(a[7] + 0x530, 4) == 0x13:
            events.append([0x5E4500, W.tagv(W.raw(1, a[7])), 1])
            marks.append('state13')
        else:
            marks.append('state_other')
    elif k == PHONECTOR:
        W.snap()
        ret = W.tagv(this)
    elif k == SLOT:
        W.snap()
        ret = 1 if a[4] in (3, 4, 5) else 0
        marks.append({3: 'slot3', 4: 'slot4', 5: 'slot5'}.get(a[4], 'slot_negative' if a[4] & 0x80000000 else ('slot_below' if a[4] < 3 else 'slot_above')))
    elif k in (SURFINFO, SURFAUD):
        audio = k == SURFAUD
        W.snap()
        data = bytes.fromhex(c['s'])
        lines, pos = [], 0
        for _ in range(a[4]):
            if pos >= len(data):
                break
            end = data.index(b'\0', pos)
            lines.append(data[pos:end].decode('latin1'))
            pos = end + 1
        path = b'data\\surfaud.dat' if audio else b'data\\surfinfo.dat'
        events.append([0x538900, fnv(path), fnv(b'r')])
        marks.append('lines0' if not lines else ('lines_many' if len(lines) >= 3 else 'lines1'))
        k_id = 0
        for line in lines:
            events.append([0x536F80, a[20]])
            if line == '':
                marks.append('empty')
                continue
            if line[0] == '#':
                marks.append('comment')
                continue
            tok = line.split()
            marks.append('data')
            if len(tok) > (10 if audio else 36):
                marks.append('extra_token')
            if '\t' in line:
                marks.append('tab_sep')
            name = tok[0]
            events.append([0x55D220, W.tagv(this), fnv(name.encode('latin1'))])
            ident = a[6 + k_id] if k_id < 6 else 0
            k_id += 1
            if ident >= 190:
                marks.append('id_high')
            ent = o + 0x90 + 12 * ident
            w1, w2 = A.rd(ent + 4, 4), A.rd(ent + 8, 4)
            if audio:
                ints = [int(t) for t in tok[1:10]]
                for i, v in enumerate(ints):
                    bit = 10 + i
                    w2 = (w2 & ~(1 << bit)) | ((v & 1) << bit)
            else:
                adh, tyre, wet, skid, fric = tok[1], tok[2], tok[3], tok[4], tok[5]
                ints = [int(t) for t in tok[6:35]]
                bullet = tok[35]
                tyre32 = Fraction(struct.unpack('<f', struct.pack('<f', float(tyre)))[0])
                wet32 = Fraction(struct.unpack('<f', struct.pack('<f', float(wet)))[0])
                tb = ftol_low8(r24(tyre32 * 10))
                wb = ftol_low8(r24(wet32 * 100))
                if not -128 <= int(r24(tyre32 * 10)) <= 127:
                    marks.append('tyre_wraps')
                if not -128 <= int(r24(wet32 * 100)) <= 127:
                    marks.append('wet_wraps')
                A.wr(ent + 0, 1, tb)
                A.wr(ent + 1, 1, wb)
                adh_map = {'RUBBER': 0, 'HARD': 1, 'ROAD': 2, 'LOOSE': 3, 'SAND': 4, 'WET': 5}
                if adh in adh_map:
                    w1 = (w1 & ~7) | adh_map[adh]
                else:
                    marks.append('unknown_adhesion')
                skid_map = {'DEFAULT': 0, 'SANDY': 2, 'MUDDY': 1}
                if skid in skid_map:
                    w1 = (w1 & ~(3 << 3)) | (skid_map[skid] << 3)
                    if skid == 'SANDY':
                        marks.append('sand_skid')
                    if skid == 'MUDDY':
                        marks.append('mud_skid')
                fric_map = {'NONE': 0, 'SPARKS': 1}
                if fric in fric_map:
                    w1 = (w1 & ~(7 << 5)) | (fric_map[fric] << 5)
                bul_map = {'NONE': 0, 'SPARKS': 1, 'SAND': 2, 'WOOD': 3, 'DUST': 4}
                if bullet in bul_map:
                    w1 = (w1 & ~(7 << 8)) | (bul_map[bullet] << 8)
                for i in range(12):
                    bit = 11 + i
                    w1 = (w1 & ~(1 << bit)) | ((ints[i] & 1) << bit)
                w1 = (w1 & ~(3 << 23)) | ((ints[12] & 3) << 23)
                w1 = (w1 & ~(3 << 25)) | ((ints[13] & 3) << 25)
                for i in range(14, 19):
                    bit = 27 + (i - 14)
                    w1 = (w1 & ~(1 << bit)) | ((ints[i] & 1) << bit)
                for i in range(19, 29):
                    bit = i - 19
                    w2 = (w2 & ~(1 << bit)) | ((ints[i] & 1) << bit)
            A.wr(ent + 4, 4, w1)
            A.wr(ent + 8, 4, w2)
        events.append([0x536F80, a[20]])
        events.append([0x5389D0, a[20]])
    elif k == POINT:
        for i in range(8):
            A.wr(o + 2 * i, 2, a[4 + i] & 0xFFFF)
        W.snap()
        cx, cy, v1x, v1y, v2x, v2y, minz, maxz = [s16(a[4 + i]) for i in range(8)]
        x, y, z = f32(a[12]), f32(a[13]), f32(a[14])
        if math.isnan(z):
            marks.append('z_nan')
        res = 0
        if not (minz < z):
            if not math.isnan(z):
                marks.append('z_below')
        elif not (maxz > z):
            marks.append('z_above')
        else:
            def sub(v, cc):
                if math.isnan(v) or math.isinf(v):
                    return v - cc
                return to_f32(r24(Fraction(v) - cc))

            dx, dy = sub(x, cx), sub(y, cy)
            if (isinstance(dx, float) and math.isnan(dx)) or (isinstance(dy, float) and math.isnan(dy)):
                marks.append('nan_xy')
            if abs(x) > 1e30 or abs(y) > 1e30:
                marks.append('huge')
            nonfinite = any(isinstance(d, float) and (math.isnan(d) or math.isinf(d)) for d in (dx, dy))

            def dot(vx, vy):
                if nonfinite:
                    return float(vx) * float(dx) + float(vy) * float(dy)
                return r24(r24(Fraction(vx) * dx) + r24(Fraction(vy) * dy))

            ok = True
            for idx, (vx, vy) in enumerate(((v1x, v1y), (v2x, v2y))):
                d = dot(vx, vy)
                ln = s32((vx * vx + vy * vy) & M32)
                if idx == 0 and ln < 0:
                    marks.append('len_wrap')
                if not (d >= 0):
                    marks.append('dot%d_neg' % (idx + 1))
                    ok = False
                    break
                if d == 0 and idx == 0:
                    marks.append('dot_zero')
                if not (d <= ln):
                    marks.append('dot%d_over' % (idx + 1))
                    ok = False
                    break
                if d == ln:
                    marks.append('dot1_eq_len' if idx == 0 else 'dot2_eq_len')
            res = 1 if ok else 0
            if ok:
                marks.append('inside')
        ret = res
    elif k == CLUMP:
        return clump(c, a, W, marks)
    elif k == JOIN:
        b, sk, sP, sD, sN, mode = a[4], a[5], a[6], a[7], a[8], a[10]
        if mode == 1:
            pOff = b + sk
            dOff = pOff + sP + 0x10
        elif mode == 2:
            pOff = a[19] + sk
            dOff = b + sk
        else:
            pOff = 0
            dOff = b + sk
        nOff = dOff + sD + 0x10
        Pa, Da, Na = W.raw(1, pOff), W.raw(1, dOff), W.raw(1, nOff)

        def node(sel):
            if sel < 8:
                return W.raw(2, a[18] + 0x20 * sel)
            return Da if sel == 8 else (Na if sel == 9 else (Pa if mode else W.raw(2, a[18])))
        if mode != 0:
            B.wr(pOff, 4, sP); B.wr(pOff + 4, 1, a[11]); B.wr(pOff + 0x10, 4, node(a[14])); B.wr(pOff + 0x14, 4, node(a[15]))
        B.wr(dOff, 4, sD); B.wr(dOff + 0xC, 4, 0 if mode == 0 else Pa)
        B.wr(nOff + 4, 1, a[12]); B.wr(nOff, 4, sN); B.wr(nOff + 0x10, 4, node(a[16])); B.wr(nOff + 0x14, 4, node(a[17]))
        W.snap()
        desc = Da
        nxt = (desc + W.rd(desc, 4) + 0x10) & M32          # lea edx,[ecx+eax+10h]
        prev = W.rd(desc + 0xC, 4)
        marks.append('no_prev' if prev == 0 else ('prev_elsewhere' if mode == 2 else 'prev_used'))
        nfree = W.rd(nxt + 4, 1) == 0
        if nfree:
            esi = W.rd(nxt + 0x10, 4); edi = W.rd(nxt + 0x14, 4); W.wr(esi + 0x14, 4, edi)
            esi = W.rd(nxt + 0x14, 4); edi = W.rd(nxt + 0x10, 4); W.wr(esi + 0x10, 4, edi)
            esi = W.rd(nxt, 4)
            nxt = (nxt + esi + 0x10) & M32
            marks.append('next_free')
        else:
            marks.append('next_used')
        pfree = False
        if prev != 0 and W.rd(prev + 4, 1) == 0:
            pfree = True
            eax = W.rd(prev + 0x10, 4); esi = W.rd(prev + 0x14, 4); W.wr(eax + 0x14, 4, esi)
            eax = W.rd(prev + 0x14, 4); esi = W.rd(prev + 0x10, 4); W.wr(eax + 0x10, 4, esi)
            desc = prev
            marks.append('prev_free')
        if nfree and pfree:
            marks.append('both_free')
        if not nfree and not pfree:
            marks.append('neither')
        W.wr(desc, 4, (nxt - desc - 0x10) & M32)
        W.wr(nxt + 0xC, 4, desc)
        ret = W.tagv(desc)
    elif k == SHUT:
        value = a[3] if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else 0)
        A.wr(o, 4, value)
        W.snap()
        events.append([0x82413F, W.tagv(value)])
        A.wr(o, 4, 0)
        marks.append('first_null' if value == 0 else ('first_arena' if W.tagv(value) != value else 'first_raw'))
    elif k in (FILL, MALLOC):
        heap_setup(W, a, o, this)
        blk = None
        if k == FILL:
            desc = W.raw(2, a[8]); size = a[9]; nd = (desc + 0x10 + size + (a[10] - 0x20)) & M32
            W.wr(W.raw(3, a[18]), 4, a[11])
            if a[4]:
                for idx in range(-1, 64):
                    ent = W.raw(1, a[5] + idx * 0x3C)
                    h = mix((a[12] + 0x9E3779B1 * (idx + 2)) & M32)
                    B.wr(a[5] + idx * 0x3C + 0x10, 4, (ent + 0x18) & M32 if h % 10 < 7 else W.raw(2, 0x40 + 0x20 * ((h >> 8) % 8)))
        else:
            blk = a[8]
            if blk & 0xF:
                blk = ((blk & ~0xF) + 0x10) & M32
            target = (blk >> 4) - 1
            if a[4]:
                for idx in range(-1, 64):
                    ent = W.raw(1, a[5] + idx * 0x3C)
                    off = a[5] + idx * 0x3C
                    h = mix((a[10] + 0x9E3779B1 * (idx + 2)) & M32)
                    nonempty = a[9] == 3 or (a[9] == 1 and (h & 1)) or (a[9] == 2 and idx == target)
                    if not nonempty:
                        B.wr(off + 0x10, 4, (ent + 0x18) & M32)
                        continue
                    coff = a[11] + 0x20 * (idx + 1)
                    nodeA, other = W.raw(2, coff), W.raw(2, a[11])
                    h2, h3 = mix((h + 1) & M32), mix((h + 2) & M32)
                    B.wr(off + 0x10, 4, nodeA)
                    Cc.wr(coff, 4, h3); Cc.wr(coff + 4, 1, (h2 >> 20) & 0xFF)
                    p2 = h2 % 100
                    p3 = h3 % 100
                    Cc.wr(coff + 0x10, 4, (ent + 0x18) & M32 if p2 < 60 else (ent if p2 < 75 else (nodeA if p2 < 85 else other)))
                    Cc.wr(coff + 0x14, 4, ent if p3 < 60 else ((ent + 0x18) & M32 if p3 < 75 else (nodeA if p3 < 85 else other)))
        W.snap()
        if k == FILL:
            fill_in(W, a, this, desc, nd, size, events, marks)
        else:
            ret = malloc(W, a, this, blk, events, marks)
    elif k == INIT:
        W.snap()
        mem, size, flag = W.raw(1, a[4]), a[5], a[6] & 0xFF
        last = (mem + size - 0x10) & M32
        W.wr(this, 4, mem); W.wr(this + 4, 4, last)
        W.wr(this + 0x40, 4, 0); W.wr(this + 0x3C, 1, 1); W.wr(this + 0x44, 4, M32)
        W.wr(last + 4, 1, 1)
        ecx = W.rd(this + 4, 4); W.wr(ecx + 6, 2, 0xFFFF)
        edx = W.rd(this + 4, 4); W.wr(edx, 4, 0)
        ecx = W.rd(this + 4, 4); edx = W.rd(this, 4); W.wr(ecx + 0xC, 4, edx)
        ecx = W.rd(this, 4); W.wr(ecx, 4, (size - 0x20) & M32)
        edx = W.rd(this, 4); W.wr(edx + 4, 1, 0)
        eax = W.rd(this, 4); W.wr(eax + 0xC, 4, 0)
        begin = (this + 8) & M32
        W.wr(this + 0x34, 4, begin)
        W.wr(this + 0x20, 4, 0x7FFFFFFF)
        W.wr(this + 0x18, 4, (this + 0x20) & M32)
        edx = W.rd(begin + 0x10, 4); ecx = W.rd(this, 4); W.wr(ecx + 0x10, 4, edx)
        edx = W.rd(begin + 0x10, 4); W.wr(edx + 0x14, 4, ecx)
        W.wr(ecx + 0x14, 4, begin)
        W.wr(begin + 0x10, 4, ecx)
        if flag:
            marks.append('sizes')
            W.wr(this + 0x38, 4, 0)
            events.append([0x72ED80, W.tagv(this), 0xF00, 0])
            cs = W.raw(2, a[8] + a[9])
            W.wr(this + 0x38, 4, cs)
            for i in range(64):
                e = (cs + 0x3C * i) & M32
                W.wr(e + 0x10, 4, (e + 0x18) & M32)
                W.wr(e + 0x2C, 4, e)
                W.wr(e + 0x34, 4, 0)
                W.wr(e + 0x38, 4, 0)
                W.wr(e + 0x30, 4, 0x10 * (i + 1))
        else:
            marks.append('no_sizes')
        marks.append('size_small' if size < 0x40 else ('size_big' if size >= 0x400 else 'size_mid'))
    elif k == MGRINIT:
        W.snap()
    elif k == UCOPY:
        n = a[4]
        first = W.raw(1, a[5])
        dest = 0 if a[7] else W.raw(2, a[6] + a[10])
        W.snap()
        for i in range(n):
            if dest:
                W.wr(dest + 4 * i, 4, W.rd(first + 4 * i, 4))
        ret = W.tagv((dest + 4 * n) & M32)
        marks.append('n0' if n == 0 else 'n_many')
        marks.append('dest_null' if a[7] else 'dest_set')
    elif k == REALLOC:
        cap, new_size, old_len, mode = a[4], a[7], a[8], a[10]
        if cap >= 0x10:
            A.wr(o + 4, 4, W.raw(1, a[5]))
        A.wr(o + 0x14, 4, a[6]); A.wr(o + 0x18, 4, cap)
        W.snap()
        new_cap = new_size | 0xF
        if new_cap > 0xFFFFFFFE:
            new_cap = new_size
        events.append([0x82119A, (new_cap + 1) & M32])
        marks.append('cap_big' if cap >= 0x10 else 'cap_small')
        marks.append('len0' if old_len == 0 else ('len_big' if old_len >= 0x20 else 'len_mid'))
        if new_size >= 0xFFFFFFF0:
            marks.append('huge_size')
        if mode >= 1:
            new_cap = new_size
            events.append([0x82119A, (new_cap + 1) & M32])
            marks.append('first_throws')
        if mode == 2:
            marks.append('both_throw')
            if cap >= 0x10:
                events.append([0x8214BD, W.tagv(A.rd(o + 4, 4))])
            A.wr(o + 0x18, 4, 0xF); A.wr(o + 0x14, 4, 0); A.wr(o + 4, 1, 0)
            threw = True
        else:
            if mode == 0:
                marks.append('new_ok')
            new_ptr = W.raw(2, a[11] if mode == 1 else a[9])
            if old_len > 0:
                src = A.rd(o + 4, 4) if cap >= 0x10 else (this + 4) & M32
                data = [W.rd(src + i, 1) for i in range(old_len)]
                for i, byte in enumerate(data):
                    W.wr(new_ptr + i, 1, byte)
            if cap >= 0x10:
                events.append([0x8214BD, W.tagv(A.rd(o + 4, 4))])
                marks.append('free_old')
            A.wr(o + 4, 1, 0)
            A.wr(o + 4, 4, new_ptr)
            A.wr(o + 0x18, 4, new_cap)
            A.wr(o + 0x14, 4, old_len)
            W.wr((new_ptr if new_cap >= 0x10 else (this + 4) & M32) + old_len, 1, 0)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'threw': threw, 'marks': marks}


# ------------------------------------------------------------------------------------------------ heap
def build_free(W, a, o, H):
    A, B, Cc, D = W.mem
    k = a[13]
    end, begin = (H + 0x20) & M32, (H + 8) & M32
    A.wr(o + 0x20, 4, 0x7FFFFFFF)
    nodes = [W.raw(3, a[19] + 0x20 * i) for i in range(k)]
    A.wr(o + 0x18, 4, nodes[0] if k else end)
    A.wr(o + 0x1C, 4, 0)
    for i in range(k):
        off = a[19] + 0x20 * i
        D.wr(off, 4, a[14 + i])
        D.wr(off + 0x10, 4, nodes[i + 1] if i + 1 < k else end)
        D.wr(off + 0x14, 4, nodes[i - 1] if i else begin)
    A.wr(o + 0x34, 4, nodes[k - 1] if k else begin)


def heap_setup(W, a, o, H):
    A = W.mem[0]
    A.wr(o + 0x38, 4, W.raw(1, a[5]) if a[4] else 0)
    A.wr(o + 0x40, 4, a[6]); A.wr(o + 0x44, 4, a[7])
    build_free(W, a, o, H)


def fill_in(W, a, this, desc, nd, size, events, marks):
    W.wr(desc, 4, size)
    nxt = (desc + size + 0x10) & M32
    if nxt < ((nd - 0x10) & M32):
        marks.append('remainder')
        W.wr(desc + 4, 1, 1)
        W.wr(desc + 6, 2, W.rd(this + 0x44, 2))
        W.wr(this + 0x40, 4, (W.rd(this + 0x40, 4) + size + 0x10) & M32)
        W.wr(nxt + 0xC, 4, desc)
        W.wr(nxt, 4, (nd - nxt - 0x10) & M32)
        W.wr(nxt + 4, 1, 0)
        W.wr(nd + 0xC, 4, nxt)
        events.append([0x72E870, W.tagv(this), W.tagv(nxt)])
        joined = W.raw(3, a[18])
        cs = W.rd(this + 0x38, 4)
        jsize = W.rd(joined, 4)
        if cs != 0 and jsize <= 0x400:
            marks.append('cs_list')
            if (jsize >> 4) == 0:
                marks.append('cs_idx_neg')
            ent = (((jsize >> 4) * 0x3C) + cs - 0x3C) & M32
            edx = W.rd(ent + 0x10, 4)
            W.wr(joined + 0x10, 4, edx)
            edx = W.rd(ent + 0x10, 4)
            W.wr(edx + 0x14, 4, joined)
            W.wr(joined + 0x14, 4, ent)
            W.wr(ent + 0x10, 4, joined)
            W.wr(ent + 0x38, 4, (W.rd(ent + 0x38, 4) + 1) & M32)
        else:
            marks.append('free_list')
            found = W.rd(this + 0x18, 4)
            if W.rd(found, 4) < jsize:
                while True:
                    found = W.rd(found + 0x10, 4)
                    if not (W.rd(found, 4) < jsize):
                        break
            before = W.rd(found + 0x14, 4)
            edx = W.rd(before + 0x10, 4)
            W.wr(joined + 0x10, 4, edx)
            ecx = W.rd(before + 0x10, 4)
            W.wr(ecx + 0x14, 4, joined)
            W.wr(joined + 0x14, 4, before)
            W.wr(before + 0x10, 4, joined)
        if size == 0:
            marks.append('size_zero')
    else:
        marks.append('no_remainder')
        if nxt < nd:
            marks.append('extend')
            W.wr(desc, 4, (nd - desc - 0x10) & M32)
        else:
            marks.append('exact' if nxt == nd else 'nd_below_next')
        W.wr(nd + 0xC, 4, desc)
        eax = W.rd(desc, 4)
        W.wr(desc + 4, 1, 1)
        W.wr(desc + 6, 2, W.rd(this + 0x44, 2))
        W.wr(this + 0x40, 4, (W.rd(this + 0x40, 4) + eax + 0x10) & M32)


def malloc(W, a, this, blk, events, marks):
    if blk == 0:
        marks.append('blk_zero')
    if blk > 0x400:
        marks.append('blk_big')
    if a[8] & 0xF:
        marks.append('rounded')
    if a[8] >= 0xFFFFFFF1:
        marks.append('wrap')
    cs = W.rd(this + 0x38, 4)
    if cs == 0:
        marks.append('no_cs')
    if cs != 0 and blk <= 0x400:
        esi = ((blk >> 4) - 1) * 0x3C
        while True:
            ent = (cs + esi) & M32
            edx = W.rd(ent + 0x10, 4)
            if edx != ((ent + 0x18) & M32):
                W.wr(ent + 0x38, 4, (W.rd(ent + 0x38, 4) - 1) & M32)
                edi = W.rd(edx + 0x14, 4); esi2 = W.rd(edx + 0x10, 4); W.wr(esi2 + 0x14, 4, edi)
                esi2 = W.rd(edx + 0x14, 4); edi = W.rd(edx + 0x10, 4)
                eax = (edx + 0x10) & M32
                W.wr(esi2 + 0x10, 4, edi)
                W.wr(edx + 4, 1, 1)
                W.wr(edx + 6, 2, W.rd(this + 0x44, 2))
                W.wr(this + 0x40, 4, (W.rd(this + 0x40, 4) + W.rd(edx, 4) + 0x10) & M32)
                marks.append('cs_hit')
                return W.tagv(eax)
            W.wr(ent + 0x34, 4, (W.rd(ent + 0x34, 4) + 1) & M32)
            esi += 0x3C
            if not s32(esi) < 0xF00:
                break
        marks.append('cs_miss_all')
    eax = W.rd(this + 0x18, 4)
    end = (this + 0x20) & M32
    if eax == end:
        marks.append('free_miss')
        return 0
    while True:
        if W.rd(eax, 4) >= blk:
            break
        eax = W.rd(eax + 0x10, 4)
        if eax == end:
            marks.append('free_miss')
            return 0
    edx = W.rd(eax + 0x10, 4); edi = W.rd(eax + 0x14, 4); esi = (eax + 0x10) & M32
    W.wr(edx + 0x14, 4, edi)
    edx = W.rd(eax + 0x14, 4); edi = W.rd(esi, 4)
    W.wr(edx + 0x10, 4, edi)
    edx = W.rd(eax, 4)
    events.append([0x72EC80, W.tagv(this), W.tagv(eax), W.tagv((edx + eax + 0x10) & M32), blk])
    marks.append('free_hit')
    return W.tagv(esi)


# ------------------------------------------------------------------------------------------------ lectura de clump
class Model:
    def __init__(self, c, W):
        self.v = c['v']
        self.W = W
        self.fc = self.rd = self.ge = self.at = 0
        self.ev = []
        self.stream = tag(0, 0x10)

    def find(self, typ, has_len, has_ver):
        k = self.fc
        self.fc += 1
        self.ev.append([1, self.stream, typ, int(has_len), int(has_ver)])
        val = self.v[k] if k < KFC else 0
        ln = self.v[32 + k] if has_len and k < KFC else 0
        return val & M32, ln

    def read(self, length):
        k = self.rd
        self.rd += 1
        self.ev.append([2, self.stream, STACK, length & M32])
        mode = self.v[64 + k] if k < KRD else 0
        pay = self.v[72 + 4 * k:72 + 4 * k + 4] if k < KRD else [0, 0, 0, 0]
        r = {0: length, 1: length - 1, 2: length + 1, 3: 0, 4: M32}[mode] & M32
        return r, pay

    def georead(self):
        k = self.ge
        self.ge += 1
        self.ev.append([11, self.stream])
        return (self.v[104 + k] if k < KGEO else 0) & M32

    def atomcreate(self):
        k = self.at
        self.at += 1
        self.ev.append([13])
        if k >= KATOMS or (self.v[128] >> k) & 1:
            return 0, k
        return tag(3, 0x40 * k), k


def clump(c, a, W, marks):
    A, B, Cc, D = W.mem
    v = c['v']
    m = Model(c, W)
    for i in range(KFA):
        B.wr(4 * i, 4, v[166 + i])
    for i in range(KGA):
        B.wr(0x100 + 4 * i, 4, v[134 + i])
    Cc.wr(4, 4, v[190])
    for i in range(KATOMS):
        D.wr(0x40 * i + 2, 1, v[182 + i])
    W.snap()
    glnum_ref = {'n': 0, 'vec': 0}
    flmode, flnum = v[132], v[133]
    frames = tag(1, 0) if flmode == 0 else (0 if flmode == 1 else 0xC0DE0000)

    def finish(ret):
        return {'events': m.ev, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'threw': False, 'marks': marks}

    def destroy_list():
        if glnum_ref['n'] != 0:
            for i in range(max(0, s32(glnum_ref['n']))):
                w = B.rd(0x100 + 4 * i, 4)
                if w:
                    m.ev.append([12, w])
            m.ev.append([10, glnum_ref['vec']])
        glnum_ref['n'] = 0

    def abort(clump_tag, with_list=False):
        if with_list:
            destroy_list()
        m.ev.append([21, STACK])
        m.ev.append([19, clump_tag])
        return finish(0)

    ok, _ = m.find(1, True, True)
    if not ok:
        marks.append('fc0_fail')
        return finish(0)
    r, pay = m.read(12)
    if r != 12:
        marks.append('read_short')
        return finish(0)
    natoms = pay[0]
    m.ev.append([17])
    clump_tag = 0 if v[129] else tag(2, 0)
    if clump_tag == 0:
        marks.append('create_null')
        return finish(0)
    ok, _ = m.find(0xE, False, True)
    if not ok:
        marks.append('fc1_fail')
        return finish(0)
    m.ev.append([20, m.stream, STACK])
    if not v[131]:
        marks.append('flread_fail')
        m.ev.append([19, clump_tag])
        return finish(0)
    ok, _ = m.find(0x1A, False, True)
    if not ok:
        marks.append('fc2_fail')
        return abort(clump_tag)
    # lista de geometrias en una fase
    ok, _ = m.find(1, True, True)
    if not ok:
        marks.append('list_struct_fail')
        return abort(clump_tag)
    r, pay = m.read(4)
    if r != 4:
        marks.append('list_count_short')
        return abort(clump_tag)
    count = pay[0]
    glnum_ref['n'] = count
    if s32(count) > 0:
        m.ev.append([9, (count * 4) & M32, 0])
        if v[130]:
            marks.append('malloc_null')
            glnum_ref['vec'] = 0
            return abort(clump_tag)
        glnum_ref['vec'] = tag(1, 0x100)
    else:
        glnum_ref['vec'] = 0
        marks.append('count_zero' if count == 0 else 'count_neg')
    for i in range(max(0, s32(count))):
        ok, _ = m.find(0xF, False, True)
        if not ok:
            marks.append('geo_fail')
            return abort(clump_tag)
        geo = m.georead()
        B.wr(0x100 + 4 * i, 4, geo)
        if geo == 0:
            marks.append('geo_fail')
            return abort(clump_tag)
    # exito de la lista: clump->parent = frames[0]
    if frames != tag(1, 0):
        raise ValueError('el vector de frames solo se desreferencia con modo 0')
    Cc.wr(4, 4, B.rd(0, 4))
    if s32(natoms) > 6:
        marks.append('atoms_many')
    if s32(natoms) <= 0:
        marks.append('atoms_zero')
    for i in range(max(0, s32(natoms))):
        ok, _ = m.find(0x14, False, True)
        if not ok:
            marks.append('atom_fail')
            return abort(clump_tag, True)
        atom = atom_read(m, c, B, D, glnum_ref, flnum, frames, marks)
        if not atom:
            marks.append('atom_fail')
            return abort(clump_tag, True)
        m.ev.append([18, clump_tag, atom])
    destroy_list()
    m.ev.append([21, STACK])
    if not any(x in marks for x in ('atom_fail',)):
        marks.append('ok')
    return finish(clump_tag)


def atom_read(m, c, B, D, gl, flnum, frames, marks):
    v = c['v']
    ok, ln = m.find(1, True, True)
    if not ok:
        return 0
    r, pay = m.read(ln)
    if r != (ln & M32):
        return 0
    atom, ak = m.atomcreate()
    if atom == 0:
        return 0
    flags = pay[2] & 0xFF
    if flags != D.rd(0x40 * ak + 2, 1):
        marks.append('flags_changed')
    D.wr(0x40 * ak + 2, 1, flags)
    if flnum != 0:
        m.ev.append([14, atom, B.rd(4 * s32(pay[0]), 4)])
        marks.append('setframe')
    if gl['n'] != 0:
        m.ev.append([15, atom, B.rd(0x100 + 4 * s32(pay[1]), 4), 0])
        marks.append('setgeom_list')
        return atom
    ok, _ = m.find(0xF, False, True)
    if not ok:
        m.ev.append([16, atom])
        return 0
    geo = m.georead()
    if geo == 0:
        m.ev.append([16, atom])
        return 0
    m.ev.append([15, atom, geo, 0])
    m.ev.append([12, geo])
    marks.append('setgeom_stream')
    return atom


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if bool(o.get('threw')) != bool(e['threw']):
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
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
