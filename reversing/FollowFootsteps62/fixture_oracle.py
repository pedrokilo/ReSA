"""Oraculo independiente del banco FollowFootsteps62: CTaskComplexFollowPedFootsteps::{ControlSubTask, CreateSubTask, CreateNextSubTask, CreateFirstSubTask, MakeAbortable}.

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. Aritmetica x87 con la palabra de control de 24 bits: cada operacion
redondea la mantisa a 24 bits con el rango de exponente extendido (r24); lo que el EXE guarda con `fstp dword` pasa por f32 (redondeo a float, desbordamiento a inf).

this = ECX (ESI): +8 m_pSubTask, +0xC m_targetPed, +0x10 m_subTaskCreateCheckTimer, +0x14 m_lineOfSightCheckTimer, +0x18 m_pointRoute, +0x1C WORD m_moveState, +0x1E BYTE bit0 = m_updateGoToPoint.
CPointRoute: +0 contador (DWORD), +4 puntos de 12 bytes (maximo 8). Posicion de un peaton: [p+0x14] != 0 ? [p+0x14]+0x30 : p+4. Bandera bInVehicle del peaton: bit 8 de [p+0x46C]. Contador: [[p+0x47C]+0x274].
Tipos: 0xCB STAND_STILL, 0x1F4 HIT_HEAD, 0x384 GO_TO_POINT, 0x38B SEEK_ENTITY, 0x516 FINISHED, 0xC8 NONE.

MakeAbortable 0x694ED0:  mov ecx,[ecx+8] ; mov eax,[ecx] ; jmp [eax+18h]            (salta a la ranura 6 de la subtarea con los argumentos de pila de la llamada: ped, prioridad, evento)
CreateNextSubTask 0x694EE0 (ret 4):
  eax=[esi+0Ch] (objetivo); si 0 -> CreateSubTask(0x516, ped) por [vtable+2Ch]. si no: t = m_pSubTask->GetTaskType() (con signo): t > 0x1F4: t-0x384 == 0 -> L0F ; t-0x38B == 0 -> L6E ; resto -> 0x516;
  t == 0x1F4 -> 0xCB; t < 0x1F4: t == 0xC8 -> 0x516 ; t == 0xCB -> L0F ; resto -> 0x516.
  L0F (0xCB y 0x384): ruta=[esi+18h]; si ruta != 0 y [ruta] != 0 -> CreateSubTask(0x384) ; si no -> CreateSubTask(0xCB).
  L6E (0x38B): d = (objetivo - peaton) sin guardar a memoria (r24); s = (dz*dz + dx*dx) + dy*dy ; si s > 1.4f (0x870C20; fcomp + test ah,41h + jne) -> 0x38B ; si no -> 0xCB.
CreateFirstSubTask 0x695000 (ret 4): objetivo 0 -> 0x516 ; si no s = (dx*dx + dz*dz) + dy*dy, s > 1.4f -> 0x38B, si no 0xCB. (sin GetTaskType).
CreateSubTask 0x695E40 (ret 8): objetivo == 0 o ruta == 0 -> 0 ; tipo con signo: > 0x384: 0x38B -> L_A8 ; 0x516 -> 0 ; otro -> 0 ; == 0x384 -> GOTO ; 0xCB -> STAND ; 0x1F4 -> HIT ; otro -> 0.
  GOTO: pt = ruta[0] (copia a la pila) ; ruta.RemoveAt(0) (0x5F13A0: for i=0; i<n-1: e[i]=e[i+1]; --n) ; new(0x30) ; nulo -> 0 ; ctor 0x667CD0(this, (int16 m_moveState sign-extendido), &pt, 0.01f (0x870C28), 0, 0).
  STAND: new(0x20) ; ctor 0x62F310(10000, 0, 0, 8.0f).  HIT: new(0x10) ; ctor 0x653060().
  SEEK: maxDist = (objetivo.bInVehicle ? 1.0*4.0 : 1.0) ; new(0x4C) ; ctor 0x46AC10(objetivo, 50000, 1000, maxDist, 2.0, 2.0, 1, 1).
ControlSubTask 0x695090 (ret 4): ver el flujo en el README (resumen):
  objetivo == 0 o ruta == 0 -> FINISHED ; IsAlive(objetivo) (AL) == 0 -> FINISHED ; GetTaskType == 0x38B -> devuelve la subtarea ; bInVehicle -> SEEK ; contador > 30 (con signo) -> HIT_HEAD ;
  si now - m_lineOfSightCheckTimer > 500 (sin signo): timer = now ; GetIsLineOfSightClear(pos peaton, pos objetivo, 1,0,0,1,0,0,0) -> bit 0 de m_updateGoToPoint = AL & 1 ;
  D = objetivo - peaton (floats guardados) ; sq = (x²+y²)+z² (guardado) ; d2 = sqrt(y²+x²) (guardado) ;
  si 1.4f*1.4f > sq: (GetTaskType != 0xCB) -> (bit0 == 0 -> devuelve la subtarea ; si no ruta=0, STAND) ; si no (o con tipo 0xCB) sigue ;
  64 < sq -> SEEK ; d2 < 1.0 y |dz| > 2.0 -> SEEK ; si no:
   si bit0: ruta=0, Add(objetivo), timer2 = now - 332, GetSimplestActiveTask, IsGoToTask(t) -> (GetSimplestActiveTask, UpdatePoint(objetivo, 0.5, 0)).
   si now - m_subTaskCreateCheckTimer < 166 (sin signo) o n >= 8: GetTaskType == 0xCB y n > 1 -> GO_TO_POINT (aborta) ; si no devuelve la subtarea.
   si no: timer2 = now ; n == 0 -> Add ; si no v = objetivo - ultimo ; (0.35f*0.35f < sq(v)) -> [n >= 2: w = ultimo - penultimo ; Normalise(w) ; Normalise(v) ; dot = (wy*vy + wx*vx) + wz*vz ; dot >= 0.95f -> RemoveAt(n-1)] ; Add(objetivo) ;
     total = 0.0f ; para i = 1..n-1: total = sq(e[i] - e[0]) + total (guardado) ; total2 = sq(peaton - objetivo) + total (sin guardar) ; m_moveState = (3.5f*3.5f < total2) ? 6 : 4 ;
     GetTaskType == 0x384 -> subtarea.m_moveState (+8) = m_moveState (sign-extendido) ; devuelve la subtarea.
  Abortar: m_pSubTask->MakeAbortable(ped, 1, 0) (ranura 6) ; AL != 0 -> CreateSubTask(nuevo tipo, ped) ([vtable+2Ch]); si no devuelve la subtarea.
Normalise (0x59C910, ECX = vector): sq = (x²+y²)+z² ; si sq <= 0 (ordenado): x = 1.0, retorno ; si no r = 1/sqrt(sq) ; x,y,z *= r (guardados).
Ningun llamador del EXE lee ECX/EDX/EAX de salida mas alla del retorno (ver README.md).
"""
import math
import struct

CASES = 4096
FUNCS = ('CTaskComplexFollowPedFootsteps::ControlSubTask', 'CTaskComplexFollowPedFootsteps::CreateSubTask', 'CTaskComplexFollowPedFootsteps::CreateNextSubTask',
         'CTaskComplexFollowPedFootsteps::CreateFirstSubTask', 'CTaskComplexFollowPedFootsteps::MakeAbortable')
NAMES = list(FUNCS)
CASES_BY_NAME = {n: CASES for n in FUNCS}
M32 = 0xFFFFFFFF
INF = float('inf')

TY_STAND, TY_HIT, TY_GOTO, TY_SEEK, TY_FINISHED, TY_NONE = 0xCB, 0x1F4, 0x384, 0x38B, 0x516, 0xC8
SIZES = {TY_GOTO: 0x30, TY_STAND: 0x20, TY_HIT: 0x10, TY_SEEK: 0x4C}


def u32(v):
    return v & M32


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def bf(u):
    return struct.unpack('<f', struct.pack('<I', u & M32))[0]


def fb(f):
    return struct.unpack('<I', struct.pack('<f', f))[0]


def f32(x):
    """Guardar en memoria como float: redondeo al mas cercano par; desbordamiento a +-inf."""
    if x != x or x in (INF, -INF):
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(INF, x)


def r24(x):
    """Redondeo de la mantisa a 24 bits (palabra de control x87 de 24 bits), exponente extendido."""
    if x != x or x == 0 or x in (INF, -INF):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)), e - 24)


def sub(a, b):
    return r24(a - b)


def mul(a, b):
    return r24(a * b)


def sqmag(v):
    """0x406DA0: (x*x + y*y) + z*z, resultado en ST0 (sin guardar)."""
    return r24(r24(mul(v[0], v[0]) + mul(v[1], v[1])) + mul(v[2], v[2]))


def sqrt24(x):
    if x != x:
        return x
    if x < 0:
        return float('nan')
    return r24(math.sqrt(x))


def normalise(v):
    """0x59C910 (en sitio)."""
    sq = sqmag(v)
    if sq == 0:
        return [1.0, v[1], v[2]]
    r = r24(1.0 / sqrt24(sq)) if sq == sq else float('nan')
    if sq == INF:
        r = 0.0
    return [f32(mul(r, v[0])), f32(mul(r, v[1])), f32(mul(r, v[2]))]


K14 = bf(0x3FB33333)
K_NEAR2 = r24(K14 * K14)
K_FAR2 = 64.0
K035 = bf(0x3EB33333)
K_STEP2 = r24(K035 * K035)
K_RUN2 = 12.25
K095 = bf(0x3F733333)


class Route:
    def __init__(self, count, ent):
        self.count = count & M32
        self.e = [list(ent[3 * i:3 * i + 3]) for i in range(8)]

    def add(self, pos):
        if s32(self.count) >= 8:
            return
        self.e[self.count] = list(pos)
        self.count = u32(self.count + 1)

    def remove_at(self, idx):
        last = s32(self.count) - 1
        i = idx
        while i < last:
            self.e[i] = list(self.e[i + 1])
            i += 1
        self.count = u32(self.count - 1)

    def raw(self):
        b = bytearray(struct.pack('<I', self.count))
        for ent in self.e:
            for w in ent:
                b += struct.pack('<I', w)
        return bytes(b)


def diff_bytes(old, new):
    return [[i, new[i]] for i in range(len(old)) if old[i] != new[i]]


def route_changes(inp, route):
    return diff_bytes(Route(inp['count'], inp['ent']).raw(), route.raw())


def pos_label(inp, who):
    if who == 'ped':
        return 'PM+48' if inp['ped_mat'] else 'D+4'
    return 'TM+48' if inp['tgt_mat'] else 'G+4'


def create_ret(inp):
    k = inp['create_kind']
    if k == 0:
        return 'A+0'
    if k == 1:
        return 0
    return inp['create_val']


def this_changes(inp, new_create, new_los, new_move, new_flags):
    old = bytearray(0x20)
    old[0x10:0x14] = struct.pack('<I', u32(inp['now'] - inp['create_timer_delta']))
    old[0x14:0x18] = struct.pack('<I', u32(inp['now'] - inp['los_timer_delta']))
    old[0x1C:0x1E] = struct.pack('<H', inp['move_state'] & 0xFFFF)
    old[0x1E] = inp['flags_byte'] & 0xFF
    new = bytearray(old)
    new[0x10:0x14] = struct.pack('<I', u32(new_create))
    new[0x14:0x18] = struct.pack('<I', u32(new_los))
    new[0x1C:0x1E] = struct.pack('<H', new_move & 0xFFFF)
    new[0x1E] = new_flags & 0xFF
    return diff_bytes(bytes(old), bytes(new))


def base_want(inp):
    return {'events': [], 'nevents': 0, 'this_changes': [], 'route_changes': [], 'sub_move': inp['sub_move0'], 'sub_rest_intact': True, 'others_intact': True, 'timer': inp['now']}


# ------------------------------------------------------------------------------------------------ MakeAbortable
def expected_abortable(inp, case):
    w = base_want(inp)
    w['events'] = [['abort', 'S+0', inp['ped_arg'], inp['prio'], inp['ev_ptr']]]
    w['nevents'] = 1
    w['ret_al'] = inp['abort_ret'] & 0xFF
    stats = {'family%d' % inp['family']: True, 'ret_true': w['ret_al'] != 0, 'ret_false': w['ret_al'] == 0, 'ret_noncanon': w['ret_al'] > 1,
             'ret_hi_bits': (inp['abort_ret'] & 0xFFFFFF00) != 0, 'prio_nonstd': inp['prio'] > 2, 'ev_null': inp['ev_ptr'] == 0, 'ped_null': inp['ped_arg'] == 0}
    return w, stats


# ------------------------------------------------------------------------------------------------ CreateNext / CreateFirst
def dist_seek(inp, order):
    t = [bf(x) for x in inp['tgt']]
    p = [bf(x) for x in inp['ped']]
    d = [sub(t[i], p[i]) for i in range(3)]   # sin guardar a memoria (r24)
    if order == 'zxy':
        s = r24(r24(mul(d[2], d[2]) + mul(d[0], d[0])) + mul(d[1], d[1]))
    else:
        s = r24(r24(mul(d[0], d[0]) + mul(d[2], d[2])) + mul(d[1], d[1]))
    return s


def expected_next(inp, case, first):
    w = base_want(inp)
    ev = []
    stats = {'family%d' % inp['family']: True}
    ty = None
    if inp['tgt_null']:
        ty = TY_FINISHED
        stats['tgt_null'] = True
    elif first:
        s = dist_seek(inp, 'xzy')
        ty = TY_SEEK if s > K14 else TY_STAND
        stats['far' if ty == TY_SEEK else 'near'] = True
        stats['sum_nan'] = s != s
    else:
        ev.append(['type', 'S+0'])
        t = s32(inp['sub_type'])
        route_ok = (not inp['route_null']) and inp['count'] != 0
        if t > 0x1F4:
            if t == TY_GOTO:
                ty = TY_GOTO if route_ok else TY_STAND
                stats['from_goto'] = True
                stats['route_ok' if route_ok else 'route_empty'] = True
            elif t == TY_SEEK:
                s = dist_seek(inp, 'zxy')
                ty = TY_SEEK if s > K14 else TY_STAND
                stats['from_seek'] = True
                stats['far' if ty == TY_SEEK else 'near'] = True
                stats['sum_nan'] = s != s
            else:
                ty = TY_FINISHED
                stats['other_type'] = True
        elif t == 0x1F4:
            ty = TY_STAND
            stats['from_hit'] = True
        else:
            if t == TY_NONE:
                ty = TY_FINISHED
                stats['from_none'] = True
            elif t == TY_STAND:
                ty = TY_GOTO if route_ok else TY_STAND
                stats['from_stand'] = True
                stats['route_ok' if route_ok else 'route_empty'] = True
            else:
                ty = TY_FINISHED
                stats['other_type'] = True
    ev.append(['create', 'T+0', ty, 'D+0'])
    w['events'] = ev
    w['nevents'] = len(ev)
    w['ret'] = create_ret(inp)
    stats['create_' + {0: 'alloc', 1: 'null'}.get(inp['create_kind'], 'value')] = True
    stats['ty_%x' % ty] = True
    return w, stats


# ------------------------------------------------------------------------------------------------ CreateSubTask
def expected_create_sub(inp, case):
    w = base_want(inp)
    route = Route(inp['count'], inp['ent'])
    stats = {'family%d' % inp['family']: True}
    t = s32(inp['type'])
    ev = []
    alloc = None
    ret = 0
    if inp['tgt_null'] or inp['route_null']:
        stats['null_args'] = True
    else:
        kind = None
        pt = None
        if t == TY_GOTO:
            kind = TY_GOTO
            pt = list(route.e[0])
            route.remove_at(0)
        elif t == TY_SEEK:
            kind = TY_SEEK
        elif t == TY_STAND:
            kind = TY_STAND
        elif t == TY_HIT:
            kind = TY_HIT
        stats['kind_%s' % (hex(kind) if kind else 'none')] = True
        if kind:
            ev.append(['new', SIZES[kind]])
            if inp['new_null']:
                stats['new_null'] = True
            else:
                ret = 'A+0'
                alloc = {}
                if kind == TY_GOTO:
                    mv = inp['move_state'] & 0xFFFF
                    alloc = {2: u32(mv - 0x10000 if mv & 0x8000 else mv), 3: pt[0], 4: pt[1], 5: pt[2], 6: 0x3C23D70A}
                    stats['goto_neg_move'] = bool(mv & 0x8000)
                elif kind == TY_STAND:
                    alloc = {2: 10000}
                elif kind == TY_SEEK:
                    veh = bool(inp['tgt_in_veh'])
                    alloc = {3: 'G+0', 4: 50000, 5: 1000, 6: 0x40800000 if veh else 0x3F800000, 7: 0x40000000, 9: 0x40000000}
                    stats['seek_veh' if veh else 'seek_walk'] = True
            if kind == TY_GOTO:
                stats['goto_count_%d' % min(inp['count'], 9)] = True
    w['events'] = ev
    w['nevents'] = len(ev)
    w['ret'] = ret
    w['route_changes'] = route_changes(inp, route)
    w['alloc_guards_intact'] = True
    w['alloc_spec'] = alloc if alloc is not None else {}
    stats['ret_alloc'] = ret == 'A+0'
    stats['route_changed'] = bool(w['route_changes'])
    return w, stats


# ------------------------------------------------------------------------------------------------ ControlSubTask
def expected_control(inp, case):
    w = base_want(inp)
    stats = {'family%d' % inp['family']: True}
    S, ped_l, tgt_l = 'S+0', 'D+0', 'G+0'
    ev = []
    now = inp['now']
    route = Route(inp['count'], inp['ent'])
    create_timer = u32(now - inp['create_timer_delta'])
    los_timer = u32(now - inp['los_timer_delta'])
    move = inp['move_state'] & 0xFFFF
    flags = inp['flags_byte'] & 0xFF
    sub_move = inp['sub_move0']
    new_type = None
    default = True   # devolver la subtarea
    tp = [bf(x) for x in inp['tgt']]
    pp = [bf(x) for x in inp['ped']]
    tgt_pos_l = pos_label(inp, 'tgt')
    ped_pos_l = pos_label(inp, 'ped')

    def path():
        nonlocal new_type
        nonlocal create_timer, los_timer, move, flags, sub_move
        if inp['tgt_null'] or inp['route_null']:
            stats['p_null'] = True
            new_type = TY_FINISHED
            return
        ev.append(['alive', tgt_l])
        if (inp['alive_ret'] & 0xFF) == 0:
            stats['p_dead'] = True
            new_type = TY_FINISHED
            return
        ev.append(['type', S])
        if inp['sub_type'] == TY_SEEK:
            stats['p_seek_sub'] = True
            return
        if inp['tgt_in_veh']:
            stats['p_inveh'] = True
            new_type = TY_SEEK
            return
        if s32(inp['counter']) > 30:
            stats['p_hithead'] = True
            new_type = TY_HIT
            return
        if u32(now - los_timer) > 500:
            los_timer = now
            ev.append(['los', ped_pos_l, inp['ped'], tgt_pos_l, inp['tgt'], 0x01000001, 0])
            flags = (flags & 0xFE) | (inp['los_ret'] & 1)
            stats['p_los_updated'] = True
            stats['los_ret_even_nonzero'] = (inp['los_ret'] & 0xFF) != 0 and (inp['los_ret'] & 1) == 0
        else:
            stats['p_los_skipped'] = True
        D = [f32(sub(tp[i], pp[i])) for i in range(3)]
        sq = f32(sqmag(D))
        d2 = f32(sqrt24(r24(mul(D[1], D[1]) + mul(D[0], D[0]))))
        stats['sq_nan'] = sq != sq
        near = K_NEAR2 > sq
        stand_type = None
        if near:
            ev.append(['type', S])
            stand_type = inp['sub_type'] == TY_STAND
            if not stand_type:
                if not (flags & 1):
                    stats['p_near_return'] = True
                    return
                route.count = 0
                new_type = TY_STAND
                stats['p_near_stand'] = True
                return
            stats['p_near_but_stand'] = True
        if K_FAR2 < sq:
            stats['p_far'] = True
            new_type = TY_SEEK
            return
        if d2 < 1.0 and abs(D[2]) > 2.0:
            stats['p_2d_z'] = True
            new_type = TY_SEEK
            return
        stats['p_normal'] = True
        if flags & 1:
            stats['p_flag'] = True
            route.count = 0
            route.add(inp['tgt'])
            create_timer = u32(now - 332)
            ev.append(['simplest', 'I+4'])
            task = 0 if inp['simplest_null'] else 'K+0'
            ev.append(['isgoto', task])
            if inp['is_goto_ret'] & 0xFF:
                stats['p_update_point'] = True
                ev.append(['simplest', 'I+4'])
                ev.append(['update', task, tgt_pos_l, inp['tgt'], 0x3F000000, 0])
        if u32(now - create_timer) < 166 or s32(route.count) >= 8:
            stats['p_wait'] = True
            ev.append(['type', S])
            if inp['sub_type'] == TY_STAND and s32(route.count) > 1:
                stats['p_stand_goto'] = True
                new_type = TY_GOTO
            return
        create_timer = now
        n = s32(route.count)
        if n == 0:
            route.add(inp['tgt'])
            stats['p_first_add'] = True
        else:
            last = [bf(x) for x in route.e[n - 1]]
            v = [f32(sub(tp[i], last[i])) for i in range(3)]
            if K_STEP2 < sqmag(v):
                stats['p_step'] = True
                if n >= 2:
                    prev = [bf(x) for x in route.e[n - 2]]
                    wv = [f32(sub(last[i], prev[i])) for i in range(3)]
                    wv = normalise(wv)
                    v = normalise(v)
                    dot = r24(r24(mul(wv[1], v[1]) + mul(wv[0], v[0])) + mul(wv[2], v[2]))
                    stats['dot_nan'] = dot != dot
                    if not (dot < K095 or dot != dot):
                        stats['p_remove'] = True
                        route.remove_at(n - 1)
                    else:
                        stats['p_keep'] = True
                route.add(inp['tgt'])
            else:
                stats['p_step_skip'] = True
        first = [bf(x) for x in route.e[0]]
        total = 0.0
        for i in range(1, s32(route.count)):
            d = [f32(sub(bf(route.e[i][j]), first[j])) for j in range(3)]
            total = f32(r24(sqmag(d) + total))
        pd = [f32(sub(pp[i], tp[i])) for i in range(3)]
        total2 = r24(sqmag(pd) + total)
        move = 6 if K_RUN2 < total2 else 4
        stats['p_run' if move == 6 else 'p_walk'] = True
        ev.append(['type', S])
        if inp['sub_type'] == TY_GOTO:
            stats['p_move_written'] = True
            sub_move = u32(move)

    path()
    ret = 'S+0'
    if new_type is not None:
        ev.append(['abort', S, ped_l, 1, 0])
        if inp['abort_ret'] & 0xFF:
            ev.append(['create', 'T+0', new_type, ped_l])
            ret = create_ret(inp)
            stats['abort_true'] = True
        else:
            stats['abort_false'] = True
        stats['nt_%x' % new_type] = True
    w['events'] = ev
    w['nevents'] = len(ev)
    w['ret'] = ret
    w['this_changes'] = this_changes(inp, create_timer, los_timer, move, flags)
    w['route_changes'] = route_changes(inp, route)
    w['sub_move'] = sub_move
    stats['ret_sub'] = ret == 'S+0'
    stats['this_changed'] = bool(w['this_changes'])
    stats['route_changed'] = bool(w['route_changes'])
    return w, stats


def expected(function, inp, case):
    if function == FUNCS[0]:
        return expected_control(inp, case)
    if function == FUNCS[1]:
        return expected_create_sub(inp, case)
    if function == FUNCS[2]:
        return expected_next(inp, case, False)
    if function == FUNCS[3]:
        return expected_next(inp, case, True)
    if function == FUNCS[4]:
        return expected_abortable(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def check_alloc(inp, observed_alloc, spec, case):
    """Palabras del objeto construido por los constructores reales que dependen de los argumentos (el resto son bytes de relleno aleatorios que el constructor no toca)."""
    for idx, want in spec.items():
        if observed_alloc[int(idx)] != want:
            raise RuntimeError('Objeto construido distinto en el caso %d: palabra %s = %r, esperado %r' % (case, idx, observed_alloc[int(idx)], want))


def _base(**kw):
    inp = dict(fn=0, family=0, eax=0, edx=0, stack_seed=0, tgt_null=0, route_null=0, move_state=4, flags_byte=0, create_timer_delta=1000, los_timer_delta=100, now=100000,
               tgt_mat=0, ped_mat=0, tgt_in_veh=0, tgt_noise=0, counter=0, tgt=[fb(5.0), 0, 0], tgt_alt=[0, 0, 0], ped=[0, 0, 0], ped_alt=[0, 0, 0], count=0, ent=[0] * 24,
               sub_type=TY_STAND, alive_ret=1, los_ret=1, simplest_null=0, is_goto_ret=0, abort_ret=0, create_kind=0, create_val=0xC0DE0001, new_null=0, type=TY_STAND, prio=1,
               ev_ptr=0, ped_arg=0, sub_move0=0x5A5A1234)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # constantes
    assert K_NEAR2 == r24(bf(0x3FB33333) ** 2) and K_RUN2 == 12.25
    assert f32(1e39) == INF and f32(-1e39) == -INF and r24(1.0 + 2.0 ** -25) == 1.0 and r24(1.0 + 3 * 2.0 ** -25) == 1.0 + 2.0 ** -23 * 1  # 3*2^-25 = 0.75 ulp24 -> redondea al siguiente
    # MakeAbortable
    w, st = expected_abortable(_base(fn=4, abort_ret=0x1200, prio=7, ev_ptr=5, ped_arg=9), 0)
    assert w['ret_al'] == 0 and w['events'] == [['abort', 'S+0', 9, 7, 5]]
    # Next: sin objetivo -> FINISHED ; tipos
    w, st = expected_next(_base(tgt_null=1), 0, False)
    assert w['events'] == [['create', 'T+0', TY_FINISHED, 'D+0']]
    w, st = expected_next(_base(sub_type=TY_STAND, count=1), 0, False)
    assert w['events'] == [['type', 'S+0'], ['create', 'T+0', TY_GOTO, 'D+0']]
    w, st = expected_next(_base(sub_type=TY_GOTO, count=0), 0, False)
    assert w['events'][-1][2] == TY_STAND
    w, st = expected_next(_base(sub_type=TY_HIT), 0, False)
    assert w['events'][-1][2] == TY_STAND
    w, st = expected_next(_base(sub_type=0x80000384), 0, False)
    assert w['events'][-1][2] == TY_FINISHED
    # Next/First compara la SUMA de cuadrados (sin raiz) con 1.4: distancia 1.4 -> 1.96 > 1.4 -> SEEK ; distancia 1.1 -> 1.21 -> STAND
    w, st = expected_next(_base(sub_type=TY_SEEK, ped=[0, 0, 0], tgt=[fb(1.4), 0, 0]), 0, False)
    assert w['events'][-1][2] == TY_SEEK
    w, st = expected_next(_base(sub_type=TY_SEEK, ped=[0, 0, 0], tgt=[fb(1.1), 0, 0]), 0, False)
    assert w['events'][-1][2] == TY_STAND
    w, st = expected_next(_base(ped=[0, 0, 0], tgt=[fb(1.1), 0, 0]), 0, True)
    assert w['events'] == [['create', 'T+0', TY_STAND, 'D+0']]
    # CreateSubTask
    r = Route(2, [fb(1.0), 0, 0, fb(2.0), 0, 0] + [0] * 18)
    r.remove_at(0)
    assert r.count == 1 and r.e[0] == [fb(2.0), 0, 0]
    w, st = expected_create_sub(_base(type=TY_GOTO, count=2, ent=[fb(1.0), 0, 0, fb(2.0), 0, 0] + [0] * 18, move_state=0x8004), 0)
    assert w['events'] == [['new', 0x30]] and w['alloc_spec'][2] == 0xFFFF8004 and w['alloc_spec'][3] == fb(1.0) and w['route_changes']
    w, st = expected_create_sub(_base(type=TY_GOTO, count=0), 0)
    assert w['events'] == [['new', 0x30]] and w['route_changes'][0] == [0, 0xFF]
    w, st = expected_create_sub(_base(type=TY_SEEK, tgt_in_veh=1, new_null=1), 0)
    assert w['events'] == [['new', 0x4C]] and w['ret'] == 0
    w, st = expected_create_sub(_base(type=TY_FINISHED), 0)
    assert w['events'] == [] and w['ret'] == 0
    # Control: sin objetivo -> FINISHED ; AL del aborto
    w, st = expected_control(_base(tgt_null=1, abort_ret=1), 0)
    assert w['events'] == [['abort', 'S+0', 'D+0', 1, 0], ['create', 'T+0', TY_FINISHED, 'D+0']]
    w, st = expected_control(_base(abort_ret=0x100, alive_ret=0), 0)
    assert w['ret'] == 'S+0' and w['events'] == [['alive', 'G+0'], ['abort', 'S+0', 'D+0', 1, 0]]
    w, st = expected_control(_base(sub_type=TY_SEEK), 0)
    assert w['events'] == [['alive', 'G+0'], ['type', 'S+0']] and w['ret'] == 'S+0'
    # temporizador de linea de vision: > 500; AL & 1
    w, st = expected_control(_base(sub_type=TY_STAND, los_timer_delta=501, los_ret=2, flags_byte=1), 0)
    assert w['events'][2][0] == 'los' and [0x14, 0xA0] in w['this_changes'] and [0x1E, 0] in w['this_changes']
    w, st = expected_control(_base(sub_type=TY_STAND, los_timer_delta=500, los_ret=2, flags_byte=1), 0)
    assert all(e[0] != 'los' for e in w['events'])
    # lejos -> SEEK ; 2D < 1 y dz > 2 -> SEEK
    w, st = expected_control(_base(tgt=[fb(8.0), 0, 0], abort_ret=1, los_timer_delta=0), 0)
    assert 'p_far' not in st    # 64 < 64 es falso
    w, st = expected_control(_base(tgt=[fb(8.001), 0, 0], abort_ret=1, los_timer_delta=0), 0)
    assert w['events'][-1][2] == TY_SEEK and 'p_far' in st
    w, st = expected_control(_base(tgt=[fb(0.5), 0, fb(2.5)], abort_ret=1, los_timer_delta=0), 0)
    assert w['events'][-1][2] == TY_SEEK and 'p_2d_z' in st
    # normal, indicador bajado, temporizador >= 166, ruta vacia: se anade el objetivo; GoToPoint recibe el estado
    w, st = expected_control(_base(tgt=[fb(3.0), 0, 0], sub_type=TY_GOTO, los_timer_delta=0, flags_byte=0, los_ret=0, create_timer_delta=1000, count=0), 0)
    assert w['sub_move'] == 4 and 'p_first_add' in st and w['route_changes']
    # paso pequeno: no se anade
    ent = [fb(4.9), 0, 0] + [0] * 21
    w, st = expected_control(_base(tgt=[fb(5.0), 0, 0], sub_type=TY_STAND, los_timer_delta=0, flags_byte=0, los_ret=0, create_timer_delta=1000, count=1, ent=ent), 0)
    assert 'p_step_skip' in st
    # paso grande con coseno 1: se elimina el ultimo punto antes de anadir
    ent = [fb(1.0), 0, 0, fb(3.0), 0, 0] + [0] * 18
    w, st = expected_control(_base(tgt=[fb(5.0), 0, 0], sub_type=TY_STAND, los_timer_delta=0, flags_byte=0, los_ret=0, create_timer_delta=1000, count=2, ent=ent), 0)
    assert 'p_remove' in st
    # coseno 0: se conserva
    ent = [fb(3.0), 0, 0, fb(3.0), fb(1.0), 0] + [0] * 18
    w, st = expected_control(_base(tgt=[fb(5.0), 0, 0], sub_type=TY_STAND, los_timer_delta=0, flags_byte=0, los_ret=0, create_timer_delta=1000, count=2, ent=ent), 0)
    assert 'p_keep' in st
    # espera: temporizador 166 y STAND con > 1 puntos -> GO_TO_POINT
    ent = [0] * 24
    w, st = expected_control(_base(tgt=[fb(5.0), 0, 0], sub_type=TY_STAND, los_timer_delta=0, flags_byte=0, los_ret=0, create_timer_delta=165, count=3, ent=ent, abort_ret=1), 0)
    assert 'p_stand_goto' in st and w['events'][-1][2] == TY_GOTO
    w, st = expected_control(_base(tgt=[fb(5.0), 0, 0], sub_type=TY_STAND, los_timer_delta=0, flags_byte=0, los_ret=0, create_timer_delta=166, count=3, ent=ent, abort_ret=1), 0)
    assert 'p_stand_goto' not in st
    print('ok')
