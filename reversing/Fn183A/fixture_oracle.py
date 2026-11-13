"""Oraculo independiente (Python) de Fn183A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 19 funciones del lote 183A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.
La tabla de nombres de huesos sale de derive_bonenames.py (lectura directa del EXE original: tabla de bytes 0x4D5864, tabla de saltos 0x4D57EC y cadenas 0x85ED68..0x85EE9C).

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x4D40A0  CLinkList<CAnimBlendHierarchy*>::Shutdown: push esi; mov esi,ecx; push [esi+30h]; call 8214BDh (operator delete); add esp,4; mov [esi+30h],0; pop esi; ret
   1  0x4D40C0  Insert(const T&): eax = [ecx+20h]; si eax == ecx+24h devuelve 0 (ret 4); [eax] = **(esp+4); edx = [eax+8]; esi = [eax+4]; [edx+4] = esi; edx = [eax+4]; esi = [eax+8]; [edx+8] = esi;
            edx = [ecx+8]; [eax+8] = edx; edx = [ecx+8]; [edx+4] = eax; [eax+4] = ecx; [ecx+8] = eax; devuelve eax (los campos se RELEEN tras cada escritura)
   2  0x4D56F0  ConvertBoneTag2BoneName: eax = dword de pila; si eax > 0C9h (con signo): 12Dh -> cadena 85ED68, 12Eh -> 85ED74, el resto nulo; si eax == 0C9h -> 85ED80; si eax > 36h (sin signo, incluye los negativos) nulo; si no, tabla de saltos
   3  0x4D5980  CPad::IsEnterJustPressed: ([0B733CCh] word != 0 && [0B7315Ch] word == 0) || ([0B733EAh] != 0 && [0B7317Ah] == 0): AL = 1 / 0
   4  0x4D59B0  CPad::IsStandardKeyJustPressed: eax = movzx byte [esp+4]; eax *= 2; ([eax+0B731A8h] word != 0 && [eax+0B72F38h] word == 0): EAX = 1 / 0; ret 4; NO lee ecx
   5  0x4D59E0  CPad::IsCrossPressed: word [ecx+20h] != 0 && word [ecx+50h] == 0: EAX = 1 / 0
   6..13  destructores con borrado de las ocho tareas de golpe: push esi; mov esi,ecx; call <cuerpo = jmp 61A6F0>; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
  14..18  igual con cuerpo 62F360, 62F5D0, 62F650 (mov [ecx],86DD74h; jmp 61A3A0), 62F6D0 (mov [ecx],86DD98h; jmp 61A3A0) y 62FB40

Los falsos anotan: operator delete del CRT 0x8214BD [direccion, puntero (etiqueta), dword de [this+30h] al entrar]; cuerpos [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar];
CTask::operator delete 0x61A5B0 [direccion, puntero (etiqueta), dword de [puntero] al entrar].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CLinkList<CAnimBlendHierarchy*>::Shutdown", "CLinkList<CAnimBlendHierarchy*>::Insert", "RpAnimBlend::ConvertBoneTag2BoneName", "CPad::IsEnterJustPressed", "CPad::IsStandardKeyJustPressed",
         "CPad::IsCrossPressed", "CTaskSimpleHitFromFront::ScalarDeletingDestructor", "CTaskSimpleHitFromBack::ScalarDeletingDestructor", "CTaskSimpleHitFromLeft::ScalarDeletingDestructor",
         "CTaskSimpleHitFromRight::ScalarDeletingDestructor", "CTaskSimpleHitByGunFromFront::ScalarDeletingDestructor", "CTaskSimpleHitByGunFromRear::ScalarDeletingDestructor",
         "CTaskSimpleHitByGunFromLeft::ScalarDeletingDestructor", "CTaskSimpleHitByGunFromRight::ScalarDeletingDestructor", "CTaskSimpleStandStill::ScalarDeletingDestructor",
         "CTaskSimpleSetStayInSamePlace::ScalarDeletingDestructor", "CTaskSimpleSetKindaStayInSamePlace::ScalarDeletingDestructor", "CTaskSimpleDuckToggle::ScalarDeletingDestructor",
         "CTaskSimpleDie::ScalarDeletingDestructor"]
ENTRIES = (0x4D40A0, 0x4D40C0, 0x4D56F0, 0x4D5980, 0x4D59B0, 0x4D59E0, 0x637300, 0x637330, 0x637360, 0x637390, 0x6373C0, 0x6373F0, 0x637420, 0x637450, 0x637480, 0x6374A0, 0x6374C0, 0x6374E0, 0x637500)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SHUT, LINSERT, BONE, ENTER, KEY, CROSS, SDD = 'shut linsert bone enter key cross sdd'.split()
KIND = [SHUT, LINSERT, BONE, ENTER, KEY, CROSS] + [SDD] * 13
NEW_ENTER, OLD_ENTER, NEW_EXT, OLD_EXT = 0xB733CC, 0xB7315C, 0xB733EA, 0xB7317A
NEW_KEYS, OLD_KEYS, KEYS_BYTES = 0xB731A8, 0xB72F38, 0x200
DELETE_CRT, TASK_DELETE = 0x8214BD, 0x61A5B0
# cuerpo / colaborador de cada destructor con borrado y vtable que deja el cuerpo real (0 = la deja el falso)
SDD_BODY = {6: 0x61A6F0, 7: 0x61A6F0, 8: 0x61A6F0, 9: 0x61A6F0, 10: 0x61A6F0, 11: 0x61A6F0, 12: 0x61A6F0, 13: 0x61A6F0, 14: 0x62F360, 15: 0x62F5D0, 16: 0x61A3A0, 17: 0x61A3A0, 18: 0x62FB40}
SDD_VT = {16: 0x86DD74, 17: 0x86DD98}
# tabla de nombres de huesos (derive_bonenames.py sobre el EXE original)
BONES = {0: "Root", 1: "Pelvis", 2: "Spine", 3: "Spine1", 4: "Neck", 5: "Head", 6: "L Brow", 7: "R Brow", 8: "Jaw", 21: "Bip01 R Clavicle", 22: "R UpperArm", 23: "R Forearm", 24: "R Hand", 25: "R Fingers",
         26: "R Finger01", 31: "Bip01 L Clavicle", 32: "L UpperArm", 33: "L Forearm", 34: "L Hand", 35: "L Fingers", 36: "L Finger01", 41: "L Thigh", 42: "L Calf", 43: "L Foot", 44: "L Toe", 51: "R Thigh",
         52: "R Calf", 53: "R Foot", 54: "R Toe", 201: "Belly", 301: "R Breast", 302: "L Breast"}


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta; memoria global del juego aparte"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[60 + i]) for i in range(4))
        self.glob = {}
        self.glob_ref = {}

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

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def getg(self, addr, n=4):
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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == SHUT:
        _k += ['ptr_zero', 'ptr_nonzero', 'ptr_exe']
    elif _kind == LINSERT:
        _k += ['free0', 'free1', 'free2', 'free3', 'used0', 'used1', 'used2', 'twist0', 'twist1', 'twist2', 'twist3', 'twist4', 'data_zero', 'data_ptr']
    elif _kind == BONE:
        _k += ['named', 'null_gap', 'null_big', 'negative', 'tag_c9', 'tag_12d', 'tag_12e', 'high_bits', 'tag_0', 'tag_201']
    elif _kind == ENTER:
        _k += ['result_true', 'result_false', 'enter_pressed', 'enter_held', 'ext_pressed', 'ext_held', 'low_byte_zero_word']
    elif _kind == KEY:
        _k += ['result_true', 'result_false', 'new_zero', 'new_old_set', 'new_set_old_zero', 'junk_high', 'low_byte_zero_word', 'key_zero', 'key_ff']
    elif _kind == CROSS:
        _k += ['result_true', 'result_false', 'new_zero', 'new_old_set', 'low_byte_zero_word']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    KEYS[_fn] = _k


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


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

    def put(addr, off, v):
        W.wr((addr + off) & M32, 4, v)

    def get(addr, off):
        return W.rd((addr + off) & M32, 4)

    if k == SHUT:
        A.wr(o + 0x30, 4, a[4])
        W.snap()
        events.append([DELETE_CRT, W.tagv(a[4]), a[4]])    # el puntero y [this+30h] al entrar (aun el puntero viejo: se libera ANTES de anular el campo)
        A.wr(o + 0x30, 4, 0)
        marks.append('ptr_zero' if a[4] == 0 else 'ptr_nonzero')
        if 0x400000 <= a[4] < 0xC00000:
            marks.append('ptr_exe')
    elif k == LINSERT:
        free, used, nb, dc, data = a[4], a[5], a[6], a[7], a[8]
        usedHead, usedTail, freeHead, freeTail = this, (this + 0xC) & M32, (this + 0x18) & M32, (this + 0x24) & M32
        node = [W.raw(1, nb + 0xC * j) for j in range(5)]
        uses = node[:used]
        frees = node[used:used + free]
        for j in range(5):
            B.wr(nb + 0xC * j, 4, a[9 + j])                            # data de cada enlace
        put(usedHead, 8, uses[0] if uses else usedTail)
        put(usedTail, 4, uses[-1] if uses else usedHead)
        for j, nd in enumerate(uses):
            put(nd, 4, uses[j - 1] if j else usedHead)
            put(nd, 8, uses[j + 1] if j + 1 < len(uses) else usedTail)
        put(freeHead, 8, frees[0] if frees else freeTail)
        put(freeTail, 4, frees[-1] if frees else freeHead)
        for j, nd in enumerate(frees):
            put(nd, 4, frees[j - 1] if j else freeHead)
            put(nd, 8, frees[j + 1] if j + 1 < len(frees) else freeTail)
        tw = a[14]
        if tw == 1:
            put(usedHead, 8, frees[0])
        elif tw == 2:
            put(frees[0], 8, frees[0])
        elif tw == 3:
            put(frees[0], 4, frees[0])
        elif tw == 4:
            put(usedHead, 8, usedHead)
        Cc.wr(dc, 4, data)
        argp = W.raw(2, dc)
        W.snap()
        marks += ['free%d' % free, 'used%d' % used, 'twist%d' % tw]
        marks.append('data_zero' if data == 0 else ('data_ptr' if 0x400000 <= data < 0xC00000 else 'data_other'))
        link = get(this, 0x20)                                          # eax = [ecx+20h]
        if link == freeTail:
            ret = 0
        else:
            put(link, 0, get(argp, 0))                                  # [eax] = **(esp+4)
            nxt, prv = get(link, 8), get(link, 4)
            put(nxt, 4, prv)                                            # [edx+4] = esi
            prv2, nxt2 = get(link, 4), get(link, 8)                     # se RELEEN
            put(prv2, 8, nxt2)                                          # [edx+8] = esi
            head = get(this, 8)
            put(link, 8, head)                                          # [eax+8] = [ecx+8]
            head2 = get(this, 8)                                        # se RELEE
            put(head2, 4, link)                                         # [edx+4] = eax
            put(link, 4, this)                                          # [eax+4] = ecx
            put(this, 8, link)                                          # [ecx+8] = eax
            ret = W.tagv(link)
    elif k == BONE:
        W.snap()
        v = s32(a[4])
        if v > 0xC9:
            ret = BONES[v] if v in (0x12D, 0x12E) else None
        elif v == 0xC9:
            ret = BONES[201]
        elif (v & M32) > 0x36:
            ret = None
        else:
            ret = BONES.get(v)
        if ret is not None:
            marks.append('named')
        elif 0 <= v <= 0x36:
            marks.append('null_gap')
        else:
            marks.append('null_big')
        if v < 0:
            marks.append('negative')
        if v == 0xC9:
            marks.append('tag_c9')
        if v == 0x12D:
            marks.append('tag_12d')
        if v == 0x12E:
            marks.append('tag_12e')
        if v == 0:
            marks.append('tag_0')
        if v == 201:
            marks.append('tag_201')
        if a[4] > 0xFFFF and ((a[4] & 0xFFFF) in BONES or (a[4] & 0xFFFF) in (0x12D, 0x12E)):
            marks.append('high_bits')
    elif k == ENTER:
        W.snap()
        w = [a[4 + i] & 0xFFFF for i in range(4)]
        newe, olde, newx, oldx = w
        pe, px = newe != 0 and olde == 0, newx != 0 and oldx == 0
        ret = 1 if (pe or px) else 0
        marks.append('result_true' if ret else 'result_false')
        if newe != 0 and olde == 0:
            marks.append('enter_pressed')
        if newe != 0 and olde != 0:
            marks.append('enter_held')
        if newx != 0 and oldx == 0:
            marks.append('ext_pressed')
        if newx != 0 and oldx != 0:
            marks.append('ext_held')
        if any(x != 0 and (x & 0xFF) == 0 for x in w):
            marks.append('low_byte_zero_word')
    elif k == KEY:
        W.snap()
        key = a[4] & 0xFF
        newk, oldk = a[5] & 0xFFFF, a[6] & 0xFFFF
        ret = 1 if (newk != 0 and oldk == 0) else 0
        marks.append('result_true' if ret else 'result_false')
        marks.append('new_zero' if newk == 0 else ('new_old_set' if oldk != 0 else 'new_set_old_zero'))
        if a[4] >> 8:
            marks.append('junk_high')
        if any(x != 0 and (x & 0xFF) == 0 for x in (newk, oldk)):
            marks.append('low_byte_zero_word')
        if key == 0:
            marks.append('key_zero')
        if key == 0xFF:
            marks.append('key_ff')
    elif k == CROSS:
        A.wr(o + 0x20, 2, a[4] & 0xFFFF)
        A.wr(o + 0x50, 2, a[5] & 0xFFFF)
        W.snap()
        newc, oldc = a[4] & 0xFFFF, a[5] & 0xFFFF
        ret = 1 if (newc != 0 and oldc == 0) else 0
        marks.append('result_true' if ret else 'result_false')
        marks.append('new_zero' if newc == 0 else ('new_old_set' if oldc != 0 else 'new_set_old_zero'))
        if any(x != 0 and (x & 0xFF) == 0 for x in (newc, oldc)):
            marks.append('low_byte_zero_word')
    elif k == SDD:
        W.snap()
        if fn in SDD_VT:
            A.wr(o, 4, SDD_VT[fn])                                      # el cuerpo real del EXE escribe su vtable y salta a CTask::Destructor (falso)
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([TASK_DELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
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
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL en IsEnterJustPressed"""
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
