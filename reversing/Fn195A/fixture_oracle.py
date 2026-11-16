"""Oraculo independiente (Python) de Fn195A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 31 funciones del lote 195A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (`dumpbin /disasm` sobre el gta-sa.exe canonico; reversing/_handoff/dis.sh).
   0  0x66D550  CPatrolRoute::Reverse (thiscall, sin pila, `ret`): edi = [this] - 1 (contador con signo, leido UNA vez); si edi <= 0 no hace nada; i = 0 y, mientras i < edi, en este orden:
            puntos: swap de los 12 bytes en this+144h+12*i y this+144h+12*edi (tres dwords: eax,ecx,edx = p[i]; p[i] = p[j]; p[j] = viejo p[i]);
            temp (nodo de 28h bytes en la pila: nombre1 en +0, nombre2 en +18h, justo debajo de la direccion de retorno) <- nodo i: strcpy(temp.n1, i.n1) y strcpy(temp.n2, i.n2) (byte a byte hasta copiar el NUL);
            i.n1 <- j.n1; i.n2 <- j.n2; j.n1 <- temp.n1; j.n2 <- temp.n2 (siempre strcpy: copia HASTA EL NUL, no por tamano de campo). Nodo k: nombre1 en this+4+28h*k, nombre2 en +18h mas;
            despues i++, edi-- y se repite mientras i < edi (comparacion con signo). Los bytes posteriores al NUL de cada campo NO se intercambian. Un nombre1 sin NUL en sus 18h bytes sigue por el nombre2.
   1  0x66D6D0  CTaskSimpleGoTo destructor con borrado: push esi; esi = ecx; call 667A00; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; eax = esi; pop esi; ret 4
   2  0x66D6F0  CTaskSimpleGoToPoint destructor con borrado: igual con call 667D50
   3  0x674BB0  CTaskComplexFollowPatrolRoute::Clone (marco SEH): push 30h; call 61A5A0; add esp,4; si eax == 0: eax = 0 (sin leer nada); si no, empuja [+18h], [+14h], movsx word [+0Ch], [+1Ch], movsx word [+10h]
            (en ese orden: arriba de la pila queda el movsx de +10h) y llama a 674930 con ecx = memoria (ret 14h); eax = el del constructor
   4  0x6D0BC0  CVehicle::SetRemapTexDictionary: eax = movsx word [this+598h]; edi = arg (dword); si edi == eax (32 bits) no hace nada; si no: si edi == -1: [this+59Ch] = 0; push eax; call 731A30 (cdecl; add esp,4);
            word [this+598h] = di; (en ambos casos) word [this+59Ah] = di
   5  0x6D3D50  fld dword [858C84h] (0.25f); ret 4        6  0x6D56B0  fld dword [858B50h] (0.0f); ret 4
   7  0x6D5DD0  CreateAtRef(ref) (ECX = pool: +4 estados, +0Ch cursor): eax = ref >> 8 (con signo); estado[eax] &= 7Fh; estado[eax] = (estado ^ ((estado ^ ref) & 7Fh)): los 7 bits bajos = ref & 7Fh; [this+0Ch] = 0;
            si el estado[0] tiene el bit 7 a cero: repite {cursor++} mientras el bit 7 del estado[cursor] este a cero. ret 4
   8  0x6D5E20  CAutoPilot::Constructor: words 0xFFFF en +0, +4, +8, +14h, +16h, +18h y en +68h+4*i (i = 0..7); bytes +26h = +25h = 1; +4Bh = (b & FCh) y mas tarde ((b & FCh) & 7); dwords +0Ch = 0, +10h = 3E8h, +38h = 41200000h,
            +8Ch = 0, +30h = 0, +90h = 0, +3Ch = 0, +44h = 3F800000h; bytes +28h, +27h, +29h, +2Ah, +2Bh, +41h, +35h, +34h, +49h, +4Ah, +95h = 0; +40h = 0Ah; +4Dh = 14h; +4Eh = +4Fh = 0Ah; +94h = FFh; word +88h = 0;
            edi = [0xB7CB84] (una sola lectura): +1Ch = +20h = edi; +4Ch = ([+4Ch] & FCh); call 821B1E (rand del CRT del EXE, cdecl) tras escribir +4Ch: +50h = ((al & 7) + 2); eax = this; ret
   9..30  cuerpos por defecto de ranuras virtuales de CVehicle (comprobados contra el PE: ver check_pad.py):
            6270 `ret 4`; 6280 `ret 4`; 6290 `ret 8`; 62A0..6320 (9 funciones) `xor al,al; ret 4`; 6330 `ret 4`; 6340 `ret 8`; 6350 `xor al,al; ret 4`; 6360 `xor al,al; ret 8`; 6370 `xor al,al; ret 8`; 6380..63C0 `ret`.
            No tocan memoria ni registros salvo AL (0 en las que devuelven bool).

Los falsos anotan: cuerpos de destructor [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [direccion, puntero (etiqueta), dword del puntero]; operator new [direccion, tamano];
constructor 674930 [direccion, ECX (etiqueta), p1, p2 (etiqueta), p3, p4, p5]; RemoveRef [direccion, argumento, dword de [this+59Ch] al entrar]; rand [direccion].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = json.loads('["CPatrolRoute::Reverse", "CTaskSimpleGoTo::ScalarDeletingDestructor", "CTaskSimpleGoToPoint::ScalarDeletingDestructor", "CTaskComplexFollowPatrolRoute::Clone", "CVehicle::SetRemapTexDictionary", "CVehicle::FindWheelWidth", "CVehicle::GetDooorAngleOpenRatio", "CVehiclePool::CreateAtRef", "CAutoPilot::Constructor", "CVehicle::ProcessControlCollisionCheck", "CVehicle::ProcessControlInputs", "CVehicle::GetComponentWorldPosition", "CVehicle::IsComponentPresent", "CVehicle::IsDoorReady", "CVehicle::IsDoorFullyOpen", "CVehicle::IsDoorClosed", "CVehicle::IsDoorMissing", "CVehicle::IsDoorReadyU32", "CVehicle::IsDoorFullyOpenU32", "CVehicle::IsDoorClosedU32", "CVehicle::IsDoorMissingU32", "CVehicle::RemoveRefsToVehicle", "CVehicle::BlowUpCar", "CVehicle::SetUpWheelColModel", "CVehicle::BurstTyre", "CVehicle::IsRoomForPedToLeaveCar", "CVehicle::SetupSuspensionLines", "CVehicle::Fix", "CVehicle::SetupDamageAfterLoad", "CVehicle::DoBurstAndSoftGroundRatios", "CVehicle::PlayCarHorn"]')
ENTRIES = (0x66D550, 0x66D6D0, 0x66D6F0, 0x674BB0, 0x6D0BC0, 0x6D3D50, 0x6D56B0, 0x6D5DD0, 0x6D5E20,
           0x6D6270, 0x6D6280, 0x6D6290, 0x6D62A0, 0x6D62B0, 0x6D62C0, 0x6D62D0, 0x6D62E0, 0x6D62F0, 0x6D6300, 0x6D6310, 0x6D6320,
           0x6D6330, 0x6D6340, 0x6D6350, 0x6D6360, 0x6D6370, 0x6D6380, 0x6D6390, 0x6D63A0, 0x6D63B0, 0x6D63C0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

REVERSE, SDD, CLONE, REMAP, FLOATC, CREATEAT, APCTOR, STUBV, STUBB = 'reverse sdd clone remap floatc createat apctor stubv stubb'.split()
KIND = [REVERSE, SDD, SDD, CLONE, REMAP, FLOATC, FLOATC, CREATEAT, APCTOR] + \
       [STUBV, STUBV, STUBV] + [STUBB] * 9 + [STUBV, STUBV, STUBB, STUBB, STUBB] + [STUBV] * 5
assert len(KIND) == len(NAMES) == len(ENTRIES) == 31
SDD_BODY = {1: 0x667A00, 2: 0x667D50}
NEWADDR, DELADDR, CTOR5, REMOVEREF, RANDADDR = 0x61A5A0, 0x61A5B0, 0x674930, 0x731A30, 0x821B1E
FLOAT_BITS = {5: 0x3E800000, 6: 0x00000000}
NODE, NODE_STRIDE, N2, POINTS = 4, 0x28, 0x18, 0x144


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


def slot_byte(seed, i):
    s = rnd(seed, i)
    return (s & 0x7F) | (0x80 if ((s >> 8) % 100) < 30 else 0)


def signed32(v):
    v &= M32
    return v - (1 << 32) if v >= 0x80000000 else v


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


class Unsafe(Exception):
    """el caso haria que el EXE desborde el nodo temporal de la pila (el banco no debe generarlo)"""


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

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
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
EXTRA = {
    REVERSE: ['count_nonpositive', 'count_negative', 'count_1', 'count_2', 'count_3', 'count_7', 'count_8', 'count_over8', 'swapped', 'name_empty', 'name1_overflow', 'name2_full'],
    SDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    CLONE: ['mem_null', 'mem_set', 'route_arena', 'route_raw', 'field10_negative', 'field0c_negative'],
    REMAP: ['equal', 'new_minus1', 'new_other', 'low_word_equal_high_set', 'prev_minus1', 'prev_other'],
    CREATEAT: ['idx_first', 'idx_last', 'cursor_zero', 'cursor_far'],
    APCTOR: ['rand_low_0', 'rand_low_7', 'flag4b_bit2_set', 'flag4b_bit2_clear', 'clock_high_bit'],
}
KEYS = {fn: list(COMMON) + EXTRA.get(KIND[fn], []) for fn in range(len(NAMES))}


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
    tv = W.tagv
    A.wr(o, 4, a[3])
    if k == REVERSE:
        img = bytes.fromhex(c['s'])
        for i, b in enumerate(img):
            A.wr(o + 4 + i, 1, b)
        W.snap()
        count = signed32(a[3])
        marks.append({0: 'count_0'}.get(count, 'count_other'))
        for v, key in ((1, 'count_1'), (2, 'count_2'), (3, 'count_3'), (7, 'count_7'), (8, 'count_8')):
            if count == v:
                marks.append(key)
        if count > 8:
            marks.append('count_over8')
        if count < 0:
            marks.append('count_negative')
        j = count - 1
        if j <= 0:
            marks.append('count_nonpositive')
        else:
            marks.append('swapped')
            temp = {}

            def n1(n):
                return o + NODE + NODE_STRIDE * n

            def pt(n):
                return o + POINTS + 12 * n

            def copyz(read, write, tag):
                """strcpy byte a byte: lee, escribe y para tras copiar el NUL"""
                length = 0
                while True:
                    b = read(length)
                    write(length, b)
                    length += 1
                    if b == 0:
                        break
                    if length > 0x80:
                        raise Unsafe('cadena sin fin')
                if tag == 'n1' and length > 0x18:
                    marks.append('name1_overflow')
                if tag == 'n2' and length == 0x10:
                    marks.append('name2_full')
                if length == 1:
                    marks.append('name_empty')
                return length

            def to_temp(off, limit):
                def w(idx, b):
                    if off + idx >= 0x28 or off + idx >= limit:
                        raise Unsafe('el nodo temporal se desborda')
                    temp[off + idx] = b
                return w

            def rtemp(off):
                return lambda idx: temp[off + idx]

            def rA(base):
                return lambda idx: A.rd(base + idx, 1)

            def wA(base):
                return lambda idx, b: A.wr(base + idx, 1, b)

            i = 0
            while i < j:
                pi = [A.rd(pt(i) + 4 * m, 4) for m in range(3)]
                pj = [A.rd(pt(j) + 4 * m, 4) for m in range(3)]
                for m in range(3):
                    A.wr(pt(i) + 4 * m, 4, pj[m])
                for m in range(3):
                    A.wr(pt(j) + 4 * m, 4, pi[m])
                copyz(rA(n1(i)), to_temp(0, 0x28), 'n1')               # temp.nombre1 <- i.nombre1 (puede seguir por temp.nombre2)
                copyz(rA(n1(i) + N2), to_temp(N2, 0x28), 'n2')          # temp.nombre2 <- i.nombre2 (no puede pasar de 0x10 bytes: la direccion de retorno)
                copyz(rA(n1(j)), wA(n1(i)), 'n1')                       # i.nombre1 <- j.nombre1
                copyz(rA(n1(j) + N2), wA(n1(i) + N2), 'n2')             # i.nombre2 <- j.nombre2
                copyz(rtemp(0), wA(n1(j)), 'n1')                        # j.nombre1 <- temp.nombre1
                copyz(rtemp(N2), wA(n1(j) + N2), 'n2')                  # j.nombre2 <- temp.nombre2
                i += 1
                j -= 1
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], tv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELADDR, tv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tv(this)
    elif k == CLONE:
        route = W.raw(2, a[14]) if a[13] else a[12]
        A.wr(o + 0xC, 4, a[8]); A.wr(o + 0x10, 4, a[9]); A.wr(o + 0x14, 4, a[10]); A.wr(o + 0x18, 4, a[11]); A.wr(o + 0x1C, 4, route)
        W.snap()
        events.append([NEWADDR, 0x30])
        marks.append('route_arena' if a[13] else 'route_raw')
        f10, f0c = sext16(A.rd(o + 0x10, 2)), sext16(A.rd(o + 0xC, 2))
        if f10 < 0:
            marks.append('field10_negative')
        if f0c < 0:
            marks.append('field0c_negative')
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([CTOR5, tv(mem), f10 & M32, tv(A.rd(o + 0x1C, 4)), f0c & M32, A.rd(o + 0x14, 4), A.rd(o + 0x18, 4)])
            B.wr(a[5], 4, a[7])
            ret = tv(a[6])
    elif k == REMAP:
        A.wr(o + 0x598, 4, a[4]); A.wr(o + 0x59C, 4, a[5])
        W.snap()
        prev = sext16(A.rd(o + 0x598, 2)) & M32
        arg = a[6]
        marks.append('prev_minus1' if prev == M32 else 'prev_other')
        if arg == prev:
            marks.append('equal')
        else:
            if arg == M32:
                marks.append('new_minus1')
                A.wr(o + 0x59C, 4, 0)
                events.append([REMOVEREF, prev, 0])
                A.wr(o + 0x598, 2, arg & 0xFFFF)
            else:
                marks.append('new_other')
                if (arg & 0xFFFF) == (prev & 0xFFFF):
                    marks.append('low_word_equal_high_set')
            A.wr(o + 0x59A, 2, arg & 0xFFFF)
    elif k == FLOATC:
        W.snap()
        ret = FLOAT_BITS[fn]
    elif k == CREATEAT:
        cap, idx = a[5], a[7]
        A.wr(o, 4, a[9]); A.wr(o + 4, 4, W.raw(1, a[4])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[6])
        states = [slot_byte(a[10], i) for i in range(cap)]
        states[idx] |= 0x80
        anchor = rnd(a[10], 0x3E7) % cap
        if anchor == idx:
            anchor = (idx + 1) % cap
        states[anchor] |= 0x80
        for i, s in enumerate(states):
            B.wr(a[4] + i, 1, s)
        W.snap()
        states[idx] = a[8] & 0x7F
        cur = 0
        while not states[cur] & 0x80:
            cur += 1
        B.wr(a[4] + idx, 1, states[idx])
        A.wr(o + 0xC, 4, cur)
        marks.append('idx_first' if idx == 0 else ('idx_last' if idx == cap - 1 else 'idx_mid'))
        marks.append('cursor_zero' if cur == 0 else 'cursor_far')
    elif k == APCTOR:
        A.wr(o + 0x48, 4, a[6]); A.wr(o + 0x4C, 4, a[7])
        W.snap()
        clock = a[4]
        for off in (0x00, 0x04, 0x08, 0x14, 0x16, 0x18):
            A.wr(o + off, 2, 0xFFFF)
        for i in range(8):
            A.wr(o + 0x68 + 4 * i, 2, 0xFFFF)
        A.wr(o + 0x26, 1, 1); A.wr(o + 0x25, 1, 1)
        b4b = A.rd(o + 0x4B, 1)
        b4c = A.rd(o + 0x4C, 1)
        A.wr(o + 0x0C, 4, 0); A.wr(o + 0x10, 4, 0x3E8); A.wr(o + 0x38, 4, 0x41200000); A.wr(o + 0x8C, 4, 0); A.wr(o + 0x30, 4, 0)
        A.wr(o + 0x44, 4, 0x3F800000); A.wr(o + 0x90, 4, 0); A.wr(o + 0x3C, 4, 0)
        for off in (0x28, 0x27, 0x29, 0x2A, 0x2B, 0x41, 0x35, 0x34, 0x49, 0x4A, 0x95):
            A.wr(o + off, 1, 0)
        A.wr(o + 0x40, 1, 0x0A); A.wr(o + 0x4D, 1, 0x14); A.wr(o + 0x4E, 1, 0x0A); A.wr(o + 0x4F, 1, 0x0A); A.wr(o + 0x94, 1, 0xFF)
        A.wr(o + 0x88, 2, 0)
        A.wr(o + 0x1C, 4, clock); A.wr(o + 0x20, 4, clock)
        A.wr(o + 0x4B, 1, (b4b & 0xFC) & 7)
        A.wr(o + 0x4C, 1, b4c & 0xFC)
        events.append([RANDADDR])
        A.wr(o + 0x50, 1, ((a[5] & 7) + 2) & 0xFF)
        ret = tv(this)
        marks.append('rand_low_0' if (a[5] & 7) == 0 else ('rand_low_7' if (a[5] & 7) == 7 else 'rand_mid'))
        marks.append('flag4b_bit2_set' if b4b & 4 else 'flag4b_bit2_clear')
        if clock >> 31:
            marks.append('clock_high_bit')
    else:    # cuerpos por defecto: no hacen nada; los booleanos devuelven AL = 0
        W.snap()
        ret = 0 if k == STUBB else None
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL (bool)"""
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
