"""Oraculo independiente del banco FightProcessPed31: CTaskSimpleFight::ProcessPed (0x629920).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco (estado de la tarea, de las asociaciones, del ped, de la tabla de combos
y de los falsos). Todo lo que la funcion toca es memoria falsa del banco o globales que el banco fija y restaura; los colaboradores se sustituyen por el mismo falso en ambos lados.

0x629920 (thiscall, ECX=this -> ESI, 1 argumento de pila (ped), `ret 4`; sin marco SEH). Campos de la tarea: +8 terminada, +9 en control, +0xA bloques referenciados, +0xC grupo exigido, +0x10 periodo ocioso (u16),
+0x12 contador (u16), +0x18 objetivo, +0x1C anim de golpe, +0x20 anim de reposo, +0x24 combo (i8), +0x25 movimiento (i8), +0x26 siguiente comando (u8), +0x27 ultimo comando (u8). Asociacion: +0x14 jerarquia (tiempo
total en +0x10), +0x18 peso, +0x1C delta, +0x20 tiempo, +0x28 paso, +0x2C id (u16), +0x2E banderas (u16). Fila de la tabla (0xC170D0 + (combo-4)*0x88): +0 grupo, +8+4m golpe, +0x1C+4m cadena, +0x48/+0x4C bloqueo,
+0x50+m nivel, +0x5C+4m sonido, +0x85 byte alto de banderas. Comparaciones x87 (fcomp + fnstsw + test ah): "test ah,41h ; jne salta" = sigue solo si a > b ; "test ah,1 ; jne salta" = sigue solo si a >= b ;
"test ah,5 ; jp salta" = sigue solo si a < b ; con NaN ninguna se cumple. La x87 va con el FCW de 24 bits: cada operacion es de float.

  629927  si finalizada: si hay reposo -> SetDeleteCallback(0x4D1390,0) ; si peso>0 y delta>=0 -> BlendAnimation(clump=[ped+18h], grupo=[ped+4D4h], 3, 8.0) ; [esi+20h]=0 ; devuelve 1
  629990  si combo!=0 y en control: contador=0 ; si no contador += (u16)ftol(paso*0.02*1000.0) (_ftol2: NaN/inf/|x|>=2^63 -> bajo 16 bits 0)
  6299BF  si no en control: (i8)siguiente < 1 -> devuelve 0 ; si hay anim y su id == 0x2F -> devuelve 0
  6299E2  sin reposo: ultimo en {1,0xF,0x10,0x11} -> finalizada=1 ; si no, sin anim y no (movestate>4 y jugador): arma=[ped+5A0+movsx([ped+718])*1Ch] ; combo=GetWeaponInfo(arma,1)->[+6Eh] ; si 4 -> [ped+72Dh] ;
          fila=(i8)combo-4 ; si no (byte [fila+0x85] & 4): elegido=4 ; si grupo==0x21: elegido=combo ; si grupo != exigido: (si referenciado: RemoveAnimBlockRef((bloque-0xB5D4A0)>>5 con signo) ; referenciado=0) ;
          exigido=grupo ; busca bloque (si nulo: GetAnimationBlock(GetAnimBlockName(exigido))) ; si grupo == exigido y referenciado: elegido=combo ; bloque cargado (byte [b+10h]): AddAnimBlockRef,
          referenciado=1, elegido=combo ; si no: RequestModel(indice+0x63E7, 8) y siguiente en 0xB..0xE (con signo) -> 0xB, elegido=4 ; combo=elegido ; reposo=BlendAnimation(clump, grupo de la fila del elegido, 0xDF, 4.0) ;
          SetDeleteCallback(0x61DAE0, this) ; SetMoveState(1) ; [ped+538h]=1 ; combo=0 ; ultimo=0
  629B9B  si exigido != 0x21 y no referenciado: GetAvailableComboSet(ped, -1)
  629BB2  sin anim: IsPlayer ; si jugador y contador > periodo (u16) y siguiente==0 y combo==0: siguiente=1 ; si (siguiente!=0 o combo!=0) y ultimo != 0x10: combo=GetAvailableComboSet(ped, siguiente) ;
          IsPlayer ; jugador: (i8)siguiente en 3..6 -> FindTargetOnGround(ped) ; si no StartAnim(ped, (i16)ChooseAttackPlayer(ped)) ; IA: StartAnim(ped, (i16)ChooseAttackAI(ped))
  629C4E  con anim: arma=[ped+5A0+..] ; si 9 (motosierra): AddAudioEvent del arma (ped+394h, 0x9A) ; combo<4 (con signo): ultimo 8 o 0xA: tiempo/total > 0.4 y delta > -4.0 -> delta=-4.0 y fin ; ultimo==3: si siguiente==3 -> siguiente=0
          si no si delta > -4.0 -> delta=-4.0 ; combo>=4: ultimo==2: (siguiente==2: si banderas&1: hit=[fila+48h], cad=[fila+4Ch]: t<hit y paso+t>=hit, o t<cad y paso+t>=cad -> banderas&=~1 ; SetCurrentTime(hit)) ;
          (si no: si no sonando y peso>0 y delta>=0 -> delta=-4.0 ; (i8)siguiente>=0xB -> SetDeleteCallback(0x4D1390,0) ; [esi+1Ch]=0) ; siguiente==2 -> 0 ; ultimo!=2: peso>0.9 y delta>=0: t>hit(m) y t-paso<hit(m): AddAudioEvent
          (ped+138h, 0x3C, 0, 1.0, nulo, 0, sonido(m), 0), combo==5 y m en 0..2: segundo AddAudioEvent con (sonido(m), [0x8D2E3C+4m]) ; siguiente en 0xB..0xE -> 0 ; nivel=[fila+50h+m] ; si != 7: punto=matriz*vector(nivel) (0x59C890) ; si m==4: punto += paso*velocidad ;
          FightStrike(ped, punto) ; si no, t >= cadena(m): siguiente en 0xB..0xE: m 0/1: combo=GetAvailableComboSet(ped, siguiente) ; jugador -> StartAnim(ChooseAttackPlayer) si no StartAnim(m+1) ; m==3: jugador y ChooseAttackPlayer==3 -> StartAnim(3) ;
          m==4: combo==0xC y jugador -> SetCurrentTime(golpe(m)-0.01) ; si no combo=GetAvailableComboSet(ped, siguiente) ; jugador -> StartAnim(ChooseAttackPlayer) si no StartAnim(m+1)
  629FF7  objetivo: ped.aim = atan2(-(obj.x-ped.x), obj.y-ped.y) ; si no: jugador y [0xB6EC2E] y GetPadFromPlayer->GetTarget(): aim = atan2(-cam.x, cam.y) ; en control = 0 ; devuelve 0

Eventos de los falsos (9 enteros: tipo + 8 argumentos): ver KIND_* abajo. Etiquetas: 1 tarea, 2 anim, 3 reposo, 4 reposo nuevo, 5 ped, 6 objetivo, 7 pad, 0x100+desplazamiento para direcciones dentro del ped, 0 nulo, 0xBAD otro.
Cambios: [arena, desplazamiento respecto al objeto, byte nuevo]; arena 0 tarea, 1 anim, 2 reposo, 3 ped (sin los 4 bytes de +0x55C), 4 reposo nuevo ; `aim` = bits escritos en ped+0x55C (inicial 0xFFFFFFFF) o null ; tolerancia 2 ulp en run_local.py.
"""
import math
import struct
import sys

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskSimpleFight::ProcessPed'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
SLOT = 7
CB_DEL = 0x4D1390
CB_MELEE = 0x61DAE0
REGION_BASE = 0xC17090
REGION = 0x8B0
TABLE = 0xC170D0
LEVELS = 0xC177D0
AIM_INIT = 0xFFFFFFFF
# etiquetas
L_TASK, L_ANIM, L_IDLE, L_NEW, L_PED, L_TGT, L_PAD = 1, 2, 3, 4, 5, 6, 7
# tipos de evento
(K_SETDEL, K_BLEND, K_SETTIME, K_ISPLAYER, K_MOVESTATE, K_ADDREF, K_REMREF, K_BLOCKNAME, K_GETBLOCK, K_REQUEST, K_WINFO, K_PEDAE, K_WEAPAE, K_STRIKE, K_AVAIL, K_FIND, K_CHOOSEP, K_CHOOSEAI,
 K_STARTANIM, K_GETPAD, K_GETTARGET) = range(1, 22)
NAME_TOKEN = 0x4E414D45        # lo que devuelve el falso de GetAnimBlockName (el nombre se identifica por este valor)
ALT_SOUND = (0x12C, 0x190, 0x1F4)   # [0x8D2E3C + 4m]
# tamanos de arena (suma a mano guarda + ventana + guarda; comprobados por fixture_mirror.py contra Fixtures.hpp)
SIZES = {'this_bytes': 0x28, 'this_off': 0x40, 'this_arena': 0x100, 'anim_bytes': 0x3C, 'anim_off': 0x20, 'anim_arena': 0xA0, 'hier_bytes': 0x14, 'hier_off': 0x20, 'hier_arena': 0x70,
         'ped_bytes': 0x730, 'ped_off': 0x40, 'ped_arena': 0x7B0, 'mat_bytes': 0x48, 'mat_off': 0x20, 'mat_arena': 0xA0, 'tgt_bytes': 0x18, 'tgt_off': 0x20, 'tgt_arena': 0x80,
         'blocks_bytes': 0x60, 'blocks_off': 0x20, 'blocks_arena': 0xC0, 'groups_bytes': 0x50, 'groups_off': 0x20, 'groups_arena': 0x110, 'winfo_bytes': 0x70, 'winfo_off': 0x20, 'winfo_arena': 0xD0,
         'pad_bytes': 0x10, 'pad_off': 0x20, 'pad_arena': 0x70, 'region': 0x8B0, 'slots': 16}


class Fail(Exception):
    """Fixture invalida: el EXE desreferenciaria algo que el banco no modela."""


# ---------------------------------------------------------------------------------------------------- numeros
def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def f32(x):
    if x != x:
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def bits(x):
    if x != x:
        return 0x7FC00000
    return struct.unpack('<I', struct.pack('<f', x))[0]


def fadd(a, b):
    return f32(a + b)


def fsub(a, b):
    return f32(a - b)


def fmul(a, b):
    return f32(a * b)


def fdiv(a, b):
    if b == 0.0:
        if a != a or a == 0.0:
            return math.nan
        neg = (math.copysign(1.0, a) < 0) != (math.copysign(1.0, b) < 0)
        return -math.inf if neg else math.inf
    return f32(a / b)


def ftol_low16(x):
    """Bajo 16 bits del truncado a int64 de _ftol2 (NaN, inf y |x| >= 2^63 -> 0x8000000000000000 -> 0)."""
    if x != x or math.isinf(x):
        return 0
    t = math.trunc(x)
    if t >= 1 << 63 or t < -(1 << 63):
        return 0
    return t & 0xFFFF


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rand(seed, i // 4))
    return bytes(out[:nbytes])


def fnv64(data):
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


class Region:
    def __init__(self, seed, writes):
        self.b = bytearray(fill_words(REGION, seed))
        for a, v in writes:
            self.w32(a, v)

    def r32(self, addr):
        o = addr - REGION_BASE
        if o < 0 or o + 4 > REGION:
            raise Fail('lectura fuera de la region: 0x%X' % addr)
        return struct.unpack_from('<I', self.b, o)[0]

    def r8(self, addr):
        o = addr - REGION_BASE
        if o < 0 or o >= REGION:
            raise Fail('lectura fuera de la region: 0x%X' % addr)
        return self.b[o]

    def w32(self, addr, v):
        o = addr - REGION_BASE
        if o < 0 or o + 4 > REGION:
            raise Fail('escritura fuera de la region: 0x%X' % addr)
        struct.pack_into('<I', self.b, o, v & M32)


def le(v, n=4):
    return [(v >> (8 * i)) & 0xFF for i in range(n)]


# ---------------------------------------------------------------------------------------------------- modelo
def expected_proc(inp, case):
    if inp['vslot'] != SLOT:
        raise RuntimeError('Ranura de la vtable inesperada %r' % inp['vslot'])
    reg = Region(inp['table_seed'], inp['table_writes'])
    ev = []
    br = {}                        # ramas recorridas (cobertura)

    def B(name):
        br['br_' + name] = True

    # ---- estado inicial
    T = {k: inp[k] for k in ('finished', 'in_control', 'anims_ref', 'req_group', 'idle_period', 'idle_counter', 'combo', 'move', 'next', 'last')}
    T0 = dict(T)
    T['anim'] = inp['anim_ptr'] if inp['has_anim'] else 0
    T['idle'] = inp['idle_ptr'] if inp['has_idle'] else 0
    T0['anim'], T0['idle'] = T['anim'], T['idle']
    A = {'d': inp['anim_d'], 'flags': inp['anim_flags'] & 0xFFFF}
    A0 = dict(A)
    I = {'d': inp['idle_d']}
    ped = {'swim': inp['swim0']}
    aim = [None]
    ped_ptr = L_PED
    ptr_new = inp['newidle_ptr']

    ped_clump, ped_group = inp['clump'] & M32, inp['group'] & M32
    w_idle, d_idle = fl(inp['idle_w']), fl(inp['idle_d'])
    pid = inp['is_player'] != 0
    step_ts = fl(inp['timestep'])

    def u8(v):
        return v & 0xFF

    def setdel(label, cb, data):
        ev.append([K_SETDEL, label, cb, data, 0, 0, 0, 0])

    def is_player():
        ev.append([K_ISPLAYER, L_PED, 0, 0, 0, 0, 0, 0])
        return pid

    def blend(group, anim, delta):
        ev.append([K_BLEND, ped_clump, group & M32, anim, delta, 0, 0, 0])

    def finish(result):
        # --- cambios (diferencias respecto al estado inicial)
        ch = []

        def word(arena, off, old, new, n=4):
            for i, (a, b) in enumerate(zip(le(old, n), le(new, n))):
                if a != b:
                    ch.append([arena, off + i, b])
        for off, key, n in ((8, 'finished', 1), (9, 'in_control', 1), (0xA, 'anims_ref', 1), (0xC, 'req_group', 4), (0x12, 'idle_counter', 2), (0x1C, 'anim', 4), (0x20, 'idle', 4),
                            (0x24, 'combo', 1), (0x26, 'next', 1), (0x27, 'last', 1)):
            word(0, off, T0[key], T[key], n)
        word(1, 0x1C, A0['d'], A['d'])
        word(1, 0x2E, A0['flags'], A['flags'], 2)
        word(3, 0x538, inp['swim0'], ped['swim'])
        ch.sort()
        want = {'ret': 1 if result else 0, 'events': [e + [0] * (9 - len(e)) for e in ev], 'changes': ch, 'aim': aim[0], 'others_unchanged': True, 'globals_unchanged': True}
        stats = {k: True for k in br}
        return want, stats

    # ===== 0x629927
    if u8(T['finished']) != 0:
        B('finished')
        if T['idle']:
            setdel(L_IDLE, CB_DEL, 0)
            if w_idle > 0.0 and d_idle >= 0.0:
                B('finished_blend')
                blend(ped_group, 3, 0x41000000)
            else:
                B('finished_noblend')
            T['idle'] = 0
        return finish(True)

    # ===== 0x629990 contador
    if u8(T['combo']) != 0 and u8(T['in_control']) != 0:
        T['idle_counter'] = 0
        B('counter_reset')
    else:
        t = fmul(fmul(step_ts, fl(0x3CA3D70A)), fl(0x447A0000))
        T['idle_counter'] = (T['idle_counter'] + ftol_low16(t)) & 0xFFFF
        B('counter_add')
        if t != t or math.isinf(t) or abs(t) >= 2.0 ** 63:
            B('counter_indefinite')
        if (T0['idle_counter'] + ftol_low16(t)) > 0xFFFF:
            B('counter_wrap')

    # ===== 0x6299BF
    if u8(T['in_control']) == 0:
        if s8(T['next']) < 1:
            B('nocontrol_next_lt1')
            return finish(False)
        if T['anim'] and (inp['anim_id'] & 0xFFFF) == 0x2F:
            B('nocontrol_anim2f')
            return finish(False)
        B('nocontrol_pass')

    # ===== 0x6299E2 reposo
    if not T['idle']:
        last = u8(T['last'])
        if last in (1, 0xF, 0x10, 0x11):
            T['finished'] = 1
            B('idle_last_finish')
        elif not T['anim'] and not (s32(inp['move_state']) > 4 and is_player()):
            B('idle_create')
            if s32(inp['move_state']) > 4:
                B('idle_create_moving_nonplayer')
            wtype = inp['wtype'] & M32
            ev.append([K_WINFO, wtype, 1, 0, 0, 0, 0, 0])
            combo = inp['base_combo'] & 0xFF
            if combo == 4:
                combo = inp['style'] & 0xFF
                B('idle_combo_style')
            row = TABLE + (s8(combo) - 4) * 0x88
            chosen = 4
            if reg.r8(row + 0x85) & 4:
                B('row_flag')
                grp = reg.r32(row)
                need = True
                if grp == 0x21:
                    chosen, need = combo, False
                    B('group_21')
                elif grp != T['req_group']:
                    B('group_differs')
                    if u8(T['anims_ref']) != 0:
                        blk = inp['group_block_addr'][T['req_group']] if T['req_group'] < 4 else None
                        if blk is None:
                            raise Fail('grupo exigido fuera de la tabla falsa')
                        idx = s32(((blk - 0xB5D4A0) & M32)) >> 5
                        ev.append([K_REMREF, idx & M32, 0, 0, 0, 0, 0, 0])
                        T['anims_ref'] = 0
                        B('release_ref')
                    T['req_group'] = reg.r32(row)
                elif u8(T['anims_ref']) != 0:
                    chosen, need = combo, False
                    B('group_same_referenced')
                else:
                    B('group_same_unreferenced')
                if need:
                    g = T['req_group']
                    if g >= 4:
                        raise Fail('grupo exigido fuera de la tabla falsa')
                    blk = inp['group_block_addr'][g]
                    if blk == 0:
                        ev.append([K_BLOCKNAME, g, 0, 0, 0, 0, 0, 0])
                        ev.append([K_GETBLOCK, NAME_TOKEN, 0, 0, 0, 0, 0, 0])
                        blk = inp['name_block_addr']
                        B('block_by_name')
                    idx = s32(((blk - 0xB5D4A0) & M32)) >> 5
                    loaded = inp['block_loaded'][inp['block_addr'].index(blk)] & 0xFF if blk in inp['block_addr'] else None
                    if loaded is None:
                        raise Fail('bloque desconocido')
                    if loaded != 0:
                        ev.append([K_ADDREF, idx & M32, 0, 0, 0, 0, 0, 0])
                        T['anims_ref'] = 1
                        chosen = combo
                        B('block_loaded')
                    else:
                        ev.append([K_REQUEST, (idx + 0x63E7) & M32, 8, 0, 0, 0, 0, 0])
                        B('block_not_loaded')
                        if 0xB <= s8(T['next']) <= 0xE:
                            T['next'] = 0xB
                            B('next_rewritten')
                        chosen = 4
            else:
                B('row_noflag')
            T['combo'] = chosen & 0xFF
            crow = TABLE + (s8(chosen) - 4) * 0x88
            igrp = reg.r32(crow)
            blend(igrp, 0xDF, 0x40800000)
            T['idle'] = ptr_new
            setdel(L_NEW, CB_MELEE, L_TASK)
            ev.append([K_MOVESTATE, L_PED, 1, 0, 0, 0, 0, 0])
            ped['swim'] = 1
            T['combo'] = 0
            T['last'] = 0
        else:
            B('idle_skip')
    else:
        B('idle_present')

    # ===== 0x629B9B
    if T['req_group'] != 0x21 and u8(T['anims_ref']) == 0:
        ev.append([K_AVAIL, L_TASK, L_PED, 0xFF, 0, 0, 0, 0])
        B('avail_minus1')

    # ===== 0x629BB2
    hit_trigger = False
    if not T['anim']:
        B('noanim')
        pl = is_player()
        if pl and T['idle_counter'] > T['idle_period'] and u8(T['next']) == 0 and u8(T['combo']) == 0:
            T['next'] = 1
            B('noanim_player_idle_next1')
        if (u8(T['next']) != 0 or u8(T['combo']) != 0) and u8(T['last']) != 0x10:
            B('noanim_command')
            ev.append([K_AVAIL, L_TASK, L_PED, u8(T['next']), 0, 0, 0, 0])
            T['combo'] = inp['r_avail'] & 0xFF
            if is_player():
                n = s8(T['next'])
                if 3 <= n <= 6:
                    ev.append([K_FIND, L_TASK, L_PED, 0, 0, 0, 0, 0])
                    B('noanim_player_find')
                else:
                    ev.append([K_CHOOSEP, L_TASK, L_PED, 0, 0, 0, 0, 0])
                    attack = s32(movsx16(inp['r_attack']))
                    ev.append([K_STARTANIM, L_TASK, L_PED, attack & M32, 0, 0, 0, 0])
                    B('noanim_player_attack')
            else:
                ev.append([K_CHOOSEAI, L_TASK, L_PED, 0, 0, 0, 0, 0])
                ev.append([K_STARTANIM, L_TASK, L_PED, s32(movsx16(inp['r_attack'])) & M32, 0, 0, 0, 0])
                B('noanim_ai_attack')
        else:
            B('noanim_nocommand')
    else:
        B('anim')
        combo = s8(T['combo'])
        row = TABLE + (combo - 4) * 0x88
        move = s8(T['move'])
        timeA, stepA = fl(inp['anim_time']), fl(inp['anim_step'])
        wA, dA = fl(inp['anim_w']), fl(A['d'])
        wtype = inp['wtype'] & M32
        if wtype == 9:
            ev.append([K_WEAPAE, 0x100 + 0x394, 0x9A, 0, 0, 0, 0, 0])
            B('chainsaw')
        if combo < 4:
            B('combo_lt4')
            last = u8(T['last'])
            done = False
            if last in (8, 0xA):
                if not inp['has_hier']:
                    raise Fail('jerarquia nula')
                ratio = fdiv(timeA, fl(inp['hier_total']))
                if ratio > fl(0x3ECCCCCD) and fl(A['d']) > -4.0:
                    A['d'] = 0xC0800000
                    done = True
                    B('lt4_blendout')
            if not done and last == 3:
                if u8(T['next']) == last:
                    T['next'] = 0
                    B('lt4_last3_next3')
                elif fl(A['d']) > -4.0:
                    A['d'] = 0xC0800000
                    B('lt4_last3_blendout')
        elif u8(T['last']) == 2:
            B('last2')
            if u8(T['next']) == 2:
                if A['flags'] & 1:
                    hit, chain = fl(reg.r32(row + 0x48)), fl(reg.r32(row + 0x4C))
                    in_hit = timeA < hit and fadd(stepA, timeA) >= hit
                    if in_hit or (timeA < chain and fadd(stepA, timeA) >= chain):
                        A['flags'] &= 0xFFFE
                        ev.append([K_SETTIME, L_ANIM, reg.r32(row + 0x48), 0, 0, 0, 0, 0])
                        B('last2_stop')
                    else:
                        B('last2_playing_nostop')
                else:
                    B('last2_notplaying')
            else:
                if not (A['flags'] & 1) and wA > 0.0 and fl(A['d']) >= 0.0:
                    A['d'] = 0xC0800000
                    B('last2_blendout')
                if s8(T['next']) >= 0xB:
                    setdel(L_ANIM, CB_DEL, 0)
                    T['anim'] = 0
                    B('last2_release')
            if u8(T['next']) == 2:
                T['next'] = 0
        else:
            B('hit_branch')
            if wA > fl(0x3F666666) and fl(A['d']) >= 0.0:
                B('hit_weight_ok')
                hitT = fl(reg.r32(row + 0x08 + move * 4))
                if timeA > hitT and fsub(timeA, stepA) < hitT:
                    B('hit_cross')
                    snd = reg.r32(row + 0x5C + move * 4)
                    # AddAudioEvent del ped (0x4E2BB0): (evento 0x3C, volumen 0.0, velocidad 1.0, fisico nulo, superficie 0, a7 = sonido, maxVol = 0)
                    ev.append([K_PEDAE, 0x100 + 0x138, 0x3C, 0, 0x3F800000, 0, 0, snd & M32, 0])
                    if u8(T['combo']) == 5:
                        cur = s8(T['move'])
                        if 0 <= cur <= 2:
                            ev.append([K_PEDAE, 0x100 + 0x138, 0x3C, 0, 0x3F800000, 0, 0, reg.r32(row + 0x5C + cur * 4), ALT_SOUND[cur]])
                            B('hit_combo5_alt')
                    if 0xB <= s8(T['next']) <= 0xE:
                        T['next'] = 0
                        B('hit_next_cleared')
                    lvl = reg.r8(row + 0x50 + s8(T['move']))
                    if lvl != 7:
                        loc = [reg.r32(LEVELS + lvl * 12 + 4 * k) for k in range(3)]
                        m = inp['mat']
                        if not inp['has_matrix']:
                            raise Fail('matriz del ped nula en el golpe')
                        # 0x59C890: x = ((up.x*z + fwd.x*y) + right.x*x) + pos.x ; y = ((up.y*z + right.y*x) + fwd.y*y) + pos.y ; z = ((up.z*z + right.z*x) + fwd.z*y) + pos.z
                        # disposicion: right = m[0..2], fwd = m[3..5], up = m[6..8], pos = m[9..11]
                        x, y, z = fl(loc[0]), fl(loc[1]), fl(loc[2])
                        R, Fw, U, P = [fl(v) for v in m[0:3]], [fl(v) for v in m[3:6]], [fl(v) for v in m[6:9]], [fl(v) for v in m[9:12]]
                        px = fadd(fadd(fadd(fmul(U[0], z), fmul(Fw[0], y)), fmul(R[0], x)), P[0])
                        py = fadd(fadd(fadd(fmul(U[1], z), fmul(R[1], x)), fmul(Fw[1], y)), P[1])
                        pz = fadd(fadd(fadd(fmul(U[2], z), fmul(R[2], x)), fmul(Fw[2], y)), P[2])
                        if s8(T['move']) == 4:
                            sp = [fl(v) for v in inp['speed']]
                            px, py, pz = fadd(fmul(step_ts, sp[0]), px), fadd(fmul(step_ts, sp[1]), py), fadd(fmul(step_ts, sp[2]), pz)
                            B('hit_move4_adjust')
                        ev.append([K_STRIKE, L_TASK, L_PED, bits(px), bits(py), bits(pz), 0, 0])
                        B('hit_strike')
                    else:
                        B('hit_level7')
                elif timeA >= fl(reg.r32(row + 0x1C + move * 4)):
                    B('chain_time')
                    nx = s8(T['next'])
                    if 0xB <= nx <= 0xE:
                        B('chain_next')
                        cur = s8(T['move'])
                        if cur in (0, 1):
                            ev.append([K_AVAIL, L_TASK, L_PED, u8(nx), 0, 0, 0, 0])
                            T['combo'] = inp['r_avail'] & 0xFF
                            if is_player():
                                ev.append([K_CHOOSEP, L_TASK, L_PED, 0, 0, 0, 0, 0])
                                ev.append([K_STARTANIM, L_TASK, L_PED, s32(movsx16(inp['r_attack'])) & M32, 0, 0, 0, 0])
                                B('chain_m01_player')
                            else:
                                ev.append([K_STARTANIM, L_TASK, L_PED, (cur + 1) & M32, 0, 0, 0, 0])
                                B('chain_m01_ai')
                        elif cur == 3:
                            if is_player():
                                ev.append([K_CHOOSEP, L_TASK, L_PED, 0, 0, 0, 0, 0])
                                if s16(inp['r_attack']) == 3:
                                    ev.append([K_STARTANIM, L_TASK, L_PED, 3, 0, 0, 0, 0])
                                    B('chain_m3_start')
                                else:
                                    B('chain_m3_other')
                            else:
                                B('chain_m3_ai')
                        elif cur == 4:
                            if u8(T['combo']) == 0xC and is_player():
                                ev.append([K_SETTIME, L_ANIM, bits(fsub(fl(reg.r32(row + 0x08 + cur * 4)), fl(0x3C23D70A))), 0, 0, 0, 0, 0])
                                B('chain_m4_settime')
                            else:
                                ev.append([K_AVAIL, L_TASK, L_PED, u8(T['next']), 0, 0, 0, 0])
                                T['combo'] = inp['r_avail'] & 0xFF
                                if is_player():
                                    ev.append([K_CHOOSEP, L_TASK, L_PED, 0, 0, 0, 0, 0])
                                    ev.append([K_STARTANIM, L_TASK, L_PED, s32(movsx16(inp['r_attack'])) & M32, 0, 0, 0, 0])
                                    B('chain_m4_player')
                                else:
                                    ev.append([K_STARTANIM, L_TASK, L_PED, (cur + 1) & M32, 0, 0, 0, 0])
                                    B('chain_m4_ai')
                        else:
                            B('chain_m_other')
                    else:
                        B('chain_nonext')
                else:
                    B('hit_nothing')
            else:
                B('hit_weight_low')

    # ===== 0x629FF7 orientacion
    if inp['has_target']:
        pp = [fl(v) for v in (inp['mat'][9:12] if inp['has_matrix'] else inp['ped_pos'])]
        tp = [fl(v) for v in (inp['tgt_mat_pos'] if inp['tgt_has_matrix'] else inp['tgt_pos'])]
        dx = fsub(tp[0], pp[0])
        dy = fsub(tp[1], pp[1])
        aim[0] = bits(f32(math.atan2(-dx, dy)))
        B('aim_target')
        B('aim_target_matrix' if inp['has_matrix'] else 'aim_target_nomatrix')
    else:
        if is_player():
            if inp['cam_flag'] & 0xFF:
                ev.append([K_GETPAD, L_PED, 0, 0, 0, 0, 0, 0])
                ev.append([K_GETTARGET, L_PAD, 0, 0, 0, 0, 0, 0])
                if inp['get_target'] & 0xFF:
                    cf = [fl(v) for v in inp['cam_front']]
                    aim[0] = bits(f32(math.atan2(-cf[0], cf[1])))
                    B('aim_camera')
                else:
                    B('aim_pad_no_target')
            else:
                B('aim_camera_off')
        else:
            B('aim_none')
    T['in_control'] = 0
    return finish(False)


def movsx16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s16(v):
    return movsx16(v)


def expected(function, inp, case):
    if function == NAME:
        want, br = expected_proc(inp, case)
        stats = dict(br)
        stats['family%d' % inp['family']] = True
        stats['ret_true'] = want['ret'] == 1
        stats['ret_false'] = want['ret'] == 0
        stats['events_ge6'] = len(want['events']) >= 6
        stats['stack_seed'] = True
        return want, stats
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    print('ok (la autoprueba funcional se hace en fixture_mirror.py contra las fixtures reales)')
