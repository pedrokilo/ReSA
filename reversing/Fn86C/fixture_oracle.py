"""Oraculo independiente del banco Fn86C: CPathFind::GeneratePedCreationCoors (EXE 0x44E790), modelado desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL)
a partir de la fila `input` que registra el banco. Devuelve lo que debe observarse en AMBOS lados (EXE con el hook apagado y DLL con el hook encendido).

--------------------------------------------------------------------------------------------------------------------------------
Revision contra el EXE (0x44E790..0x44EC9C, `ret 30h`; argumentos: x, y, min1, max1, min2, max2, salidaCoords, salidaDir1, salidaDir2, salidaRumbo, byte bajoTrafico, matriz):
* rand() del CRT del EXE (0x821B1E): primera tirada r0 = rand & 0xFFFF; umbral = ftol(float(r0) * 2^-15 * 15.0) (0x858B14 y 0x858B48, x87 a 24 bits).
* maxSq = float((max1 + 30.0) * (max1 + 30.0)) (0x858CA4); area = FindRegionForCoors(x, y) (0x44D830: region(x) + 8 * region(y)).
* Hasta 300 iteraciones con la MISMA area: sin nodos cargados o sin nodos de peaton se pasa a la siguiente; nodo = nVeh + (rand() >> 6) % nPed (0x1C bytes cada uno).
  x = int16(+8) * 0.125 (0x858C48), y = int16(+0xA) * 0.125; s = ((y - py) * (y - py)) + ((x - px) * (x - px)) (registro de 24 bits); se exige s < maxSq (comparacion ordenada).
  Se exige trafico(nodo) = byte(+0x1A) & 0xF > umbral (con signo) y enlaces = byte(+0x18) & 0xF distinto de cero.
* Por enlace i (base = int16(+0x10)): se salta si el byte base+i de las intersecciones (this+0xC84) tiene el bit 0 (cruce de calle); destino = dword base+i de los enlaces (this+0xA44):
  area (16 bits bajos) >= 0x40 o sin cargar: se salta; nodo destino = nodos[area] + (dword >> 16) * 0x1C; si el nodo origen o el destino llevan el bit 0x20 del byte +0x18 se exige que el byte bajo
  del argumento de trafico bajo sea distinto de cero; trafico(destino) > umbral; d1 = float(sqrt(float(s))) y suma2 = (oy - py) * (oy - py guardado como float) + (ox - px) * (ox - px) del destino
  (posicion = GetPosition 0x420A10: int16 * 0.125, cuatro lecturas iguales); se sigue solo si d1 < max1 (ordenado) o sqrt(suma2) < max1.
* Hasta 5 intentos por enlace: t = float(rand() & 0xFF) * 2^-8 (0x859AA0) guardado como float en *salidaRumbo (en CADA intento); X = (1 - t) * xo + float(t * xd) (el producto del origen queda en el registro, el del destino
  se guarda como float), Y igual, Z = float((1 - t) * zo) + float(t * zd); todo guardado como float; dist = float(sqrt((Y - py)^2 + (X - px)^2)).
  IsSphereVisible(&(X, Y, Z), 2.0, matriz) 0x420C40 con matriz no nula, o IsSphereVisible(&(X, Y, Z), 2.0) 0x420D40 sin ella; AL decide:
    visible: min1 < dist < max1 (ordenado); no visible: min2 < dist < max2 y (rand() & 1) != 0 (la tirada solo se hace si pasan las dos comparaciones).
  Si se acepta: *salidaDir1 = dword +0x12 del origen, *salidaDir2 = el del destino, *salidaCoords = (X, Y, Z) (Z sin suelo), y CWorld::FindGroundZFor3DCoord(X, Y, float(Z + 2.0), &encontrado, 0) (0x5696C0):
  si no hay suelo se sigue con el intento siguiente; si lo hay: |suelo - Z| > 3.0 (0x858B3C; ordenado) devuelve FALSO ya (no se sigue buscando) y si no (tambien con NaN) *salidaCoords.z = suelo y devuelve VERDADERO.
* Tras 300 iteraciones devuelve falso.
x87 a 24 bits con exponente extendido: cada operacion redondea el significando a 24 bits; solo los fstp dword (y los argumentos en memoria) pasan a float.
"""
import math
import struct

M32 = 0xFFFFFFFF
NAMES = ['CPathFind::GeneratePedCreationCoors']
CASES = 16384
# Constantes de arena (deben coincidir con Fixtures.hpp)
THIS_OFF, THIS_ARENA = 0x40, 0x3D00
NODES_OFF, NODES_ARENA = 0x40, 0x320
LINKS_OFF, LINKS_ARENA = 0x40, 0x300
INTER_OFF, INTER_ARENA, INTER_BYTES = 0x40, 0x120, 0xA0
SMALL_OBJ, SMALL_ARENA = 0x40, 0x140
KEEP = 24
GROUND_N = 16


# ------------------------------------------------------------------------------------------------ generadores (copia de Fixtures.hpp)
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix(((c & M32) * 0x9E3779B1 + (k & M32) * 0x85EBCA6B + 86003) & M32)


def fill_words(seed, nbytes):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += rnd(seed, i // 4).to_bytes(4, 'little')[:min(4, nbytes - i)]
    return out


def hash_step(h, v):
    return mix(((((h + v) & M32) * 0x01000193) & M32) ^ 0xA5A5A5A5)


def script_rand(mode, seed, k):
    if mode == 0:
        return 0
    if mode == 1:
        return 0x7FFF
    if mode == 2:
        return (k * 64) & 0x7FFF
    if mode == 3:
        return 0x7FFF if rnd(seed, k) & 1 else 0
    if mode == 4:
        return rnd(seed, k) & 0x7FFF
    if mode == 5:
        return (rnd(seed, k) & 0x7FFF) | 1
    if mode == 6:
        return (rnd(seed, k) & 0x7FFF) & ~1
    return (k * 37 + 11) & 0x7FFF


def cam_al(mode, seed, k):
    """AL devuelto por el k-esimo IsSphereVisible falso (el resto de EAX es basura y no se observa)."""
    vis = script_rand(mode, seed, k) & 1
    if not vis:
        return 0
    return (1 + rnd(seed, 2000 + k) % 255) & 0xFF


# ------------------------------------------------------------------------------------------------ x87 minimo
INF = float('inf')


def f32(u):
    return struct.unpack('<f', struct.pack('<I', u & M32))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', r32(x)))[0]


def r32(x):
    """fstp dword: redondeo a float (con denormales, desbordes a infinito)."""
    if x != x or x in (INF, -INF):
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return INF if x > 0 else -INF


def r24(x):
    """Operacion x87 con control de precision de 24 bits y exponente extendido."""
    if x == 0 or x != x or x in (INF, -INF):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * 16777216.0), e - 24)


def add(a, b):
    return r24(a + b)


def sub(a, b):
    return r24(a - b)


def mul(a, b):
    try:
        return r24(a * b)
    except OverflowError:
        return INF


def fsqrt(a):
    if a != a:
        return a
    if a < 0:
        return float('nan')
    return r24(math.sqrt(a)) if a != INF else INF


def ftol_low32(v):
    if v != v or v in (INF, -INF):
        return 0
    if v >= 9223372036854775808.0 or v < -9223372036854775808.0:
        return 0
    r = int(v) & M32
    return r - (1 << 32) if r >= (1 << 31) else r


def region(p):
    r = ftol_low32(mul(sub(p, -3000.0), f32(0x3AAEC33E)))
    return 0 if r <= 0 else (r if r < 7 else 7)


def i16(u):
    u &= 0xFFFF
    return u - 0x10000 if u >= 0x8000 else u


def runs(mem, ref, obj_off):
    out = []
    i, n = 0, len(mem)
    while i < n:
        if mem[i] == ref[i]:
            i += 1
            continue
        j = i
        while j < n and mem[j] != ref[j]:
            j += 1
        out.append([i - obj_off, bytes(mem[i:j]).hex()])
        i = j
    return out


class Ev:
    """Acumula eventos: guarda los primeros KEEP renderizados y el hash de todos."""

    def __init__(self):
        self.h = 0
        self.n = 0
        self.items = []

    def push(self, kind, vals, render):
        v = list(vals) + [0] * (14 - len(vals))
        h = hash_step(self.h, kind)
        for x in v:
            h = hash_step(h, x & M32)
        self.h = h
        if self.n < KEEP:
            self.items.append(render)
        self.n += 1


class World:
    """Mundo falso con las arenas (bytes) tal como las construye el banco, incluidas las guardas (la base de enlaces negativa lee de ellas)."""

    def __init__(self, inp, n):
        self.areas = inp['areas']
        self.nveh = inp['nveh']
        self.nped = inp['nped']
        self.slot = {a: s for s, a in enumerate(self.areas)}
        self.nodes, self.links, self.inter = [], [], []
        for s in range(len(self.areas)):
            nd = fill_words(rnd(n, 9100 + s), NODES_ARENA)
            for i, v in enumerate(inp.get('nodes%d' % s, [])):
                nd[NODES_OFF + 4 * i:NODES_OFF + 4 * i + 4] = (v & M32).to_bytes(4, 'little')
            self.nodes.append(nd)
            lk = fill_words(rnd(n, 9200 + s), LINKS_ARENA)
            for i, v in enumerate(inp.get('links%d' % s, [])):
                lk[LINKS_OFF + 4 * i:LINKS_OFF + 4 * i + 4] = (v & M32).to_bytes(4, 'little')
            self.links.append(lk)
            it = fill_words(rnd(n, 9300 + s), INTER_ARENA)
            for i, v in enumerate(inp.get('inter%d' % s, [])):
                it[INTER_OFF + 4 * i:INTER_OFF + 4 * i + 4] = (v & M32).to_bytes(4, 'little')
            self.inter.append(it)

    def loaded(self, a):
        return a in self.slot

    def total(self, a):
        s = self.slot[a]
        return self.nveh[s] + self.nped[s]

    def node(self, a, i):
        s = self.slot[a]
        off = NODES_OFF + i * 0x1C
        return [int.from_bytes(self.nodes[s][off + 4 * k:off + 4 * k + 4], 'little') for k in range(7)]

    def link(self, a, idx):
        s = self.slot[a]
        off = LINKS_OFF + 4 * idx
        return int.from_bytes(self.links[s][off:off + 4], 'little')

    def inter_byte(self, a, idx):
        return self.inter[self.slot[a]][INTER_OFF + idx]


def node_xyz(nw):
    return i16(nw[2]) * 0.125, i16(nw[2] >> 16) * 0.125, i16(nw[3]) * 0.125


def b18(nw):
    return nw[6] & 0xFF


def b1a(nw):
    return (nw[6] >> 16) & 0xFF


def node_addr(nw):
    """dword +0x12: area (16 bits altos de la palabra 4) y nodo (16 bits bajos de la palabra 5)."""
    return ((nw[4] >> 16) & 0xFFFF) | ((nw[5] & 0xFFFF) << 16)


def expected(name, inp, case):
    w = World(inp, case)
    n = case
    x, y = f32(inp['x']), f32(inp['y'])
    min1, max1, min2, max2 = f32(inp['min1']), f32(inp['max1']), f32(inp['min2']), f32(inp['max2'])
    low = inp['low_traffic'] & 0xFF
    matrix = inp['matrix_mode']
    ev = Ev()
    st = {}
    ridx, gidx, cidx = [0], [0], [0]
    out_coords = list(inp['out_init'])
    out_ori = inp['out_ori_init']
    out_addr = list(inp['out_addr_init'])

    def rand():
        k = ridx[0]
        ev.push(6, [k], ['rand', k])
        ridx[0] += 1
        return script_rand(inp['rand_mode'], inp['rand_seed'], k)

    r0 = rand() & 0xFFFF
    thr = ftol_low32(mul(mul(float(r0), 3.0517578125e-05), 15.0))
    st['thr_zero'] = thr == 0
    st['thr_high'] = thr >= 12
    max_s = add(max1, 30.0)
    max_sq = r32(mul(max_s, max_s))
    rx, ry = region(x), region(y)
    area = rx + ry * 8
    result = False
    aborted = False
    iters = 0
    st['no_area'] = not w.loaded(area)
    st['matrix_mode_%d' % matrix] = True
    for it in range(300):
        iters += 1
        if not w.loaded(area):
            continue
        s = w.slot[area]
        nped = w.nped[s]
        if nped == 0:
            st['nped0'] = True
            continue
        r = rand() >> 6
        idx = w.nveh[s] + (r % nped)
        nw = w.node(area, idx)
        nx, ny, nz = node_xyz(nw)
        a = sub(ny, y)
        dy2 = mul(a, a)
        c = sub(nx, x)
        dx2 = mul(c, c)
        sreg = add(dy2, dx2)
        sumf = r32(sreg)
        if not (sreg < max_sq):
            st['node_far'] = True
            continue
        if not ((b1a(nw) & 0xF) > thr):
            st['traffic_fail'] = True
            continue
        nl = b18(nw) & 0xF
        if nl == 0:
            st['nolinks'] = True
            continue
        base = i16(nw[4])
        st['neg_base'] = base < 0
        dval = sumf     # el EXE guarda la raiz en el mismo float [esp+20h] que la suma: cada enlace que llega a la raiz la aplica sobre el valor anterior
        for li in range(nl):
            if w.inter_byte(area, base + li) & 1:
                st['cross_skip'] = True
                continue
            linked = w.link(area, base + li)
            la = linked & 0xFFFF
            if la >= 0x40:
                st['link_area_big'] = True
                continue
            if not w.loaded(la):
                st['link_unloaded'] = True
                continue
            ow = w.node(la, linked >> 16)
            if (b18(nw) & 0x20) or (b18(ow) & 0x20):
                if low == 0:
                    st['switched_skip'] = True
                    continue
                st['lowtraffic_pass'] = True
            if not ((b1a(ow) & 0xF) > thr):
                st['link_traffic_fail'] = True
                continue
            dval = r32(fsqrt(dval))
            d1 = dval
            ox, oy, oz = node_xyz(ow)
            t3 = r32(sub(oy, y))
            t4 = sub(oy, y)
            m1 = mul(t4, t3)
            e1 = sub(ox, x)
            m2 = mul(e1, e1)
            sum2 = add(m1, m2)
            if d1 < max1:
                st['d1_pass'] = True
            else:
                if not (fsqrt(sum2) < max1):
                    st['d1_fail_sqrt_fail'] = True
                    continue
                st['d1_fail_sqrt_pass'] = True
            st['link_reached'] = True
            for j in range(5):
                rv = rand() & 0xFF
                t = r32(mul(float(rv), 0.00390625))
                out_ori = bits(t)
                ax, ay, az = r32(mul(t, ox)), r32(mul(t, oy)), r32(mul(t, oz))
                omt = r32(sub(1.0, t))
                bx, by, bz = mul(omt, nx), mul(omt, ny), r32(mul(omt, nz))
                xf = r32(add(bx, ax))
                yf = r32(add(by, ay))
                zf = r32(add(bz, az))
                dxx = sub(xf, x)
                dyy = sub(yf, y)
                dist = r32(fsqrt(add(mul(dyy, dyy), mul(dxx, dxx))))
                k = cidx[0]
                cidx[0] += 1
                al = cam_al(inp['cam_mode'], inp['cam_seed'], k)
                vals = [bits(xf), bits(yf), bits(zf), 0x40000000]
                if matrix:
                    ev.push(8, vals + [1, 1, k], ['cam_m'] + vals + [1, 1, k])
                else:
                    ev.push(9, vals + [1, k], ['cam_s'] + vals + [1, k])
                if al:
                    st['cam_visible'] = True
                    if not (dist > min1):
                        st['vis_min_fail'] = True
                        continue
                    if not (dist < max1):
                        st['vis_max_fail'] = True
                        continue
                    st['vis_ok'] = True
                else:
                    st['cam_hidden'] = True
                    if not (dist > min2):
                        st['hid_min_fail'] = True
                        continue
                    if not (dist < max2):
                        st['hid_max_fail'] = True
                        continue
                    if not (rand() & 1):
                        st['hid_parity_fail'] = True
                        continue
                    st['hid_ok'] = True
                out_addr = [node_addr(nw), node_addr(ow)]
                out_coords = [bits(xf), bits(yf), bits(zf)]
                zarg = r32(add(zf, 2.0))
                kg = gidx[0]
                gidx[0] += 1
                gv = [bits(xf), bits(yf), bits(zarg), 1, 0, kg]
                ev.push(7, gv, ['ground'] + gv)
                found = inp['ground_found'][kg] if kg < GROUND_N else 0
                delta = f32(inp['ground_delta'][kg]) if kg < GROUND_N else 0.0
                gz = r32(zarg + delta)
                if not found:
                    st['ground_notfound'] = True
                    continue
                st['ground_found'] = True
                diff = abs(sub(gz, zf))
                if diff > 3.0:
                    st['ground_far'] = True
                    aborted = True
                    break
                if diff != diff:
                    st['ground_nan'] = True
                st['ground_ok'] = True
                out_coords[2] = bits(gz)
                result = True
                break
            if result or aborted:
                break
        if result or aborted:
            break
    st['success'] = result
    st['fail'] = not result
    st['aborted'] = aborted
    st['exhausted'] = not result and not aborted and iters == 300
    st['many_rand'] = ridx[0] > 100
    st['ret_1'] = result
    st['ret_0'] = not result
    st['ground_called'] = gidx[0] > 0
    st['multi_ground'] = gidx[0] > 1
    for q in (inp['x'], inp['y'], inp['min1'], inp['max1'], inp['min2'], inp['max2']):
        if ((q >> 23) & 0xFF) == 0xFF or (((q >> 23) & 0xFF) == 0 and (q & 0x7FFFFF) != 0):
            st['special_arg'] = True
    st['ground_delta_nan'] = any(f32(d) != f32(d) for d in inp['ground_delta'])
    st['rand_mode%d' % inp['rand_mode']] = True
    st['cam_mode%d' % inp['cam_mode']] = True
    ns = len(w.areas)
    obs = {'events': ev.items, 'nevents': ev.n, 'ev_hash': ev.h, 'rand_calls': ridx[0], 'ground_calls': gidx[0], 'cam_calls': cidx[0], 'ret_al': 1 if result else 0,
           'coords': out_coords, 'coords_other': [], 'ori': out_ori, 'ori_other': [], 'addr': out_addr, 'addr_other': [[], []], 'matrix_changes': [],
           'world': {'this': [], 'nodes': [[] for _ in range(ns)], 'links': [[] for _ in range(ns)], 'inter': [[] for _ in range(ns)]}}
    st = {k: v for k, v in st.items() if v is True}
    return obs, st


def selftest():
    """Autoprueba: valores calculados a mano desde el desensamblado (no desde las filas del banco)."""
    assert region(-3000.0) == 0 and region(0.0) == 4 and region(2999.0) == 7 and region(float('nan')) == 0
    assert r24(1.0 + 2.0 ** -25) == 1.0 and hash_step(0, 0) == mix(0xA5A5A5A5) and script_rand(2, 0, 3) == 192
    assert ftol_low32(mul(mul(65535.0, 3.0517578125e-05), 15.0)) == 29 and ftol_low32(mul(mul(32767.0, 3.0517578125e-05), 15.0)) == 14
    print('ok')


if __name__ == '__main__':
    selftest()
