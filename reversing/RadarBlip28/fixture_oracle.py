"""Oraculo independiente del banco RadarBlip28: CRadar::DrawEntityBlip (0x587000..0x587BE8).

Se modela desde el DESENSAMBLADO del EXE (dis.sh 0x587000 0x587C10), no desde el C++ de la DLL, a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits:
+ - * / y fsqrt de floats redondean UNA vez a 24 bits (aqui: se calculan en double y se redondean a float, que es exacto para estas operaciones); fild/fmul con enteros y el
coseno/seno del destello usan racionales exactos (r24) porque fcos/fsin no se redondean antes de multiplicar.

Mapa del desensamblado (pila tras el prologo: blipIndex=+50h, isSprite=+54h (SOLO el byte bajo), entidad=+14h, luz=+1Ch, bl=aeropuerto, pos=+40h..48h, radar=+28h, pantalla=+34h):
  587000 trace = 0xBA86F0 + 40*blipIndex ; tipo = (byte[+0x26] >> 2) & 15 ; salto por (tipo-1): 1 vehiculo, 2 peaton, 3 objeto, 6 foco de script, 7 pickup, 8 pista; 0, 4, 5 y >8 salen
  58703C vehiculo (pool 0xB74494, 0x4048E0), 58706F peaton (0xB74490, 0x404910; bit 8 de +0x46C -> m_pVehicle +0x58C, nulo sale), 58709C objeto (0xB7449C, 0x465040)
  58704E entidad nula sale ; pos = matriz ? matriz+0x30 : +4 ; si (byte +0x36 & 7) == 3 y [+0x78C] != 0 -> 0x43EA00(this = [+0x78C], &pos)
  5870B0 foco: 0x4839A0(handle, 2) < 0 sale ; pos = foco+0x14 ; 5870F6 pickup: 0x4552A0(handle) < 0 sale ; pos = 0x4549A0 (int16 * 0.125)
  58737B pista (bl = 1): 4 x FindPlayerVehicle(-1, 0) (0x56E0D0) ; d = sqrt((ay - p4.y)*(ay - p3.y) + (ax - p1.x)*(ax - p2.x)) guardado como float ; d < 500 (si no: 0xBAA370 = 0, pos queda sin inicializar)
         estaticas: 0xBAA380 guarda (bit 0: 0xBAA37C paso, bit 1: 0xBAA378 fin; ambas = (int16)ftol(-longitud)) ; pos = pos del blip ; 0x583530 (descartada) ; gira: a = rumbo*0x8595EC,
         x = cos(a)*paso + x, y = y - sin(a)*paso ; 0x583530 del punto girado -> radar ; dd = sqrt(ry*ry + rx*rx) ; si !(dd < 0.9) (fcom: NaN cuenta como menor) o (ahora - 0xBAA374) > 3 sin signo:
         paso = (int16)(paso + 100) ; paso > longitud -> (int16)ftol(-longitud) ; fin == paso -> 0xBAA370 = 0 ; dd < 0.9 -> fin = paso, 0xBAA370 = 1 ; 0xBAA374 = ahora ; despues 0x5832F0 (descartada)
  58713D 0x584770(color, brillo, amistoso) ; pantalla 1 o 3 y DbgFlag: 0x584480(pos, color, radio) y radio = radio - 0.1 ; < 1.0 -> 5.0 ; enex del blip -> 0x43EA00 ;
         pantalla distinta de 2 y 3 sale ; 0x583530(pos.xy) -> radar ; 0x5832F0 -> distancia ; 0x583480 -> pantalla ; corto alcance y distancia > 1.0 y no mapa sale
  587264 zoom != 0 y 0x56E0D0(-1,0) nulo y tipo no 3/7: isSprite sale ; sin bit 4 (remain) y !(sqrt(ry*ry+rx*rx) <= 1.0) sale ; rumbo = entidad ? 0x441DB0 : 0 ; mapa y no ShowMission sale ;
         0x56E400(&centro, 0) ; z-2 > cz: ShowAll ? 0x584070(alto 0) : sale ; z+4 < cz: alto 1 ; si no flecha 0x584850(sprite 2, x, y, rumbo + pi | rumbo - (orientacion + pi), w, h, color) con
         h = ftol(alto*0x859524*8), w = ftol(ancho*0x859520*8) y camara activa en modo 1 (word)
  587790 isSprite: sprite == 0 sale ; 0x583AF0(blip) ; 0x585FF0(sprite, x, y, 255) ; si no: sprite != 0 sale ; mapa y no ShowMission sale ;
         foco: dd <= 0.9 y !mapa -> 2 circulos 0x727D60 (radio = ((ancho*0x859520)*94/rango*radioFoco)*0.6 ; luego radio - ancho*0x859520 ; colores 0,0,0,150 y 220,220,220,200 ; 15 segmentos) y sale
         pista: !mapa y dd < 0.9 y d < 500 -> 0x584850(sprite 56, x, y, 0, w, h (x8), blanco) ; pos = pos del blip ; 0x583530, 0x5832F0, 0x583480 de nuevo ;
                mapa: rumbo = dir (-90 si no es la pista 1) ; 0x584850(sprite 57, x, y, rumbo*rad, w, h (x16)) y 0x5859F0(0, 57) ; sin mapa y 0xBAA370 == 0: 0x584850(sprite 57, x, y, (-orient) - (dir-90)*rad)
         resto: 0x56E400(&centro, 0) ; alto por z-2 > cz / z+4 < cz ; 0x584070(x, y, tam, c>>24, c>>16, c>>8, c, alto) ; mapa: 0x5859F0(1, blipIndex)
"""
import math
import struct
import sys
from fractions import Fraction
from pathlib import Path

M32 = 0xFFFFFFFF
CASES = 12288
NAME = 'CRadar::DrawEntityBlip'
NAMES = [NAME]
EXE = Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe')
# constantes del EXE (se contrastan con el EXE si esta disponible)
CONST_BITS = {0x858624: 0x3F800000, 0x858B58: 0x43FA0000, 0x858C20: 0x3F666666, 0x858CA0: 0x40000000, 0x858B90: 0x40800000, 0x858CB8: 0x40490FDB, 0x8595EC: 0x3C8EFA35,
              0x859000: 0x41000000, 0x866B78: 0x42BC0000, 0x859520: 0x3ACCCCCD, 0x859524: 0x3B124925, 0x858CC8: 0x3F19999A, 0x8599D0: 0x41800000, 0x85991C: 0x42B40000,
              0x858B1C: 0x3DCCCCCD, 0x858C48: 0x3E000000}
STRIPS = [(1750.0, -2494.0, 180.0, 1000.0), (-1373.0, 120.0, 315.0, 1500.0), (1478.0, 1461.0, 90.0, 1200.0), (175.0, 2502.0, 180.0, 1000.0)]   # 0x8D06E0, 16 bytes cada una


def exe_constants_ok():
    if not EXE.exists():
        return None
    d = EXE.read_bytes()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    n = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    secs = [struct.unpack_from('<8sIIII', d, pe + 24 + opt + i * 40) for i in range(n)]

    def off(va):
        va -= 0x400000
        for _, vs, vaa, rs, ro in secs:
            if vaa <= va < vaa + rs:
                return ro + va - vaa
        raise KeyError(hex(va))
    bad = {hex(a): (hex(struct.unpack_from('<I', d, off(a))[0]), hex(b)) for a, b in CONST_BITS.items() if struct.unpack_from('<I', d, off(a))[0] != b}
    for i, (x, y, dr, ln) in enumerate(STRIPS):
        got = struct.unpack_from('<4f', d, off(0x8D06E0 + 16 * i))
        if got != (x, y, dr, ln):
            bad['strip%d' % i] = (got, (x, y, dr, ln))
    return bad


def c(addr):
    return CONST_BITS[addr]


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


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def isnan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def qn(b):
    return b | 0x400000


def _op2(a, b, f):
    if isnan(a):
        return qn(a)
    if isnan(b):
        return qn(b)
    return bits(f32(f(fl(a), fl(b))))


def fadd(a, b): return _op2(a, b, lambda x, y: x + y)
def fsub(a, b): return _op2(a, b, lambda x, y: x - y)
def fmul(a, b): return _op2(a, b, lambda x, y: x * y)
def fdiv(a, b): return _op2(a, b, lambda x, y: x / y)


def fsqrt(a):
    if isnan(a):
        return qn(a)
    v = fl(a)
    if v < 0:
        return 0xFFC00000          # indefinido real del x87 (y de sqrtss)
    return bits(f32(math.sqrt(v)))


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


def r24bits(x):
    return bits(float(r24(x)))


def mulint(n, kbits):
    """fild n ; fmul dword k  (n exacto, un redondeo a 24 bits)."""
    return r24bits(Fraction(n) * Fraction(fl(kbits)))


def cos_sin(a, prec=200):
    """cos y sin de un racional exacto con ~200 bits (serie de Taylor en punto fijo, terminos con signo explicito)."""
    one = 1 << prec
    x = int(Fraction(a) * one)
    neg = x < 0
    ax = -x if neg else x
    x2 = (ax * ax) >> prec
    cos, mag, sign, k = 0, one, 1, 0
    while mag:
        cos += sign * mag
        mag = ((mag * x2) >> prec) // ((2 * k + 1) * (2 * k + 2))
        sign = -sign
        k += 1
    sin, mag, sign, k = 0, ax, 1, 0
    while mag:
        sin += sign * mag
        mag = ((mag * x2) >> prec) // ((2 * k + 2) * (2 * k + 3))
        sign = -sign
        k += 1
    if neg:
        sin = -sin
    return Fraction(cos, one), Fraction(sin, one)


def ftol(bits_):
    """_ftol2: trunca a entero de 64 bits (la funcion usa solo la parte baja)."""
    v = fl(bits_)
    return int(v)


def u32at(b, at):
    return struct.unpack_from('<I', b, at)[0]


class Done(Exception):
    pass


class Model:
    def __init__(self, inp):
        self.inp = inp
        self.calls = []
        self.cx = self.cl = self.cs = self.cv = 0
        self.trace = bytearray.fromhex(inp['trace'])
        self.st = bytearray.fromhex(inp['statics'])
        self.stats = {}
        self.garbage = False

    # ------------------------------------------------------------------ colaboradores sustituidos
    def veh(self):
        self.calls.append(['veh', M32, 0])
        call = self.cv
        self.cv += 1
        mode = self.inp['pv_mode']
        if mode == 0:
            return None
        i = 0 if mode == 1 else min(call, 3)
        return {'desc': self.inp['pv'][i], 'id': 0x500 + i}

    def centre(self):
        self.calls.append(['centre', 0])
        return list(self.inp['centre'])

    def xform(self, x, y):
        k = min(self.cx, 3)
        first = self.cx == 0
        self.cx += 1
        self.calls.append(['xform', 0, 0] if (self.garbage and first) else ['xform', x, y])
        return list(self.inp['xform'][k])

    def limit(self, p):
        k = min(self.cl, 2)
        self.cl += 1
        self.calls.append(['limit', p[0], p[1]])
        p[0], p[1] = self.inp['limit_pt'][k]
        return self.inp['limit_dist'][k]

    def screen(self, p):
        k = min(self.cs, 1)
        self.cs += 1
        self.calls.append(['screen', p[0], p[1]])
        return list(self.inp['screen'][k])

    def enex(self, ptr, pos):
        self.calls.append(['enex', ptr] + ([0, 0, 0] if self.garbage else list(pos)))
        k = float(((ptr >> 2) & 0xFF) + 1)
        pos[0] = fadd(pos[0], bits(f32(k * 3.5)))
        pos[1] = fsub(pos[1], bits(f32(k * 1.25)))
        pos[2] = fadd(pos[2], bits(10.0))

    def trace_height(self, sp, size, colour, height):
        self.calls.append(['trace_height', sp[0], sp[1], size, (colour >> 24) & M32, (colour >> 16) & M32, (colour >> 8) & M32, colour & M32, height])

    # ------------------------------------------------------------------ pools
    def pool_get(self, pool, handle):
        idx = s32(handle) >> 8
        if 0 <= idx < 8 and self.inp['states'][pool][idx] == (handle & 0xFF):
            return {'desc': self.inp['ents'][pool][idx], 'id': (0x100, 0x200, 0x300)[pool] + idx}
        return None

    def run(self):
        inp = self.inp
        T = self.trace
        colour_ = u32at(T, 0)
        handle = u32at(T, 4)
        pos = [u32at(T, 8), u32at(T, 12), u32at(T, 16)]
        enex_ptr = u32at(T, 0x20)
        size = struct.unpack_from('<h', T, 0x1C)[0] & M32
        sprite = T[0x24]
        f25, f26 = T[0x25], T[0x26]
        typ = (f26 >> 2) & 0xF
        disp = f26 & 3
        is_sprite = (inp['is_sprite'] & 0xFF) != 0
        drawing_map = inp['drawing_map'] != 0
        show_mission = inp['show_mission'] != 0
        st = self.st
        stats = self.stats
        stats.update(typ=typ, disp=disp, is_sprite=is_sprite, path='none')
        stats['sprite_raw_garbage'] = (inp['is_sprite'] & 0xFFFFFF00) != 0
        stats['sprite_zero_in_sprite_pass'] = is_sprite and sprite == 0
        entity = None
        spot = False
        air = False
        air_dist = 0
        sx_ = lambda: mulint(inp['width'], c(0x859520))
        sy_ = lambda: mulint(inp['height'], c(0x859524))

        def set_entity(e):
            nonlocal entity, pos
            entity = e
            if e is None:
                return False
            d = e['desc']
            pos = list(d['mpos'] if d['mat'] else d['pos'])
            stats['matrix' if d['mat'] else 'no_matrix'] = True
            if (d['type'] & 7) == 3 and d['enex']:
                self.enex(d['enex'], pos)
                stats['entity_enex'] = True
            elif (d['type'] & 7) == 3:
                stats['type3_no_enex'] = True
            elif d['enex']:
                stats['enex_ignored_type'] = True
            return True

        def vehicle_of(kind):
            if kind < 0:
                return None
            if kind == 4:
                return {'desc': inp['extra_veh'], 'id': 0x400}
            return {'desc': inp['ents'][0][kind], 'id': 0x100 + kind}

        try:
            if typ == 1:
                if not set_entity(self.pool_get(0, handle)):
                    stats['pool_miss'] = True
                    raise Done
            elif typ == 2:
                e = self.pool_get(1, handle)
                if e is None:
                    stats['pool_miss'] = True
                    raise Done
                if e['desc']['f46d'] & 1:
                    stats['ped_in_veh'] = True
                    if e['desc']['veh'] == 4:
                        stats['ped_veh_extra'] = True
                    if not set_entity(vehicle_of(e['desc']['veh'])):
                        stats['ped_veh_null'] = True
                        raise Done
                else:
                    stats['ped_foot'] = True
                    if not set_entity(e):
                        raise Done
            elif typ == 3:
                if not set_entity(self.pool_get(2, handle)):
                    stats['pool_miss'] = True
                    raise Done
            elif typ == 6:
                self.calls.append(['script_index', handle, 2])
                if s32(inp['script_idx']) < 0:
                    stats['script_idx_neg'] = True
                    raise Done
                spot = True
                pos = list(inp['spot_target'])
            elif typ == 7:
                self.calls.append(['pickup_index', handle])
                if s32(inp['pickup_idx']) < 0:
                    stats['pickup_idx_neg'] = True
                    raise Done
                pos = [bits(f32(v * 0.125)) for v in inp['pickup_raw']]
            elif typ == 8:
                air = True
                pvs = [self.veh() for _ in range(4)]
                loc = inp['air_loc']
                ax, ay, adir, alen = STRIPS[loc]
                pp = [p['desc']['mpos'] if p['desc']['mat'] else p['desc']['pos'] for p in pvs]
                prod_y = fmul(fsub(bits(ay), pp[3][1]), fsub(bits(ay), pp[2][1]))
                prod_x = fmul(fsub(bits(ax), pp[0][0]), fsub(bits(ax), pp[1][0]))
                air_dist = fsqrt(fadd(prod_y, prod_x))
                near = fl(air_dist) < fl(c(0x858B58)) if not isnan(air_dist) else False
                stats['air_near'] = near
                stats['air_far'] = not near
                if isnan(air_dist):
                    stats['air_dist_nan'] = True
                if not near:
                    st[0] = 0
                    pos = [0, 0, 0]              # basura en el EXE; todo lo derivado se enmascara
                    self.garbage = True
                else:
                    guard = u32at(st, 0x10)
                    step = None
                    if not guard & 1:
                        guard |= 1
                        step = s16(int(-alen))
                        struct.pack_into('<h', st, 0xC, step)
                    else:
                        step = struct.unpack_from('<h', st, 0xC)[0]
                    if not guard & 2:
                        guard |= 2
                        struct.pack_into('<h', st, 8, s16(int(-alen)))
                    struct.pack_into('<I', st, 0x10, guard)
                    stats['guard_init'] = (guard & 3) != 3 or True
                    pos = [u32at(T, 8), u32at(T, 12), u32at(T, 16)]
                    self.xform(pos[0], pos[1])                       # descartada
                    a = r24(Fraction(fl(bits(adir))) * Fraction(fl(c(0x8595EC))))
                    cs, sn = cos_sin(a)
                    n = Fraction(step)
                    nx = r24bits(r24(cs * n) + Fraction(fl(pos[0])))
                    ny = r24bits(Fraction(fl(pos[1])) - r24(sn * n))
                    pos[0], pos[1] = nx, ny
                    rp = self.xform(pos[0], pos[1])
                    dd = fsqrt(fadd(fmul(rp[1], rp[1]), fmul(rp[0], rp[0])))
                    now = inp['time_ms'] & M32
                    last = u32at(st, 4)
                    ge = (not isnan(dd)) and fl(dd) >= fl(c(0x858C20))
                    if (not ge) and ((now - last) & M32) <= 3:
                        stats['air_skip'] = True
                    else:
                        step = s16(step + 100)
                        struct.pack_into('<h', st, 0xC, step)
                        if float(step) > alen:
                            step = s16(int(-alen))
                            struct.pack_into('<h', st, 0xC, step)
                            stats['air_wrap'] = True
                        if struct.unpack_from('<h', st, 8)[0] == step:
                            st[0] = 0
                        if (not isnan(dd)) and fl(dd) < fl(c(0x858C20)):
                            struct.pack_into('<h', st, 8, step)
                            st[0] = 1
                        struct.pack_into('<I', st, 4, now)
                        stats['air_update'] = True
                    self.limit(rp)
            else:
                raise Done
            stats['path'] = 'air' if air else 'spot' if spot else 'entity'

            # ---- 58713D
            bright, friendly = f25 & 1, (f25 >> 3) & 1
            self.calls.append(['colour', colour_, bright, friendly])
            colour = inp['colour'] & M32
            if disp in (3, 1) and inp['dbg']:
                self.calls.append(['marker'] + ([0, 0, 0] if self.garbage else list(pos)) + [colour, u32at(T, 0x18)])
                rad = fsub(u32at(T, 0x18), c(0x858B1C))
                if (not isnan(rad)) and fl(rad) < fl(c(0x858624)):
                    rad = bits(5.0)
                    stats['marker_wrap'] = True
                struct.pack_into('<I', T, 0x18, rad)
                stats['marker'] = True
            if enex_ptr:
                self.enex(enex_ptr, pos)
                stats['trace_enex'] = True
            if disp not in (3, 2):
                stats['disp_exit'] = True
                raise Done
            rp = self.xform(pos[0], pos[1])
            dist = self.limit(rp)
            sp = self.screen(rp)
            if (f25 & 4) and fl(dist) > 1.0 and not drawing_map:
                stats['shortrange_exit'] = True
                raise Done
            white = 0xFFFFFFFF
            rgba = ((colour >> 24) & 0xFF) | (((colour >> 16) & 0xFF) << 8) | (((colour >> 8) & 0xFF) << 16) | (0xFF << 24)
            sprite_idx = lambda s: (s - 256 if s >= 128 else s) & 0xFFFF

            # ---- 587264: zoom
            if inp['zoom'] != 0:
                v = self.veh()
                if v is not None:
                    stats['zoom_with_vehicle'] = True
                elif typ in (3, 7):
                    stats['zoom_obj_pickup'] = True
                if v is None and typ not in (3, 7):
                    stats['zoom_path'] = True
                    if is_sprite:
                        raise Done
                    if f25 & 0x10:
                        stats['zoom_remain'] = True
                    else:
                        dd = fsqrt(fadd(fmul(rp[1], rp[1]), fmul(rp[0], rp[0])))
                        if not ((not isnan(dd)) and fl(dd) <= 1.0):
                            stats['zoom_far_exit'] = True
                            raise Done
                    heading = 0
                    if entity is not None:
                        self.calls.append(['heading', entity['id']])
                        heading = inp['heading']
                    if drawing_map and not show_mission:
                        stats['zoom_map_exit'] = True
                        raise Done
                    ce = self.centre()
                    z = pos[2]
                    cz = fl(ce[2])
                    if fl(fsub(z, c(0x858CA0))) > cz:
                        stats['zoom_height'] = 'low'
                        if not inp['show_all']:
                            raise Done
                        self.trace_height(sp, size, colour, 0)
                        raise Done
                    if fl(fadd(z, c(0x858B90))) < cz:
                        stats['zoom_height'] = 'high'
                        if not inp['show_all']:
                            raise Done
                        self.trace_height(sp, size, colour, 1)
                        raise Done
                    stats['zoom_height'] = 'same'
                    h = ftol(fmul(sy_(), c(0x859000))) & M32
                    w = ftol(fmul(sx_(), c(0x859000))) & M32
                    mode = inp['cam_modes'][inp['cam_active']] & 0xFFFF
                    stats['topdown'] = mode == 1
                    if mode == 1:
                        angle = fadd(heading, c(0x858CB8))
                    else:
                        angle = fsub(heading, fadd(inp['orient'], c(0x858CB8)))
                    self.calls.append(['rotating_sprite', 2, sp[0], sp[1], angle, w, h, rgba])
                    raise Done

            # ---- 587790
            if is_sprite:
                if sprite == 0:
                    raise Done
                self.calls.append(['revealed', inp['blip']])
                if not inp['revealed']:
                    raise Done
                self.calls.append(['sprite', sprite_idx(sprite), sp[0], sp[1], 255])
                stats['sprite_draw'] = True
                if sprite >= 128:
                    stats['sprite_negative'] = True
                raise Done
            if sprite != 0:
                raise Done
            if drawing_map and not show_mission:
                stats['map_exit'] = True
                raise Done
            if spot:
                dd = fsqrt(fadd(fmul(rp[1], rp[1]), fmul(rp[0], rp[0])))
                if (not isnan(dd)) and fl(dd) <= fl(c(0x858C20)) and not drawing_map:
                    rad = fmul(sx_(), c(0x866B78))
                    rad = fdiv(rad, inp['range'])
                    rad = fmul(rad, inp['spot_radius'])
                    rad = fmul(rad, c(0x858CC8))
                    self.calls.append(['circle', sp[0], sp[1], rad, 0x96000000, 15])
                    self.calls.append(['circle', sp[0], sp[1], fsub(rad, sx_()), 0xC8DCDCDC, 15])
                    stats['circles'] = True
                elif drawing_map:
                    stats['spot_map'] = True
                else:
                    stats['spot_far'] = True
                raise Done
            if air:
                loc = inp['air_loc']
                ax, ay, adir, alen = STRIPS[loc]
                if not drawing_map:
                    dd = fsqrt(fadd(fmul(rp[1], rp[1]), fmul(rp[0], rp[0])))
                    if (not isnan(dd)) and fl(dd) < fl(c(0x858C20)) and (not isnan(air_dist)) and fl(air_dist) < fl(c(0x858B58)):
                        h = ftol(fmul(sy_(), c(0x859000))) & M32
                        w = ftol(fmul(sx_(), c(0x859000))) & M32
                        self.calls.append(['rotating_sprite', 56, sp[0], sp[1], 0, w, h, white])
                        stats['air_light'] = True
                pos = [u32at(T, 8), u32at(T, 12), u32at(T, 16)]
                rp = self.xform(pos[0], pos[1])
                self.limit(rp)
                sp = self.screen(rp)
                h = ftol(fmul(sy_(), c(0x8599D0))) & M32
                w = ftol(fmul(sx_(), c(0x8599D0))) & M32
                if drawing_map:
                    heading = bits(adir)
                    if loc != 1:
                        heading = fsub(heading, c(0x85991C))
                    self.calls.append(['rotating_sprite', 57, sp[0], sp[1], fmul(heading, c(0x8595EC)), w, h, white])
                    self.calls.append(['legend', 0, 57])
                    stats['air_map'] = True
                elif st[0] == 0:
                    turn = fmul(fsub(bits(adir), c(0x85991C)), c(0x8595EC))
                    neg_orient = inp['orient'] ^ 0x80000000
                    self.calls.append(['rotating_sprite', 57, sp[0], sp[1], fsub(neg_orient, turn), w, h, white])
                    stats['air_runway'] = True
                raise Done

            ce = self.centre()
            z = pos[2]
            cz = fl(ce[2])
            if fl(fsub(z, c(0x858CA0))) > cz:
                height = 0
            elif fl(fadd(z, c(0x858B90))) < cz:
                height = 1
            else:
                height = 2
            stats['height'] = height
            self.trace_height(sp, size, colour, height)
            if drawing_map:
                self.calls.append(['legend', 1, inp['blip']])
                stats['legend'] = True
        except Done:
            pass
        return self


def expected(inp, case):
    m = Model(inp).run()
    out = {'calls': m.calls, 'trace_slot': bytes(m.trace).hex(), 'statics': bytes(m.st).hex(), 'others_unchanged': True, 'trace_overflow': False}
    return out, m.stats


def _selftest():
    bad = exe_constants_ok()
    assert bad in (None, {}), bad
    # redondeo, trigonometria y x87
    assert r24(Fraction(1, 3)) == Fraction(11184811, 2 ** 25)
    cs, sn = cos_sin(Fraction(1, 2))
    assert abs(float(cs) - math.cos(0.5)) < 1e-15 and abs(float(sn) - math.sin(0.5)) < 1e-15
    cs, sn = cos_sin(Fraction(11, 2))
    assert abs(float(cs) - math.cos(5.5)) < 1e-15 and abs(float(sn) - math.sin(5.5)) < 1e-15
    assert cs * cs + sn * sn - 1 < Fraction(1, 2 ** 150) and 1 - (cs * cs + sn * sn) < Fraction(1, 2 ** 150)
    assert fsqrt(bits(-1.0)) == 0xFFC00000 and fsqrt(bits(4.0)) == bits(2.0)
    assert mulint(640, c(0x859520)) == bits(f32(640 * fl(c(0x859520))))

    def ent(typ=2, pos=(0.0, 0.0, 0.0), mat=False, mpos=(0.0, 0.0, 0.0), f46d=0, veh=-1, enex=0):
        return {'type': typ, 'mat': int(mat), 'f46d': f46d, 'veh': veh, 'pos': [bits(v) for v in pos], 'mpos': [bits(v) for v in mpos], 'enex': enex}

    def base():
        trace = bytearray(40)
        struct.pack_into('<I', trace, 4, (1 << 8) | 5)               # handle: ranura 1, estado 5
        struct.pack_into('<fff', trace, 8, 100.0, 200.0, 30.0)
        struct.pack_into('<f', trace, 0x18, 2.5)
        struct.pack_into('<h', trace, 0x1C, 3)
        trace[0x25] = 0
        trace[0x26] = (1 << 2) | 3                                      # vehiculo, pantalla ambos
        ents = [[ent() for _ in range(4)] for _ in range(3)]
        ents[0][1] = ent(2, (10.0, 20.0, 30.0), True, (11.0, 21.0, 31.0))
        states = [[0x80] * 8 for _ in range(3)]
        states[0][1] = 5
        return dict(kind=0, n=0, blip=7, is_sprite=0, eax=0, edx=0, stack_seed=0, trace=trace.hex(), drawing_map=0, show_mission=1, show_all=1, zoom=0, dbg=0, air_loc=0, cam_active=0,
                    width=640, height=480, orient=bits(0.5), range=bits(1000.0), time_ms=100000, cam_modes=[1, 0, 0], statics=bytes(20).hex(), states=states, ents=ents,
                    extra_veh=ent(), pv_mode=0, pv=[ent() for _ in range(4)], xform=[[bits(0.25), bits(0.5)]] * 4, limit_pt=[[bits(0.25), bits(0.5)]] * 3, limit_dist=[bits(0.5)] * 3,
                    screen=[[bits(300.0), bits(200.0)]] * 2, colour=0x11223344, revealed=1, pickup_idx=0, script_idx=0, heading=bits(1.0), centre=[bits(0.0), bits(0.0), bits(30.0)],
                    spot_target=[0, 0, 0], spot_radius=bits(10.0), pickup_raw=[0, 0, 0])
    # 1) vehiculo normal, no mapa: color, transformaciones, cuadrado con altura 2 (misma altura), sin leyenda
    out, stats = expected(base(), 0)
    names = [x[0] for x in out['calls']]
    assert names == ['colour', 'xform', 'limit', 'screen', 'centre', 'trace_height'], names
    assert out['calls'][0] == ['colour', 0, 0, 0], out['calls'][0]
    th = out['calls'][-1]
    assert th[3] == 3 and th[4] == 0x11 and th[5] == 0x1122 and th[6] == 0x112233 and th[7] == 0x11223344 and th[8] == 2, th
    # 2) con mapa: leyenda (1, blip)
    inp = base(); inp['drawing_map'] = 1
    out, _ = expected(inp, 0)
    assert out['calls'][-1] == ['legend', 1, 7]
    # 3) altura: el jugador 10 por encima: pos.z (31 de la matriz) + 4 < 50 -> alto 1 ; 40 m por debajo -> 0
    inp = base(); inp['centre'][2] = bits(50.0)
    assert expected(inp, 0)[0]['calls'][-1][8] == 1
    inp = base(); inp['centre'][2] = bits(-5.0)
    assert expected(inp, 0)[0]['calls'][-1][8] == 0
    # 4) pase de iconos con sprite 0xFB (-5) revelado: DrawRadarSprite(0xFFFB)
    inp = base(); inp['is_sprite'] = 0xAB01
    t = bytearray.fromhex(inp['trace']); t[0x24] = 0xFB; inp['trace'] = t.hex()
    out, _ = expected(inp, 0)
    assert out['calls'][-2:] == [['revealed', 7], ['sprite', 0xFFFB, bits(300.0), bits(200.0), 255]], out['calls']
    # 5) corto alcance lejano fuera del mapa: sale tras la pantalla
    inp = base(); t = bytearray.fromhex(inp['trace']); t[0x25] |= 4; inp['trace'] = t.hex(); inp['limit_dist'] = [bits(2.0)] * 3
    assert [x[0] for x in expected(inp, 0)[0]['calls']] == ['colour', 'xform', 'limit', 'screen']
    # 6) estatica del destello: pista en el aire cerca, vehiculos iguales a 100 m al este
    inp = base(); t = bytearray(40); t[0x26] = (8 << 2) | 3; struct.pack_into('<fff', t, 8, 1750.0, -2494.0, 30.0); inp['trace'] = t.hex()
    inp['pv_mode'] = 1; inp['pv'][0] = ent(2, (1850.0, -2494.0, 0.0)); inp['pv'][1] = inp['pv'][2] = inp['pv'][3] = inp['pv'][0]
    inp['xform'] = [[bits(0.5), bits(0.5)]] * 4; inp['limit_dist'] = [bits(0.5)] * 3; inp['time_ms'] = 1000
    out, stats = expected(inp, 0)
    assert [x[0] for x in out['calls']][:4] == ['veh', 'veh', 'veh', 'veh'] and stats['air_near'], out['calls']
    assert struct.unpack_from('<I', bytes.fromhex(out['statics']), 0x10)[0] == 3 and stats.get('air_update'), (out['statics'], stats)
    stat = bytes.fromhex(out['statics'])
    assert struct.unpack_from('<h', stat, 0xC)[0] == -900 and struct.unpack_from('<h', stat, 8)[0] == -900 and stat[0] == 1, stat.hex()   # -1000 + 100; dd < 0.9 -> fin = paso y bandera = 1
    print('ok')


if __name__ == '__main__':
    _selftest()
