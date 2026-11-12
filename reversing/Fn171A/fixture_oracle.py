"""Oraculo independiente (Python) de Fn171A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 171A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x632B30  CTaskSimpleWaitForPizza cuerpo del constructor: push esi; mov esi,ecx; call 61A390; xor eax,eax; mov [esi],86E1DCh; mov [esi+8],eax; mov [esi+0Ch],eax; mov [esi+10h],al; mov [esi+11h],al; mov eax,esi; pop esi; ret
   1  0x632B50  GetTaskType: mov eax,0EFh
   2  0x632B60  MakeAbortable: mov al,1; ret 0Ch
   3  0x632B70  cuerpo del destructor: mov [ecx],86E1DCh; jmp 61A3A0
   4  0x632B80  ProcessPed: si [ecx+10h] == 0: [ecx+8] = reloj (0xB7CB84), [ecx+0Ch] = 7D0h, [ecx+10h] = 1; si [ecx+11h] != 0: [ecx+8] = reloj, [ecx+11h] = 0; AL = !(reloj < [ecx+8] + [ecx+0Ch]) sin signo; ret 4
   5  0x636B40  Clone: new(14h) (61A5A0); si no es nulo: call 61A390 (ecx = mem); [mem] = 86E1DCh; +8, +0Ch = 0; +10h, +11h = 0; eax = mem (nulo si no hubo memoria)
   6  0x4B86C0  CTaskSimpleOnEscalator::Clone: new(20h); ctor 62F310 (ecx = mem; pila: 0, 1, 0, 41000000h); [mem] = 85B8DCh; eax = mem
   7  0x4B8720  GetTaskType: mov eax,51Ch
   8  0x4B8730  destructor con borrado: push esi; mov esi,ecx; call 4B8750; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   9  0x4B8750  cuerpo del destructor: mov [ecx],85B8DCh; jmp 62F360
  10  0x62F520  ProcessPed: push ped; call 62F370 (ecx = this); contacto = [ped+584h]; si no es nulo: modelo = movsx word [contacto+22h]; si es igual a movzx word [8CD674h] o movzx word [8CD678h]:
            atan = 53CBE0(x = [contacto+44h], y = [contacto+48h], 0, 0) (cdecl; fstp dword); 53CB50(atan) (cdecl); fstp dword [ped+55Ch]; AL = 0; si no AL = 1; ret 4
  11  0x62F350  GetTaskType: mov eax,0CBh
  12  0x6490F0  GetTaskType: mov eax,33Eh
  13  0x649160  ComputeTargetPos: veh = [this+18h]; mi = [0A9B0C8h + movsx word [veh+22h] * 4]; dummy = [mi+5Ch] (+30h si [mi+3Ch] != 5); d = copia (x, y, z); r = 59C790(&r, [veh+14h], &d) (cdecl);
            pos = [veh+14h] ? [veh+14h]+30h : veh+4; [this+0Ch] = r.x + pos.x (fld r.x; fadd pos.x; fstp), idem y, z
  14  0x64E480  CTaskSimpleCreateCarAndGetIn destructor con borrado: como 8 con el cuerpo 64CEA0
  15  0x64E4B0  ComputeCriticalBlendTime(grupo, anim): esi = 0C1CDC0h + grupo * 94h; esi.GetGroup(anim) (6E3B00, thiscall `ret 4`); esi.ComputeCriticalBlendTime(anim) (6E3C80); el float de ST0 es el resultado
  16  0x68CD10  CTaskSimpleArrestPed::Clone: new(14h); ctor 68B620 (ecx = mem; pila: [this+8]); eax = el del constructor (nulo si no hubo memoria)
  17  0x68CD70  CTaskComplexArrestPed::Clone: new(24h); ctor 68B990 (pila: [this+10h]); eax = el del constructor
  18  0x68CF40  CTaskSimpleArrestPed destructor con borrado: como 8 con el cuerpo 68B690
  19  0x68CF60  CTaskComplexArrestPed destructor con borrado: como 8 con el cuerpo 68BA00
  20  0x6550C0  GetTaskType: mov eax,1FEh
  21  0x6550D0  cuerpo del destructor (marco SEH): [this] = 86F364h; si [this+1Ch]: ecx = obj; push this+1Ch; call 571A00; si [this+20h]: push ruta; call 41B7D0 (cdecl); ecx = this; call 61A3D0
  22  0x655290  CreateSubTask(tipo, ped): tipo 0xCB: new(20h); si hay memoria: ctor 62F310 (500, 0, 0, 41000000h) y eax = el del constructor; si no 0; tipo 0x389: 655140(ped) (ecx = this), eax = el suyo; tipo 0x516 o cualquier otro: 0

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (los de un argumento anaden el argumento etiquetado, los de cuatro los cuatro dwords); operator delete [direccion, puntero (etiqueta),
dword del puntero]; operator new [direccion, tamano]; CleanUpOldReference [direccion, ECX, argumento]; ProcessPed de StandStill [direccion, ECX, ped]; 53CBE0 [direccion, bits de los cuatro floats]; 53CB50 [direccion, bits];
Multiply3x3 [direccion, resultado, matriz, vector (los de pila como 0x7F000000), x, y, z del vector]; GetGroup y ComputeCriticalBlendTime [direccion, ECX crudo, anim]; CreateRouteTask [direccion, ECX, ped].
Los floats que devuelven los falsos pasan por la x87 (fld / fstp): un NaN senalizante sale silencioso (bit 22).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskSimpleWaitForPizza::Constructor", "CTaskSimpleWaitForPizza::GetTaskType", "CTaskSimpleWaitForPizza::MakeAbortable", "CTaskSimpleWaitForPizza::DestructorBody",
         "CTaskSimpleWaitForPizza::ProcessPed", "CTaskSimpleWaitForPizza::Clone", "CTaskSimpleOnEscalator::Clone", "CTaskSimpleOnEscalator::GetTaskType",
         "CTaskSimpleOnEscalator::ScalarDeletingDestructor", "CTaskSimpleOnEscalator::DestructorBody", "CTaskSimpleOnEscalator::ProcessPed", "CTaskSimpleStandStill::GetTaskType",
         "CTaskComplexGoToBoatSteeringWheel::GetTaskType", "CTaskComplexGoToBoatSteeringWheel::ComputeTargetPos", "CTaskSimpleCreateCarAndGetIn::ScalarDeletingDestructor",
         "CVehicleAnimGroupData::ComputeCriticalBlendTime", "CTaskSimpleArrestPed::Clone", "CTaskComplexArrestPed::Clone", "CTaskSimpleArrestPed::ScalarDeletingDestructor",
         "CTaskComplexArrestPed::ScalarDeletingDestructor", "CTaskComplexWalkRoundObject::GetTaskType", "CTaskComplexWalkRoundObject::DestructorBody", "CTaskComplexWalkRoundObject::CreateSubTask"]
ENTRIES = (0x632B30, 0x632B50, 0x632B60, 0x632B70, 0x632B80, 0x636B40, 0x4B86C0, 0x4B8720, 0x4B8730, 0x4B8750, 0x62F520, 0x62F350, 0x6490F0, 0x649160, 0x64E480, 0x64E4B0,
           0x68CD10, 0x68CD70, 0x68CF40, 0x68CF60, 0x6550C0, 0x6550D0, 0x655290)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000

CONST, MAKEAB, PIZZACTOR, BODY, PIZZAPROC, CLONEPIZZA, CLONEESC, SDD, ESCPROC, TARGETPOS, CBT, CLONECTOR1, WALKBODY, CREATESUB = \
    'const makeab pizzactor body pizzaproc clonepizza cloneesc sdd escproc targetpos cbt clonector1 walkbody createsub'.split()
KIND = [PIZZACTOR, CONST, MAKEAB, BODY, PIZZAPROC, CLONEPIZZA, CLONEESC, CONST, SDD, BODY, ESCPROC, CONST, CONST, TARGETPOS, SDD, CBT, CLONECTOR1, CLONECTOR1, SDD, SDD, CONST, WALKBODY, CREATESUB]
CONSTS = {1: 0xEF, 7: 0x51C, 11: 0xCB, 12: 0x33E, 20: 0x1FE}
BODY_VT = {3: (0x86E1DC, 0x61A3A0), 9: (0x85B8DC, 0x62F360)}      # vtable que deja el cuerpo y direccion del colaborador al que salta
SDD_BODY = {8: 0x4B8750, 14: 0x64CEA0, 18: 0x68B690, 19: 0x68BA00}
CLONE_NEW = {5: 0x14, 6: 0x20, 16: 0x14, 17: 0x24}
PIZZA_VT, ESC_VT, WALK_VT = 0x86E1DC, 0x85B8DC, 0x86F364
NEWADDR, DELADDR, BASECTOR = 0x61A5A0, 0x61A5B0, 0x61A390
STANDCTOR, STANDPROC, ATAN, LIMIT, MUL3, GETGROUP, CBTADDR, CLEANUP, PTRDEL, TASKCDTOR, ROUTE = 0x62F310, 0x62F370, 0x53CBE0, 0x53CB50, 0x59C790, 0x6E3B00, 0x6E3C80, 0x571A00, 0x41B7D0, 0x61A3D0, 0x655140
ARREST_CTOR = {16: 0x68B620, 17: 0x68B990}
ARREST_PED = {16: 0x8, 17: 0x10}
GROUPS, GROUP_SIZE = 0xC1CDC0, 0x94


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


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    """un float que pasa por la x87 (fld / fstp): un NaN senalizante sale silencioso"""
    return (b | 0x400000) & M32 if is_nan(b) else b & M32


def f32_add(abits, bbits):
    """abits + bbits en float de 24 bits (fld a; fadd b; fstp dword), con el redondeo al mas cercano par; NaN: se devuelve el NaN (silenciado); inf - inf: indefinido (0xFFC00000)"""
    na, nb = is_nan(abits), is_nan(bbits)
    if na and nb:
        raise ValueError('dos NaN: ambiguo entre x87 y SSE')
    if na:
        return quiet(abits)
    if nb:
        return quiet(bbits)
    ia, ib = (abits & 0x7FFFFFFF) == 0x7F800000, (bbits & 0x7FFFFFFF) == 0x7F800000
    sa, sb = abits >> 31, bbits >> 31
    if ia and ib:
        return abits if sa == sb else 0xFFC00000
    if ia:
        return abits
    if ib:
        return bbits

    def dec(b):
        s, e, f = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
        return (-1 if s else 1) * (f if e == 0 else (f | 0x800000)), (-149 if e == 0 else e - 150)

    ma, ea = dec(abits)
    mb, eb = dec(bbits)
    e = min(ea, eb)
    total = (ma << (ea - e)) + (mb << (eb - e))
    if total == 0:
        return 0x80000000 if (sa and sb) else 0
    sign = 0x80000000 if total < 0 else 0
    mag = abs(total)
    length = mag.bit_length()
    shift = max(length - 24, -149 - e)
    if shift > 0:
        q, rem, half = mag >> shift, mag & ((1 << shift) - 1), 1 << (shift - 1)
        if rem > half or (rem == half and (q & 1)):
            q += 1
    else:
        q = mag << (-shift)
    ex = e + shift
    if q >= (1 << 24):
        q >>= 1
        ex += 1
    if q >= (1 << 23):
        biased = ex + 150
        if biased >= 255:
            return sign | 0x7F800000
        return sign | (biased << 23) | (q - (1 << 23))
    return sign | q


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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind in (PIZZACTOR, BODY):
        _k += ['prev_zero', 'prev_other']
    elif _kind == PIZZAPROC:
        _k += ['not_started', 'started', 'stopped', 'not_stopped', 'finished', 'running', 'wrap', 'flag_byte_high']
    elif _kind in (CLONEPIZZA, CLONEESC, CLONECTOR1):
        _k += ['mem_null', 'mem_set']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == ESCPROC:
        _k += ['contact_null', 'contact_set', 'model_match1', 'model_match2', 'model_none', 'matched', 'unmatched', 'model_negative', 'nan_speed']
    elif _kind == TARGETPOS:
        _k += ['boat', 'not_boat', 'matrix', 'no_matrix', 'result_nan', 'result_inf', 'index_zero', 'index_last']
    elif _kind == CBT:
        _k += ['group_zero', 'group_negative', 'group_large', 'anim_zero', 'ret_nan']
    elif _kind == WALKBODY:
        _k += ['obj_null', 'obj_set', 'obj_raw', 'route_null', 'route_set']
    elif _kind == CREATESUB:
        _k += ['type_cb', 'type_389', 'type_516', 'type_other', 'mem_null', 'mem_set']
    KEYS[_fn] = _k


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
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == MAKEAB:
        W.snap()
        ret = 1
    elif k == PIZZACTOR:
        W.snap()
        events.append([BASECTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, PIZZA_VT)
        A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0); A.wr(o + 0x10, 1, 0); A.wr(o + 0x11, 1, 0)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BODY:
        W.snap()
        vt, target = BODY_VT[fn]
        A.wr(o, 4, vt)
        events.append([target, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == PIZZAPROC:
        A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, a[5])
        A.wr(o + 0x10, 4, (a[6] & 0xFF) | ((a[7] & 0xFF) << 8) | ((a[10] << 16) & M32))
        W.snap()
        now = a[8]
        start, interval = a[4], a[5]
        if (a[6] & 0xFF) == 0:
            start, interval = now, 2000
            A.wr(o + 8, 4, start); A.wr(o + 0xC, 4, interval); A.wr(o + 0x10, 1, 1)
            marks.append('not_started')
        else:
            marks.append('started')
        if (a[7] & 0xFF) != 0:
            start = now
            A.wr(o + 8, 4, start); A.wr(o + 0x11, 1, 0)
            marks.append('stopped')
        else:
            marks.append('not_stopped')
        if (a[6] & 0xFF) > 1 or (a[7] & 0xFF) > 1:
            marks.append('flag_byte_high')
        if start + interval > M32:
            marks.append('wrap')
        ret = 1 if now >= ((start + interval) & M32) else 0
        marks.append('finished' if ret else 'running')
    elif k in (CLONEPIZZA, CLONEESC, CLONECTOR1):
        if fn in ARREST_PED:
            A.wr(o + ARREST_PED[fn], 4, a[8])
        W.snap()
        events.append([NEWADDR, CLONE_NEW[fn]])
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            if k == CLONEPIZZA:
                events.append([BASECTOR, W.tagv(mem), W.rd(mem, 4)])
                W.wr(mem, 4, a[7])
                W.wr(mem, 4, PIZZA_VT)
                W.wr(mem + 8, 4, 0); W.wr(mem + 0xC, 4, 0); W.wr(mem + 0x10, 1, 0); W.wr(mem + 0x11, 1, 0)
                ret = W.tagv(mem)
            elif k == CLONEESC:
                events.append([STANDCTOR, W.tagv(mem), 0, 1, 0, 0x41000000])
                W.wr(mem, 4, a[7])
                W.wr(mem, 4, ESC_VT)
                ret = W.tagv(mem)
            else:
                ped = A.rd(o + ARREST_PED[fn], 4)
                events.append([ARREST_CTOR[fn], W.tagv(mem), W.tagv(ped)])
                W.wr(mem, 4, a[7])
                ret = W.tagv(a[6])
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELADDR, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == ESCPROC:
        ped = W.raw(1, a[4])
        contact = 0 if a[5] == 0 else W.raw(2, a[6])
        B.wr(a[4] + 0x584, 4, contact); B.wr(a[4] + 0x55C, 4, a[15])
        if contact:
            Cc.wr(a[6] + 0x20, 4, a[7]); Cc.wr(a[6] + 0x44, 4, a[10]); Cc.wr(a[6] + 0x48, 4, a[11])
        W.snap()
        events.append([STANDPROC, W.tagv(this), W.tagv(ped)])
        ret = 1
        if contact:
            marks.append('contact_set')
            model = Cc.rd(a[6] + 0x22, 2)
            signed = model - 0x10000 if model >= 0x8000 else model
            m1, m2 = a[8] & 0xFFFF, a[9] & 0xFFFF
            if signed == m1:
                marks.append('model_match1')
            if signed == m2:
                marks.append('model_match2')
            if model >= 0x8000:
                marks.append('model_negative')
            if signed == m1 or signed == m2:
                vx, vy = Cc.rd(a[6] + 0x44, 4), Cc.rd(a[6] + 0x48, 4)
                events.append([ATAN, vx, vy, 0, 0])
                events.append([LIMIT, quiet(a[13])])
                B.wr(a[4] + 0x55C, 4, quiet(a[14]))
                ret = 0
                marks.append('matched')
                if is_nan(a[10]) or is_nan(a[11]):
                    marks.append('nan_speed')
            else:
                marks.append('model_none')
                marks.append('unmatched')
        else:
            marks.append('contact_null')
            marks.append('unmatched')
    elif k == TARGETPOS:
        veh = W.raw(1, a[4])
        matrix = 0 if a[8] == 0 else W.raw(2, a[9])
        A.wr(o + 0x18, 4, veh)
        B.wr(a[4] + 4, 4, a[5]); B.wr(a[4] + 8, 4, a[6]); B.wr(a[4] + 0xC, 4, a[7])
        B.wr(a[4] + 0x14, 4, matrix)
        B.wr(a[4] + 0x20, 4, ((a[13] << 16) & M32) | (a[14] & 0xFFFF))
        if matrix:
            Cc.wr(a[9] + 0x30, 4, a[10]); Cc.wr(a[9] + 0x34, 4, a[11]); Cc.wr(a[9] + 0x38, 4, a[12])
        D.wr(a[15] + 0x3C, 4, a[16])
        W.snap()
        boat = a[16] == 5
        d = a[18:21] if boat else a[21:24]
        events.append([MUL3, STACK_TAG, W.tagv(matrix), STACK_TAG, d[0], d[1], d[2]])
        pos = [Cc.rd(a[9] + 0x30 + 4 * i, 4) for i in range(3)] if matrix else [a[5], a[6], a[7]]
        for i in range(3):
            A.wr(o + 0xC + 4 * i, 4, f32_add(a[24 + i], pos[i]))
        marks.append('boat' if boat else 'not_boat')
        marks.append('matrix' if matrix else 'no_matrix')
        res = [f32_add(a[24 + i], pos[i]) for i in range(3)]
        if any(is_nan(r) for r in res):
            marks.append('result_nan')
        if any((r & 0x7FFFFFFF) == 0x7F800000 for r in res):
            marks.append('result_inf')
        if a[13] == 0:
            marks.append('index_zero')
        if a[13] == 19999:
            marks.append('index_last')
    elif k == CBT:
        W.snap()
        ecx = (GROUPS + a[4] * GROUP_SIZE) & M32
        events.append([GETGROUP, ecx, a[5]])
        events.append([CBTADDR, ecx, a[5]])
        ret = quiet(a[7])
        sg = a[4] - (1 << 32) if a[4] >= 0x80000000 else a[4]
        if sg == 0:
            marks.append('group_zero')
        if sg < 0:
            marks.append('group_negative')
        if sg >= 30:
            marks.append('group_large')
        if a[5] == 0:
            marks.append('anim_zero')
        if is_nan(a[7]):
            marks.append('ret_nan')
    elif k == WALKBODY:
        obj = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else (W.raw(2, a[5]) if a[4] == 2 else a[6]))
        route = 0 if a[7] == 0 else W.raw(1, a[8])
        A.wr(o + 0x1C, 4, obj); A.wr(o + 0x20, 4, route)
        W.snap()
        A.wr(o, 4, WALK_VT)
        if obj:
            events.append([CLEANUP, W.tagv(obj), W.tagv((this + 0x1C) & M32)])
        if route:
            events.append([PTRDEL, W.tagv(route), W.rd(route, 4)])
        events.append([TASKCDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[10])
        marks.append('obj_null' if a[4] == 0 else ('obj_raw' if a[4] == 3 else 'obj_set'))
        marks.append('route_null' if a[7] == 0 else 'route_set')
    elif k == CREATESUB:
        W.snap()
        t = a[4]
        if t == 0xCB:
            marks.append('type_cb')
            events.append([NEWADDR, 0x20])
            if a[6] == 0:
                ret = 0
                marks.append('mem_null')
            else:
                marks.append('mem_set')
                mem = W.raw(1, a[7])
                events.append([STANDCTOR, W.tagv(mem), 500, 0, 0, 0x41000000])
                W.wr(mem, 4, a[9])
                ret = W.tagv(a[8])
        elif t == 0x389:
            marks.append('type_389')
            events.append([ROUTE, W.tagv(this), W.tagv(a[5])])
            ret = W.tagv(a[10])
        else:
            marks.append('type_516' if t == 0x516 else 'type_other')
            ret = 0
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
