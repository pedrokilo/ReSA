"""Oraculo independiente del banco WorldScan64 (lote 64): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de CWorldScan::ScanWorld (0x72CAE0) y
CWorldScan::SetExtraRectangleToScan (0x72D5E0) para una fila `input` (la especificacion semantica del caso que registra Core.hpp).

Aritmetica: el x87 del EXE trabaja a 24 bits de mantisa (palabra de control PC=24): cada +, -, *, / se redondea a float, igual que el modelo `f32` de aqui (operar en double y
redondear a float32 es exacto para estas operaciones). Las comparaciones reproducen fcom/fnstsw/test ah,mascara/jp de cada rama (con NaN: C0=C2=C3=1).

Mapa de la pila de ScanWorld (esp tras el prologo), tal y como lo usa el EXE:
  0x0C maxX (int, inicial -9999)   0x10 minX (int, inicial 9999; durante las devoluciones de llamada se pisa con contador-1 y se reescribe antes de leerse)
  0x14 pendiente izquierda (float) 0x18 maxY y luego pendiente derecha (float)   0x1C indice del vertice izquierdo actual y luego xLeft (float)
  0x20 numero de vertices del casco 0x24 xRight (float)   0x28 fila actual y   0x2C indice siguiente izquierdo   0x30 suma de angulos (float) / indice siguiente derecho
  0x34 angulo minimo (float) / ultima fila   0x38..0x3C banderas "visitado"   0x40+8i, 0x44+8i: x, y del vertice i del casco
Fases: 0x72CAE0 quita duplicados; 0x72CBA3 punto de menor y; 0x72CC64 envoltura de regalo con GetATanOfXY (0x53CC70); 0x72CD5C filas minima/maxima; 0x72CEC0 cadena izquierda;
0x72CF47 cadena derecha; 0x72CFC1 pendientes; 0x72D0F0 bucle de filas (devolucion de llamada scanFunction(x, y), y quitar de las celdas extra la celda visitada); 0x72D593 vaciado de
las celdas extra restantes.

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos EN ORDEN, nevents, cambios de los puntos y de la region de
celdas extra 0xC81598..0xC815BC respecto de la imagen del caso); `stats` son banderas de cobertura. `python -I fixture_oracle.py` ejecuta la autoprueba.
"""
import math
import struct

NAME_SCAN = 'CWorldScan::ScanWorld'
NAME_EXTRA = 'CWorldScan::SetExtraRectangleToScan'
NAMES = [NAME_SCAN, NAME_EXTRA]
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
MAX_EVENTS = 700
REGION_WORDS = 9
Y0, X0, CNT = 0, 4, 8
TWO_PI_BITS = 0x40C90FDB
ANGLE_INIT_BITS = 0x47C34FF3


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def bits(f):
    return struct.unpack('<I', struct.pack('<f', f))[0]


def flt(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fadd(a, b): return f32(a + b)
def fsub(a, b): return f32(a - b)
def fmul(a, b): return f32(a * b)


def fdiv(a, b):
    if b == 0.0:
        if a == 0.0 or math.isnan(a) or math.isnan(b):
            return flt(0xFFC00000)       # indefinido real del x87
        neg = (math.copysign(1.0, a) < 0) != (math.copysign(1.0, b) < 0)
        return -math.inf if neg else math.inf
    return f32(a / b)


def fcom(a, b):
    """(C0, C2, C3) de fcom: a < b, desordenado, a == b (desordenado pone los tres)"""
    if math.isnan(a) or math.isnan(b):
        return (1, 1, 1)
    return (int(a < b), 0, int(a == b))


def ah(st):
    return st[0] | (st[1] << 2) | (st[2] << 6)


def jp(value, mask):
    return bin(value & mask).count('1') % 2 == 0


def ftol_floor(v):
    """call 0x8219F0 (floor) + call 0x821B40 (_ftol2): int64 truncado, se usa el dword bajo; NaN/inf/fuera de rango -> 0x8000000000000000"""
    if math.isnan(v) or math.isinf(v):
        return 0
    n = math.floor(v)
    if n >= (1 << 63) or n < -(1 << 63):
        return 0
    return s32(n)


def ceil_f(v):
    """call 0x823820 (ceil) sobre un float finito: valor double exacto"""
    return float(math.ceil(v))


class Mem:
    """imagen de la region 0xC81598..0xC8159C+...: Y[0..3] palabras 0..3, X[0..3] palabras 4..7, contador palabra 8"""
    def __init__(self, pre):
        self.w = list(pre)

    def Y(self, i): return s32(self.w[Y0 + i])
    def X(self, i): return s32(self.w[X0 + i])
    @property
    def count(self): return s32(self.w[CNT])

    @count.setter
    def count(self, v): self.w[CNT] = v & M32


def scan_world(inp):
    n = inp['n']
    P = [[inp['p'][2 * i], inp['p'][2 * i + 1]] for i in range(8)]     # bits
    mem = Mem(inp['pre'])
    ev = []
    stats = {}

    def px(i): return flt(P[i][0])
    def py(i): return flt(P[i][1])

    flags = [0] * 8
    ebx = n
    # ---- 0x72CAE0: quita los puntos repetidos (el arreglo del llamador se desplaza)
    eax = ebx - 1
    if eax > 0:
        ecx = 1
        while True:
            if ecx < ebx:
                edi = ebx - 1
                j = ecx
                while True:
                    a = P[ecx - 1]
                    eq = fcom(flt(a[0]), flt(P[j][0]))
                    if not jp(ah(eq), 0x44):          # x iguales
                        eq2 = fcom(flt(a[1]), flt(P[j][1]))
                        if not jp(ah(eq2), 0x44):     # y iguales
                            if j < edi:
                                for k in range(edi - j):
                                    P[j + k] = list(P[j + k + 1])
                            ebx -= 1
                            edi -= 1
                            j -= 1
                            stats['dedup'] = True
                    j += 1
                    if not j < ebx:
                        break
            ecx += 1
            if not (ecx - 1) < ebx - 1:
                break
    n = ebx
    # ---- 0x72CBA3: punto de menor y
    st = flt(P[0][1])
    edi = 0
    for esi in range(1, n):
        m = flt(P[esi][1])
        if not ((ah(fcom(st, m)) & 0x41) != 0):       # ST0 > m (ordenado)
            st = m
            edi = esi
    flags[edi] = 1
    hx = [0.0] * 5
    hy = [0.0] * 5
    hx[0] = flt(P[edi][0])
    hy[0] = flt(P[edi][1])
    cnt = 1
    ebx = flags[0] | (flags[1] << 8) | (flags[2] << 16) | (flags[3] << 24)      # ebx = dword de banderas (indice basura si ningun angulo mejora)
    s30 = 0.0
    # ---- 0x72CC64: envoltura de regalo
    while True:
        s34 = flt(ANGLE_INIT_BITS)
        for esi in range(n):
            if esi == edi:
                continue
            dx = fsub(px(esi), px(edi))
            dy = fsub(py(esi), py(edi))
            ret = f32(math.atan2(dy, dx))
            ev.append([1, bits(dx), bits(dy), bits(ret)])
            a = fsub(ret, s30)
            if not jp(ah(fcom(a, 0.0)), 5):                # a < 0
                while True:
                    a = fadd(a, flt(TWO_PI_BITS))
                    if jp(ah(fcom(a, 0.0)), 5):
                        break
            if (ah(fcom(a, flt(TWO_PI_BITS))) & 1) == 0:   # a >= 2pi
                while True:
                    a = fsub(a, flt(TWO_PI_BITS))
                    if (ah(fcom(a, flt(TWO_PI_BITS))) & 1) != 0:
                        break
            if not jp(ah(fcom(a, s34)), 5):                # a < minimo (ordenado)
                s34 = a
                ebx = esi
        assert 0 <= ebx < 8, 'indice basura: el EXE leeria fuera de rango'
        if flags[ebx]:
            break
        s30 = fadd(s34, s30)
        hx[cnt] = flt(P[ebx][0])
        hy[cnt] = flt(P[ebx][1])
        cnt += 1
        flags[ebx] = 1
        edi = ebx
    stats['hull%d' % cnt] = True
    if cnt < 3:
        stats['hull_lt3'] = True
        return finish(inp, P, mem, ev, stats)
    # ---- 0x72CD5C: fila minima y maxima
    s18 = hy[0]
    st = hy[0]
    edi = 0
    for ecx in range(1, cnt):
        m = hy[ecx]
        if (ah(fcom(st, m)) & 0x41) == 0:
            st = m
            edi = ecx
        else:
            if (ah(fcom(m, s18)) & 0x41) == 0:
                s18 = m
    y = ftol_floor(st)           # [28h]
    s34i = ftol_floor(s18)       # [34h]
    s10 = 9999
    s0c = -9999
    # ---- 0x72CEC0: cadena izquierda
    esi = edi
    steps = 0
    while True:
        steps += 1
        left_cur = esi
        esi -= 1
        if esi < 0:
            esi = cnt - 1
        if (ah(fcom(float(s10), hx[left_cur])) & 0x41) == 0:
            s10 = ftol_floor(hx[left_cur])
        a_ = ftol_floor(hy[left_cur])
        b_ = ftol_floor(hy[esi])
        if not (a_ == b_ and steps < cnt):
            break
    s2c = esi
    # ---- 0x72CF47: cadena derecha
    edi2 = edi
    steps = 0
    while True:
        steps += 1
        right_cur = edi2
        edi2 += 1
        if edi2 == cnt:
            edi2 = 0
        if not jp(ah(fcom(float(s0c), hx[right_cur])), 5):
            s0c = ftol_floor(hx[right_cur])
        a_ = ftol_floor(hy[edi2])
        b_ = ftol_floor(hy[right_cur])
        if not (b_ == a_ and steps < cnt):
            break
    s30i = edi2
    # ---- 0x72CFC1: pendientes y primeras x
    s18 = fdiv(fsub(hx[s30i], hx[right_cur]), fsub(hy[s30i], hy[right_cur]))      # pendiente derecha
    s24 = fadd(fmul(fsub(ceil_f(hy[right_cur]), hy[right_cur]), s18), hx[right_cur])
    s14 = fdiv(fsub(hx[s2c], hx[left_cur]), fsub(hy[s2c], hy[left_cur]))          # pendiente izquierda
    s1c = fadd(fmul(fsub(ceil_f(hy[left_cur]), hy[left_cur]), s14), hx[left_cur])
    if y != s34i:
        if not jp(ah(fcom(s14, 0.0)), 5):                 # pendiente izquierda < 0
            v = ftol_floor(s1c)
            if s10 > v:
                s10 = v
        if (ah(fcom(s18, 0.0)) & 1) == 0:                 # pendiente derecha >= 0
            v = ftol_floor(s24)
            if s0c < v:
                s0c = v
    cells = 0
    if y <= s34i:
        while True:
            # ---- 0x72D0F0: fila
            x = s10
            if not x > s0c:
                while True:
                    ev.append([2, x, y])
                    cells += 1
                    count = mem.count
                    ecx = 0
                    if count > 0:
                        last = count - 1
                        while True:
                            if x == mem.X(ecx) and y == mem.Y(ecx):
                                if ecx < last:
                                    nw = last - ecx
                                    for k in range(nw):
                                        mem.w[Y0 + ecx + k] = mem.w[Y0 + ecx + k + 1]
                                    for k in range(nw):
                                        mem.w[X0 + ecx + k] = mem.w[X0 + ecx + k + 1]
                                    count = mem.count
                                count -= 1
                                last -= 1
                                mem.count = count
                                ecx = 4
                                stats['extra_removed'] = True
                            ecx += 1
                            if not ecx < count:
                                break
                    x += 1
                    if not x <= s0c:
                        break
            # ---- 0x72D1B8: siguiente fila
            s24 = fadd(s18, s24)
            y += 1
            s1c = fadd(s14, s1c)
            ebxl = s2c
            row = ftol_floor(hy[ebxl])
            st_l = fcom(s14, 0.0)                        # estado que el EXE conserva en AX
            if y != row:
                # 0x72D380
                if jp(ah(st_l), 5):
                    v = fsub(s1c, s14)
                else:
                    v = s1c
                s10 = ftol_floor(v)
            elif y == s34i:
                # 0x72D329
                if jp(ah(st_l), 5):
                    s10 = ftol_floor(fsub(s1c, s14))
                else:
                    while True:
                        esi_ = ftol_floor(hx[ebxl])
                        ebxl -= 1
                        s10 = esi_
                        if ebxl < 0:
                            ebxl = cnt - 1
                        eax_ = ftol_floor(hx[ebxl])
                        if not esi_ > eax_:
                            break
                    s2c = ebxl
            else:
                # 0x72D214
                if jp(ah(st_l), 5):
                    v = fsub(s1c, s14)
                else:
                    v = hx[ebxl]
                s10 = ftol_floor(v)
                while True:
                    edi_ = ebxl
                    ebxl -= 1
                    s2c = ebxl
                    if ebxl < 0:
                        s2c = cnt - 1
                    s1c = hx[edi_]
                    v = ftol_floor(s1c)
                    if s10 > v:
                        s10 = v
                    ebxl = s2c
                    if not y == ftol_floor(hy[ebxl]):
                        break
                s14 = fdiv(fsub(hx[ebxl], hx[edi_]), fsub(hy[ebxl], hy[edi_]))
                s1c = fadd(fmul(fsub(ceil_f(hy[edi_]), hy[edi_]), s14), s1c)
                if not jp(ah(fcom(s14, 0.0)), 5):
                    v = ftol_floor(s1c)
                    if s10 > v:
                        s10 = v
            # ---- 0x72D3AC: lado derecho
            edi_ = s30i
            row = ftol_floor(hy[edi_])
            st_r = fcom(s18, 0.0)
            if y != row:
                # 0x72D559
                if (ah(st_r) & 1) != 0:
                    v = fsub(s24, s18)
                else:
                    v = s24
                s0c = ftol_floor(v)
            elif y == s34i:
                # 0x72D50A
                if (ah(st_r) & 1) != 0:
                    s0c = ftol_floor(fsub(s24, s18))
                else:
                    while True:
                        esi_ = ftol_floor(hx[edi_])
                        edi_ += 1
                        s0c = esi_
                        if edi_ == cnt:
                            edi_ = 0
                        eax_ = ftol_floor(hx[edi_])
                        if not esi_ < eax_:
                            break
                    s30i = edi_
            else:
                # 0x72D3EB
                if (ah(st_r) & 1) != 0:
                    v = fsub(s24, s18)
                else:
                    v = hx[edi_]
                s0c = ftol_floor(v)
                esi_ = s30i
                while True:
                    edi_ = esi_
                    esi_ += 1
                    s30i = esi_
                    if esi_ == cnt:
                        s30i = 0
                    s24 = hx[edi_]
                    v = ftol_floor(s24)
                    if not s0c >= v:
                        s0c = v
                    esi_ = s30i
                    if not y == ftol_floor(hy[esi_]):
                        break
                s18 = fdiv(fsub(hx[esi_], hx[edi_]), fsub(hy[esi_], hy[edi_]))
                s24 = fadd(fmul(fsub(ceil_f(hy[edi_]), hy[edi_]), s18), s24)
                if (ah(fcom(s18, 0.0)) & 1) == 0:
                    v = ftol_floor(s24)
                    if not s0c >= v:
                        s0c = v
            if not y <= s34i:
                break
    stats['cells'] = cells
    # ---- 0x72D593: celdas extra restantes
    i = 0
    while i < mem.count:
        ev.append([2, mem.X(i), mem.Y(i)])
        i += 1
    if mem.count > 0:
        stats['extra_flushed'] = True
    mem.count = 0
    return finish(inp, P, mem, ev, stats)


def finish(inp, P, mem, ev, stats):
    pts = []
    for i in range(8):
        for k in range(2):
            if P[i][k] != inp['p'][2 * i + k]:
                pts.append([2 * i + k, P[i][k]])
    region = [[i, mem.w[i]] for i in range(REGION_WORDS) if mem.w[i] != inp['pre'][i]]
    observed = {'events': ev[:MAX_EVENTS], 'nevents': len(ev), 'points': pts, 'region': region}
    return observed, stats


def set_extra(inp):
    mem = Mem(inp['pre'])
    minx, maxx, miny, maxy = (flt(v) for v in inp['a'])
    stats = {}
    edi = ftol_floor(minx)
    if edi < ftol_ceil(maxx):
        while True:
            esi = ftol_floor(miny)
            if esi < ftol_ceil(maxy):
                while True:
                    c = mem.count
                    mem.w[Y0 + c] = esi & M32
                    mem.w[X0 + c] = edi & M32
                    mem.count = c + 1
                    esi += 1
                    if not esi < ftol_ceil(maxy):
                        break
            edi += 1
            if not edi < ftol_ceil(maxx):
                break
    stats['added'] = mem.count - s32(inp['pre'][CNT])
    return finish_extra(inp, mem, stats)


def ftol_ceil(v):
    if math.isnan(v) or math.isinf(v):
        return 0
    n = math.ceil(v)
    if n >= (1 << 63) or n < -(1 << 63):
        return 0
    return s32(n)


def finish_extra(inp, mem, stats):
    region = [[i, mem.w[i]] for i in range(REGION_WORDS) if mem.w[i] != inp['pre'][i]]
    return {'events': [], 'nevents': 0, 'points': [], 'region': region}, stats


def expected(name, inp, case):
    if name == NAME_SCAN:
        observed, stats = scan_world(inp)
        out = {}
        if stats.get('cells', 0):
            out['cells_some'] = True
        for k, v in stats.items():
            if k.startswith('hull') or k in ('dedup', 'extra_removed', 'extra_flushed'):
                out[k if k != 'hull_lt3' else 'hull_lt3'] = bool(v)
        out['cells_gt50'] = stats.get('cells', 0) > 50
        out['pre_extras'] = s32(inp['pre'][CNT]) > 0
        out['row_single'] = stats.get('cells', 0) > 0
        return observed, out
    if name == NAME_EXTRA:
        observed, stats = set_extra(inp)
        out = {'added_0': stats['added'] == 0, 'added_1': stats['added'] == 1, 'added_2_4': stats['added'] >= 2, 'pre_count': s32(inp['pre'][CNT]) > 0}
        return observed, out
    raise KeyError(name)


def selftest():
    # SetExtraRectangleToScan: rectangulo 2x1 de celdas con contador previo 1
    pre = [7, 7, 7, 7, 8, 8, 8, 8, 1]
    obs, _ = expected(NAME_EXTRA, {'pre': pre, 'a': [bits(1.0), bits(3.0), bits(5.0), bits(6.0)]}, 0)
    assert obs['region'] == [[1, 5], [2, 5], [5, 1], [6, 2], [8, 3]], obs
    # minX >= maxX: nada
    obs, _ = expected(NAME_EXTRA, {'pre': pre, 'a': [bits(3.0), bits(3.0), bits(5.0), bits(6.0)]}, 0)
    assert obs['region'] == []
    # un cuadrado alineado 2x2 cuyas esquinas son de enteros: celdas (x, y) por filas
    sq = [bits(2.0), bits(2.0), bits(4.0), bits(2.0), bits(4.0), bits(4.0), bits(2.0), bits(4.0)] + [0] * 8
    obs, st = expected(NAME_SCAN, {'n': 4, 'p': sq, 'pre': [0] * 9}, 0)
    cells = [tuple(e[1:]) for e in obs['events'] if e[0] == 2]
    assert cells and cells == sorted(cells, key=lambda c: (c[1], c[0])), cells
    print('celdas del cuadrado 2..4:', cells)
    # duplicado consecutivo: se quita y el arreglo se desplaza
    dup = [bits(2.0), bits(2.0), bits(2.0), bits(2.0), bits(5.0), bits(2.0), bits(5.0), bits(5.0), bits(2.0), bits(5.0)] + [0] * 6
    obs, st = expected(NAME_SCAN, {'n': 5, 'p': dup, 'pre': [0] * 9}, 0)
    assert obs['points'] and obs['points'][0] == [2, bits(5.0)], obs['points']
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
