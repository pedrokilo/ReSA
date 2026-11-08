"""Oraculo independiente (Python) de Fn130A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las diez funciones del lote 130A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):
  0  0x63A040  CPointRoute::CopyFrom (thiscall, src en pila, `ret 4`)
       esi = src; eax = [esi]; edx = 0; [ecx] = eax; si eax <= 0 (con signo) fin; bucle: edi = src + 4 + 12 i; copia [edi],[edi+4],[edi+8] a [dst + 4 + 12 i ..] UNO A UNO (leer y escribir cada dword); edx++;
       edi = [ecx] (contador del DESTINO, releido); si edx < edi (con signo) repite.
  1  0x4DFB60  CAEBankLoader::ResetRequests (thiscall, `ret`)
       50 veces (paso 0x20): words -1 en this+0x3C, +0x3E, +0x40; dwords 0 en this+0x24 y this+0x38; despues words 0 en this+0x664, +0x666, +0x668.
  2  0x5B3680  LoadingScreenLoadingFile (cdecl, 1 argumento, `ret`)
       esi = path; p = strrchr(path, 0x5C); nombre = p ? p + 1 : path; sprintf(0xB71670, "Loading %s", nombre); LoadingScreen("Loading the Game", 0xB71670, 0) (0x53DED0).
  3  0x536E90  CompressVectorExe (cdecl, out int16* y src float*, `ret`)
       por cada componente i (en orden): fld dword [src + 4 i]; fmul dword [128.0]; call _ftol2 (0x821B40); mov word [out + 2 i], ax (word bajo del int64 truncado).
  4  0x583AF0  CRadar::HasThisBlipBeenRevealed (cdecl, indice, `ret`; AL)
       al = byte [0xBA67A1]; si 0 -> 1. eax = indice * 40; si ([eax + 0xBA8715] & 4) == 0 -> 1; si dword [0xBA372C] >= 0x50 (sin signo) -> 1; llama a 0x572130(x = dword [eax + 0xBA86F8], y = dword [eax + 0xBA86FC])
       (cdecl, dos dwords crudos, add esp,8); si AL != 0 -> 1; si no, AL = 0.
  5  0x405A60  CIplDefPool::ScalarDeletingDestructor (thiscall, banderas en pila, `ret 4`)
       si dword [this+8] > 0 (con signo): si byte [this+0x10] != 0: free([this]) (0x8213AE) y free([this+4]); despues [this], [this+4], [this+8], [this+0xC] = 0 (el byte +0x10 NO se toca); si el bit 0 del
       BYTE de banderas: operator delete(this) (0x8214BD); devuelve this.
  6  0x632E00  CTaskSequences::GetAvailableSlot (cdecl, byte, `ret`)
       cl = byte [esp+4]; eax = -1; si cl != 0: desde 0x20 hasta 0x40 (exclusivo), si no desde 0 hasta 0x20; para i: si byte [0xC17898 + i] == 0 y dword [0xC17900 + 0x40 i] == 0 -> eax = i y fin.
  7  0x49CD50  CTagManager::ResetAlpha (cdecl, entidad, `ret`)
       edx = dword [entidad + 0x18]; si 0 fin; eax = dword [0xA9AD70] (numTags); desde eax hasta 1: si dword [0xA9A8B8 + 8 eax] == entidad -> alfa = byte [0xA9A8C0 + 8 (eax - 1) + 4]; SetUserValue(edx, alfa) (0x732380).
       (sin coincidencia el EXE lee la direccion 4: los casos garantizan una coincidencia).
  8  0x40FAF0  CBoundingBox::Constructor (thiscall, `ret`): [this..+0xC) = 1.0f x3, [this+0xC..+0x18) = -1.0f x3; devuelve this.
  9  0x532770  CDummy::ScalarDeletingDestructor (thiscall, banderas, `ret 4`)
       call 0x532560 (cuerpo); si el bit 0 del BYTE de banderas: pool = dword [0xB744A0]; idx = (this - [pool]) / 56 (con signo, truncado hacia cero); byte [[pool+4] + idx] |= 0x80;
       si idx < dword [pool+0xC] (con signo): [pool+0xC] = idx; devuelve this.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CPointRoute::CopyFrom', 'CAEBankLoader::ResetRequests', 'LoadingScreenLoadingFile', 'CompressVectorExe', 'CRadar::HasThisBlipBeenRevealed', 'CIplDefPool::ScalarDeletingDestructor',
         'CTaskSequences::GetAvailableSlot', 'CTagManager::ResetAlpha', 'CBoundingBox::Constructor', 'CDummy::ScalarDeletingDestructor', 'CTaskSimpleAffectSecondaryBehaviour::Destructor']
ENTRIES = (0x63A040, 0x4DFB60, 0x5B3680, 0x536E90, 0x583AF0, 0x405A60, 0x632E00, 0x49CD50, 0x40FAF0, 0x532770, 0x6912C0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
GSTRING = 352


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


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytearray(out[:nbytes])


def fill_kind(nbytes, kind, seed):
    if kind == 0:
        return fill_words(nbytes, seed)
    return bytearray([0x00 if kind == 1 else (0xFF if kind == 2 else 0xCD)]) * nbytes


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


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


def f2i(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def ftol2_word(bits):
    """word bajo del int64 que devuelve _ftol2(float * 128.0f): NaN, infinito y fuera de rango -> 0x8000000000000000 (word 0)"""
    v = f2i(bits)
    if v != v or v in (float('inf'), float('-inf')):
        return 0, 'nonfinite'
    p = v * 128.0   # exacto (potencia de dos) tambien para denormales y valores enormes (doble)
    t = int(p)      # trunca hacia cero
    if abs(t) >= (1 << 63):
        return 0, 'indef'
    cls = 'zero' if t == 0 else ('small' if abs(t) < 32768 else ('wrap16' if abs(t) < (1 << 31) else 'wrap32'))
    return t & 0xFFFF, cls


KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'cnt_neg', 'cnt_zero', 'cnt_1', 'cnt_2_7', 'cnt_8', 'cnt_9plus', 'same_arena', 'sep_arena', 'overlap', 'this_unaligned', 'src_unaligned', 'writes_guard'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill'],
    2: ['kind0', 'kind1', 'kind2', 'fill0', 'gfill0', 'gfill1', 'gfill2', 'gfill3', 'has_backslash', 'no_backslash', 'ends_backslash', 'empty', 'len_long', 'high_bytes', 'percent', 'only_backslashes'],
    3: ['kind0', 'kind1', 'kind2', 'kind3', 'overlap', 'comp_nonfinite', 'comp_zero', 'comp_small', 'comp_wrap16', 'comp_wrap32', 'comp_indef', 'comp_neg', 'comp_denorm', 'comp_nan_signaling', 'out_unaligned'],
    4: ['kind0', 'kind1', 'kind2', 'kind3', 'early_draw', 'early_flags', 'early_revealed', 'call', 'call_true', 'call_false', 'ret_low_zero_high_set', 'draw_multi', 'revealed_80', 'revealed_79', 'pos_nan_signaling'],
    5: ['kind0', 'kind1', 'kind2', 'kind3', 'size_pos_owner', 'size_pos_noowner', 'size_zero', 'size_neg', 'delete', 'keep', 'flags_upper_only', 'owner_nonone'],
    6: ['flag_zero', 'flag_nonzero', 'flag_low_zero_upper_set', 'found_lo', 'found_hi', 'not_found', 'skip_opened', 'skip_task', 'found_first', 'found_last_of_range', 'fill0', 'fill1'],
    7: ['kind0', 'kind1', 'kind2', 'kind3', 'null_atomic', 'found_last', 'found_first', 'found_mid', 'dup', 'ntags_1', 'ntags_150', 'outside_match'],
    8: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_unaligned', 'spill'],
    9: ['kind0', 'kind1', 'kind2', 'kind3', 'delete', 'keep', 'idx_neg', 'idx_zero', 'idx_pos', 'first_lowered', 'first_kept', 'diff_neg_rem', 'flags_upper_only', 'prev_equals_new'],
    10: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'task_null', 'task_set', 'prev_vtable_same', 'prev_vtable_other', 'this_unaligned', 'spill'],
}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    s = bytes.fromhex(c['s'])
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[12]), Mem(fill, a[13]), Mem(fill, a[14])
    events, gch, ret = [], [], None
    ES = []

    if fn == 0:
        src = A if a[4] else B
        src.wr(a[3], 4, a[5])
        A.snap(); B.snap(); C.snap()
        cnt = s32(src.rd(a[3], 4))
        A.wr(a[0], 4, cnt)
        if cnt > 0:
            i = 0
            while True:
                for k in range(3):
                    v = src.rd(a[3] + 4 + 12 * i + 4 * k, 4)
                    A.wr(a[0] + 4 + 12 * i + 4 * k, 4, v)
                i += 1
                if not i < s32(A.rd(a[0], 4)):
                    break
        marks.append('cnt_neg' if cnt < 0 else 'cnt_zero' if cnt == 0 else 'cnt_1' if cnt == 1 else 'cnt_2_7' if cnt < 8 else 'cnt_8' if cnt == 8 else 'cnt_9plus')
        marks.append('same_arena' if a[4] else 'sep_arena')
        if a[4]:
            span = 4 + 12 * max(cnt, 0)
            if a[3] < a[0] + span and a[0] < a[3] + span:
                marks.append('overlap')
        if a[0] % 4:
            marks.append('this_unaligned')
        if a[3] % 4:
            marks.append('src_unaligned')
        if a[0] + 4 + 12 * max(cnt, 0) > WIN:
            marks.append('writes_guard')
    elif fn == 1:
        A.snap(); B.snap(); C.snap()
        o = a[0]
        for i in range(50):
            for off in (0x3C, 0x3E, 0x40):
                A.wr(o + off + 0x20 * i, 2, 0xFFFF)
            A.wr(o + 0x24 + 0x20 * i, 4, 0)
            A.wr(o + 0x38 + 0x20 * i, 4, 0)
        for off in (0x664, 0x666, 0x668):
            A.wr(o + off, 2, 0)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + 0x66A > WIN:
            marks.append('spill')
    elif fn == 2:
        A.snap(); B.snap(); C.snap()
        g = fill_kind(GSTRING, 0, a[15]) if a[3] == 0 else (bytearray([0x5A]) * GSTRING if a[3] == 3 else fill_kind(GSTRING, a[3], a[15]))
        g0 = bytes(g)
        p = s.rfind(b'\\')
        name = s[p + 1:] if p >= 0 else s
        text = b'Loading ' + name
        g[0:len(text) + 1] = text + b'\0'
        gch = [[0, i, g[i]] for i in range(GSTRING) if g[i] != g0[i]]
        events = [[0x53DED0, fnv(b'Loading the Game'), 16, 1, fnv(text), len(text), 0]]
        marks.append('gfill%d' % a[3])
        marks.append('has_backslash' if p >= 0 else 'no_backslash')
        if s.endswith(b'\\'):
            marks.append('ends_backslash')
        if not s:
            marks.append('empty')
        if len(s) > 200:
            marks.append('len_long')
        if any(b >= 0x80 for b in s):
            marks.append('high_bytes')
        if b'%' in s:
            marks.append('percent')
        if s and set(s) == {0x5C}:
            marks.append('only_backslashes')
    elif fn == 3:
        src = A if a[4] else B
        for k in range(3):
            src.wr(a[3] + 4 * k, 4, a[5 + k])
        A.snap(); B.snap(); C.snap()
        for k in range(3):
            bits = src.rd(a[3] + 4 * k, 4)
            w, cls = ftol2_word(bits)
            A.wr(a[0] + 2 * k, 2, w)
            marks.append('comp_nonfinite' if cls in ('nonfinite',) else 'comp_indef' if cls == 'indef' else 'comp_' + cls)
            if bits & 0x80000000:
                marks.append('comp_neg')
            if (bits & 0x7F800000) == 0 and (bits & 0x7FFFFF):
                marks.append('comp_denorm')
            if (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) and not (bits & 0x400000):
                marks.append('comp_nan_signaling')
        if a[4] and a[0] < a[3] + 12 and a[3] < a[0] + 6:
            marks.append('overlap')
        if a[0] % 2:
            marks.append('out_unaligned')
    elif fn == 4:
        A.snap(); B.snap(); C.snap()
        draw, flags, rev = a[3] & 0xFF, a[4] & 0xFF, a[5]
        marks.append('draw_multi' if draw > 1 else 'draw_single')
        if draw == 0:
            res = 1; marks.append('early_draw')
        elif not (flags & 4):
            res = 1; marks.append('early_flags')
        elif rev >= 0x50:
            res = 1; marks.append('early_revealed')
        else:
            events = [[0x572130, a[6], a[7]]]
            res = 1 if (a[8] & 0xFF) else 0
            marks.append('call'); marks.append('call_true' if res else 'call_false')
            if (a[8] & 0xFF) == 0 and a[8] & ~0xFF:
                marks.append('ret_low_zero_high_set')
            if (a[6] & 0x7F800000) == 0x7F800000 and (a[6] & 0x7FFFFF) and not (a[6] & 0x400000):
                marks.append('pos_nan_signaling')
        marks.append('revealed_80' if rev == 80 else 'revealed_79' if rev == 79 else 'revealed_other')
        ret = res
    elif fn == 5:
        o = a[0]
        A.wr(o + 0, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5]); A.wr(o + 0xC, 4, a[6]); A.wr(o + 0x10, 4, (a[8] | (a[9] << 8)) & M32)
        A.snap(); B.snap(); C.snap()
        size = s32(a[5])
        owner = a[8] & 0xFF
        if size > 0:
            if owner != 0:
                # free([this]) y free([this+4]); el puntero crudo no cae en una arena
                events.append([0x8213AE, a[3], 0xDEAD0000])
                events.append([0x8213AE, a[4], 0xDEAD0000])
                marks.append('size_pos_owner')
            else:
                marks.append('size_pos_noowner')
            for off in (0, 4, 8, 0xC):
                A.wr(o + off, 4, 0)
        else:
            marks.append('size_zero' if size == 0 else 'size_neg')
        if a[7] & 1:
            events.append([0x8214BD, tag(0, o), A.rd(o, 4)])
            marks.append('delete')
        else:
            marks.append('keep')
        if (a[7] & 0xFF) == 0 and a[7] & ~0xFF:
            marks.append('flags_upper_only')
        if owner not in (0, 1):
            marks.append('owner_nonone')
        ret = tag(0, o)
    elif fn == 6:
        A.snap(); B.snap(); C.snap()
        flag = a[0] & 0xFF
        first, last = (32, 64) if flag else (0, 32)
        res = M32
        skipped_open = skipped_task = False
        for i in range(first, last):
            opened = s[i]
            nonnull = (a[4] >> i) & 1 if i < 32 else (a[5] >> (i - 32)) & 1
            if opened == 0 and not nonnull:
                res = i
                break
            skipped_open |= opened != 0
            skipped_task |= opened == 0 and bool(nonnull)
        marks.append('flag_nonzero' if flag else 'flag_zero')
        if flag == 0 and a[0] & ~0xFF:
            marks.append('flag_low_zero_upper_set')
        if res == M32:
            marks.append('not_found')
        else:
            marks.append('found_hi' if flag else 'found_lo')
            if res == first:
                marks.append('found_first')
            if res == last - 1:
                marks.append('found_last_of_range')
        if skipped_open:
            marks.append('skip_opened')
        if skipped_task:
            marks.append('skip_task')
        ret = res
    elif fn == 7:
        A.wr(a[0] + 0x18, 4, a[3])
        A.snap(); B.snap(); C.snap()
        nt = a[4]
        mask = [(a[5 + i // 32] >> (i % 32)) & 1 for i in range(150)]
        if a[3] == 0:
            marks.append('null_atomic')
        else:
            hits = [i for i in range(nt) if mask[i]]
            i = hits[-1]
            events = [[0x732380, a[3], s[i] & 0xFFFF]]
            marks.append('found_last' if i == nt - 1 else 'found_first' if i == 0 else 'found_mid')
            if len(hits) > 1:
                marks.append('dup')
            if any(mask[nt:]):
                marks.append('outside_match')
        if nt == 1:
            marks.append('ntags_1')
        if nt == 150:
            marks.append('ntags_150')
    elif fn == 8:
        A.snap(); B.snap(); C.snap()
        o = a[0]
        for k in range(3):
            A.wr(o + 4 * k, 4, 0x3F800000)
            A.wr(o + 12 + 4 * k, 4, 0xBF800000)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + 0x18 > WIN:
            marks.append('spill')
        ret = tag(0, o)
    elif fn == 9:
        o = a[0]
        A.wr(o, 4, a[6])
        pool = a[10]
        # el pool de la arena B lleva (+0) el almacenamiento (this - diff), (+4) el vector de estados (direccion de la arena C + a[4]) y (+0xC) el contador; solo este ultimo cambia
        B.wr(pool + 0xC, 4, a[5])
        A.snap(); B.snap(); C.snap()
        events = [[0x532560, tag(0, o), a[6]]]
        A.wr(o, 4, a[8])
        diff = s32(a[3])
        idx = abs(diff) // 56 * (1 if diff >= 0 else -1)
        marks.append('prev_equals_new' if a[6] == a[8] else 'prev_differs')
        if a[7] & 1:
            C.wr(a[4] + idx, 1, C.rd(a[4] + idx, 1) | 0x80)
            if idx < s32(a[5]):
                B.wr(pool + 0xC, 4, idx)
                marks.append('first_lowered')
            else:
                marks.append('first_kept')
            marks.append('delete')
            marks.append('idx_neg' if idx < 0 else 'idx_zero' if idx == 0 else 'idx_pos')
            if diff < 0 and diff % 56:
                marks.append('diff_neg_rem')
        else:
            marks.append('keep')
        if (a[7] & 0xFF) == 0 and a[7] & ~0xFF:
            marks.append('flags_upper_only')
        ret = tag(0, o)
    else:
        o = a[0]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x870B08)
        if a[4]:
            events.append([0xFAB1, tag(1, a[10]), 1])
            marks.append('task_set')
        else:
            marks.append('task_null')
        events.append([0x61A3A0, tag(0, o), 0x870B08])
        marks.append('prev_vtable_same' if a[3] == 0x870B08 else 'prev_vtable_other')
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + 0x14 > WIN:
            marks.append('spill')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


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
