"""Oraculo independiente del banco ModifySpeed27: CAutoPilot::ModifySpeed (0x41B980).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits:
cada operacion aritmetica se redondea a 24 bits de mantisa (exponente extendido); los enteros entran por fild (exactos). Las operaciones float32 +,-,* entre
valores float32 se calculan en doble (resultado correctamente redondeado a float32: 53 >= 2*24+2); las que mezclan enteros de 32 bits con floats (cociente por
m_nSpeedScaleFactor, newScale*q, now - prod) se calculan con Fraction (exactas) y se redondean UNA vez a 24 bits (r24), como el x87.

  41B980 fld [esp+4]; fcom 0,01f; test ah,41h; je L; fstp st0; fld 0,01f        target = target > 0,01f ? target : 0,01f  (NaN -> 0,01f)
  41B9A0 eax = [B7CB84] - [esi+0C]; fild eax; jge L; fadd 2^32; fidiv [esi+10]; fstp [esp+1C] = q       q = r24(elapsed' / s32(escala))
         fstp [esi+38] = target (m_speed)
  41B9CE [96F854 + (word[esi+14]>>10)*4] == 0 -> ret ;  [96F854 + (word[esi+16]>>10)*4] == 0 -> ret    (EAX/ECX/EDX distintos, no observables)
  41BA2F..  dirs: aDirX = r24(r24(adx*0,01f)*laneA) (adx = int8 [A+8], laneA = int8 [esi+25]) ; aDirY con [A+9] ; bDirX/Y con [B+8]/[B+9] y [esi+26]
  41BAC1 call 44DB00(A) ; widthA = r24(r24(off + int8 [esi+27]) * 5,4f) ; call 44DB00(B) ; widthB con int8 [esi+28]
  44DB00 f = byte [ecx+0B]; opp = f&7; opp==0 -> 0,5 - ((f>>3)&7)*0,5 ; (f&0x38)==0 -> 0,5 - opp*0,5 ; si no byte[ecx+0A]*0,011574074 + 0,5
  41BB0F start = (ax*0,125 + widthA*aDirY, ay*0,125 - widthA*aDirX, 0) ; end = (bx*0,125 + widthB*bDirY, by*0,125 - widthB*bDirX, 0)  (ax.. = int16 [A],[A+2])
  41BC20 call 43C710(&start, &end, aDirX, aDirY, bDirX, bDirY)   -> ST0 = scale
  41BC28 fld 1000 ; fdiv [esi+38] ; fmulp ; call _ftol2 ; [esi+10] = eax     d = r24(1000/speed) ; newScale = trunc(r24(scale*d)) & 0xFFFFFFFF
  41BC3B fild [B7CB84] (+2^32 si < 0) ; fild newScale ; fmul q ; fsubp ; call _ftol2 ; [esi+0C] = eax     field_C = trunc(r24(nowD - r24(newScale*q)))
_ftol2 (0x821B40): truncacion hacia cero; NaN/inf/|x| >= 2^63 -> 0x8000000000000000 (parte baja 0).
"""
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
CASES = 4096
NAME = 'CAutoPilot::ModifySpeed'
NAMES = [NAME]
HEAD = 0x98


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


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


def ftol_low(x):
    """_ftol2: parte baja (32 bits) del int64 truncado; fuera de rango o no finito -> 0."""
    if isinstance(x, float) and not math.isfinite(x):
        return 0
    x = Fraction(x)
    if x >= 2 ** 63 or x < -(2 ** 63):
        return 0
    return int(x) & M32


C_HALF = 0.5
K_WIDTH = struct.unpack('<f', struct.pack('<f', 0.011574073694646358))[0]   # 0x858EEC
F_001 = fl(0x3C23D70A)      # 0x858C58
F_54 = fl(0x40ACCCCD)       # 0x858C50 = 5,400000095367432
F_0125 = 0.125
F_1000 = 1000.0


def lane_offset(link):
    """0x44DB00 (x87 24 bits)."""
    flags = link[0xB]
    opp = flags & 7
    if opp == 0:
        return f32(C_HALF - f32(((flags >> 3) & 7) * C_HALF))
    if (flags & 0x38) == 0:
        return f32(C_HALF - f32(opp * C_HALF))
    return f32(f32(link[0xA] * K_WIDTH) + C_HALF)


def tohex(b):
    return bytes(b).hex()


def expected(inp, case):
    head = bytearray(bytes.fromhex(inp['head_in']))
    stats = {}
    target = fl(inp['target'])
    clamped = not (target > F_001)
    if clamped:
        target = F_001
    stats['clamped'] = clamped
    stats['target_nan'] = math.isnan(fl(inp['target']))
    now = inp['now'] & M32
    field_c = inp['field_c'] & M32
    scale_old = s32(inp['speed_scale'])
    elapsed = s32((now - field_c) & M32)
    stats['elapsed_negative'] = elapsed < 0
    elapsed_d = r24(Fraction(elapsed) + (1 << 32)) if elapsed < 0 else Fraction(elapsed)
    q = r24(elapsed_d / Fraction(scale_old))
    head[0x38:0x3C] = struct.pack('<I', bits(target))
    pres = inp['pres']
    stats['pres'] = pres
    stats['now_negative'] = s32(now) < 0
    cur, nxt = inp['cur'], inp['next']
    stats['same_area'] = (cur >> 10) == (nxt >> 10)
    stats['same_addr'] = cur == nxt
    if pres != 2:
        stats['ran'] = False
        return {'head': tohex(head), 'guard_ok': True, 'arenas_unchanged': True, 'tables_unchanged': True, 'trace': []}, stats
    stats['ran'] = True
    a = bytes.fromhex(inp['link_a'])
    b = bytes.fromhex(inp['link_b'])
    lane_a, lane_b = float(inp['smth_curr']), float(inp['smth_next'])
    adx, ady, bdx, bdy = float(s8(a[8])), float(s8(a[9])), float(s8(b[8])), float(s8(b[9]))
    a_dx = f32(f32(adx * F_001) * lane_a)
    a_dy = f32(f32(ady * F_001) * lane_a)
    b_dx = f32(f32(bdx * F_001) * lane_b)
    b_dy = f32(f32(bdy * F_001) * lane_b)
    off_a, off_b = lane_offset(a), lane_offset(b)
    for link, k in ((a, 'a'), (b, 'b')):
        f = link[0xB]
        stats['branch_' + k] = 1 if (f & 7) == 0 else 2 if (f & 0x38) == 0 else 3
    width_a = f32(f32(off_a + inp['lane']) * F_54)
    width_b = f32(f32(off_b + inp['next_lane']) * F_54)
    ax, ay = float(s16(struct.unpack_from('<h', a, 0)[0])), float(s16(struct.unpack_from('<h', a, 2)[0]))
    bx, by = float(s16(struct.unpack_from('<h', b, 0)[0])), float(s16(struct.unpack_from('<h', b, 2)[0]))
    sx = f32(f32(ax * F_0125) + f32(width_a * a_dy))
    sy = f32(f32(ay * F_0125) - f32(width_a * a_dx))
    ex = f32(f32(bx * F_0125) + f32(width_b * b_dy))
    ey = f32(f32(by * F_0125) - f32(width_b * b_dx))
    trace = [1] + [bits(v) for v in (sx, sy, 0.0, ex, ey, 0.0, a_dx, a_dy, b_dx, b_dy)]
    scale = fl(inp['scale_ret'])
    stats['scale_nonfinite'] = not math.isfinite(scale)
    if math.isinf(target):
        d = Fraction(0)
    else:
        d = r24(Fraction(F_1000) / Fraction(target))
    if not math.isfinite(scale):
        new_scale = 0
    else:
        new_scale = ftol_low(r24(Fraction(scale) * d))
    stats['new_scale_zero'] = new_scale == 0
    head[0x10:0x14] = struct.pack('<I', new_scale)
    now_s = s32(now)
    now_d = r24(Fraction(now_s) + (1 << 32)) if now_s < 0 else Fraction(now_s)
    prod = r24(Fraction(s32(new_scale)) * q)
    exact = r24(now_d - prod)
    new_c = ftol_low(exact)
    head[0xC:0x10] = struct.pack('<I', new_c)
    # Lo que daria la fuente actual (todo en double y un solo (float) final): (float)(nowD - (double)prod). Se anota para el informe; NO se usa como esperado.
    try:
        src = f32(float(now_d) - float(prod))
        src_c = ftol_low(Fraction(src)) if math.isfinite(src) else 0
    except OverflowError:
        src_c = new_c
    stats['source_style_differs'] = src_c != new_c
    return {'head': tohex(head), 'guard_ok': True, 'arenas_unchanged': True, 'tables_unchanged': True, 'trace': trace}, stats


if __name__ == '__main__':
    # autoprueba minima del modelo (sin juego)
    assert r24(Fraction(16777219) - Fraction(1, 10 ** 9)) == 16777218
    assert float(r24(Fraction(16777219))) == 16777220.0
    print('ok')
