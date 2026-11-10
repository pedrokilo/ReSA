"""Oraculo independiente (Python) de Fn156A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 16 funciones del lote 156A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x4B8340  CEventLeaderExitedCarAsDriver::CloneEditable (vtable 0x85B898, ranura 16): push 14h; call 4B5620; si 0: eax = 0; si no: ecx = memoria; call 4AC450; [memoria] = 85B898h; eax = memoria
   1  0x661020  CTaskLeanOnVehicle::GetTaskType (vtable 0x86FAF4, ranura 4): mov eax,4C1h
   2  0x664A30  CTaskComplexPassObject (vtable 0x86FB18) destructor con borrado: push esi; mov esi,ecx; call 661230; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   3  0x6610A0  CTaskLeanOnVehicle::Clone (ranura 1): push 2Ch; call 61A5A0; si 0: eax = 0; si no: push byte [this+10h] (extendido con ceros); push [this+0Ch]; push [this+8]; ecx = memoria; call 660F90; eax = el del constructor
   4  0x7467D0  suma: eax = 0; si inicio >= fin (sin signo): ret; bucle: eax += [p] & 0FFFFFFh; p += 4; mientras p < fin (sin signo)
   5  0x679AE0  CTaskSimpleJump::GetTaskType (vtable 0x87054C, ranura 4): mov eax,0D2h
   6  0x67D960  CTaskComplexJump (vtable 0x870570) destructor con borrado: cuerpo 67A060; liberacion 61A5B0
   7  0x679AF0  CTaskSimpleJump cuerpo del destructor: [this] = 87054Ch; si [this+28h]: push 0; push 4D1390h; call 4CEBE0 (ecx = animacion); si [this+1Ch]: push this+1Ch; call 571A00 (ecx = entidad); ecx = this; call 61A3A0
   8  0x63A140  CEventLeaderQuitEnteringCarAsDriver::GetLifeTime (vtable 0x86E6B0, ranura 3): xor eax,eax
   9  0x4B1040  ...::TakesPriorityOver (ranura 11): mov al,1; ret 4
  10  0x63A1A0  ...::AffectsPed (ranura 5): xor al,al; ret 4
  11  0x63A130  ...::GetEventType (ranura 1): mov eax,39h
  12  0x63A1E0  ...::GetEventPriority (ranura 2): mov eax,30h
  13  0x63A1D0  ...::cuerpo del destructor: mov [ecx],86E6B0h; jmp 4AC010
  14  0x63A1B0  ...::destructor con borrado: cuerpo 63A1D0; liberacion 4B5630
  15  0x63A150  ...::Clone (ranura 4): push 0Ch; call 4B5620; si 0: eax = 0; si no: ecx = memoria; call 4ABFC0; [memoria] = 86E6B0h; eax = memoria

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos de destructor, ~CTask, ~CEvent y constructores de CEvent); operator delete anota
[direccion, puntero (etiqueta), dword del puntero] (prueba que se borra DESPUES de destruir); operator new anota [direccion, tamano]; CleanUpOldReference [direccion, ECX, argumento]; el constructor de
CTaskLeanOnVehicle [direccion, ECX, vehiculo, duracion, byte de banderas extendido]; SetFinishCallback [direccion, ECX, funcion (0x4D1390), datos]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CEventLeaderExitedCarAsDriver::CloneEditable", "CTaskLeanOnVehicle::GetTaskType", "CTaskComplexPassObject::ScalarDeletingDestructor", "CTaskLeanOnVehicle::Clone", "VideoPlayer::SumMasked24",
         "CTaskSimpleJump::GetTaskType", "CTaskComplexJump::ScalarDeletingDestructor", "CTaskSimpleJump::DestructorBody", "CEventLeaderQuitEnteringCarAsDriver::GetLifeTime",
         "CEventLeaderQuitEnteringCarAsDriver::TakesPriorityOver", "CEventLeaderQuitEnteringCarAsDriver::AffectsPed", "CEventLeaderQuitEnteringCarAsDriver::GetEventType",
         "CEventLeaderQuitEnteringCarAsDriver::GetEventPriority", "CEventLeaderQuitEnteringCarAsDriver::DestructorBody", "CEventLeaderQuitEnteringCarAsDriver::ScalarDeletingDestructor",
         "CEventLeaderQuitEnteringCarAsDriver::Clone"]
ENTRIES = (0x4B8340, 0x661020, 0x664A30, 0x6610A0, 0x7467D0, 0x679AE0, 0x67D960, 0x679AF0, 0x63A140, 0x4B1040, 0x63A1A0, 0x63A130, 0x63A1E0, 0x63A1D0, 0x63A1B0, 0x63A150)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SETFINISH, CLEANUP, TASKNEW, EVNEW, LEANCTOR = 0x4CEBE0, 0x571A00, 0x61A5A0, 0x4B5620, 0x660F90
DEFAULT_CB = 0x4D1390

# forma de cada funcion
EVCLONE, CONST, SDD, LEANCLONE, SUM24, JUMPBODY, CONSTAL, VTBODY = 'evclone const sdd leanclone sum24 jumpbody constal vtbody'.split()
KIND = [EVCLONE, CONST, SDD, LEANCLONE, SUM24, CONST, SDD, JUMPBODY, CONST, CONSTAL, CONSTAL, CONST, CONST, VTBODY, SDD, EVCLONE]
CONSTS = {1: 0x4C1, 5: 0xD2, 8: 0, 9: 1, 10: 0, 11: 0x39, 12: 0x30}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {2: 0x661230, 6: 0x67A060, 14: 0x63A1D0}
DELETE = {2: 0x61A5B0, 6: 0x61A5B0, 14: 0x4B5630}
# cuerpo con vtable: (vtable, colaborador)
VTBODY_INFO = {13: (0x86E6B0, 0x4AC010)}
# clones de eventos: (tamano, constructor, vtable)
EVCLONE_INFO = {0: (0x14, 0x4AC450, 0x85B898), 15: (0xC, 0x4ABFC0, 0x86E6B0)}
JUMP_VT, JUMP_DTOR = 0x87054C, 0x61A3A0


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


KEYS = {}
for _fn in range(16):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind != SUM24:
        _k += ['this_aligned', 'this_unaligned']
    if _kind == CONSTAL:
        _k += ['arg_zero', 'arg_other']
    elif _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == VTBODY:
        _k += ['prev_zero', 'prev_other']
    elif _kind == EVCLONE:
        _k += ['memory_null', 'memory_in_b', 'eax_ignored']
    elif _kind == LEANCLONE:
        _k += ['memory_null', 'memory_in_b', 'memory_in_a', 'eax_memory', 'eax_other', 'byte_high_bits', 'vehicle_null']
    elif _kind == JUMPBODY:
        _k += ['anim_null', 'anim_set', 'climb_null', 'climb_set', 'climb_raw', 'both_set', 'both_null', 'prev_zero', 'prev_other']
    elif _kind == SUM24:
        _k += ['mode_0', 'mode_1', 'mode_2', 'mode_3', 'mode_4', 'mode_5', 'mode_6', 'wraps', 'n_zero', 'sum_nonzero', 'mask_effect', 'begin_unaligned']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if KIND[fn] != SUM24:
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    return marks


def ctor_eax(sel, mem, raw):
    return mem if sel == 0 else ((mem + 4) & M32 if sel == 1 else (0 if sel == 2 else raw))


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn]
    elif k == CONSTAL:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn] & 0xFF
        marks.append('arg_zero' if a[4] == 0 else 'arg_other')
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], W.tagv(this), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], W.tagv(this), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = W.tagv(this)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == VTBODY:
        vt, body = VTBODY_INFO[fn]
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, vt)
        events.append([body, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == EVCLONE:
        size, ctor, vt = EVCLONE_INFO[fn]
        W.snap()
        events.append([EVNEW, size])
        mem = 0 if a[3] == 0 else W.raw(1, a[4])
        if mem == 0:
            ret = 0
            marks.append('memory_null')
        else:
            events.append([ctor, W.tagv(mem), W.rd(mem, 4)])
            W.wr(mem, 4, a[5])
            W.wr(mem, 4, vt)
            ret = W.tagv(mem)
            marks.append('memory_in_b')
            marks.append('eax_ignored')
    elif k == LEANCLONE:
        A.wr(o + 8, 4, a[3]); A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5])
        W.snap()
        events.append([TASKNEW, 0x2C])
        mem = 0 if a[6] == 0 else (W.raw(1, a[7]) if a[6] == 1 else W.raw(0, a[7]))
        if a[3] == 0:
            marks.append('vehicle_null')
        if a[5] >> 8:
            marks.append('byte_high_bits')
        if mem == 0:
            ret = 0
            marks.append('memory_null')
        else:
            events.append([LEANCTOR, W.tagv(mem), a[3], a[4], a[5] & 0xFF])
            W.wr(mem, 4, a[9])
            eax = ctor_eax(a[8], mem, a[10])
            ret = W.tagv(eax)
            marks.append('memory_in_b' if a[6] == 1 else 'memory_in_a')
            marks.append('eax_memory' if a[8] == 0 else 'eax_other')
    elif k == JUMPBODY:
        anim = 0 if a[6] == 0 else W.raw(1, a[7])
        climb = 0 if a[8] == 0 else (W.raw(1, a[9]) if a[8] == 1 else a[10])
        A.wr(o, 4, a[3]); A.wr(o + 0x1C, 4, climb); A.wr(o + 0x28, 4, anim)
        W.snap()
        A.wr(o, 4, JUMP_VT)
        if anim:
            events.append([SETFINISH, W.tagv(anim), DEFAULT_CB, 0])
            marks.append('anim_set')
        else:
            marks.append('anim_null')
        if climb:
            events.append([CLEANUP, W.tagv(climb), W.tagv((this + 0x1C) & M32)])
            marks.append('climb_set' if a[8] == 1 else 'climb_raw')
        else:
            marks.append('climb_null')
        marks.append('both_set' if anim and climb else ('both_null' if not anim and not climb else 'one_set'))
        events.append([JUMP_DTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == SUM24:
        W.snap()
        mode, n, r = a[3], a[4], a[5]
        begin = this
        b = begin
        if mode == 0:
            e = (begin + 4 * n) & M32
        elif mode == 1:
            e = begin
        elif mode == 2:
            e = (begin - r) & M32
        elif mode == 3:
            e = (begin + 4 * n - r) & M32
        elif mode in (4, 5):
            b, e = a[6], a[7]
        else:
            e = (begin + r) & M32
        total = 0
        mask_effect = False
        p = b
        if b < e:                                         # comparacion sin signo
            while p < e:
                w = W.rd(p, 4)
                total += w & 0xFFFFFF
                mask_effect |= (w >> 24) != 0
                p = (p + 4) & M32
        ret = total & M32
        marks.append('mode_%d' % mode)
        if total > M32:
            marks.append('wraps')
        if n == 0:
            marks.append('n_zero')
        if ret:
            marks.append('sum_nonzero')
        if mask_effect:
            marks.append('mask_effect')
        if mode <= 3 and o % 4:
            marks.append('begin_unaligned')
    out = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}
    return out


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
