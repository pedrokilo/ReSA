"""Oraculo independiente (Python) de Fn164A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 164A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtable de CBike 0x871360 leida del propio EXE; ranura = indice del dword en la vtable).
   0  0x6B5820 OpenDoor (27): ret 14h            1  0x6B5830 GetDooorAngleOpenRatioU32 (29): fld [858B50] (=0.0f); ret 4     2  0x6B5840 IsOpenTopCar (39): xor al,al; ret
   3  0x6B5850 BlowUpCarCutSceneNoExtras (42): ret 10h
   4  0x6B5860 AddMovingCollisionSpeed (49): eax=[esp+4]; ecx=0; [eax]=[eax+4]=[eax+8]=ecx; ret 8 (devuelve la salida)
   5  0x6B5880 SetTowLink (61): xor al,al; ret 8   6  0x6B5890 BreakTowLink (62): xor al,al; ret   7  0x6B58A0 GetNumContactWheels (55): movzx eax, byte [ecx+804h]; ret
   8  0x6B58B0 GetHeightAboveRoad (53): fld dword [ecx+790h]; ret   9  0x6B58C0 GetRideAnimData (47): lea eax,[ecx+640h]; ret
  10  0x6B58E0 IsDoorReady (32): mov al,1; ret 4   11  0x6B58F0 IsDoorFullyOpen (34): xor al,al; ret 4   12  0x6B5900 IsDoorClosed (36): xor al,al; ret 4   13  0x6B5910 IsDoorMissing (38): mov al,1; ret 4
  14  0x6B5920 IsDoorReadyU32 (31): mov al,1   15  0x6B5930 IsDoorFullyOpenU32 (33): xor al,al   16  0x6B5940 IsDoorClosedU32 (35): xor al,al   17  0x6B5950 IsDoorMissingU32 (37): mov al,1 (todas `ret 4`)
  18  0x6B59E0 IsComponentPresent (26): eax=[esp+4]; edx=[ecx+eax*4+5A0h]; test edx,edx; setne al; ret 4
  19  0x6B7270 IsRoomForPedToLeaveCar (45): mov al,1; ret 8   20  0x6B8940 FindWheelWidth (63): fld [858FCC] (=0.15f = 0x3E19999A); ret 4
  21  0x6B8950 CBike destructor con borrado: push esi; mov esi,ecx; call 6B57A0; test byte [esp+8],1; je +; push esi; call 6E2D90; add esp,4; mov eax,esi; pop esi; ret 4
  22  0x622680 CTaskSimpleStealthKill cuerpo del destructor (marco SEH): [esi]=86DA74h; si [esi+0Ch]: call 571A00 (ecx=[esi+0Ch], arg=esi+0Ch); si [esi+18h]: call 4CEBC0 (ecx=[esi+18h], args 4D1390h y 0); call 61A3A0 (ecx=esi)
  23  0x6298E0 CTaskComplexKillPedFromBoat destructor con borrado (vtable 0x86DA98): cuerpo 622830; CTask::operator delete 61A5B0
  24  0x629900 CTaskComplexSeekCoverUntilTargetDead destructor con borrado: cuerpo 622C20; CTask::operator delete 61A5B0
Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas. Los float de ST0 son EXACTOS (fld dword: NaN senalizante silenciado, denormal normalizado).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CBike::OpenDoor", "CBike::GetDooorAngleOpenRatioU32", "CBike::IsOpenTopCar", "CBike::BlowUpCarCutSceneNoExtras", "CBike::AddMovingCollisionSpeed", "CBike::SetTowLink", "CBike::BreakTowLink", "CBike::GetNumContactWheels", "CBike::GetHeightAboveRoad", "CBike::GetRideAnimData", "CBike::IsDoorReady", "CBike::IsDoorFullyOpen", "CBike::IsDoorClosed", "CBike::IsDoorMissing", "CBike::IsDoorReadyU32", "CBike::IsDoorFullyOpenU32", "CBike::IsDoorClosedU32", "CBike::IsDoorMissingU32", "CBike::IsComponentPresent", "CBike::IsRoomForPedToLeaveCar", "CBike::FindWheelWidth", "CBike::ScalarDeletingDestructor", "CTaskSimpleStealthKill::DestructorBody", "CTaskComplexKillPedFromBoat::ScalarDeletingDestructor", "CTaskComplexSeekCoverUntilTargetDead::ScalarDeletingDestructor"]
ENTRIES = (0x6B5820, 0x6B5830, 0x6B5840, 0x6B5850, 0x6B5860, 0x6B5880, 0x6B5890, 0x6B58A0, 0x6B58B0, 0x6B58C0, 0x6B58E0, 0x6B58F0, 0x6B5900, 0x6B5910, 0x6B5920, 0x6B5930, 0x6B5940, 0x6B5950, 0x6B59E0, 0x6B7270, 0x6B8940, 0x6B8950, 0x622680, 0x6298E0, 0x629900)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CLEANUP, SETDELCB, TASKDTOR = 0x571A00, 0x4CEBC0, 0x61A3A0
DEFCB = 0x4D1390

VOID, AL, FLOAT0, FLOATC, FLOATFIELD, ADDSPEED, WHEELS, RIDEANIM, COMP, SDD, STEALTH = 'void al float0 floatc floatfield addspeed wheels rideanim comp sdd stealth'.split()
KIND = [VOID, FLOAT0, AL, VOID, ADDSPEED, AL, AL, WHEELS, FLOATFIELD, RIDEANIM, AL, AL, AL, AL, AL, AL, AL, AL, COMP, AL, FLOATC, SDD, STEALTH, SDD, SDD]
AL_VALUE = {2: 0, 5: 0, 6: 0, 10: 1, 11: 0, 12: 0, 13: 1, 14: 1, 15: 0, 16: 0, 17: 1, 19: 1}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {21: 0x6B57A0, 23: 0x622830, 24: 0x622C20}
DELETE = {21: 0x6E2D90, 23: 0x61A5B0, 24: 0x61A5B0}
SPAN = {STEALTH: 0x20}
SPAN_CBIKE = 0x814


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


class World:
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
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

    def snap(self):
        for m in self.mem:
            m.snap()



def f32_to_x80(bits):
    """lo que deja `fld dword`: (signo, exponente de 15 bits, mantisa de 64 bits); NaN senalizante silenciado, denormal normalizado"""
    s, e, f = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 255:
        if f == 0:
            return s, 0x7FFF, 1 << 63
        return s, 0x7FFF, (1 << 63) | (1 << 62) | (f << 40)
    if e == 0:
        if f == 0:
            return s, 0, 0
        shift = 24 - f.bit_length()
        return s, 16383 - 126 - shift, (f << shift) << 40
    return s, e - 127 + 16383, (1 << 63) | (f << 40)


def x80_bytes(t):
    s, e, m = t
    return struct.pack('<QH', m, (s << 15) | e)


def float_class(bits):
    e, f = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 255:
        return 'f_inf' if f == 0 else ('f_qnan' if f >> 22 else 'f_snan')
    if e == 0:
        return 'f_zero' if f == 0 else 'f_denorm'
    return 'f_normal'


KEYS = {}
for _fn in range(25):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == STEALTH:
        _k += ['spill', 'prev_zero', 'prev_other', 'target_set', 'target_null', 'anim_set', 'anim_null', 'target_in_arena', 'anim_in_arena']
    elif _kind == FLOATFIELD:
        _k += ['f_normal', 'f_zero', 'f_denorm', 'f_inf', 'f_qnan', 'f_snan']
    elif _kind == ADDSPEED:
        _k += ['out_b', 'out_this', 'out_this16', 'out_c', 'arg2_alias', 'arg2_raw']
    elif _kind == COMP:
        _k += ['comp_real', 'comp_other', 'present_true', 'present_false']
    elif _kind == WHEELS:
        _k += ['wheels_zero', 'wheels_other']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    k = KIND[fn]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    span = SPAN.get(k, SPAN_CBIKE if fn <= 20 else 4)
    if a[0] + span > WIN:
        marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret, st0 = [], None, None
    o = a[0]
    this = W.raw(0, o)
    if k in (VOID, AL, FLOAT0, FLOATC, RIDEANIM):
        A.wr(o, 4, a[3])
        W.snap()
        if k == AL:
            ret = AL_VALUE[fn]
        elif k == FLOAT0:
            st0 = ('exact', x80_bytes(f32_to_x80(0)))
        elif k == FLOATC:
            st0 = ('exact', x80_bytes(f32_to_x80(0x3E19999A)))
        elif k == RIDEANIM:
            ret = W.tagv((this + 0x640) & M32)
    elif k == FLOATFIELD:
        A.wr(o, 4, a[3])
        A.wr(o + 0x790, 4, a[3])
        W.snap()
        st0 = ('exact', x80_bytes(f32_to_x80(a[3])))
        marks.append(float_class(a[3]))
    elif k == WHEELS:
        A.wr(o + 0x804, 1, a[3] & 0xFF)
        W.snap()
        ret = A.rd(o + 0x804, 1)
        marks.append('wheels_zero' if ret == 0 else 'wheels_other')
    elif k == ADDSPEED:
        mode = a[4]
        if mode == 1:
            out = this
            marks.append('out_this')
        elif mode == 2:
            out = (this + 0x10) & M32
            marks.append('out_this16')
        elif mode == 3:
            out = W.raw(2, a[3])
            marks.append('out_c')
        else:
            out = W.raw(1, a[3])
            marks.append('out_b')
        marks.append('arg2_alias' if a[6] else 'arg2_raw')
        W.snap()
        for d in (0, 4, 8):
            W.wr((out + d) & M32, 4, 0)
        ret = W.tagv(out)
    elif k == COMP:
        A.wr(o, 4, a[5])
        idx = a[4] - (1 << 32) if a[4] >> 31 else a[4]
        slot = o + 0x5A0 + 4 * idx
        A.wr(slot, 4, a[3])
        W.snap()
        val = A.rd(slot, 4)
        ret = 1 if val else 0
        marks.append('comp_real' if 0 <= idx < 10 else 'comp_other')
        marks.append('present_true' if ret else 'present_false')
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], tag(0, o), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == STEALTH:
        target = {0: 0, 1: W.raw(1, a[7]), 2: a[8]}[a[6]]
        anim = {0: 0, 1: W.raw(1, a[10]), 2: a[11]}[a[9]]
        A.wr(o, 4, a[3])
        A.wr(o + 0xC, 4, target)
        A.wr(o + 0x18, 4, anim)
        W.snap()
        A.wr(o, 4, 0x86DA74)
        if target:
            events.append([CLEANUP, W.tagv(target), tag(0, o + 0xC)])
            marks.append('target_set')
            if a[6] == 1:
                marks.append('target_in_arena')
        else:
            marks.append('target_null')
        if anim:
            events.append([SETDELCB, W.tagv(anim), DEFCB, 0])
            marks.append('anim_set')
            if a[9] == 1:
                marks.append('anim_in_arena')
        else:
            marks.append('anim_null')
        events.append([TASKDTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'st0': st0, 'marks': marks}


def check_st0(exp, img):
    ftw = int.from_bytes(img[8:10], 'little')
    fsw = int.from_bytes(img[4:6], 'little')
    top = (fsw >> 11) & 7
    tags = [(ftw >> (2 * i)) & 3 for i in range(8)]
    if tags[top] == 3 or any(t != 3 for i, t in enumerate(tags) if i != top):
        return 'x87: debe haber solo ST0 valido (ftw %04x top %d)' % (ftw, top)
    raw = bytes(img[28:38])
    if raw != exp[1]:
        return 'ST0 %s != %s' % (raw.hex(), exp[1].hex())
    return None


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
    if e['st0'] is not None:
        return check_st0(e['st0'], img)
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
