"""Oraculo independiente del banco PathFind62: seis funciones de CPathFind, modeladas desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL)
a partir de la fila `input` que registra el banco. Devuelve lo que debe observarse en AMBOS lados (EXE con el hook apagado y DLL con el hook encendido).

--------------------------------------------------------------------------------------------------------------------------------
Convenciones (todas verificadas en el desensamblado): CPathNode = 0x1C bytes (+8/+A/+C x,y,z int16 en 1/8, +E total, +10 base de enlaces int16, +12 area, +14 nodo,
+16 anchura, +17 inundacion, +18 byte0 [bits 0-3 enlaces, 0x10 callejon, 0x20 apagado, 0x40, 0x80 agua], +19 byte1 [1, 2 marca, 4 no deambular, 8, 0x10, 0x20],
+1A byte2 [alto = tipo de conducta: 2 aparcamiento], +1B). this+0x804 punteros de nodos, +0xA44 de enlaces, contadores +0xFA4 (total), +0x10C4 (vehiculos), +0x11E4 (peatones).
x87 a 24 bits con exponente extendido: cada operacion redondea el significando a 24 bits; solo los fstp dword (y los argumentos en memoria) pasan a float.

0x44F460 FindNodeClosestToCoors (thiscall, retptr, x,y,z, tipo, dist, u2,u3,u4,barcos,u6; ret 2Ch)
  res = (area -1, nodo = residuo de pila). rx = clamp(ftol((x + 3000) * 0.0013333333), 0..7) (ftol = 32 bits bajos del entero de 64 bits truncado), idem ry.
  minD = x - (rx*750-3000); minD = (minD < este) ? minD : este - (rx+1)*750.. (ver codigo: este, sur, norte con "si !(min < nuevo) min = nuevo").
  region(rx + ry*8, ..., u6); si dist > minD: anillos 1..4 [oeste, este, sur, norte] llamando a region(area, ..., u6) y tras cada anillo minD += 750; si dist < minD: fin.
  si (u6 & 0xFF) == 0: region(a, ..., 0) para a = 0x40..0x47.  Devuelve el dword res por el puntero oculto (EAX = puntero).
0x44F8C0 FindNthNodeClosestToCoors: limpia el bit 2 (0xFD) del byte +0x19 de los nodos de tipo (tipo 0: [0, veh), tipo 1: [veh, total); otro tipo: nada),
  luego, si nth > 0: repite {res = find(.., u2=bajo, u3=unkn, u4=1, barcos, u6=ignorar); si hay salida: *salida = res; si area == -1: devolver res;
  nodo(res).byte+0x19 |= 2} mientras --nth > 0;  al final devuelve find(.., u6 = 0).
0x451B70 FindNextNodeWandering, 0x452270 CompleteNewInterior, 0x4513F0 FindParkingNodeInArea y 0x44ECA0 GeneratePedCreationCoors_Interior: ver los comentarios de cada funcion.
"""
import math
import struct

M32 = 0xFFFFFFFF
NAMES = ['CPathFind::FindNodeClosestToCoors', 'CPathFind::FindNthNodeClosestToCoors', 'CPathFind::FindNextNodeWandering',
         'CPathFind::CompleteNewInterior', 'CPathFind::FindParkingNodeInArea', 'CPathFind::GeneratePedCreationCoors_Interior']
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
# Constantes de arena (deben coincidir con Fixtures.hpp: lo comprueba fixture_mirror.py)
THIS_OFF, THIS_ARENA = 0x40, 0x3D00
NODES_OFF, NODES_ARENA = 0x40, 0x320
LINKS_OFF, LINKS_ARENA = 0x40, 0x300
SMALL_OBJ, SMALL_ARENA = 0x40, 0x90
MALLOC_OFF, MALLOC_ARENA, MALLOC_BYTES = 0x40, 0x2880, 0x2800
GLOB_A_ADDR, GLOB_A_BYTES, GLOB_B_ADDR, GLOB_B_BYTES = 0x96EA98, 0x5B8, 0x977B70, 0x30
G_EXT_LINKS, G_CONNECTS, G_DONT_WANDER = 0x96EAB8 - GLOB_A_ADDR, 0x96EAC0 - GLOB_A_ADDR, 0x96EC40 - GLOB_A_ADDR
G_BUILDING = 0x96F031 - GLOB_A_ADDR
OFF_PATH_NODES, OFF_NODE_LINKS, OFF_LINK_LENGTHS, OFF_INTERSECTIONS = 0x804, 0xA44, 0xB64, 0xC84
OFF_NUM_NODES, OFF_NUM_VEH, OFF_NUM_PED, OFF_NUM_CAR, OFF_NUM_ADDR, OFF_INTERIOR_IDS = 0xFA4, 0x10C4, 0x11E4, 0x1304, 0x1424, 0x3548
KEEP = 24


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
    return mix(((c & M32) * 0x9E3779B1 + (k & M32) * 0x85EBCA6B + 12345) & M32)


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
    return rnd(seed, k) & 0x7FFF


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
    return r24(a * b)


def fdiv(a, b):
    if b == 0:
        if a != a or a == 0:
            return float('nan')
        neg = (math.copysign(1.0, a) < 0) != (math.copysign(1.0, b) < 0)
        return -INF if neg else INF
    return r24(a / b)


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


def i32(u):
    u &= M32
    return u - (1 << 32) if u >= (1 << 31) else u


# ------------------------------------------------------------------------------------------------ mundo falso
class World:
    def __init__(self, inp):
        self.areas = inp['areas']
        self.nveh = inp['nveh']
        self.nped = inp['nped']
        self.nodes = [list(inp.get('nodes%d' % s, [])) for s in range(len(self.areas))]
        self.links = [list(inp.get('links%d' % s, [])) for s in range(len(self.areas))]
        self.slot = {a: s for s, a in enumerate(self.areas)}

    def loaded(self, a):
        return a in self.slot

    def total(self, a):
        s = self.slot[a]
        return self.nveh[s] + self.nped[s]

    def node(self, a, i):
        w = self.nodes[self.slot[a]]
        return w[i * 7:i * 7 + 7]

    def link(self, a, idx):
        return self.links[self.slot[a]][idx]


def node_xyz(nw):
    return i16(nw[2]) * 0.125, i16(nw[2] >> 16) * 0.125, i16(nw[3]) * 0.125


def b18(nw):
    return nw[6] & 0xFF


def b19(nw):
    return (nw[6] >> 8) & 0xFF


def b1a(nw):
    return (nw[6] >> 16) & 0xFF


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


def small_ref(seed):
    return fill_words(seed, SMALL_ARENA)


def world_arenas(w, n):
    nodes, links = [], []
    for s in range(len(w.areas)):
        nd = fill_words(rnd(n, 9100 + s), NODES_ARENA)
        for i, v in enumerate(w.nodes[s]):
            nd[NODES_OFF + 4 * i:NODES_OFF + 4 * i + 4] = (v & M32).to_bytes(4, 'little')
        nodes.append(nd)
        lk = fill_words(rnd(n, 9200 + s), LINKS_ARENA)
        for i, v in enumerate(w.links[s]):
            lk[LINKS_OFF + 4 * i:LINKS_OFF + 4 * i + 4] = (v & M32).to_bytes(4, 'little')
        links.append(lk)
    return nodes, links


def world_changes(w, n, nodes_after=None):
    nodes, links = world_arenas(w, n)
    after = nodes_after if nodes_after is not None else nodes
    return {'this': [], 'nodes': [runs(after[s], nodes[s], NODES_OFF) for s in range(len(w.areas))], 'links': [[] for _ in w.areas]}


def edge_dist(x, y, rx, ry):
    min_d = sub(x, sub(mul(float(rx), 750.0), 3000.0))
    east = sub(sub(mul(float(rx + 1), 750.0), 3000.0), x)
    if not (min_d < east):
        min_d = east
    south = sub(y, sub(mul(float(ry), 750.0), 3000.0))
    if not (min_d < south):
        min_d = south
    north = sub(sub(mul(float(ry + 1), 750.0), 3000.0), y)
    if not (min_d < north):
        min_d = north
    return min_d


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


def rdword(b, off=0):
    return int.from_bytes(bytes(b[off:off + 4]), 'little')


# ================================================================================================ 0: FindNodeClosestToCoors
def exp_closest(inp, n):
    x, y = f32(inp['pos'][0]), f32(inp['pos'][1])
    nt = inp['node_type'] & 0xFF
    state = {'res': 0xFFFF, 'dist': inp['max_dist']}    # la mitad de nodo de un resultado invalido no se modela (residuo de pila)
    res_area, res_node, valid = 0xFFFF, 0, False
    ev = Ev()
    st = {'valid': False, 'area': 0xFFFF, 'node': 0xFFFF, 'dist': inp['max_dist']}
    sc = list(zip(inp['s_area'], inp['s_node'], inp['s_dist']))
    stats = {}

    def call(area, u6):
        vals = [area, inp['pos'][0], inp['pos'][1], inp['pos'][2], nt, st['dist'], inp['unk2'], inp['unk4'], inp['boats'], u6, 1, 0, 1]
        ev.push(1, vals, ['region'] + vals)
        for a, nd, d in sc:
            if a != (area & 0xFFFF):
                continue
            cand, cur = f32(d), f32(st['dist'])
            if cand < cur:
                st['valid'], st['area'], st['node'], st['dist'] = True, area & 0xFFFF, nd, d
            break

    rx, ry = region(x), region(y)
    min_d = edge_dist(x, y, rx, ry)
    call(rx + ry * 8, inp['unk6'])
    ring_taken = False
    if f32(st['dist']) > min_d:
        xmin_base, xmax, ymin = rx, rx + 1, ry - 1
        for ring in range(1, 5):
            ring_taken = True
            xmin = xmin_base - 1
            ymax = ry + ring
            if 0 <= xmin < 8:
                for j in range(ymin, ymax + 1):
                    if 0 <= j < 8:
                        call(xmin + j * 8, inp['unk6'])
            if 0 <= xmax < 8:
                for j in range(ymin, ymax + 1):
                    if 0 <= j < 8:
                        call(xmax + j * 8, inp['unk6'])
            if 0 <= ymin < 8:
                for i in range(xmin_base, xmax):
                    if 0 <= i < 8:
                        call(i + ymin * 8, inp['unk6'])
            if 0 <= ymax < 8:
                for i in range(xmin_base, xmax):
                    if 0 <= i < 8:
                        call(i + ymax * 8, inp['unk6'])
            min_d = add(min_d, 750.0)
            if f32(st['dist']) < min_d:
                break
            xmin_base -= 1
            xmax += 1
            ymin -= 1
    interior = (inp['unk6'] & 0xFF) == 0
    if interior:
        for a in range(0x40, 0x48):
            call(a, 0)
    obs = {'events': ev.items, 'nevents': ev.n, 'ev_hash': ev.h,
           'area': st['area'], 'node': st['node'] if st['valid'] else 0xFFFF, 'ret_other': [],
           'world': {'this': [], 'nodes': [], 'links': []}, 'ret_ptr_ok': True}
    stats['rings'] = ring_taken
    stats['interior'] = interior
    stats['found'] = st['valid']
    stats['calls_many'] = ev.n >= 12
    stats['edge_dist'] = f32(inp['max_dist']) == min_d if True else False
    stats['region_clamped'] = rx in (0, 7) or ry in (0, 7)
    stats['huge_pos'] = (abs(x) > 1e12 and abs(x) != INF) or (abs(y) > 1e12 and abs(y) != INF)
    stats['nan_inf_pos'] = x != x or y != y or abs(x) == INF or abs(y) == INF
    stats['nt_other'] = nt > 1
    stats['maxdist_nan'] = f32(inp['max_dist']) != f32(inp['max_dist'])
    stats['maxdist_default'] = inp['max_dist'] == 0x497423FE
    stats['skip_rings'] = not ring_taken
    stats['u6_garbage_hi'] = (inp['unk6'] >> 8) != 0
    return obs, stats


# ================================================================================================ 1: FindNthNodeClosestToCoors
def exp_nth(inp, n):
    w = World(inp)
    nodes, links = world_arenas(w, n)
    nodes_after = [bytearray(b) for b in nodes]
    nt = inp['node_type'] & 0xFF
    nth = i32(inp['nth'])
    ev = Ev()
    stats = {}
    # 1) limpieza de la marca
    for a in range(72):
        if not w.loaded(a):
            continue
        s = w.slot[a]
        total, veh = w.nveh[s] + w.nped[s], w.nveh[s]
        if nt == 0:
            first, last = 0, veh
        elif nt == 1:
            first, last = veh, total
        else:
            continue
        for i in range(first, last):
            nodes_after[s][NODES_OFF + i * 28 + 0x19] &= 0xFD
    idx = [0]

    def find(u6):
        k = idx[0]
        vals = [inp['pos'][0], inp['pos'][1], inp['pos'][2], nt, inp['max_dist'], inp['low_traffic'], inp['unkn'], 1, inp['boats'], u6, 1, k]
        ev.push(2, vals, ['find'] + vals)
        idx[0] += 1
        if k < len(inp['r_area']):
            return inp['r_area'][k] | (inp['r_node'][k] << 16)
        return 0xFFFF | (0xBEEF << 16)

    ret = None
    out_val = None
    marked = 0
    if nth > 0:
        while True:
            res = find(inp['ignore'])
            if inp['has_out']:
                out_val = res
            if (res & 0xFFFF) == 0xFFFF:
                ret = res
                break
            s = w.slot[res & 0xFFFF]
            nodes_after[s][NODES_OFF + (res >> 16) * 28 + 0x19] |= 2
            marked += 1
            nth -= 1
            if nth <= 0:
                break
    if ret is None:
        ret = find(0)
    outref = small_ref(rnd(n, 201))
    outmem = bytearray(outref)
    if out_val is not None:
        outmem[SMALL_OBJ:SMALL_OBJ + 4] = out_val.to_bytes(4, 'little')
    obs = {'events': ev.items, 'nevents': ev.n, 'ev_hash': ev.h, 'ret': ret, 'ret_other': [], 'out': runs(outmem, outref, SMALL_OBJ),
           'world': world_changes(w, n, nodes_after), 'ret_ptr_ok': True}
    stats['marked'] = marked > 0
    stats['early_invalid'] = (ret & 0xFFFF) == 0xFFFF
    stats['nth_big'] = i32(inp['nth']) > 3
    stats['nth_nonpos'] = i32(inp['nth']) <= 0
    stats['type_veh'] = nt == 0
    stats['type_ped'] = nt == 1
    stats['type_other'] = nt > 1
    stats['out_set'] = out_val is not None
    stats['nth_neg'] = i32(inp['nth']) < 0
    stats['nth_huge'] = i32(inp['nth']) > 100000
    stats['calls_many'] = ev.n >= 4
    return obs, stats


# ================================================================================================ 2: FindNextNodeWandering
SIN_COS = {}


def sin_cos(dirb):
    if dirb not in SIN_COS:
        ang = r24(float(dirb) * f32(0x3F490FDB))
        SIN_COS[dirb] = (r32(r24(math.sin(ang))), r32(r24(math.cos(ang))))
    return SIN_COS[dirb]


def exp_wander(inp, n):
    w = World(inp)
    x, y, z = f32(inp['pos'][0]), f32(inp['pos'][1]), f32(inp['pos'][2])
    nt = inp['node_type'] & 0xFF
    ev = Ev()
    stats = {}
    kind = inp['origin_kind']
    mem = {'origin': inp['origin'], 'target': inp['target_init'], 'out_dir': inp['out_dir_init']}
    idx = [0]

    def find():
        k = idx[0]
        vals = [inp['pos'][0], inp['pos'][1], inp['pos'][2], nt, 0x497423FE, 0, 0, 0, 0, 0, 1, k]
        ev.push(2, vals, ['find'] + vals)
        idx[0] += 1
        return inp['result'] if k == 0 else (0xFFFF | (0xBEEF << 16))

    ocopy = None            # copia del dword de origen (None: sin origen)
    cur = None

    def finish():
        obs = {'events': ev.items, 'nevents': ev.n, 'ev_hash': ev.h,
               'origin': mem['origin'], 'origin_other': [], 'target': mem['target'], 'target_other': [], 'out_dir': mem['out_dir'], 'out_dir_other': [],
               'world': world_changes(w, n), 'ret_ptr_ok': None}
        return obs

    search = False
    if kind == 0:
        search = True
    else:
        ocopy = inp['origin']
        if (ocopy & 0xFFFF) == 0xFFFF:
            search = True
        else:
            oa = ocopy & 0xFFFF
            if not w.loaded(oa):
                mem['target'] = (mem['target'] & 0xFFFF0000) | 0xFFFF
                mem['out_dir'] = 0
                stats['origin_unloaded'] = True
                return finish(), stats
            nw = w.node(oa, ocopy >> 16)
            px, py, pz = node_xyz(nw)
            dx, dy, dz = sub(x, px), sub(y, py), sub(z, pz)
            dist2 = r32(add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx)))
            radius = mul(float((nw[5] >> 16) & 0xFF), 0.125)
            if 7.0 > radius:
                radius = 7.0
            if fsqrt(dist2) > radius:
                search = True
            else:
                cur = ocopy
                stats['stay'] = True
    if search:
        cur = find()
        stats['search'] = True
    ca = cur & 0xFFFF
    if ca == 0xFFFF or not w.loaded(ca):
        mem['target'] = (mem['target'] & 0xFFFF0000) | 0xFFFF
        cur = (cur & 0xFFFF0000) | 0xFFFF
        mem['out_dir'] = 0
        mem['target'] = cur
        stats['no_node'] = True
    else:
        nw = w.node(ca, cur >> 16)
        mem['target'] = (mem['target'] & 0xFFFF0000) | 0xFFFF
        sn, cs = sin_cos(inp['dir'] & 0xFF)
        best = f32(0xC97423F0)
        nlinks = b18(nw) & 0xF
        picked = False
        for i in range(nlinks):
            base = i16(nw[4])
            area_f = (nw[4] >> 16) & 0xFFFF
            linked = w.link(area_f, base + i)
            la = linked & 0xFFFF
            if not w.loaded(la):
                continue
            ow = w.node(la, linked >> 16)
            if not (b18(nw) & 0x20) and (b18(ow) & 0x20):
                continue
            if not (b19(nw) & 4) and (b19(ow) & 4):
                continue
            nx, ny, _ = node_xyz(nw)
            ox, oy, _ = node_xyz(ow)
            dx, dy = sub(ox, nx), sub(oy, ny)
            sq = fsqrt(add(mul(dy, dy), mul(dx, dx)))
            inv = fdiv(1.0, sq)
            ndx, ndy = r32(mul(inv, dx)), r32(mul(inv, dy))
            dot = add(mul(ndy, cs), mul(ndx, sn))
            if dot < best or dot != dot:
                continue
            best = r32(dot)
            mem['target'] = linked
            picked = True
            if ndx >= 0.0 if ndx == ndx else False:
                if mul(2.0, abs(ndy)) < ndx:
                    d = 2
                elif mul(2.0, ndx) < ndy:
                    d = 0
                elif ndx * -2.0 > ndy:
                    d = 4
                else:
                    d = 1 if ndy > 0.0 else 3
            else:
                if mul(2.0, abs(ndy)) < -ndx:
                    d = 6
                elif ndx * -2.0 < ndy:
                    d = 0
                elif mul(2.0, ndx) > ndy:
                    d = 4
                else:
                    d = 7 if ndy > 0.0 else 5
            mem['out_dir'] = d
            stats['od%d' % d] = True
        if kind != 0:
            mem['origin'] = cur
        else:
            raise RuntimeError('fixture no soportada: origen nulo con nodo valido')
        if (mem['target'] & 0xFFFF) == 0xFFFF:
            mem['out_dir'] = 0
            mem['target'] = cur
        stats['picked'] = picked
        stats['dir_q%d' % ((inp['dir'] & 0xFF) // 64)] = True
        stats['nlinks0'] = nlinks == 0
    # el destino coincide con el origen original: primer vecino con area cargada
    if ocopy is not None and (mem['target'] & 0xFFFF) == (ocopy & 0xFFFF) and (mem['target'] >> 16) == (ocopy >> 16) and (ocopy & 0xFFFF) != 0xFFFF and w.loaded(ocopy & 0xFFFF):
        nw = w.node(ocopy & 0xFFFF, ocopy >> 16)
        for i in range(b18(nw) & 0xF):
            linked = w.link((nw[4] >> 16) & 0xFFFF, i16(nw[4]) + i)
            if w.loaded(linked & 0xFFFF):
                mem['target'] = linked
                stats['same_node_swap'] = True
                break
    return finish(), stats


# ================================================================================================ 3: CompleteNewInterior
def exp_interior(inp, n):
    w = World(inp)
    stats = {}
    nn, slot, num_ext = inp['num_nodes'], inp['slot'], i32(inp['num_ext'])
    area = 64 + slot
    ev = Ev()
    outref = small_ref(rnd(n, 200))
    outmem = bytearray(outref)
    outmem[SMALL_OBJ:SMALL_OBJ + 4] = inp['out_init'].to_bytes(4, 'little')
    outref = bytearray(outmem)
    if inp['out_kind']:
        outmem[SMALL_OBJ:SMALL_OBJ + 2] = b'\xff\xff'
    mallocref = fill_words(inp['fill'], MALLOC_ARENA)
    mm = bytearray(mallocref)
    top = [0]
    fields = {k: None for k in ('path_nodes', 'node_links', 'link_lengths', 'intersections', 'num_nodes', 'num_veh', 'num_ped', 'num_car_links', 'num_addresses', 'interior_id')}
    conn = bytearray(inp['connects'])
    gA = bytearray(GLOB_A_BYTES)            # cambios respecto a la imagen fijada: se acumulan como (desplazamiento, byte)
    changes = {}

    def malloc(size):
        aligned = (top[0] + 15) & ~15
        ev.push(4, [size, 0, 0, aligned], ['malloc', size, 0, 0, aligned])
        top[0] = aligned + size
        return aligned

    if nn != 0:
        flood = (slot + 100) & 0xFF
        if num_ext > 0:
            ext = inp['ext_linked'][0]
            flood = (w.node(ext & 0xFFFF, ext >> 16)[5] >> 24) & 0xFF
        fields['interior_id'] = inp['interior_id']
        o_nodes = malloc(nn * 0x1C)
        fields['path_nodes'] = o_nodes
        xs = list(inp['x']) + [0] * (64 - len(inp['x']))
        ys = list(inp['y']) + [0] * (64 - len(inp['y']))
        zs = list(inp['z']) + [0] * (64 - len(inp['z']))
        dw = list(inp['dont_wander']) + [0] * (64 - len(inp['dont_wander']))
        for i in range(nn):
            b = MALLOC_OFF + o_nodes + i * 0x1C

            def put16(off, v):
                mm[b + off:b + off + 2] = (v & 0xFFFF).to_bytes(2, 'little')

            # x87: fld; fmul 8.0; _ftol -> ax
            put16(8, ftol_low32(mul(f32(xs[i]), 8.0)))
            put16(0xA, ftol_low32(mul(f32(ys[i]), 8.0)))
            put16(0xC, ftol_low32(mul(f32(zs[i]), 8.0)))
            put16(0x14, i)
            put16(0x12, slot + 0x40)
            mm[b + 0x16] = 0
            mm[b + 0x17] = flood
            v18 = mm[b + 0x18]
            v18 = ((((v18 & 0xEF) | 0x20) & 0xBF) & 0x7F)
            mm[b + 0x18] = v18
            v19 = mm[b + 0x19]
            v19 = ((v19 | 1) & 0xFD)
            v19 = (v19 & ~4) | (((dw[i] << 2) & 0xFF) & 4)
            v19 = ((v19 | 8) & 0xCF)
            mm[b + 0x19] = v19
            mm[b + 0x1A] = 0x0F
            put16(0xE, 0x7FFE)
        # simetria
        for i in range(nn):
            for j in range(6):
                c = conn[i * 6 + j]
                if c >= 0x80:
                    continue
                found = any(conn[c * 6 + k] == (i & 0xFF) for k in range(6))
                if found:
                    continue
                for k in range(6):
                    if conn[c * 6 + k] >= 0x80:
                        conn[c * 6 + k] = i & 0xFF
                        break
        nl = sum(1 for i in range(nn) for j in range(6) if conn[i * 6 + j] < 0x80)
        cap = nl + 0xC0
        o_links = malloc(cap * 4)
        o_len = malloc(cap)
        o_int = malloc(cap)
        fields['node_links'], fields['link_lengths'], fields['intersections'] = o_links, o_len, o_int
        counter = 0
        for i in range(nn):
            b = MALLOC_OFF + o_nodes + i * 0x1C
            mm[b + 0x10:b + 0x12] = (counter & 0xFFFF).to_bytes(2, 'little')
            mm[b + 0x18] &= 0xF0
            for j in range(6):
                c = conn[i * 6 + j]
                if c < 0x80:
                    ev.push(3, [slot + 0x40, i, slot + 0x40, c, slot, counter, 1], ['link', slot + 0x40, i, slot + 0x40, c, slot, counter, 1])
                    counter += 1
        for k in range(nl, cap):
            o = MALLOC_OFF + o_links + k * 4
            mm[o:o + 2] = b'\xff\xff'
        fields['num_nodes'], fields['num_veh'], fields['num_ped'], fields['num_car_links'], fields['num_addresses'] = nn, 0, nn, 0, nl
        for i in range(max(num_ext, 0)):
            interior = (slot + 0x40) | ((inp['int_linked'][i] & 0xFFFF) << 16)
            exterior = inp['ext_linked'][i]
            ev.push(5, [interior, exterior, 1], ['adddyn', interior, exterior, 1])
            ev.push(5, [exterior, interior, 1], ['adddyn', exterior, interior, 1])
        stats['built'] = True
        stats['n_ge32'] = nn >= 32
        stats['n_64'] = nn == 64
        stats['n_1'] = nn == 1
        stats['neg_ext'] = num_ext < 0
        stats['ext_many'] = num_ext >= 4
        stats['building'] = bool(inp['building'])
        stats['no_out'] = not inp['out_kind']
        stats['x_special'] = any((v & 0x7F800000) == 0x7F800000 for v in inp['x'])
        stats['links'] = nl > 0
        stats['ext'] = num_ext > 0
        stats['flood_ext'] = num_ext > 0
        stats['symmetry'] = conn != bytearray(inp['connects'])
    else:
        stats['empty'] = True
    glob_a = []
    # globales A: la imagen fijada tiene las conexiones originales; cambian las simetricas y el byte de "construyendo"
    for i in range(384):
        if conn[i] != inp['connects'][i]:
            glob_a.append((G_CONNECTS + i, conn[i]))
    if inp['building']:
        glob_a.append((G_BUILDING, 0))
    glob_a.sort()
    ga_runs = []
    for off, v in glob_a:
        if ga_runs and ga_runs[-1][0] + len(ga_runs[-1][1]) // 2 == off:
            ga_runs[-1][1] += '%02x' % v
        else:
            ga_runs.append([off, '%02x' % v])
    obs = {'events': ev.items, 'nevents': ev.n, 'ev_hash': ev.h, 'malloc_top': top[0],
           'out': runs(outmem, outref, SMALL_OBJ),
           'this_fields': fields,
           'this_other': [], 'malloc': runs(mm, mallocref, MALLOC_OFF), 'glob_a': ga_runs, 'glob_b': [],
           'nodes': [[] for _ in w.areas], 'ret_ptr_ok': None}
    return obs, stats


# ================================================================================================ 4: FindParkingNodeInArea
def exp_parking(inp, n):
    w = World(inp)
    box = [f32(b) for b in inp['box']]
    min_x, max_x, min_y, max_y, min_z, max_z = box
    num_found = 0
    first = None
    selected = None
    cursor = i32(inp['cursor'])
    stats = {}
    for a in range(64):
        if not w.loaded(a):
            continue
        s = w.slot[a]
        for i in range(w.nveh[s]):
            nw = w.nodes[s][i * 7:i * 7 + 7]
            x, y, z = node_xyz(nw)
            if not (x > min_x and x < max_x):
                continue
            if not (y > min_y and y < max_y):
                continue
            if not (z > min_z and z < max_z):
                continue
            if (b1a(nw) & 0xF0) != 0x20:
                continue
            if num_found == 0:
                first = (x, y, z)
            if num_found == cursor:
                selected = (x, y, z)
            num_found += 1
    cursor = i32(cursor + 1)
    if not (cursor < num_found):
        cursor = 0
    if first is None:
        ret = [0, 0, 0]
    elif selected is not None:
        ret = [bits(v) for v in selected]
    else:
        ret = [bits(v) for v in first]
    obs = {'ret': ret, 'ret_other': [], 'cursor': cursor & M32, 'world': world_changes(w, n), 'glob_b_other': [], 'ret_ptr_ok': True}
    stats['found'] = first is not None
    stats['selected'] = selected is not None
    stats['many'] = num_found >= 3
    stats['wrap'] = cursor == 0
    stats['none'] = first is None
    stats['cursor_hit_not_first'] = selected is not None and selected != first
    stats['neg_cursor'] = i32(inp['cursor']) < 0
    stats['nan_box'] = any(b != b for b in box)
    return obs, stats


# ================================================================================================ 5: GeneratePedCreationCoors_Interior
def exp_generate(inp, n):
    w = World(inp)
    x, y = f32(inp['x']), f32(inp['y'])
    ev = Ev()
    stats = {}
    rx, ry = region(x), region(y)
    area_id = rx + ry * 8
    oc = [v for v in inp['out_init']]
    ori = inp['out_ori_init']
    ridx = [0]
    gidx = [0]

    def rand():
        k = ridx[0]
        ev.push(6, [k], ['rand', k])
        ridx[0] += 1
        return script_rand(inp['rand_mode'], inp['rand_seed'], k)

    result = False
    ground_used = 0
    reached_link = False
    for attempt in range(200):
        if not w.loaded(area_id):
            continue
        s = w.slot[area_id]
        nped = w.nped[s]
        if nped == 0:
            continue
        r = rand() >> 6
        nveh = w.nveh[s]
        idx = nveh + (r % nped)
        nw = w.nodes[s][idx * 7:idx * 7 + 7]
        nx, ny, nz = node_xyz(nw)
        dy = sub(ny, y)
        dx = sub(nx, x)
        if not (add(mul(dy, dy), mul(dx, dx)) < 2500.0):
            continue
        if not (nz > 900.0):
            continue
        nlinks = b18(nw) & 0xF
        if nlinks == 0:
            continue
        for li in range(nlinks):
            linked = w.link(area_id, i16(nw[4]) + li)
            la = linked & 0xFFFF
            if la >= 0x40 or not w.loaded(la):
                continue
            reached_link = True
            ow = w.node(la, linked >> 16)
            ox, oy, oz = node_xyz(ow)
            for tries in range(5):
                t = (rand() & 0xFF) * 0.00390625
                ori = bits(t)
                a1, a2, a3 = r32(mul(ox, t)), mul(oy, t), mul(oz, t)
                omt = r32(sub(1.0, t))
                b1, b2, b3 = r32(mul(nx, omt)), mul(ny, omt), r32(mul(nz, omt))
                outx = r32(add(b1, a1))
                outy = r32(add(b2, a2))
                outz = r32(add(b3, r32(a3)))
                oc = [bits(outx), bits(outy), bits(outz)]
                zarg = r32(add(outz, 2.0))
                k = gidx[0]
                vals = [bits(outx), bits(outy), bits(zarg), 1, 0, k]
                ev.push(7, vals, ['ground'] + vals)
                gidx[0] += 1
                found = inp['ground_found'][k] if k < len(inp['ground_found']) else 0
                delta = f32(inp['ground_delta'][k]) if k < len(inp['ground_delta']) else 0.0
                gz = r32(zarg + delta) if True else 0
                ground_used += 1
                if found and abs(sub(gz, outz)) < 3.0:
                    oc[2] = bits(gz)
                    result = True
                    break
            if result:
                break
        if result:
            break
    ocref = small_ref(rnd(n, 200))
    ocm = bytearray(ocref)
    for i in range(3):
        ocref[SMALL_OBJ + 4 * i:SMALL_OBJ + 4 * i + 4] = inp['out_init'][i].to_bytes(4, 'little')
    ocm = bytearray(ocref)
    oriref = bytearray(small_ref(rnd(n, 203)))
    oriref[SMALL_OBJ:SMALL_OBJ + 4] = inp['out_ori_init'].to_bytes(4, 'little')
    obs = {'events': ev.items, 'nevents': ev.n, 'ev_hash': ev.h, 'rand_calls': ridx[0], 'ground_calls': gidx[0], 'ret_al': 1 if result else 0,
           'coords': oc, 'coords_other': [], 'ori': ori, 'ori_other': [], 'unused': [[], []], 'world': world_changes(w, n), 'ret_ptr_ok': None}
    stats['success'] = result
    stats['fail'] = not result
    stats['ground_called'] = gidx[0] > 0
    stats['rand_called'] = ridx[0] > 0
    stats['link_reached'] = reached_link
    stats['no_area'] = not w.loaded(area_id)
    stats['many_attempts'] = ridx[0] > 100
    stats['ground_nan'] = any(f32(d) != f32(d) for d in inp['ground_delta'])
    stats['multi_try'] = gidx[0] > 1
    stats['rand_mode%d' % inp['rand_mode']] = True
    return obs, stats


FUNCS = [exp_closest, exp_nth, exp_wander, exp_interior, exp_parking, exp_generate]


def expected(name, inp, case):
    return FUNCS[NAMES.index(name)](inp, case)


def selftest():
    """Autoprueba: valores calculados a mano desde el desensamblado (no desde las filas del banco)."""
    assert region(-3000.0) == 0 and region(0.0) == 4 and region(2999.0) == 7 and region(float('nan')) == 0 and region(INF) == 0
    assert region(1e13) == 7 and ftol_low32(13333332992.0) == 448431104        # _ftol: 32 bits bajos del entero de 64 bits
    assert ftol_low32(2.0 ** 63) == 0 and ftol_low32(-2.0 ** 63) == 0 and ftol_low32(-1.5) == -1 and ftol_low32(1.9999) == 1
    assert edge_dist(0.0, 0.0, 4, 4) == 0.0 and edge_dist(375.0, 1.0, 4, 4) == 1.0 and edge_dist(-2999.0, 100.0, 0, 4) == 1.0
    assert r24(1.0 + 2.0 ** -25) == 1.0 and 0.0 < r24(1e-40) and abs(r24(1e-40) / 1e-40 - 1.0) < 1e-7 and r32(1e-40) != 0.0 and r32(1e39) == INF
    assert sin_cos(0) == (0.0, 1.0) and abs(sin_cos(2)[0] - 1.0) == 0.0 and sin_cos(4)[0] != 0.0           # sin(float(pi)) no es 0
    assert hash_step(0, 0) == mix(0xA5A5A5A5) and script_rand(1, 0, 3) == 0x7FFF and script_rand(2, 0, 3) == 192
    # FindNodeClosestToCoors con dist = 0: ni anillos ni resultado; tipo con byte bajo 0 -> busca las 8 areas interiores (9 llamadas)
    inp = {'pos': [0, 0, 0], 'node_type': 0, 'max_dist': 0, 'unk2': 0, 'unk3': 0, 'unk4': 0, 'boats': 0, 'unk6': 0, 's_area': [], 's_node': [], 's_dist': []}
    obs, st = exp_closest(inp, 0)
    assert obs['nevents'] == 9 and obs['area'] == 0xFFFF and obs['events'][0][1] == 4 + 4 * 8 and obs['events'][8][1] == 0x47
    inp.update({'unk6': 1, 'max_dist': 0x497423FE, 's_area': [36], 's_node': [7], 's_dist': [0x3F800000]})
    obs, st = exp_closest(inp, 0)       # region (4, 4) = 36 devuelve el nodo a distancia 1.0 (minD = 0 < dist: anillos hasta dist < minD + 750 tras el primero)
    assert obs['area'] == 36 and obs['node'] == 7 and obs['nevents'] == 1 + 3 + 3 + 1 + 1, obs['nevents']
    # FindParkingNodeInArea: dos nodos de aparcamiento dentro de la caja, cursor 1 -> el segundo; el cursor pasa a 0 (1 + 1 < 2 es falso)
    nodes = []
    for x in (8, 16):
        nodes += [0, 0, x | (8 << 16), 8, 0, 0, 0x20 << 16]
    w = {'areas': [3], 'nveh': [2], 'nped': [0], 'nodes0': nodes, 'links0': [], 'box': [bits(0.0), bits(10.0), bits(0.0), bits(10.0), bits(-1.0), bits(2.0)], 'cursor': 1}
    obs, st = exp_parking(w, 0)
    assert obs['ret'] == [bits(2.0), bits(1.0), bits(1.0)] and obs['cursor'] == 0, obs['ret']
    w['cursor'] = 0xFFFFFFFF
    obs, st = exp_parking(w, 0)
    assert obs['ret'] == [bits(1.0), bits(1.0), bits(1.0)] and obs['cursor'] == 0 and st['found'] and not st['selected']
    print('ok')


if __name__ == '__main__':
    selftest()
