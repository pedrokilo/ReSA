"""Oraculo independiente (Python) de Fn143A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 143A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x4ABAA0  CEventGlobalGroup (vtable 0x85AAB4, ranura 0, destructor con borrado)   push esi; mov esi,ecx; call 4AB9B0; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
                4AB9B0 (cuerpo de ~CEventGroup, sin hook): push 0; mov [ecx],85AAB0h; call 4AB370 (Flush(false)); ret
   1  0x734EA0  RwIm2DRenderIndexedPrimitive (salto del dispositivo)                    mov edx,[0C97B24h]; jmp [edx+34h]   (RwGlobals + 0x34 = fpIm2DRenderIndexedPrimitive)
   2  0x734EC0  RwIm2DRenderLine (salto del dispositivo)                                mov ecx,[0C97B24h]; jmp [ecx+28h]   (RwGlobals + 0x28 = fpIm2DRenderLine)
   3  0x4C4A60  CBaseModelInfo (cuerpo del constructor)                                 mov eax,ecx; mov [eax],85BB9Ch; mov word [eax+8],0; mov word [eax+0Ah],0FFFFh; ret
   4  0x4C5540  CAtomicModelInfo (constructor)                                          push esi; mov esi,ecx; call 4C4A60; mov [esi],85BBF0h; mov eax,esi; pop esi; ret
   5  0x4C5920  CVehicleModelInfo (cuerpo del destructor)                               mov dword [ecx],85BB9Ch; ret
   6  0x4AC6B0  CEventGunShot::GetEventType (vtable 0x85ABE0, ranura 1)                 mov eax,0Fh; ret
   7  0x4AC6C0  CEventGunShot::GetLifeTime (ranura 3)                                   xor eax,eax; ret
   8  0x4AC6D0  CEventGunShot::GetSourceEntity (ranura 10)                              mov eax,[ecx+14h]; ret
   9  0x4AC6E0  CEventGunShot::GetLocalSoundLevel (ranura 12)                           mov al,[ecx+30h]; test al,al; je +; fld [858B50h] (0.0); ret; fld [858970h] (160.0); ret
  10  0x4AC700  CEventGunShot::CanBeInterruptedBySameEvent (ranura 15)                  mov al,1; ret
  11  0x4AC710  CEventGunShot::GetEventPriority (ranura 2)                              mov eax,23h; ret
  12  0x4AC830  CEventGunShot::ReportCriminalEvent (ranura 8)                           ret 4
  13  0x6A0650  CEventVehicleDamageCollision (cuerpo del destructor)                    mov [ecx],8710D0h; jmp 4B1990
  14  0x6A0660  CEventVehicleDamageCollision::GetEventType (vtable 0x8710D0, ranura 1)  mov eax,49h; ret
  15  0x6A06D0  CEventVehicleDamageCollision (ranura 0, destructor con borrado)         igual con call 6A0650 y call 4B5630
  16  0x61C310  CEventVehicleDamageWeapon (cuerpo del destructor)                       mov [ecx],86D640h; jmp 4B1990
  17  0x61C320  CEventVehicleDamageWeapon::GetEventType (vtable 0x86D640, ranura 1)     mov eax,29h; ret
  18  0x61C390  CEventVehicleDamageWeapon (ranura 0, destructor con borrado)            igual con call 61C310 y call 4B5630
  19  0x658AD0  CTaskComplexTreatAccident::GetTaskType (vtable 0x86F4EC, ranura 4)      mov eax,65h; ret
  20  0x658AE0  CTaskComplexTreatAccident (cuerpo del destructor)                       mov [ecx],86F4ECh; jmp 61A3D0
  21  0x659FC0  CTaskComplexMedicTreatInjuredPed (vtable 0x86F518, ranura 0)            igual con call 658C30 y call 61A5B0

En los destructores con borrado el falso del cuerpo deja el dword del caso en [ECX] y devuelve un EAX que no es `this`; el falso de operator delete anota el dword de [argumento] al entrar (= lo que dejo
el cuerpo: prueba que se borra DESPUES de destruir). Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas. En los saltos RW (1 y 2) la funcion falsa del dispositivo recibe los argumentos de
pila intactos (cdecl) y su EAX es el de la funcion; los punteros a la arena C se anotan como etiquetas. GetLocalSoundLevel (9): ST0 = el float cargado con fld, EXACTO (0.0 o 160.0).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CEventGlobalGroup::ScalarDeletingDestructor', 'RwIm2DRenderIndexedPrimitiveThunk', 'RwIm2DRenderLineThunk', 'CBaseModelInfo::ConstructorBody', 'CAtomicModelInfo::ConstructorBody',
         'CVehicleModelInfo::DestructorBody', 'CEventGunShot::GetEventType', 'CEventGunShot::GetLifeTime', 'CEventGunShot::GetSourceEntity', 'CEventGunShot::GetLocalSoundLevel',
         'CEventGunShot::CanBeInterruptedBySameEvent', 'CEventGunShot::GetEventPriority', 'CEventGunShot::ReportCriminalEvent', 'CEventVehicleDamageCollision::DestructorBody',
         'CEventVehicleDamageCollision::GetEventType', 'CEventVehicleDamageCollision::ScalarDeletingDestructor', 'CEventVehicleDamageWeapon::DestructorBody',
         'CEventVehicleDamageWeapon::GetEventType', 'CEventVehicleDamageWeapon::ScalarDeletingDestructor', 'CTaskComplexTreatAccident::GetTaskType', 'CTaskComplexTreatAccident::DestructorBody',
         'CTaskComplexMedicTreatInjuredPed::ScalarDeletingDestructor']
ENTRIES = (0x4ABAA0, 0x734EA0, 0x734EC0, 0x4C4A60, 0x4C5540, 0x4C5920, 0x4AC6B0, 0x4AC6C0, 0x4AC6D0, 0x4AC6E0, 0x4AC700, 0x4AC710, 0x4AC830,
           0x6A0650, 0x6A0660, 0x6A06D0, 0x61C310, 0x61C320, 0x61C390, 0x658AD0, 0x658AE0, 0x659FC0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD


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


# ------------------------------------------------------------------------------------------------ tablas leidas del desensamblado
# destructores con borrado: (cuerpo del destructor, operator delete)
SDD = {0: (0x4AB9B0, 0x8214BD), 15: (0x6A0650, 0x4B5630), 18: (0x61C310, 0x4B5630), 21: (0x658C30, 0x61A5B0)}
# cuerpos de destructor con colaborador: (colaborador, vtable que el propio cuerpo deja antes de llamarlo)
BODIES = {13: (0x4B1990, 0x8710D0), 16: (0x4B1990, 0x86D640), 20: (0x61A3D0, 0x86F4EC)}
CONSTANT = {6: 0x0F, 7: 0, 11: 0x23, 14: 0x49, 17: 0x29, 19: 0x65}
RW = (1, 2)
THIS_FNS = tuple(i for i in range(22) if i not in RW)
SPAN = {3: 0xC, 4: 0xC, 8: 0x18, 9: 0x31}
FLOAT_FNS = (9,)


def ptr_mark(prefix, v):
    return prefix + ('_zero' if v == 0 else '_ffff' if v == M32 else '_exe' if 0x400000 <= v < 0xC00000 else '_other')


# ------------------------------------------------------------------------------------------------ float: fld de 0.0 y 160.0
def f32_to_x80(bits):
    """lo que deja `fld dword` para un float normal o cero: (signo, exponente de 15 bits, mantisa de 64 bits)"""
    s, e, f = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 0 and f == 0:
        return s, 0, 0
    return s, e - 127 + 16383, (1 << 63) | (f << 40)


def x80_bytes(t):
    s, e, m = t
    return struct.pack('<QH', m, (s << 15) | e)


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if fn in THIS_FNS:
        o = a[0]
        span = SPAN.get(fn, 4)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + span > WIN:
            marks.append('spill')
    return marks


KEYS = {}
for _fn in range(22):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    if _fn in THIS_FNS:
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _fn in SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    elif _fn in BODIES or _fn in (4, 5):
        _k += ['prev_zero', 'prev_other']
    elif _fn == 8:
        _k += ['src_zero', 'src_ffff', 'src_exe', 'src_other']
    elif _fn == 9:
        _k += ['sound_zero', 'sound_nonzero']
    elif _fn in RW:
        _k += ['verts_arena', 'verts_raw', 'idx_arena', 'idx_raw', 'ret_zero', 'ret_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret, st0 = [], None, None
    o = a[0]
    if fn in SDD:
        body, delete = SDD[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events.append([body, tag(0, o), a[3]])
        A.wr(o, 4, a[5])                                    # el falso del cuerpo deja su dword en [this]
        flags = a[6] & 0xFF                                 # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([delete, tag(0, o), a[5]])        # operator delete ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn in BODIES:
        collab, vt = BODIES[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, vt)                                      # el propio cuerpo deja su vtable y llama al colaborador
        events.append([collab, tag(0, o), vt])
        A.wr(o, 4, a[5])                                    # el falso deja su dword en [this]
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 3:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x85BB9C)
        A.wr(o + 8, 2, 0)
        A.wr(o + 0xA, 2, 0xFFFF)
        ret = tag(0, o)
    elif fn == 4:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events.append([0x4C4A60, tag(0, o), a[3]])          # el cuerpo base (falso) ve el dword previo
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x85BBF0)                                # el constructor deja su vtable despues
        ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 5:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x85BB9C)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 8:
        A.wr(o + 0x14, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = a[3]
        marks.append(ptr_mark('src', a[3]))
    elif fn == 9:
        A.m[GUARD + o + 0x30] = a[3] & 0xFF
        A.snap(); B.snap(); C.snap()
        bits = 0x00000000 if (a[3] & 0xFF) else 0x43200000   # byte distinto de cero: 0.0 (fld [858B50]); cero: 160.0 (fld [858970])
        st0 = ('exact', x80_bytes(f32_to_x80(bits)))
        marks.append('sound_nonzero' if a[3] & 0xFF else 'sound_zero')
    elif fn == 10:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = 1                                             # solo AL
    elif fn == 12:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
    elif fn in CONSTANT:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    else:   # 1 y 2: saltos RW por el dispositivo del RwGlobals falso (arena B)
        A.snap(); B.snap(); C.snap()
        verts = tag(2, a[9]) if a[6] & 1 else a[7]
        indices = tag(2, a[10]) if a[6] & 2 else a[8]
        if fn == 1:
            events.append([0x734EA0, a[3], verts, a[4], indices, a[5]])
        else:
            events.append([0x734EC0, verts, a[3], a[4], a[5]])
        ret = a[11]
        marks.append('verts_arena' if a[6] & 1 else 'verts_raw')
        marks.append('idx_arena' if a[6] & 2 else 'idx_raw')
        marks.append('ret_zero' if a[11] == 0 else 'ret_other')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'st0': st0, 'marks': marks}


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
