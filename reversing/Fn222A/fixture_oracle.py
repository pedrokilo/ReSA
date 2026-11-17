"""Oraculo independiente (Python) de Fn222A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 16 funciones del lote 222A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh). Las lecturas y escrituras a memoria se modelan EN EL ORDEN del EXE (importa cuando la tabla de
cifrado se solapa con el bloque, o el nodo nuevo con la lista):
   0  0x4032A4  push ebp; mov ebp,esp; push ebx/esi/edi; jmp +0; pop edi/esi/ebx; leave; ret 4 (nada)
   1  0x403638  cifrado: t=[S0]; [B+4] = [B+4] + t; u=[B]; [B+4] escrito; [B] = u + [S1] (S1 leido DESPUES de escribir B+4); 12 rondas: a=[B], b=[B+4] (leidos de memoria); b' = rotl(a^b, (a>>4)&31) - [S+8+16i... S[2+2i]];
             [B+4] = b'; a' = rotl(b'^a, (b'>>7)&31) + S[3+2i] (leida tras escribir B+4); [B] = a'
   2  0x4036C1  descifrado: 12 rondas: b=[B+4], a=[B]; a' = rotr(a - S[25-2i], (b>>7)&31) ^ b; [B] = a'; b' = rotr(b + S[24-2i] (leida tras escribir B), (a'>>4)&31) ^ a'; [B+4] = b';
             al final: x=[B]; [B] = x - S[1]; [B+4] = b' - S[0] (S[0] leida tras escribir B)
   3  0x403E00  push esi; esi=ecx; call 0x532A90; [esi+0x36] = ([esi+0x36] & 0xF9) | 1 (byte); [esi+0x1C] |= 1 (dword); [esi] = 0x8585C8; eax = esi
   4, 5  0x447000, 0x447050  [ecx] = 0x859988; ecx += 8; jmp 0x4EF660 (CAESound::~CAESound sobre this + 8)
   6  0x463490  if byte [ecx+0x2A] not in (0x39, 0x3A): byte [ecx+0x2A] = byte [esp+4]; ret 4
   7  0x4634D0  41 veces (esi = this + 0x3C*i): ecx = esi; call 0x600530
   8  0x46A2F0  eax = [ecx+0x1C]; AL = 1 si bit 9 o bit 7 apagado; si no 0
   9  0x46A340  copia byte a byte (hasta el cero inclusive) de la primera cadena a this+0 y despues de la segunda a this+0x18; devuelve this
  10  0x533020  sub esp,0x10; esi=ecx; call [[esi]+0x24](ecx=esi, &rect); call [[esi]+4](ecx=esi, &rect) (la vtable se vuelve a leer del objeto)
  11  0x5335E0  nodo = operator new(8) (0x552380); [nodo] = item; [nodo+4] = [this]; [this] = nodo; devuelve nodo
  12  0x533610  busqueda desde [this] (guarda el anterior); si es la cabeza [this] = [nodo+4]; si no y hay anterior [anterior+4] = [nodo+4]; operator delete(nodo) (0x552390)
  13  0x533670  nodo = operator new(12) (0x5523C0); [nodo] = item; [nodo+8] = 0; [nodo+4] = [this]; si [this] != 0: [[this]+8] = nodo (cabeza releida); [this] = nodo; devuelve nodo
  14  0x5336B0  busqueda desde [this]; si es la cabeza [this] = [nodo+4]; si hay anterior (+8): [anterior+4] = [nodo+4]; si hay siguiente (+4): [siguiente+8] = [nodo+8]; operator delete (0x5523D0)
  15  0x536C20  push esi; esi=ecx; call 0x535E90 (CEntity::~CEntity); si byte [esp+8] & 1: operator delete(esi) (0x8214BD); eax = esi

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CProtectionStub::Empty", "CProtectionStub::BlockEncrypt", "CProtectionStub::BlockDecrypt", "CBuilding::Constructor", "CAEAudioEntity::Destructor-6", "CAEAudioEntity::Destructor-7",
         "CAutoPilot::SetCarMissionUnlessCrashAndBurn", "CDecisionMaker::SetDefault", "CEntity::IsInvisibleOrRenderDamaged", "CTheScripts::ScriptTextPairSet", "CEntity::Add-void",
         "CPtrNodeSingleLinkPool::AddItem", "CPtrNodeSingleLinkPool::RemoveItem", "CPtrNodeDoubleLinkPool::AddItem", "CPtrNodeDoubleLinkPool::RemoveItem", "CEntity::ScalarDeletingDestructor"]
ENTRIES = (0x4032A4, 0x403638, 0x4036C1, 0x403E00, 0x447000, 0x447050, 0x463490, 0x4634D0, 0x46A2F0, 0x46A340, 0x533020, 0x5335E0, 0x533610, 0x533670, 0x5336B0, 0x536C20)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(STDEMPTY, ENC, DEC, BLDCTOR, AUDDTOR, MISSION, DECISION, INVIS, TEXTPAIR, ENTADD, SADD, SREM, DADD, DREM, SCALARDEL) = \
    'stdempty enc dec bldctor auddtor mission decision invis textpair entadd sadd srem dadd drem scalardel'.split()
KIND = [STDEMPTY, ENC, DEC, BLDCTOR, AUDDTOR, AUDDTOR, MISSION, DECISION, INVIS, TEXTPAIR, ENTADD, SADD, SREM, DADD, DREM, SCALARDEL]
RET_TAG = {BLDCTOR, TEXTPAIR, SADD, DADD, SCALARDEL}
RET_AL = {INVIS}
STACK_TAG = 0x7F000000
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


def item_value(seed, mode, i):
    if mode == 1:
        return 0x887D00 + (rnd(seed, i) % 3) * 8
    if rnd(seed, 200 + i) % 6 == 0:
        return 0
    return 0x10000000 + (rnd(seed, 100 + i) & 0x00FFFFF0)


def str_char(seed, i):
    return 1 + rnd(seed, 500 + i) % 255


def rotl(x, n):
    n &= 31
    x &= M32
    return ((x << n) | (x >> (32 - n))) & M32 if n else x


def rotr(x, n):
    return rotl(x, (32 - (n & 31)) & 31)


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

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    STDEMPTY: [],
    ENC: ['key_b', 'key_b_unaligned', 'key_overlap', 'key_after', 'rot0'],
    DEC: ['key_b', 'key_b_unaligned', 'key_overlap', 'key_after', 'rot0'],
    BLDCTOR: [], AUDDTOR: [], DECISION: [],
    MISSION: ['cur_plane', 'cur_heli', 'cur_other', 'arg_high_bits'],
    INVIS: ['ret_damaged', 'ret_invisible', 'ret_false'],
    TEXTPAIR: ['s1_b', 's1_c', 's1_a', 's2_ext', 's2_a', 's2_reads_first', 'long1', 'long2', 'empty1', 'empty2'],
    ENTADD: ['swap', 'noswap'],
    SADD: ['head_null', 'head_node', 'head_this', 'node_overlap', 'node_is_head'],
    DADD: ['head_null', 'head_node', 'head_this', 'node_overlap', 'node_is_head'],
    SREM: ['empty', 'found_head', 'found_mid', 'found_tail', 'absent', 'dup', 'zero_item'],
    DREM: ['empty', 'found_head', 'found_mid', 'found_tail', 'absent', 'dup', 'zero_item', 'prev_null', 'prev_junk'],
    SCALARDEL: ['delete', 'nodelete'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def put_string(W, addr, length, seed):
    for i in range(length):
        W.wr(addr + i, 1, str_char(seed, i))
    W.wr(addr + length, 1, 0)


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
    tg = W.tagv

    if k == STDEMPTY:
        W.snap()
    elif k in (ENC, DEC):
        mode = a[4]
        key = W.raw(1, a[5]) if mode in (0, 1) else W.raw(0, a[5])
        blk = this
        marks.append(['key_b', 'key_b_unaligned', 'key_overlap', 'key_after'][mode])
        W.snap()
        S = lambda i: W.rd(key + 4 * i)
        zero_rot = False
        if k == ENC:
            t = S(0)
            edx = (W.rd(blk + 4) + t) & M32
            ecx = W.rd(blk)
            W.wr(blk + 4, 4, edx)
            ecx = (ecx + S(1)) & M32
            W.wr(blk, 4, ecx)
            for i in range(12):
                av = W.rd(blk)
                bv = W.rd(blk + 4)
                n1 = (av >> 4) & 31
                zero_rot |= n1 == 0
                bn = (rotl(av ^ bv, n1) - S(2 + 2 * i)) & M32
                W.wr(blk + 4, 4, bn)
                n2 = (bn >> 7) & 31
                zero_rot |= n2 == 0
                an = (rotl(bn ^ av, n2) + S(3 + 2 * i)) & M32
                W.wr(blk, 4, an)
        else:
            bv = 0
            for i in range(12):
                bv = W.rd(blk + 4)
                av = W.rd(blk)
                n1 = (bv >> 7) & 31
                zero_rot |= n1 == 0
                an = rotr((av - S(25 - 2 * i)) & M32, n1) ^ bv
                W.wr(blk, 4, an)
                n2 = (an >> 4) & 31
                zero_rot |= n2 == 0
                bn = rotr((bv + S(24 - 2 * i)) & M32, n2) ^ an
                W.wr(blk + 4, 4, bn)
                bv = bn
            x = W.rd(blk)
            W.wr(blk, 4, (x - S(1)) & M32)
            W.wr(blk + 4, 4, (bv - S(0)) & M32)
        if zero_rot:
            marks.append('rot0')
    elif k == BLDCTOR:
        W.snap()
        events.append([0x532A90, tg(this)])
        W.wr(this, 4, a[4])
        W.wr(this + 0x1C, 4, a[5])
        W.wr(this + 0x36, 1, a[6] & 0xFF)
        W.wr(this + 0x36, 1, (W.rd(this + 0x36, 1) & 0xF9) | 1)
        W.wr(this + 0x1C, 4, W.rd(this + 0x1C) | 1)
        W.wr(this, 4, 0x8585C8)
        ret = tg(this)
    elif k == AUDDTOR:
        W.snap()
        W.wr(this, 4, 0x859988)
        events.append([0x4EF660, tg(this + 8)])
        W.wr(this + 8, 4, a[4])
    elif k == MISSION:
        W.wr(this + 0x2A, 1, a[4] & 0xFF)
        W.snap()
        cur = a[4] & 0xFF
        marks.append('cur_plane' if cur == 0x39 else ('cur_heli' if cur == 0x3A else 'cur_other'))
        if a[5] > 0xFF:
            marks.append('arg_high_bits')
        if cur not in (0x39, 0x3A):
            W.wr(this + 0x2A, 1, a[5] & 0xFF)
    elif k == DECISION:
        W.snap()
        for i in range(41):
            events.append([0x600530, tg(this + 0x3C * i)])
    elif k == INVIS:
        W.wr(this + 0x1C, 4, a[4])
        W.snap()
        f = a[4]
        r = 1 if (f & 0x200) or not (f & 0x80) else 0
        ret = r
        marks.append('ret_damaged' if f & 0x200 else ('ret_invisible' if r else 'ret_false'))
    elif k == TEXTPAIR:
        L1, L2, m1, off1, m2, off2, s1, s2c, s2 = a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]
        if m1 == 0:
            src1 = W.raw(1, off1); marks.append('s1_b')
        elif m1 == 1:
            src1 = W.raw(2, off1); marks.append('s1_c')
        else:
            src1 = (this + off1) & M32; marks.append('s1_a')
        put_string(W, src1, L1, s1)
        if m2 == 0:
            ar = 2 if m1 == 0 else (1 if m1 == 1 else (2 if s2c else 1))
            src2 = W.raw(ar, off2)
            put_string(W, src2, L2, s2)
            marks.append('s2_ext')
        elif m2 == 2:
            src2 = (this + 0x18 + off2) & M32
            put_string(W, src2, L2, s2)
            marks.append('s2_a')
        else:
            src2 = (this + off2) & M32
            marks.append('s2_reads_first')
        W.snap()
        steps = 0
        for s_addr, d_addr in ((src1, this), (src2, (this + 0x18) & M32)):
            while True:
                ch = W.rd(s_addr, 1)
                W.wr(d_addr, 1, ch)
                s_addr = (s_addr + 1) & M32
                d_addr = (d_addr + 1) & M32
                steps += 1
                if steps > 0x400:
                    raise RuntimeError('copia sin fin en el caso')
                if ch == 0:
                    break
        ret = tg(this)
        if L1 >= 0x18:
            marks.append('long1')
        if m2 != 3 and L2 >= 0x18:
            marks.append('long2')
        if L1 == 0:
            marks.append('empty1')
        if m2 != 3 and L2 == 0:
            marks.append('empty2')
    elif k == ENTADD:
        vt1, vt2 = W.raw(1, a[5]), W.raw(2, a[6])
        W.wr(this, 4, vt1)
        W.snap()
        events.append([0xB001, tg(this), STACK_TAG])
        rect = a[7:11]
        if a[4] == 1:
            W.wr(this, 4, vt2)
            marks.append('swap')
            events.append([0xC002, tg(this), STACK_TAG] + rect)
        else:
            marks.append('noswap')
            events.append([0xC001, tg(this), STACK_TAG] + rect)
    elif k in (SADD, DADD):
        head = [0, W.raw(2, a[5]), this, (this + 4) & M32, (this - 4) & M32][a[4]]
        W.wr(this, 4, head)
        mode = a[6]
        if mode in (0, 1):
            node = W.raw(3, a[7])
        elif mode == 2:
            node = this
        elif mode == 3:
            node = (this - 4) & M32
        elif mode == 4:
            node = (this - 8) & M32
        elif mode == 5:
            node = (this + 4) & M32
        else:
            node = head if head else W.raw(3, a[7])
        W.snap()
        marks.append(['head_null', 'head_node', 'head_this', 'head_node', 'head_node'][a[4]])
        if mode in (2, 3, 4, 5):
            marks.append('node_overlap')
        if mode == 6 and head:
            marks.append('node_is_head')
        item = a[8] if a[9] == 0 else (this if a[9] == 1 else W.raw(2, a[5]))
        if k == SADD:
            events.append([0x552380, 8])
            W.wr(node, 4, item)
            edx = W.rd(this)
            W.wr(node + 4, 4, edx)
            W.wr(this, 4, node)
        else:
            events.append([0x5523C0, 12])
            W.wr(node, 4, item)
            W.wr(node + 8, 4, 0)
            edx = W.rd(this)
            W.wr(node + 4, 4, edx)
            ecx = W.rd(this)
            if ecx:
                W.wr(ecx + 8, 4, node)
            W.wr(this, 4, node)
        ret = tg(node)
    elif k in (SREM, DREM):
        L, base, sep, idx, imode, iseed, sm, incoh, junk = a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]
        nodes = [W.raw(1, base + sep * i) for i in range(L)]
        items = [item_value(iseed, imode, i) for i in range(L)]
        double = k == DREM
        for i in range(L):
            W.wr(nodes[i], 4, items[i])
            W.wr(nodes[i] + 4, 4, nodes[i + 1] if i + 1 < L else 0)
            if double:
                W.wr(nodes[i] + 8, 4, nodes[i - 1] if i > 0 else 0)
        if double and idx < L and incoh == 1:
            W.wr(nodes[idx] + 8, 4, 0)
        elif double and idx < L and incoh == 2:
            W.wr(nodes[idx] + 8, 4, W.raw(2, junk))
        W.wr(this, 4, nodes[0] if L else 0)
        if sm == 0:
            search = items[idx]
        elif sm == 1:
            search = 0xE0000000 | (iseed & 0x0FFFFFFF)
        else:
            search = 0
        W.snap()
        if L == 0:
            marks.append('empty')
        if imode == 1:
            marks.append('dup')
        if sm == 2:
            marks.append('zero_item')
        if double and incoh == 1:
            marks.append('prev_null')
        if double and incoh == 2:
            marks.append('prev_junk')
        first = W.rd(this)
        node = first
        prev = 0
        found = False
        while node:
            if W.rd(node) == search:
                found = True
                break
            prev = node
            node = W.rd(node + 4)
        if not first:
            pass
        elif not found:
            marks.append('absent')
        else:
            delete_addr = 0x5523D0 if double else 0x552390
            if not double:
                if first == node:
                    marks.append('found_head')
                    W.wr(this, 4, W.rd(node + 4))
                else:
                    marks.append('found_tail' if W.rd(node + 4) == 0 else 'found_mid')
                    if prev:
                        W.wr(prev + 4, 4, W.rd(node + 4))
                events.append([delete_addr, tg(node)])
            else:
                if first == node:
                    marks.append('found_head')
                    W.wr(this, 4, W.rd(node + 4))
                else:
                    marks.append('found_tail' if W.rd(node + 4) == 0 else 'found_mid')
                p = W.rd(node + 8)
                if p:
                    W.wr(p + 4, 4, W.rd(node + 4))
                nx = W.rd(node + 4)
                if nx:
                    W.wr(nx + 8, 4, W.rd(node + 8))
                events.append([delete_addr, tg(node)])
    elif k == SCALARDEL:
        W.snap()
        events.append([0x535E90, tg(this)])
        W.wr(this, 4, a[5])
        if a[4] & 1:
            events.append([EV_DELETE, tg(this)])
            marks.append('delete')
        else:
            marks.append('nodelete')
        ret = tg(this)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'threw': threw, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL"""
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
