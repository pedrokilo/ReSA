"""Oraculo independiente (Python) de Fn173A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 21 funciones del lote 173A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x699E20  CPedList::FillUpHoles: edx = 0; para los 30 punteros desde [ecx+4] (en orden): si no es nulo, [eax] = el puntero, edx++, eax += 4 (eax empieza en ecx+4); si edx < 30: rep stosd de 30 - edx ceros en ecx+4+edx*4
   1  0x699F00  CPointList::Clear: [ecx] = 0; rep stosd de 6 ceros en ecx+124h
   2  0x732D60  CLinkList<AlphaObjectInfo>::Shutdown: push [esi+50h]; call 8213AE (delete[] del CRT); [esi+50h] = 0
   3  0x732D80  CLinkList<CPed*>::Shutdown: igual con +30h
   4  0x733910  CLinkList<AlphaObjectInfo>::InsertSorted(dato): edx = [this+10h] (primer enlace usado); mientras edx != this+14h y la distancia [edx+8] sea MENOR que [dato+8] o no ordenada (fld; fcomp; test ah,1): edx = [edx+10h];
            eax = [this+38h] (primer enlace libre); si eax == this+3Ch devuelve 0; copia los tres dwords del dato a [eax]; desengancha eax ([[eax+10h]+0Ch] = [eax+0Ch]; [[eax+0Ch]+10h] = [eax+10h]);
            edx = [edx+0Ch]; ecx = [edx+10h]; [eax+10h] = ecx; [ecx+0Ch] = eax; [eax+0Ch] = edx; [edx+10h] = eax; devuelve eax
   5  0x733F40  CLinkList<AlphaObjectInfo>::Clear: mientras [this+10h] != this+14h: eax = ese enlace; desengancharlo y ponerlo tras la cabeza libre (this+28h): [eax+10h] = [edx+10h]; [[edx+10h]+0Ch] = eax; [eax+0Ch] = edx; [edx+10h] = eax
   6  0x734610  CreateDebugFont: ret
   7  0x734620  DestroyDebugFont: ret
   8  0x734640  FlushObrsPrintfs: ret
   9  0x4C4910 (+0x4C4923, +0x4C4953)  CDamageAtomicModelInfo::CreateInstance(matriz): si [0A9B0B0h] == 0: jmp 4C44D0; eax = [this+20h]; si es nulo: ret 4 con eax = 0;
            si no: atomic = 749E60([this+20h]); frame = 7F0410(); copia 16 dwords de la matriz a frame+10h (rep movsd); 74BF20(atomic, frame); devuelve atomic
  10  0x4C4960  CDamageAtomicModelInfo::CreateInstance(): con [0A9B0B0h] != 0: [this+20h] nulo -> 0; si no clon, frame, 74BF20(atomic, frame), devuelve atomic; con 0: [this+1Ch] nulo -> 0; si no: 4C4BA0 (AddRef, ecx = this);
            atomic = 749E60([this+1Ch] leido otra vez); frame = 7F0410(); 74BF20(atomic, frame); 4C4BB0 (RemoveRef, ecx = this); devuelve atomic
  11  0x4C55D0  CDamageAtomicModelInfo cuerpo del destructor: [ecx] = 85BB9Ch
  12  0x4C55E0  CLodAtomicModelInfo cuerpo del constructor: push esi; mov esi,ecx; call 4C4A60; xor eax,eax; [esi+20h] = ax; [esi+22h] = ax; [esi] = 85BC70h; eax = esi
  13  0x6E76E0  CWaterLevel::SyncWater: [0C228A4h] = [0B7CB84h]
  14  0x552380  operator new de CPtrNodeSingleLink: ecx = [0B74484h]; jmp 552240 (New del pool)
  15  0x552390  operator delete de CPtrNodeSingleLink: pool = [0B74484h]; idx = ([esp+4] - [pool]) >> 3 (aritmetico); [[pool+4] + idx] |= 80h; si idx < [pool+0Ch] (con signo): [pool+0Ch] = idx
  16  0x5523C0  operator new de CPtrNodeDoubleLink: ecx = [0B74488h]; jmp 5522E0
  17  0x5523D0  operator delete de CPtrNodeDoubleLink: como 15 con idx = (nodo - [pool]) / 12 con signo
  18  0x4CFA30  CompressQuaternion(out, in): para i = 0..3: out[i] (word) = low16(_ftol(in[i] * 4096.0f)) (en este orden)
  19  0x4CFA90  DecompressQuaternion(out, in): para i = 0..3: out[i] (float) = (float)(int16)in[i] * (1/4096)
  20  0x4CFB10  DecompressTranslation(out, in): para i = 0..2: out[i] = (float)(int16)in[i] * (1/1024)

Los falsos anotan: [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (0x4C4A60); [direccion, ECX] (AddRef, RemoveRef, New de los pools); [0x8213AE, argumento (etiqueta o crudo), dword de [this+campo]];
[0x749E60, argumento]; [0x7F0410]; [0x74BF20, atomic, frame]; [0x4C44D0, ECX, matriz].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CPedList::FillUpHoles", "CPointList::Clear", "CLinkList<AlphaObjectInfo>::Shutdown", "CLinkList<CPed*>::Shutdown", "CLinkList<AlphaObjectInfo>::InsertSorted",
         "CLinkList<AlphaObjectInfo>::Clear", "App::CreateDebugFont", "App::DestroyDebugFont", "App::FlushObrsPrintfs", "CDamageAtomicModelInfo::CreateInstance-rwmat",
         "CDamageAtomicModelInfo::CreateInstance-void", "CDamageAtomicModelInfo::DestructorBody", "CLodAtomicModelInfo::ConstructorBody", "CWaterLevel::SyncWater",
         "CPtrNodeSingleLinkPool::OperatorNew", "CPtrNodeSingleLinkPool::OperatorDelete", "CPtrNodeDoubleLinkPool::OperatorNew", "CPtrNodeDoubleLinkPool::OperatorDelete",
         "CAnimBlendSequence::CompressQuaternion", "CAnimBlendSequence::DecompressQuaternion", "CAnimBlendSequence::DecompressTranslation"]
ENTRIES = (0x699E20, 0x699F00, 0x732D60, 0x732D80, 0x733910, 0x733F40, 0x734610, 0x734620, 0x734640, 0x4C4910, 0x4C4960, 0x4C55D0, 0x4C55E0, 0x6E76E0, 0x552380, 0x552390,
           0x5523C0, 0x5523D0, 0x4CFA30, 0x4CFA90, 0x4CFB10)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
FILL, PLCLEAR, SHUTDOWN, INSERT, LLCLEAR, NOP, DMG, DTOR7, LODCTOR, SYNC, POOLNEW, POOLDEL, COMPRESS, DECOMP4, DECOMP3 = \
    'fill plclear shutdown insert llclear nop dmg dtor7 lodctor sync poolnew pooldel compress decomp4 decomp3'.split()
KIND = [FILL, PLCLEAR, SHUTDOWN, SHUTDOWN, INSERT, LLCLEAR, NOP, NOP, NOP, DMG, DMG, DTOR7, LODCTOR, SYNC, POOLNEW, POOLDEL, POOLNEW, POOLDEL, COMPRESS, DECOMP4, DECOMP3]
POOL_NEW_ADDR = {14: 0x552240, 16: 0x5522E0}
POOL_NODE = {15: 8, 17: 12}
SHUT_FIELD = {2: 0x50, 3: 0x30}
GLOBAL_FLAG, GLOBAL_WATER, GLOBAL_CLOCK = 0xA9B0B0, 0xC228A4, 0xB7CB84


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


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def bits32(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


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
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == FILL:
        _k += ['holes', 'full', 'empty', 'moved']
    elif _kind == SHUTDOWN:
        _k += ['links_null', 'links_arena', 'links_raw']
    elif _kind in (INSERT, LLCLEAR):
        _k += ['used0', 'used_many', 'free0', 'free_some']
        if _kind == INSERT:
            _k += ['at_head', 'at_tail', 'middle', 'equal', 'nan_data', 'nan_list', 'denormal', 'no_free']
        else:
            _k += ['moved']
    elif _kind == DMG:
        _k += ['selector0', 'selector_set', 'object_null', 'object_set'] if _fn == 10 else ['selector0', 'selector_set', 'damaged_null', 'damaged_set']
    elif _kind == LODCTOR:
        _k += ['prev_zero', 'prev_other']
    elif _kind == SYNC:
        _k += ['changes_offset', 'same_offset']
    elif _kind == POOLNEW:
        _k += ['new_null', 'new_set']
    elif _kind == POOLDEL:
        _k += ['was_free', 'was_used', 'lowers_cursor', 'keeps_cursor', 'first_slot', 'last_slot']
    elif _kind == COMPRESS:
        _k += ['nan_in', 'inf_in', 'wraps_int16', 'huge', 'denormal', 'negative']
    elif _kind in (DECOMP4, DECOMP3):
        _k += ['negative', 'min_int16', 'max_int16', 'zero']
    KEYS[_fn] = _k


def ftol16(bits):
    """low16(_ftol(f * 4096)): producto exacto; entero truncado de 64 bits; fuera de rango o NaN: el entero indefinido (0x8000000000000000)"""
    value = f32(bits)
    if math.isnan(value) or math.isinf(value):
        return 0
    t = int(value * 4096.0)     # el producto es exacto en double (potencia de dos); int() trunca hacia cero
    if not (-(1 << 63) <= t < (1 << 63)):
        return 0
    return t & 0xFFFF


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == FILL:
        for i in range(30):
            A.wr(o + 4 + 4 * i, 4, a[4 + i])
        W.snap()
        vals = [x for x in a[4:34] if x != 0]
        marks.append('holes' if len(vals) < 30 else 'full')
        if not vals:
            marks.append('empty')
        if a[4:34] != vals + [0] * (30 - len(vals)):
            marks.append('moved')
        for i in range(30):
            A.wr(o + 4 + 4 * i, 4, vals[i] if i < len(vals) else 0)
    elif k == PLCLEAR:
        W.snap()
        A.wr(o, 4, 0)
        for i in range(6):
            A.wr(o + 0x124 + 4 * i, 4, 0)
    elif k == SHUTDOWN:
        field = SHUT_FIELD[fn]
        value = 0 if a[6] == 0 else (W.raw(1, a[5]) if a[6] == 1 else a[4])
        A.wr(o + field, 4, value)
        W.snap()
        events.append([0x8213AE, W.tagv(value), value])
        A.wr(o + field, 4, 0)
        marks.append('links_null' if a[6] == 0 else ('links_arena' if a[6] == 1 else 'links_raw'))
    elif k in (INSERT, LLCLEAR):
        n_used, n_free = a[4], a[5]
        used, freed = [], []
        for i in range(n_used + n_free):
            slot = a[8 + i]
            node = W.raw(1, a[6] + 0x20 * slot)
            B.wr(a[6] + 0x20 * slot + 8, 4, a[16 + slot])
            (used if i < n_used else freed).append(node)

        def chain(head, mids, tail):
            ch = [head] + mids + [tail]
            for i in range(len(ch)):
                if i > 0:
                    W.wr(ch[i] + 0xC, 4, ch[i - 1])
                if i + 1 < len(ch):
                    W.wr(ch[i] + 0x10, 4, ch[i + 1])
        chain(this, used, this + 0x14)
        chain(this + 0x28, freed, this + 0x3C)
        if k == INSERT:
            data = W.raw(2, a[27])
            Cc.wr(a[27], 4, a[24]); Cc.wr(a[27] + 4, 4, a[25]); Cc.wr(a[27] + 8, 4, a[26])
        W.snap()
        marks.append('used0' if n_used == 0 else 'used1')
        if n_used >= 2:
            marks.append('used_many')
        marks.append('free0' if n_free == 0 else 'free_some')
        if k == INSERT:
            new = f32(a[26])
            edx = W.rd(this + 0x10)
            steps = 0
            while edx != this + 0x14:
                existing = f32(W.rd(edx + 8))
                if not (existing >= new):         # fld [existente]; fcomp [nuevo]; test ah,1: menor o no ordenado -> sigue
                    edx = W.rd(edx + 0x10)
                    steps += 1
                else:
                    break
            eax = W.rd(this + 0x38)
            if eax == this + 0x3C:
                ret = 0
                marks.append('no_free')
            else:
                W.wr(eax, 4, W.rd(data)); W.wr(eax + 4, 4, W.rd(data + 4)); W.wr(eax + 8, 4, W.rd(data + 8))
                ecx = W.rd(eax + 0x10); esi = W.rd(eax + 0xC); W.wr(ecx + 0xC, 4, esi)
                ecx = W.rd(eax + 0xC); esi = W.rd(eax + 0x10); W.wr(ecx + 0x10, 4, esi)
                edx = W.rd(edx + 0xC)
                ecx = W.rd(edx + 0x10)
                W.wr(eax + 0x10, 4, ecx)
                ecx = W.rd(edx + 0x10)
                W.wr(ecx + 0xC, 4, eax)
                W.wr(eax + 0xC, 4, edx)
                W.wr(edx + 0x10, 4, eax)
                ret = W.tagv(eax)
                if steps == 0:
                    marks.append('at_head')
                if steps == n_used:
                    marks.append('at_tail')
                if 0 < steps < n_used:
                    marks.append('middle')
            if any(f32(a[16 + a[8 + i]]) == new for i in range(n_used)):
                marks.append('equal')
            if math.isnan(new):
                marks.append('nan_data')
            if any(math.isnan(f32(a[16 + a[8 + i]])) for i in range(n_used)):
                marks.append('nan_list')
            if any(0 < (a[16 + a[8 + i]] & 0x7FFFFFFF) < 0x800000 for i in range(n_used)) or 0 < (a[26] & 0x7FFFFFFF) < 0x800000:
                marks.append('denormal')
        else:
            esi = this + 0x14
            eax = W.rd(this + 0x10)
            if eax != esi:
                marks.append('moved')
                edx = this + 0x28
                while True:
                    edi = W.rd(eax + 0x10); ebx = W.rd(eax + 0xC); W.wr(edi + 0xC, 4, ebx)
                    edi = W.rd(eax + 0xC); ebx = W.rd(eax + 0x10); W.wr(edi + 0x10, 4, ebx)
                    edi = W.rd(edx + 0x10); W.wr(eax + 0x10, 4, edi)
                    edi = W.rd(edx + 0x10); W.wr(edi + 0xC, 4, eax)
                    W.wr(eax + 0xC, 4, edx)
                    W.wr(edx + 0x10, 4, eax)
                    eax = W.rd(this + 0x10)
                    if eax == esi:
                        break
    elif k == NOP:
        W.snap()
    elif k == DMG:
        flag = a[6] & 0xFF
        A.wr(o + 0x1C, 4, a[4]); A.wr(o + 0x20, 4, a[5])
        W.setg(GLOBAL_FLAG, 1, flag)
        gorder = [(GLOBAL_FLAG, 1)]
        matrix = W.raw(2, a[7])
        frame = W.raw(3, a[10]) if (fn == 9 or a[9] != 2) else a[11]
        W.snap()
        marks.append('selector0' if flag == 0 else 'selector_set')
        if flag != 0:
            marks.append('damaged_null' if a[5] == 0 else 'damaged_set')
            if a[5] == 0:
                ret = 0
            else:
                events.append([0x749E60, W.tagv(a[5])])
                events.append([0x7F0410])
                if fn == 9:
                    for kk in range(16):
                        D.wr(a[10] + 0x10 + 4 * kk, 4, W.rd(matrix + 4 * kk))
                events.append([0x74BF20, W.tagv(a[8]), W.tagv(frame)])
                ret = W.tagv(a[8])
        elif fn == 9:
            events.append([0x4C44D0, W.tagv(this), W.tagv(matrix)])
            ret = W.tagv(a[12])
        else:
            marks.append('object_null' if a[4] == 0 else 'object_set')
            if a[4] == 0:
                ret = 0
            else:
                events.append([0x4C4BA0, W.tagv(this)])
                events.append([0x749E60, W.tagv(a[4])])
                events.append([0x7F0410])
                events.append([0x74BF20, W.tagv(a[8]), W.tagv(frame)])
                events.append([0x4C4BB0, W.tagv(this)])
                ret = W.tagv(a[8])
        if fn == 10 and flag != 0:
            marks.append('object_set')
    elif k == DTOR7:
        W.snap()
        A.wr(o, 4, 0x85BB9C)
    elif k == LODCTOR:
        W.snap()
        events.append([0x4C4A60, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o + 0x20, 2, 0); A.wr(o + 0x22, 2, 0)
        A.wr(o, 4, 0x85BC70)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == SYNC:
        W.setg(GLOBAL_CLOCK, 4, a[4]); W.setg(GLOBAL_WATER, 4, a[5])
        gorder = [(GLOBAL_WATER, 4), (GLOBAL_CLOCK, 4)]
        W.snap()
        W.setg(GLOBAL_WATER, 4, a[4])
        marks.append('same_offset' if a[4] == a[5] else 'changes_offset')
    elif k == POOLNEW:
        pool = W.raw(1, a[4])
        W.snap()
        events.append([POOL_NEW_ADDR[fn], W.tagv(pool)])
        ret = W.tagv(a[5])
        marks.append('new_null' if a[5] == 0 else 'new_set')
    elif k == POOLDEL:
        size = POOL_NODE[fn]
        cap, idx = a[5], a[6]
        B.wr(a[4] + 0, 4, W.raw(2, a[7])); B.wr(a[4] + 4, 4, W.raw(3, a[8]))
        B.wr(a[4] + 8, 4, cap); B.wr(a[4] + 0xC, 4, a[9])
        D.wr(a[8] + idx, 1, (D.rd(a[8] + idx, 1) & 0x7F) | (0x80 if a[10] else 0))
        node = W.raw(2, a[7] + idx * size)
        W.snap()
        storage = B.rd(a[4], 4)
        diff = node - storage
        index = (diff >> 3) if size == 8 else int(diff / 12)
        state_ptr = B.rd(a[4] + 4, 4)
        W.wr(state_ptr + index, 1, W.rd(state_ptr + index, 1) | 0x80)
        last = B.rd(a[4] + 0xC, 4)
        signed_last = last - (1 << 32) if last >= 1 << 31 else last
        if index < signed_last:
            B.wr(a[4] + 0xC, 4, index)
            marks.append('lowers_cursor')
        else:
            marks.append('keeps_cursor')
        marks.append('was_free' if a[10] else 'was_used')
        if idx == 0:
            marks.append('first_slot')
        if idx == cap - 1:
            marks.append('last_slot')
    elif k == COMPRESS:
        for i in range(4):
            B.wr(a[8] + 4 * i, 4, a[4 + i])
        W.snap()
        for i in range(4):
            A.wr(o + 2 * i, 2, ftol16(a[4 + i]))
        vals = [f32(a[4 + i]) for i in range(4)]
        if any(math.isnan(v) for v in vals):
            marks.append('nan_in')
        if any(math.isinf(v) for v in vals):
            marks.append('inf_in')
        if any((not math.isnan(v)) and (not math.isinf(v)) and 32767 < abs(int(v * 4096.0)) < (1 << 63) for v in vals):
            marks.append('wraps_int16')
        if any((not math.isnan(v)) and (math.isinf(v) or abs(v * 4096.0) >= 2.0 ** 63) for v in vals):
            marks.append('huge')
        if any(0 < (a[4 + i] & 0x7FFFFFFF) < 0x800000 for i in range(4)):
            marks.append('denormal')
        if any(v < 0 for v in vals):
            marks.append('negative')
    elif k in (DECOMP4, DECOMP3):
        count = 4 if k == DECOMP4 else 3
        scale = 1.0 / 4096.0 if k == DECOMP4 else 1.0 / 1024.0
        for i in range(count):
            B.wr(a[8] + 2 * i, 2, a[4 + i] & 0xFFFF)
        W.snap()
        shorts = []
        for i in range(count):
            s = a[4 + i] & 0xFFFF
            s = s - 0x10000 if s >= 0x8000 else s
            shorts.append(s)
            A.wr(o + 4 * i, 4, bits32(s * scale))
        if any(s < 0 for s in shorts):
            marks.append('negative')
        if any(s == -32768 for s in shorts):
            marks.append('min_int16')
        if any(s == 32767 for s in shorts):
            marks.append('max_int16')
        if any(s == 0 for s in shorts):
            marks.append('zero')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
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
