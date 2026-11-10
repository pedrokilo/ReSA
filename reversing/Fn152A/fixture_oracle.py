"""Oraculo independiente (Python) de Fn152A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 24 funciones del lote 152A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x492CB0  tUserList::AddPedType: push esi; mov esi,[esp+8]; xor edx,edx; mov byte [ecx+20h],1; xor al,al; bucle(edx<4): test al,al; jne fin; cmp [ecx+edx*4],-1; jne +; mov [ecx+edx*4],-2;
                                       mov [ecx+edx*4+10h],esi; mov al,1; inc edx; ... pop esi; ret 4
   1  0x678D40  CTaskSimpleInAir::GetTaskType (vtable 0x8704D8, ranura 4): mov eax,0F1h; ret
   2  0x678D50  CTaskSimpleInAir cuerpo del destructor (marco SEH): [esi]=8704D8h; ecx=[esi+1Ch]; si no es nulo: push 0; push 4D1390h; call 4CEBC0 (SetDeleteCallback, thiscall, ret 8);
                                       ecx=[esi+38h]; eax=esi+38h; si no es nulo: push eax; call 571A00 (CleanUpOldReference, thiscall, ret 4); mov ecx,esi; call 61A3A0 (~CTask: mov [ecx],86D48Ch); ret
   3  0x53ED30  TestModelIndices: ret
   4  0x53EDC0  CPad: cmp word [ecx+8],0; je no; cmp word [ecx+38h],0; jne no; mov eax,1; ret; no: xor eax,eax; ret
   5  0x53EE20  idem con [ecx+0Ch] y [ecx+3Ch]
   6  0x53EE60  idem con [ecx+10h] y [ecx+40h]
   7  0x53EF40  idem con [ecx+1Eh] y [ecx+4Eh]
   8  0x53EF60  idem con [ecx+22h] y [ecx+52h]
   9  0x4AE640  CEventPotentialWalkIntoObject (vtable 0x85AF18, ranura 1): mov eax,2Ch; ret
  10  0x4AE650  ranura 3: xor eax,eax; ret
  11  0x4AE660  ranura 2: mov eax,1Bh; ret
  12  0x4AE6D0  ranura 11: mov al,1; ret 4
  13  0x4B4AA0  CEventKnockOffBike::operator=: mov eax,[esp+4]; push esi; push eax; mov esi,ecx; call 4AFDD0 (From, thiscall, ret 4); mov eax,esi; pop esi; ret 4
  14  0x4B4A50  CEventKnockOffBike(const&) (marco SEH): esi=ecx; [esi+4]=0; byte [esi+8]=0; push arg; [esi]=85B2E0h; call 4AFDD0; eax=esi; ret 4
  15  0x6426E0  CTaskSimpleCarDrive destructor con borrado (vtable 0x86E904): push esi; mov esi,ecx; call 63C460; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
  16  0x642700  CTaskSimpleCarDrive::StartRollingDoor: edi=ped; eax=[edi+58Ch]; ecx=[eax+384h]; movzx ecx,byte [ecx+0DEh]; imul ecx,ecx,94h; push 182h; add ecx,0C1CDC0h; call 6E3B00 (GetGroup, ret 4);
                                       edx=[edi+18h]; push 182h; push eax; push edx; call 4D3AA0 (AddAnimation, cdecl); add esp,0Ch; push esi; push 63C840h; ecx=eax; [esi+0Ch]=eax; call 4CEBE0 (SetFinishCallback, ret 8); ret 4
  17  0x6C0560  CBmx::BlowUpCar (vtable 0x871528, ranura 41): ret 8
  18  0x6C0570  CBmx destructor con borrado (vtable 0x871528): push esi; mov esi,ecx; call 6BF9D0; test byte [esp+8],1; je +; push esi; call 6E2D90; add esp,4; mov eax,esi; pop esi; ret 4
  19  0x6BF9D0  CBmx cuerpo del destructor (marco SEH): [esi]=871528h; ecx=esi+138h; call 4FB8C0 (CAEVehicleAudioEntity::Terminate); ecx=esi; call 6B57A0 (~CBike); ret
  20  0x62F270  CTaskComplexLeaveCarAndDie cuerpo del destructor: mov [ecx],86DD00h; jmp 63B970 (~CTaskComplexLeaveCar)
  21  0x62F250  CTaskComplexLeaveCarAndDie destructor con borrado (vtable 0x86DD00): cuerpo 62F270; delete 61A5B0 si el bit 0 del byte de banderas
  22  0x6422A0  CTaskComplexLeaveCarAndWander destructor con borrado (vtable 0x86E880): cuerpo 63BE50; delete 61A5B0
  23  0x62F2D0  mov ax,[esp+4]; ax en {0,2,0Dh,6,0Bh,11h} -> 1, si no xor al,al; ret   (cdecl)

Los falsos de los colaboradores anotan: cuerpos de destructor y ~CTask/~CBike/~CTaskComplexLeaveCar [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]);
operator delete [direccion, puntero (etiqueta), dword del puntero] (prueba que se borra DESPUES de destruir); SetDeleteCallback [direccion, ECX, callback, dato]; CleanUpOldReference [direccion, ECX,
etiqueta del argumento, dword del argumento]; From [direccion, ECX, etiqueta del argumento, dword de [ECX], dword de [ECX+4], byte de [ECX+8]] (deja un dword del caso en [ECX+4] y un byte en [ECX+8], y
devuelve un EAX del caso); GetGroup [direccion, ECX, argumento]; AddAnimation [direccion, clump, grupo, animacion]; SetFinishCallback (puerta) [direccion, ECX, callback, etiqueta del dato, dword de
[dato+0Ch]] (el callback de la DLL se anota como 0x63C840); Terminate [direccion, etiqueta de ECX, dword de [ECX-138h]]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['tUserList::AddPedType', 'CTaskSimpleInAir::GetTaskType', 'CTaskSimpleInAir::DestructorBody', 'TestModelIndices', 'CPad::IsLeftShoulder1Pressed', 'CPad::IsRightShoulder1Pressed',
         'CPad::IsDPadUpPressed', 'CPad::IsTrianglePressed', 'CPad::IsCirclePressed', 'CEventPotentialWalkIntoObject::GetEventType', 'CEventPotentialWalkIntoObject::GetLifeTime',
         'CEventPotentialWalkIntoObject::GetEventPriority', 'CEventPotentialWalkIntoObject::TakesPriorityOver', 'CEventKnockOffBike::operator=', 'CEventKnockOffBike::CopyConstructor',
         'CTaskSimpleCarDrive::ScalarDeletingDestructor', 'CTaskSimpleCarDrive::StartRollingDoor', 'CBmx::BlowUpCar', 'CBmx::ScalarDeletingDestructor', 'CBmx::DestructorBody',
         'CTaskComplexLeaveCarAndDie::DestructorBody', 'CTaskComplexLeaveCarAndDie::ScalarDeletingDestructor', 'CTaskComplexLeaveCarAndWander::ScalarDeletingDestructor', 'IsExtraSunnyWeather']
ENTRIES = (0x492CB0, 0x678D40, 0x678D50, 0x53ED30, 0x53EDC0, 0x53EE20, 0x53EE60, 0x53EF40, 0x53EF60, 0x4AE640, 0x4AE650, 0x4AE660,
           0x4AE6D0, 0x4B4AA0, 0x4B4A50, 0x6426E0, 0x642700, 0x6C0560, 0x6C0570, 0x6BF9D0, 0x62F270, 0x62F250, 0x6422A0, 0x62F2D0)
NFN = len(NAMES)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

# direcciones de los colaboradores (EXE)
SETDEL, CLEANUP, FROM, GETGROUP, ADDANIM, SETFINISH, TERMINATE = 0x4CEBC0, 0x571A00, 0x4AFDD0, 0x6E3B00, 0x4D3AA0, 0x4CEBE0, 0x4FB8C0
EXE_DEFAULT_ANIM_CB = 0x4D1390
ROLL_CB = 0x63C840
INAIR_VTABLE, KNOCK_VTABLE, BMX_VTABLE, LEAVEDIE_VTABLE = 0x8704D8, 0x85B2E0, 0x871528, 0x86DD00

# forma de cada funcion
SDD, CONST, BOOLARG, NOPC, NOPT, PAD, USERLIST, INAIR, ASSIGN, CCTOR, BMXBODY, LEAVEBODY, ROLL, WEATHER = 'sdd const boolarg nopc nopt pad userlist inair assign cctor bmxbody leavebody roll weather'.split()
KIND = [USERLIST, CONST, INAIR, NOPC, PAD, PAD, PAD, PAD, PAD, CONST, CONST, CONST, BOOLARG, ASSIGN, CCTOR, SDD, ROLL, NOPT, SDD, BMXBODY, LEAVEBODY, SDD, SDD, WEATHER]
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {15: 0x63C460, 18: 0x6BF9D0, 21: 0x62F270, 22: 0x63BE50}
DELETE = {15: 0x61A5B0, 18: 0x6E2D90, 21: 0x61A5B0, 22: 0x61A5B0}
CONSTANT = {1: 0xF1, 9: 0x2C, 10: 0, 11: 0x1B}
PAD_OFF = {4: 0x08, 5: 0x0C, 6: 0x10, 7: 0x1E, 8: 0x22}
# cuerpos: (direccion del colaborador final que se falsea con FkBody, vtable que deja el cuerpo)
TASK_DTOR, CBIKE_DTOR, LEAVECAR_DTOR = 0x61A3A0, 0x6B57A0, 0x63B970
SPAN = {0: 0x24, 2: 0x3C, 4: 0x58, 5: 0x58, 6: 0x58, 7: 0x58, 8: 0x58, 13: 0x10, 14: 0x10, 16: 0x10}
EXTRA_SUNNY = (0, 2, 0xD, 6, 0xB, 0x11)


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
for _fn in range(NFN):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind not in (NOPC, WEATHER):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == INAIR:
        _k += ['prev_zero', 'prev_other', 'anim_null', 'anim_set', 'climb_null', 'climb_set']
    elif _kind in (ASSIGN, CCTOR, BMXBODY, LEAVEBODY):
        _k += ['prev_zero', 'prev_other']
    elif _kind == PAD:
        _k += ['new_zero', 'new_set', 'old_zero', 'old_set', 'pressed', 'not_pressed']
    elif _kind == USERLIST:
        _k += ['free0', 'free1', 'free2', 'free3', 'none_free', 'flag_prev_nonzero']
    elif _kind == ROLL:
        _k += ['group_low', 'group_high', 'handling_unaligned', 'ped_unaligned']
    elif _kind == WEATHER:
        _k += ['sunny', 'not_sunny', 'high_junk']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if KIND[fn] not in (NOPC, WEATHER):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        if a[0] + SPAN.get(fn, 4) > WIN:
            marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k == SDD:
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
    elif k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT[fn]
    elif k == BOOLARG:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 1                                           # solo AL
    elif k == NOPC:
        W.snap()
    elif k == NOPT:
        A.wr(o, 4, a[3])
        W.snap()
    elif k == PAD:
        off = PAD_OFF[fn]
        al = off & ~3
        jn, jo = a[5] & 0xFFFF, a[6] & 0xFFFF
        nw, ow = a[3] & 0xFFFF, a[4] & 0xFFFF
        A.wr(o + al, 4, ((nw << 16) | jn) if off & 2 else ((jn << 16) | nw))
        A.wr(o + 0x30 + al, 4, ((ow << 16) | jo) if off & 2 else ((jo << 16) | ow))
        W.snap()
        ret = 1 if (nw != 0 and ow == 0) else 0
        marks.append('new_set' if nw else 'new_zero')
        marks.append('old_set' if ow else 'old_zero')
        marks.append('pressed' if ret else 'not_pressed')
    elif k == USERLIST:
        for i in range(4):
            A.wr(o + 4 * i, 4, a[3 + i])
            A.wr(o + 0x10 + 4 * i, 4, a[7 + i])
        A.wr(o + 0x20, 4, a[11])
        W.snap()
        A.wr(o + 0x20, 1, 1)
        taken = False
        for i in range(4):
            if A.rd(o + 4 * i, 4) == 0xFFFFFFFF:
                A.wr(o + 4 * i, 4, 0xFFFFFFFE)
                A.wr(o + 0x10 + 4 * i, 4, a[16])
                marks.append('free%d' % i)
                taken = True
                break
        if not taken:
            marks.append('none_free')
        if a[11] & 0xFF:
            marks.append('flag_prev_nonzero')
    elif k == INAIR:
        A.wr(o, 4, a[3])
        A.wr(o + 0x1C, 4, a[6])
        A.wr(o + 0x38, 4, a[7])
        W.snap()
        A.wr(o, 4, INAIR_VTABLE)
        if a[6]:
            events.append([SETDEL, W.tagv(a[6]), EXE_DEFAULT_ANIM_CB, 0])
        if a[7]:
            events.append([CLEANUP, W.tagv(a[7]), W.tagv(W.raw(0, o + 0x38)), A.rd(o + 0x38, 4)])
        events.append([TASK_DTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        marks.append('anim_set' if a[6] else 'anim_null')
        marks.append('climb_set' if a[7] else 'climb_null')
    elif k in (ASSIGN, CCTOR):
        A.wr(o, 4, a[3])
        A.wr(o + 4, 4, a[7])
        A.wr(o + 8, 4, a[8])
        W.snap()
        if k == CCTOR:
            A.wr(o + 4, 4, 0)
            A.wr(o + 8, 1, 0)
            A.wr(o, 4, KNOCK_VTABLE)
        events.append([FROM, tag(0, o), tag(1, a[6]), A.rd(o, 4), A.rd(o + 4, 4), A.rd(o + 8, 1)])
        A.wr(o + 4, 4, a[5])                              # el falso de From
        A.wr(o + 8, 1, (a[4] >> 8) & 0xFF)
        ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BMXBODY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, BMX_VTABLE)
        events.append([TERMINATE, W.tagv(W.raw(0, o + 0x138)), A.rd(o, 4)])
        events.append([CBIKE_DTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == LEAVEBODY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, LEAVEDIE_VTABLE)
        events.append([LEAVECAR_DTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ROLL:
        ped, veh, hnd = a[6], a[8], a[9]
        A.wr(o + 0xC, 4, a[3])
        B.wr(ped + 0x18, 4, a[7])
        B.wr(ped + 0x58C, 4, W.raw(2, veh))
        Cc.wr(veh + 0x384, 4, W.raw(1, hnd))
        B.wr(hnd + 0xDC, 4, (a[11] & 0xFF00FFFF) | ((a[10] & 0xFF) << 16))
        W.snap()
        group = a[10] & 0xFF
        events.append([GETGROUP, (0xC1CDC0 + group * 0x94) & M32, 0x182])
        events.append([ADDANIM, a[7], a[16], 0x182])
        A.wr(o + 0xC, 4, a[17])
        events.append([SETFINISH, a[17], ROLL_CB, tag(0, o), A.rd(o + 0xC, 4)])
        marks.append('group_high' if group >= 30 else 'group_low')
        marks.append('handling_unaligned' if hnd % 4 else 'handling_aligned')
        marks.append('ped_unaligned' if ped % 4 else 'ped_aligned')
    elif k == WEATHER:
        w = a[3] & 0xFFFF
        ret = 1 if w in EXTRA_SUNNY else 0
        marks.append('sunny' if ret else 'not_sunny')
        if ret and (a[3] >> 16):
            marks.append('high_junk')
        W.snap()
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
