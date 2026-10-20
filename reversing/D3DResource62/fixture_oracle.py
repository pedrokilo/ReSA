"""Oraculo independiente del banco D3DResource62 (lote 62): modela, DESDE EL DESENSAMBLADO del EXE (0x730510, 0x7306A0, 0x730830, 0x730900, 0x730A00, 0x730B70, 0x730D30), la salida
observable de cada funcion para una fila `input` (la especificacion semantica del caso que registra Core.hpp), con las mismas fichas simbolicas de punteros que usa el banco:

  R<id> / B<id>   textura / index buffer falso;  S<k>  pila k (0..16 texturas, 17..32 index buffers, 33 generico);  N<j> bloque j del new falso;  O<w>.<b> palabra de out;  D  dispositivo
  T<i> / I<i> / IG  buffer de texturas i, de index buffers i, generico (con +<campo> si apunta dentro);  numero = valor crudo de 32 bits.

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos de los falsos EN ORDEN, cambios del bloque de globales y de
la arena, byte de uso y, en las funciones que devuelven HRESULT, EAX); `stats` son banderas de cobertura. Lee SOLO la especificacion: nada de las constantes de arena del C++ salvo la
numeracion de pilas (documentada arriba) y la disposicion de campos de los buffers (del desensamblado).
`python -I fixture_oracle.py` ejecuta la autoprueba (casos construidos a mano contra el desensamblado).
"""
import struct

NAME_CANCEL = 'D3DResourceSystem::CancelBuffering'
NAME_CREATE_IB = 'D3DResourceSystem::CreateIndexBuffer'
NAME_CREATE_TEX = 'D3DResourceSystem::CreateTexture'
NAME_DESTROY_IB = 'D3DResourceSystem::DestroyIndexBuffer'
NAME_DESTROY_TEX = 'D3DResourceSystem::DestroyTexture'
NAME_INIT = 'D3DResourceSystem::Init'
NAME_SHUTDOWN = 'D3DResourceSystem::Shutdown'
NAMES = [NAME_CANCEL, NAME_CREATE_IB, NAME_CREATE_TEX, NAME_DESTROY_IB, NAME_DESTROY_TEX, NAME_INIT, NAME_SHUTDOWN]
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF


def u32(x):
    return x & M32


def s32(x):
    x &= M32
    return x - (1 << 32) if x & 0x80000000 else x


def f32(x):
    """redondeo a float de 32 bits (fstp dword) de un valor exacto"""
    return struct.unpack('<f', struct.pack('<f', float(x)))[0]


def tdiv(a, b):
    """division con signo truncada hacia cero (idiv / imul 0x51EB851F)"""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


# --- disposicion del bloque 0xC87C60 (palabras) y de las pilas (del desensamblado)
TEX_BASE, TEX_STRIDE, IB_BASE, IB_STRIDE, GEN_BASE, FREE_TEX, FREE_IB = 2, 7, 122, 6, 218, 224, 225
ARG = 127
STALE_TEX = 0xC1000000
STALE_IB = 0xC1000001
STALE_GEN = 0xC10000FF


def tex_w(i, f):   # campos: fmt 0, width 1, levels 2, capacity 3, num 4, size 5, ptr 6
    return TEX_BASE + TEX_STRIDE * i + f


def ib_w(i, f):    # campos: fmt 0, field_4 1, capacity 2, num 3, size 4, ptr 5 (i == 16: generico)
    return (GEN_BASE if i == 16 else IB_BASE + IB_STRIDE * i) + f


def tex_tok(i):
    return 'T%d' % i


def ib_tok(i):
    return 'IG' if i == 16 else 'I%d' % i


class Model:
    def __init__(self, inp):
        self.inp = inp
        self.G = {}       # palabra -> valor (entero o ficha); ausente = basura (desconocida)
        self.S = {}       # (pila, posicion) -> ficha
        self.O = {}       # palabra de out -> valor
        self.N = {}       # (bloque, palabra) -> ficha
        self.ev = []
        self.use = inp['use']
        self.rel = 0
        self.flags = {}
        fn = inp['fn']
        self.G[0] = inp['ntex']
        if fn == 5 and inp['prior']:
            p = inp['prior']
            self.G[0] = 1
            self.use = 1
            t = tex_w(0, 0)
            self.G[t], self.G[t + 1], self.G[t + 2], self.G[t + 3] = 0, 0, M32, 0x10
            self.G[t + 4] = 3 if p == 2 else 0
            self.G[t + 5] = 3 if p == 2 else 0
            self.G[t + 6] = STALE_TEX
            for k in range(16):
                b = ib_w(k, 0)
                self.G[b], self.G[b + 1], self.G[b + 2] = 0x65, 100 * (k + 1), 0x10
                self.G[b + 3] = k % 4 if p == 2 else 0
                self.G[b + 4] = k % 3 if p == 2 else 0
                self.G[b + 5] = STALE_IB + k
            g = ib_w(16, 0)
            self.G[g], self.G[g + 1], self.G[g + 2] = 0x65, 0, 0x10
            self.G[g + 3] = 2 if p == 2 else 0
            self.G[g + 4] = 1 if p == 2 else 0
            self.G[g + 5] = STALE_GEN
        if inp['free'][0]:
            self.G[FREE_TEX], self.G[FREE_IB] = inp['free'][1], inp['free'][2]
        for b in inp['tex']:
            idx, a, bb, c, cap, num, size, hasptr, items = b
            w = tex_w(idx, 0)
            self.G.update({w: a, w + 1: bb, w + 2: c, w + 3: cap, w + 4: num, w + 5: size, w + 6: ('S%d' % idx) if hasptr else 0})
            for j, it in enumerate(items):
                self.S[(idx, j)] = 'R%d' % it
        for b in inp['ib']:
            idx, a, bb, c, cap, num, size, hasptr, items = b
            w = ib_w(idx, 0)
            self.G.update({w: a, w + 1: bb, w + 2: cap, w + 3: num, w + 4: size, w + 5: ('S%d' % (17 + idx)) if hasptr else 0})
            for j, it in enumerate(items):
                self.S[(17 + idx, j)] = 'B%d' % it
        if inp['out']:
            self.O[1] = inp['out']
        self.G0 = dict(self.G)
        self.S0 = dict(self.S)
        self.O0 = dict(self.O)
        self.use0 = self.use

    # ---- memoria
    def r(self, w):
        if w not in self.G:
            raise KeyError('palabra %d del bloque sin especificar' % w)
        return self.G[w]

    def stack_of(self, ptr):
        if not (isinstance(ptr, str) and ptr.startswith('S') and ptr[1:].isdigit()):
            raise KeyError('puntero de pila invalido %r' % (ptr,))
        return int(ptr[1:])

    def sget(self, ptr, idx):
        k = self.stack_of(ptr)
        idx = s32(idx)
        if (k, idx) not in self.S:
            raise KeyError('posicion %d de la pila %d sin especificar' % (idx, k))
        return self.S[(k, idx)]

    def sput(self, ptr, idx, tok):
        self.S[(self.stack_of(ptr), s32(idx))] = tok

    # ---- eventos de los falsos
    def release(self, tok):
        self.ev.append([1, tok])
        k = self.rel
        self.rel += 1
        f = self.inp['fk']
        reK, kind, idx, val = f[5], f[6], f[7], f[8]
        if k == reK and kind:
            self.flags['reentry_fired'] = True
            self.flags['reentry_kind%d' % kind] = True
            if kind == 1:
                self.G[0] = val
            elif kind == 2:
                self.G[tex_w(idx, 5)] = val
            elif kind == 3:
                self.G[ib_w(idx, 4)] = val
            elif kind == 4:
                self.G[ib_w(16, 4)] = val

    def resize_effect(self, capword, new_cap):
        mode = self.inp['fk'][4]
        if mode == 1:
            self.G[capword] = u32(new_cap)
        elif mode == 2:
            self.G[capword] = 0
        elif mode == 3:
            self.G[capword] = u32(new_cap - 1)

    # ---- resultado
    def observed(self, extra=None):
        ch = []
        for w in range(226):
            if w in self.G and self.G[w] != self.G0.get(w, object()):
                ch.append(['G', w, self.G[w]])
        for key in sorted(self.S):
            if self.S[key] != self.S0.get(key, object()):
                ch.append(['S', key[0], key[1], self.S[key]])
        for key in sorted(self.N):
            ch.append(['N', key[0], key[1], self.N[key]])
        for w in sorted(self.O):
            if self.O[w] != self.O0.get(w, object()):
                ch.append(['O', w, self.O[w]])
        obs = {'events': self.ev, 'nevents': len(self.ev), 'changes': ch, 'use': self.use & 0xFF, 'device_unchanged': True}
        if extra:
            obs.update(extra)
        return obs


# ======================================================================================== CancelBuffering (0x730900) y Shutdown (0x730A00)
def cancel(inp, shutdown):
    m = Model(inp)
    fl = m.flags
    count = s32(m.r(0))
    if count > 0:
        i = 0
        while True:
            w = tex_w(i, 0)
            size = s32(m.r(w + 5))
            j = 0
            if size > 0:
                while True:
                    ptr = m.r(w + 6)
                    m.release(m.sget(ptr, j))
                    size = s32(m.r(w + 5))
                    j += 1
                    if not (j < size):
                        break
            m.G[w + 5] = 0
            m.G[w + 4] = 0
            if not shutdown:
                ptr = m.r(w + 6)
                if ptr != 0:
                    m.ev.append([15, ptr])
                    m.G[w + 6] = 0
            i += 1
            if not (i < s32(m.r(0))):
                break
    m.G[0] = 0
    for k in range(16):
        w = ib_w(k, 0)
        size = s32(m.r(w + 4))
        j = 0
        if size > 0:
            while True:
                m.release(m.sget(m.r(w + 5), j))
                size = s32(m.r(w + 4))
                j += 1
                if not (j < size):
                    break
        m.G[w + 4] = 0
        m.G[w + 3] = 0
        if not shutdown:
            ptr = m.r(w + 5)
            if ptr != 0:
                m.ev.append([15, ptr])
                m.G[w + 5] = 0
    w = ib_w(16, 0)
    size = s32(m.r(w + 4))
    j = 0
    if size > 0:
        while True:
            m.release(m.sget(m.r(w + 5), j))
            size = s32(m.r(w + 4))
            j += 1
            if not (j < size):
                break
    ptr = m.r(w + 5)
    m.G[w + 4] = 0
    m.G[w + 3] = 0
    if not shutdown:
        if ptr != 0:
            m.ev.append([15, ptr])
            m.G[w + 5] = 0
        m.use = 0
    # estadisticas
    nt = s32(inp['ntex'])
    fl['ntex_zero'] = nt == 0
    fl['ntex_one'] = nt == 1
    fl['ntex_many'] = 2 <= nt <= 16
    fl['ntex_17'] = nt == 17
    fl['ntex_neg'] = nt < 0
    rel = [e for e in m.ev if e[0] == 1]
    fl['releases_ge_10'] = len(rel) >= 10
    fl['releases_zero'] = len(rel) == 0
    fl['releases_ge_30'] = len(rel) >= 30
    fl['frees_ge_1'] = any(e[0] == 15 for e in m.ev)
    fl['frees_ge_10'] = sum(1 for e in m.ev if e[0] == 15) >= 10
    fl['size_neg_seen'] = any(s32(b[6]) < 0 for b in inp['tex'] + inp['ib'])
    fl['ib_released'] = any(e[0] == 1 and e[1].startswith('B') for e in m.ev)
    fl['tex_released'] = any(e[0] == 1 and e[1].startswith('R') for e in m.ev)
    fl['gen_released'] = s32(next(b for b in inp['ib'] if b[0] == 16)[6]) > 0
    fl['null_ptr_zero_size'] = any(not b[7] and s32(b[6]) <= 0 for b in inp['tex'][:max(1, min(nt if nt > 0 else 1, 17))] + inp['ib'])
    fl['use_zero'] = (inp['use'] & 0xFF) == 0
    fl['use_other'] = (inp['use'] & 0xFF) not in (0, 1)
    return m.observed(), fl


# ======================================================================================== CreateIndexBuffer (0x7306A0)
def create_ib(inp):
    m = Model(inp)
    fl = m.flags
    num, _fmt = inp['args'][0], inp['args'][1]
    q = tdiv(s32(u32(num - 1)), 100)
    indices = u32((q + 1) * 100)
    hit = None
    if q < 16:
        if q < 0:
            raise KeyError('grupo negativo no modelado')
        w = ib_w(q, 0)
        size = m.r(w + 4)
        if size != 0:
            m.G[w + 3] = u32(m.r(w + 3) - 1)
            top = u32(size - 1)
            m.G[w + 4] = top
            hit = m.sget(m.r(w + 5), top)
        else:
            hit = 0
        m.O[1] = hit
        fl['cache_group'] = True
    else:
        f = inp['fk']
        m.ev.append([12, 'IG', indices])
        hit = ('B%d' % (f[0] - 1)) if f[0] else 0
        m.O[1] = hit
        fl['generic_group'] = True
    eax = None
    if hit:
        if q < 16:   # 0x730714 (`jne 0x730700`): con un buffer del generico NO hay GetDesc
            m.ev.append([4, hit, 1])
        eax = 0
        fl['hit'] = True
    else:
        f = inp['fk']
        m.ev.append([6, 'D', u32(indices * 2), 8, 0x65, 1, 'O1.0', 0])
        if f[2] == 1:
            m.O[1] = 'B%d' % f[3]
        elif f[2] == 2:
            m.O[1] = 0
        eax = f[1]
        fl['device'] = True
        fl['dev_write%d' % f[2]] = True
        fl['dev_ok'] = s32(eax) >= 0
        fl['dev_fail'] = s32(eax) < 0
    fl['num_zero'] = num == 0
    fl['num_huge'] = num >= 0x7FFFFF00
    fl['group_ge16'] = q >= 16
    fl['group0'] = q == 0
    fl['group15'] = q == 15
    fl['boundary_100'] = u32(num - 1) % 100 in (0, 99)
    fl['pop_null'] = q >= 16 and not inp['fk'][0]
    fl['pop_hit'] = q >= 16 and bool(inp['fk'][0])
    fl['cache_empty'] = q < 16 and not hit
    return m.observed({'eax': u32(eax)}), fl


# ======================================================================================== CreateTexture (0x730510)
def create_tex(inp):
    m = Model(inp)
    fl = m.flags
    w, h, lv, fmt = inp['args'][0], inp['argx'][0], inp['argx'][1], inp['argx'][2]
    f = inp['fk']
    if s32(lv) > 1:
        lv = 0
    match = None
    near = set()
    if w == h:
        cnt = s32(m.r(0))
        if cnt > 1:
            wf = f32(s32(w))
            i = 1
            while i < cnt:
                b = tex_w(i, 0)
                bw = float(s32(m.r(b + 1)))
                wok = bw == wf
                lok = lv == m.r(b + 2)
                fok = fmt == m.r(b)
                if wok and lok and fok:
                    match = i
                    break
                if wok and lok:
                    near.add('fmt')
                elif wok and fok:
                    near.add('levels')
                elif lok and fok:
                    near.add('width')
                i += 1
    tex = None
    if match is not None:
        b = tex_w(match, 0)
        size = m.r(b + 5)
        if size != 0:
            m.G[b + 4] = u32(m.r(b + 4) - 1)
            top = u32(size - 1)
            m.G[b + 5] = top
            tex = m.sget(m.r(b + 6), top)
        else:
            tex = 0
        fl['match_empty'] = size == 0
        fl['match_pop'] = size != 0
    else:
        m.ev.append([7, 'T0', fmt, w, h, lv])
        tex = ('R%d' % (f[0] - 1)) if f[0] else 0
        fl['generic_pop'] = True
        fl['generic_pop_null'] = not f[0]
        fl['generic_pop_hit'] = bool(f[0])
    m.O[1] = tex
    if tex:
        eax = 0
    else:
        m.ev.append([5, 'D', w, h, lv, 0, fmt, 1, 'O1.0', 0])
        if f[2] == 1:
            m.O[1] = 'R%d' % f[3]
        elif f[2] == 2:
            m.O[1] = 0
        eax = f[1]
        fl['device'] = True
        fl['dev_write%d' % f[2]] = True
        fl['dev_ok'] = s32(eax) >= 0
        fl['dev_fail'] = s32(eax) < 0
    fl['square'] = w == h
    fl['nonsquare'] = w != h
    fl['match'] = match is not None
    fl['nomatch_square_multi'] = w == h and s32(inp['ntex']) > 1 and match is None
    fl['first_of_many'] = match is not None and any(b[0] > match and b[1] == m.G0[tex_w(match, 1)] for b in inp['tex'])
    fl['levels_gt1'] = s32(inp['argx'][1]) > 1
    fl['levels_neg'] = s32(inp['argx'][1]) < 0
    fl['levels_0_1'] = inp['argx'][1] in (0, 1)
    fl['w_ge_2p24'] = 0x1000000 <= w <= 0x7FFFFFFF
    fl['w_neg'] = w >= 0x80000000
    fl['near_width'] = 'width' in near
    fl['near_levels'] = 'levels' in near
    fl['near_fmt'] = 'fmt' in near
    fl['ntex_neg'] = s32(inp['ntex']) < 0
    fl['ntex_le1'] = 0 <= s32(inp['ntex']) <= 1
    fl['ntex_big'] = s32(inp['ntex']) >= 17
    fl['width_rounding'] = w == h and 0x1000000 < w < 0x7FFFFFFF and f32(s32(w)) != float(w)
    return m.observed({'eax': u32(eax)}), fl


# ======================================================================================== DestroyIndexBuffer (0x730D30)
def destroy_ib(inp):
    m = Model(inp)
    fl = m.flags
    f = inp['fk']
    tok = 'B%d' % ARG
    if (m.use & 0xFF) == 0:
        m.ev.append([1, tok])
        fl['use0'] = True
        return m.observed(), fl
    m.ev.append([4, tok, 1])
    nbytes = inp['res'][4]
    g = (((nbytes >> 1) - 1) & M32) // 100
    fl['odd_bytes'] = bool(nbytes & 1)
    fl['bytes_zero'] = nbytes == 0
    fl['group_ge16'] = s32(g) >= 16
    fl['group_lt16'] = s32(g) < 16
    idx = 16 if s32(g) >= 16 else g
    w = ib_w(idx, 0)
    t = ib_tok(idx)
    m.G[w + 3] = u32(m.r(w + 3) + 1)
    size = m.r(w + 4)
    cap = m.r(w + 2)
    stored = False
    if s32(size) < s32(cap):
        stored = True
    else:
        m.ev.append([13, t, u32(cap * 2)])
        m.resize_effect(w + 2, cap * 2)
        fl['resize_called'] = True
        size = m.r(w + 4)
        cap = m.r(w + 2)
        if s32(size) < s32(cap):
            stored = True
            fl['resize_grew'] = True
        else:
            fl['dropped'] = True
    if stored:
        m.sput(m.r(w + 5), size, tok)
        m.G[w + 4] = u32(size + 1)
        fl['stored'] = True
    fl['cap_neg'] = s32(m.G0[w + 2]) < 0
    fl['cap_huge'] = m.G0[w + 2] == 0x7FFFFFFF
    fl['cap_zero'] = m.G0[w + 2] == 0
    fl['size_ge_cap'] = s32(m.G0[w + 4]) >= s32(m.G0[w + 2])
    fl['gen_stored'] = idx == 16 and stored
    fl['cache_stored'] = idx != 16 and stored
    return m.observed(), fl


# ======================================================================================== DestroyTexture (0x730B70)
def destroy_tex(inp):
    m = Model(inp)
    fl = m.flags
    tok = 'R%d' % ARG
    if (m.use & 0xFF) == 0:
        m.ev.append([1, tok])
        fl['use0'] = True
        return m.observed(), fl
    lc, fmt, w, h = inp['res'][0], inp['res'][1], inp['res'][2], inp['res'][3]
    m.ev.append([2, tok])
    single = 0 if lc > 1 else 1
    m.ev.append([3, tok, 0, 1])
    cnt = s32(m.r(0))
    fl['lc_gt1'] = lc > 1
    fl['square'] = w == h
    fl['w_ge_2p31'] = w >= 0x80000000
    fl['w_big_exact'] = w == h and 0x1000000 < w < 0x7FFFFFFF and f32(w) != float(w)
    fl['ntex_neg'] = cnt < 0
    fl['ntex_ge17'] = cnt >= 17
    release = False
    done = False
    if w == h and cnt > 1:
        wf = f32(w)
        i = 1
        near = False
        while i < cnt:
            b = tex_w(i, 0)
            bw = float(s32(m.r(b + 1)))
            if bw == wf and fmt == m.r(b) and single == m.r(b + 2):
                m.G[b + 4] = u32(m.r(b + 4) + 1)
                size, cap = m.r(b + 5), m.r(b + 3)
                t = tex_tok(i)
                if s32(size) < s32(cap):
                    m.sput(m.r(b + 6), size, tok)
                    m.G[b + 5] = u32(m.r(b + 5) + 1)
                    fl['match_store'] = True
                else:
                    m.ev.append([11, t, u32(cap * 2)])
                    m.resize_effect(b + 3, cap * 2)
                    m.ev.append([10, t, tok])
                    size2, cap2 = m.r(b + 5), m.r(b + 3)
                    fl['match_resize'] = True
                    if s32(size2) < s32(cap2):
                        m.sput(m.r(b + 6), size2, tok)
                        m.G[b + 5] = u32(size2 + 1)
                        fl['match_resize_stored'] = True
                    else:
                        fl['match_resize_dropped'] = True
                done = True
                break
            if bw == wf:
                near = True
            i += 1
        fl['loop_near_miss'] = near and not done
        if done:
            return m.observed(), fl
    # 730C07
    if cnt < 17 and w == h and w >= 0x20:
        if w > 0x100:
            m.ev.append([1, tok])
            fl['release_big_square'] = True
            return m.observed(), fl
        t = tex_tok(cnt)
        m.ev.append([8, t, fmt, w, single, 0x10])
        b = tex_w(cnt, 0)
        m.G[b], m.G[b + 1], m.G[b + 2], m.G[b + 3], m.G[b + 4], m.G[b + 5] = fmt, w, single, 0x10, 0, 0
        m.G[b + 6] = 'N19'
        nxt = s32(m.r(0))
        t2 = tex_tok(nxt)
        m.ev.append([9, t2, tok])
        b2 = tex_w(nxt, 0)
        m.G[b2 + 4] = u32(m.r(b2 + 4) + 1)
        size, cap = m.r(b2 + 5), m.r(b2 + 3)
        if s32(size) < s32(cap):
            m.N[(19, s32(size))] = tok
            m.G[b2 + 5] = u32(size + 1)
        m.G[0] = u32(nxt + 1)
        fl['new_buffer'] = True
        return m.observed(), fl
    if w > 0x100 or h > 0x100:
        m.ev.append([1, tok])
        fl['release_big'] = True
        return m.observed(), fl
    # buffer generico (T0)
    m.G[tex_w(0, 4)] = u32(m.r(tex_w(0, 4)) + 1)
    size, cap = m.r(tex_w(0, 5)), m.r(tex_w(0, 3))
    stored = False
    if s32(size) < s32(cap):
        stored = True
    else:
        m.ev.append([11, 'T0', u32(cap * 2)])
        m.resize_effect(tex_w(0, 3), cap * 2)
        size, cap = m.r(tex_w(0, 5)), m.r(tex_w(0, 3))
        fl['gen_resize'] = True
        if s32(size) < s32(cap):
            stored = True
        else:
            fl['gen_dropped'] = True
    if stored:
        m.sput(m.r(tex_w(0, 6)), size, tok)
        m.G[tex_w(0, 5)] = u32(size + 1)
        fl['gen_store'] = True
    return m.observed(), fl


# ======================================================================================== Init (0x730830)
def init(inp):
    m = Model(inp)
    fl = m.flags
    nr = inp['newRet']

    def new(j):
        m.ev.append([14, 0x40])
        return ('N%d' % j) if nr[j] == 1 else nr[j]

    m.G[0] = 1
    m.use = 1
    t = tex_w(0, 0)
    m.G[t], m.G[t + 1], m.G[t + 2] = 0, 0, M32
    m.G[t + 6] = new(0)
    m.G[t + 3], m.G[t + 5], m.G[t + 4] = 0x10, 0, 0
    size = 100
    for k in range(16):
        b = ib_w(k, 0)
        m.G[b], m.G[b + 1] = 0x65, size
        m.G[b + 5] = new(1 + k)
        m.G[b + 2], m.G[b + 4], m.G[b + 3] = 0x10, 0, 0
        size += 100
    g = ib_w(16, 0)
    m.G[g], m.G[g + 1] = 0x65, 0
    m.G[g + 5] = new(17)
    m.G[g + 2], m.G[g + 4], m.G[g + 3] = 0x10, 0, 0
    fl['prior0'] = inp['prior'] == 0
    fl['prior1'] = inp['prior'] == 1
    fl['prior2'] = inp['prior'] == 2
    fl['new_all_pool'] = all(x == 1 for x in nr[:18])
    fl['new_raw_some'] = any(x != 1 for x in nr[:18])
    fl['new_null'] = any(x == 0 for x in nr[:18])
    fl['use_was_zero'] = inp['use'] == 0
    return m.observed(), fl


def expected(name, inp, case):
    if name == NAME_CANCEL:
        return cancel(inp, False)
    if name == NAME_SHUTDOWN:
        return cancel(inp, True)
    if name == NAME_CREATE_IB:
        return create_ib(inp)
    if name == NAME_CREATE_TEX:
        return create_tex(inp)
    if name == NAME_DESTROY_IB:
        return destroy_ib(inp)
    if name == NAME_DESTROY_TEX:
        return destroy_tex(inp)
    if name == NAME_INIT:
        return init(inp)
    raise KeyError(name)


# ======================================================================================== autoprueba
def _mk(fn, **kw):
    d = dict(fn=fn, fam=0, use=1, ntex=0, tex=[], ib=[], free=[0, 0, 0], args=[0, 0, 0], argx=[0, 0, 0], res=[0, 0, 0, 0, 0], fk=[0, 0, 0, 0, 0, 0xFFFFFFFF, 0, 0, 0],
             newRet=[], prior=0, out=0, regs=[0, 0, 0, 0])
    d.update(kw)
    return d


def selftest():
    # Init con basura previa: 18 new (0x40) y todos los campos
    o, _ = expected(NAME_INIT, _mk(5, newRet=[1] * 18), 0)
    assert o['events'] == [[14, 0x40]] * 18 and o['use'] == 1
    ch = {(c[0], c[1]): c[2:] for c in o['changes']}
    assert ch[('G', 0)] == [1] and ch[('G', tex_w(0, 2))] == [M32]
    assert ch[('G', tex_w(0, 6))] == ['N0'] and ch[('G', ib_w(3, 1))] == [400] and ch[('G', ib_w(16, 5))] == ['N17']
    # Cancel: dos buffers con recursos, reentrada en el Release 0 que pone NumTextureBuffers a 1
    tex = [[0, 0, 0, 0, 4, 1, 2, 1, [5, 6]], [1, 0, 0, 0, 4, 1, 1, 1, [7]]]
    ib = [[k, 0, 0, 0, 4, 0, 0, 0, []] for k in range(17)]
    inp = _mk(0, ntex=2, tex=tex, ib=ib)
    inp['fk'][5], inp['fk'][6], inp['fk'][7], inp['fk'][8] = 0, 1, 0, 1
    o, fl = expected(NAME_CANCEL, inp, 0)
    assert [e for e in o['events'] if e[0] == 1] == [[1, 'R5'], [1, 'R6']], o['events']
    assert [e for e in o['events'] if e[0] == 15] == [[15, 'S0']]
    assert o['use'] == 0 and ('G', tex_w(0, 6), 0) in [tuple(c) for c in o['changes']]
    # Shutdown: no libera pilas ni toca el byte de uso
    o, _ = expected(NAME_SHUTDOWN, _mk(6, ntex=2, tex=tex, ib=ib), 0)
    assert all(e[0] == 1 for e in o['events']) and len(o['events']) == 3 and o['use'] == 1
    # CreateTexture: coincidencia con ancho exacto 64 y pila con un elemento
    tex = [[1, 0x15, 64, 0, 4, 0, 1, 1, [9]]]
    o, _ = expected(NAME_CREATE_TEX, _mk(2, ntex=2, tex=tex, args=[64, 0, 0], argx=[64, 5, 0x15], out=0xC0000001), 0)
    assert o['eax'] == 0 and o['events'] == [] and ('O', 1, 'R9') in [tuple(c) for c in o['changes']], o
    # CreateTexture: ancho 16777217 redondea a 16777216; un buffer con 16777217 NO coincide (fild exacto)
    tex = [[1, 0x15, 16777217, 0, 4, 0, 1, 1, [9]]]
    o, fl = expected(NAME_CREATE_TEX, _mk(2, ntex=2, tex=tex, args=[16777217, 0, 0], argx=[16777217, 0, 0x15], out=0xC0000001), 0)
    assert o['events'][0][:2] == [7, 'T0'], o
    # DestroyTexture: ancho 0x80000000 (sin signo) no coincide con ningun buffer; 0x80000000 > 0x100 -> libera
    o, _ = expected(NAME_DESTROY_TEX, _mk(4, ntex=1, tex=[[0, 0, 0, 0, 4, 0, 0, 1, []]], res=[1, 0x15, 0x80000000, 0x80000000, 0]), 0)
    assert o['events'][-1] == [1, 'R127'], o
    print('autoprueba del oraculo: ok')


if __name__ == '__main__':
    selftest()
