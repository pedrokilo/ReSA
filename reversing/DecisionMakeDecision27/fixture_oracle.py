"""Oraculo independiente del banco DecisionMakeDecision27: CDecision::MakeDecision (0x6040D0) con su colaborador 0x6007A0 y rand 0x821B1E.

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits:
cada operacion aritmetica se redondea UNA vez a 24 bits de mantisa (r24, exponente extendido); los enteros entran por fild/fiadd (exactos).

  6040D0 sub esp,60h ; this en ECX ; args: [esp+74h]=sourceType, +78h=inVehicle (SOLO byte bajo: mov al,[..]), +7Ch/+80h/+84h=avoid1/2/3, +88h=seek, +8Ch=&taskSlot, +90h=&facialSlot
  60410E locales: tasks[6]=200 (dword), probs[6]=0 (byte); ebp=this+30h+(inVehicle!=0); edi=this+18h+sourceType (sin comprobar limites)
  604130 para i=0..5: p=byte[edi+4i]; p==0 -> salta (jbe); t=dword[this+4i]; t==-1 / ==avoid1 / ==avoid2 / ==avoid3 -> salta; byte[ebp+2i]==0 -> salta;
         tasks[n]=t ; probs[n]=p ; n++
  604181 word[taskSlot]=200 ; n<=0 (signed) -> word[facialSlot]=-1 ; ret 20h   (sin x87 ni rand)
  60418D st0=0.0 (0x858B50); i=0..5: st0 = r24(st0 + probs[i]) (fiadd, probs = movzx byte) ; cumul[i]=float(st0)     (acumulado, 6 entradas SIEMPRE: las >=n repiten el total)
  6041C8 st0 = r24(1.0f / st0)  (fdivr [858624]; 1.0/total, UN redondeo a 24 bits) ; para i=0..5: cumul[i]=float(r24(st0 * cumul[i]))   (fmul + fstp, otro redondeo)
  6041F4 call 6007A0(ecx=&tasks2[6] (tasks copiadas, n..5 = 200), count=6 en +30h, cumul en +18h; args seek, &taskSlot, &dummy)
  6007A0 word[taskSlot]=200 ; dword[dummy]=-1 ; seek != -1: i=0..5: seek == tasks2[i] -> word[taskSlot]=low16(tasks2[i]); dummy=i ; break
         word[taskSlot] != 200 -> fin (SIN rand) ; si no: r=rand() (0x821B1E, LCG 0x343FD/0x269EC3, (estado>>16)&0x7FFF) ;
         roll = float(r24(r * 0x38000100)) (0x858C7C = 3,0518509e-5 = nextafter(1/32768 * (1+2^-15))) ; i=0..5: fcomp roll,cumul[i]; test ah,41h; jnp  ->  roll <= cumul[i]
         (NaN imposible) -> word[taskSlot]=low16(tasks2[i]) ; si ninguno: queda 200
  604200 word[facialSlot]=-1 ; ret 20h
rand: estado (tiddata+0x14) = estado*0x343FD+0x269EC3 SOLO si se llama; sin llamada el estado queda igual a la semilla (srand del EXE 0x821B11 = mov [tiddata+14h],arg).
"""
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
CASES = 6144
NAME = 'CDecision::MakeDecision'
NAMES = [NAME]
OBJ = 0x3C


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def r24(x):
    """Redondeo exacto (al par) de un racional a 24 bits de mantisa, exponente sin limite (como el x87 con PC=24)."""
    x = Fraction(x)
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    m = a / ulp
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return sign * q * ulp


K_ROLL = Fraction(fl(0x38000100))      # 0x858C7C
ONE = Fraction(1)                       # 0x858624


def next_state(x):
    return (x * 0x343FD + 0x269EC3) & M32


def rand_output(state):
    return (next_state(state) >> 16) & 0x7FFF


def collect(inp):
    obj = bytes.fromhex(inp['obj'])
    assert len(obj) == OBJ
    st = s32(inp['st'])
    veh = (inp['in_veh'] & 0xFF) != 0           # mov al,[esp+78h] ; test al,al
    avoid = [s32(v) for v in inp['avoid']]
    tasks, probs, rows = [200] * 6, [0] * 6, []
    n = 0
    for i in range(6):
        idx = 0x18 + 4 * i + st
        # fuera del objeto el EXE lee las guardas de la arena (relleno 0xC3, comprobado por Core.hpp)
        p = obj[idx] if 0 <= idx < OBJ else 0xC3
        t = s32(int.from_bytes(obj[4 * i:4 * i + 4], 'little'))
        b = obj[0x30 + 2 * i + (1 if veh else 0)]
        rows.append((p, t, b))
        if p == 0 or t == -1 or t in avoid or b == 0:
            continue
        tasks[n], probs[n] = t, p
        n += 1
    return obj, st, veh, avoid, tasks, probs, n, rows


def normalise(total_cum, n_probs):
    """Acumulados normalizados como el x87 a 24 bits: dos redondeos a 24 bits (1/total y producto)."""
    acc = Fraction(0)
    cum = []
    for p in n_probs:
        acc = r24(acc + p)
        cum.append(acc)
    inv = r24(ONE / acc)
    return [r24(inv * c) for c in cum], acc


def expected(inp, case):
    obj, st, veh, avoid, tasks, probs, n, rows = collect(inp)
    seed = inp['seed'] & M32
    seek = s32(inp['seek'])
    stats = {'count': n, 'st_in_range': 0 <= st <= 3, 'veh': veh, 'seek_given': seek != -1}
    stats['in_veh_raw_garbage'] = (inp['in_veh'] & 0xFFFFFFFF) > 1
    for k in range(3):
        stats['avoid%d' % (k + 1)] = any(p != 0 and t != -1 and t == avoid[k] for p, t, b in rows)
    stats['bool_filtered'] = any(p != 0 and t != -1 and t not in avoid and b == 0 for p, t, b in rows)
    stats['task_minus1'] = any(p != 0 and t == -1 for p, t, b in rows)
    stats['prob_zero_filtered'] = any(p == 0 for p, t, b in rows)
    stats['total'] = sum(probs)
    out = {'obj': obj.hex(), 'guard_ok': True, 'slot_guard_ok': True, 'face_slot': -1}
    stats.update(rand_calls=0, selected=-2, found_seek=False, roll=None, miss_total_roll1=False, old_double_style_differs=False, near_threshold=False,
                 roll_is_max=False, all_255=False, cum_last=None)
    if n <= 0:
        out.update(task_slot=200, rng_state=seed, rand_calls=0)
        return out, stats
    cum2, total = normalise(None, probs)
    stats['cum_last'] = float(cum2[5])
    stats['all_255'] = n == 6 and all(p == 255 for p in probs)
    slot = 200
    if seek != -1:
        for i in range(6):
            if seek == tasks[i]:
                slot = s16(tasks[i])
                # una tarea hallada cuya palabra baja es 200 deja la ranura en 200 y el EXE sigue con la tirada (rand)
                stats['found_seek'] = (slot & 0xFFFF) != 200
                break
    state, calls, sel = seed, 0, -1
    if (slot & 0xFFFF) == 200:
        r = rand_output(seed)
        state, calls = next_state(seed), 1
        roll = r24(Fraction(r) * K_ROLL)
        stats['roll'] = float(roll)
        stats['roll_is_max'] = r == 32767
        stats['r'] = r
        slot = 200
        for i in range(6):
            if roll <= cum2[i]:
                slot, sel = s16(tasks[i]), i
                break
        stats['near_threshold'] = any(abs(r - float(c) * 32767) <= 2 for c in cum2)
        stats['miss_total_roll1'] = sel == -1 and r == 32767 and cum2[5] < 1
        # estilo anterior de la fuente (1/total en double): solo para saber si estas fixtures lo habrian distinguido
        inv_d = 1.0 / float(total)
        cum_d = []
        acc = 0.0
        for p in probs:
            acc += p
            cum_d.append(acc)
        cum_d = [f32(inv_d * c) for c in cum_d]
        sel_d = next((i for i in range(6) if float(roll) <= cum_d[i]), -1)
        stats['old_double_style_differs'] = sel_d != sel
        sel = sel
    stats['rand_calls'] = calls
    stats['selected'] = sel if calls else -1
    out.update(task_slot=slot, rng_state=state, rand_calls=calls)
    return out, stats


if __name__ == '__main__':
    # autoprueba minima del modelo (sin juego): total 41 con tirada 32767 -> roll 1.0f > cumul 0.99999994 -> sin acierto (taskSlot = 200)
    assert r24(r24(Fraction(1, 41)) * 41) < 1
    seed = 0
    for cand in range(1 << 20):   # una semilla con primera salida 32767
        seed = (cand * 2654435761) & M32
        if rand_output(seed) == 32767:
            break
    obj = bytearray(OBJ)
    for i in range(6):
        obj[4 * i:4 * i + 4] = struct.pack('<i', 10 + i)
    obj[0x18 + 4 * 0 + 1] = 41
    obj[0x30] = 1
    inp = dict(obj=bytes(obj).hex(), st=1, in_veh=0, avoid=[0x7000, 0x7001, 0x7002], seek=-1, seed=seed)
    want, stats = expected(inp, 0)
    assert want['task_slot'] == 200 and stats['miss_total_roll1'] and want['rand_calls'] == 1, (want, stats)
    # el mismo caso con tirada 0 elige la unica opcion
    seed0 = next(s for s in ((c * 2654435761) & M32 for c in range(1 << 20)) if rand_output(s) == 0)
    inp['seed'] = seed0
    want, stats = expected(inp, 0)
    assert want['task_slot'] == 10 and stats['selected'] == 0
    print('ok')
