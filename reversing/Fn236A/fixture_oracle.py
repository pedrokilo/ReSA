"""Oraculo independiente (Python) de Fn236A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (dumpbin /disasm sobre gta-sa.exe, 0x5CFD20..0x5D023F, con las tablas de salto
0x5D0048 y de indices 0x5D005C leidas del ejecutable), lo que debe observarse de las 4 funciones del lote 236A (jdapimin de la libjpeg 6b de descompresion embebida: jpeg_CreateDecompress,
jpeg_consume_input, jpeg_finish_decompress y jpeg_read_header, con default_decompress_parms 0x5CFE20 incluida en jpeg_consume_input) y lo compara con las filas de comparisons.jsonl (lado original y
candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y la escena del banco (Fixtures.hpp / LiveComparison.hpp) para conocer la
imagen inicial.

Escena (arena A, base S = arena A + 0x40 + P_OFF): jpeg_decompress_struct en S+0 (0x1B0 bytes), err S+0x1C0, src S+0x240, inputctl S+0x260, master S+0x280. Arena B (B0): tabla de componentes en B0+0
(0x54 por componente). Las arenas C y D no se tocan.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["LibJpegDecompressApi::jpeg_CreateDecompress", "LibJpegDecompressApi::jpeg_consume_input", "LibJpegDecompressApi::jpeg_finish_decompress", "LibJpegDecompressApi::jpeg_read_header"]
ENTRIES = (0x5CFD20, 0x5CFFB0, 0x5D0110, 0x5D01D0)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

_P = ("P_OFF P_KIND P_FILL P_ERRMODE P_BOFF F_GSTATE A_VER A_SIZE A_REQ F_CLIENT PRE_NC PRE_JFIF PRE_ADOBE PRE_XF PRE_ID0 PRE_ID1 PRE_ID2 "
      "POST_ON POST_NC POST_JFIF POST_ADOBE POST_XF POST_ID0 POST_ID1 POST_ID2 CR0 CR1 CR2 CR3 CR4 CR5 CR6 CR7 CRDEF EOI_AT EOI_INIT F_BUFIMG F_OUTSCAN F_OUTH COMP_BAD "
      "SEED0 SEED1 SEED2 SEED3").split()
IDX = {name: i for i, name in enumerate(_P)}
P_COUNT = len(_P)
globals().update(IDX)
K_CI, K_ERR, K_SRC, K_INP, K_MASTER = 0x000, 0x1C0, 0x240, 0x260, 0x280
CI_SIZE = 0x1B0
B_COMPS = 0x000


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

    def changes(self):
        """cambios en tramos contiguos: [[desplazamiento del primer byte, bytes nuevos en hexadecimal]]"""
        diff = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                diff.append(i)
        out = []
        k = 0
        while k < len(diff):
            j = k
            while j + 1 < len(diff) and diff[j + 1] == diff[j] + 1:
                j += 1
            out.append([diff[k] - GUARD, ''.join('%02x' % self.m[i] for i in diff[k:j + 1])])
            k = j + 1
        return out


class World:
    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[P_FILL], a[SEED0 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


class Escape(Exception):
    pass


class Sim:
    def __init__(self, c):
        self.a = [x & M32 for x in c['a']]
        a = self.a
        self.c = c
        self.fn = c['fn']
        W = self.W = World(c)
        self.events = []
        self.marks = set()
        self.S = W.raw(0, a[P_OFF])
        self.B0 = W.raw(1, a[P_BOFF])
        self.errmode = a[P_ERRMODE]
        self.calls = 0
        self.ci = self.S + K_CI
        self.build_scene()
        W.snap()
        self.img = bytearray(W.rd(self.ci + i, 1) for i in range(CI_SIZE))   # imagen de la estructura (para los resumenes FNV)

    # ---------------------------------------------------------------- escena
    def build_scene(self):
        a, W, S, B0 = self.a, self.W, self.S, self.B0
        wr = W.wr
        # los punteros a los falsos (error_exit, emit_message, consume_input, reset, init_source, term_source, finish_output_pass) son direcciones del ASI del banco: no cambian ni entran en los
        # resumenes FNV de la estructura, asi que no se modelan
        wr(S + K_INP + 0x11, 1, a[EOI_INIT])
        if not a[COMP_BAD]:
            wr(B0 + B_COMPS + 0x00, 4, a[PRE_ID0]); wr(B0 + B_COMPS + 0x54, 4, a[PRE_ID1]); wr(B0 + B_COMPS + 0xA8, 4, a[PRE_ID2])
        ci = self.S + K_CI
        wr(ci + 0x00, 4, S + K_ERR); wr(ci + 0x0C, 4, a[F_CLIENT]); wr(ci + 0x14, 4, a[F_GSTATE]); wr(ci + 0x18, 4, S + K_SRC)
        wr(ci + 0x24, 4, a[PRE_NC]); wr(ci + 0x40, 1, a[F_BUFIMG]); wr(ci + 0x60, 4, a[F_OUTH]); wr(ci + 0x78, 4, a[F_OUTSCAN])
        wr(ci + 0xC4, 4, (0xDEAD0000 + (a[P_BOFF] & 0xFFFF)) & M32 if a[COMP_BAD] else B0 + B_COMPS)
        wr(ci + 0x100, 1, a[PRE_JFIF]); wr(ci + 0x108, 1, a[PRE_ADOBE]); wr(ci + 0x109, 1, a[PRE_XF])
        wr(ci + 0x180, 4, S + K_MASTER); wr(ci + 0x190, 4, S + K_INP)

    # ---------------------------------------------------------------- memoria
    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)
        off = (addr - self.ci) & M32
        if off < CI_SIZE:
            for k in range(n):
                if off + k < CI_SIZE:
                    self.img[off + k] = (v >> (8 * k)) & 0xFF

    def tag(self, v):
        return self.W.tagv(v)

    def ci_hash(self):
        return fnv(self.img)

    # ---------------------------------------------------------------- falsos
    def err_exit(self):
        err = self.rd(self.ci)
        self.events.append([1, self.rd(err + 0x14), self.rd(err + 0x18), self.rd(err + 0x1C), self.tag(self.ci)])
        self.marks.add('err_exit')
        if self.errmode == 0:
            self.marks.add('escape')
            raise Escape()
        self.marks.add('err_return')

    def errx(self, code, p0=None, p1=None):
        err = self.rd(self.ci)
        self.wr(err + 0x14, 4, code)
        if p0 is not None:
            self.wr(err + 0x18, 4, p0 & M32)
        if p1 is not None:
            self.wr(err + 0x1C, 4, p1 & M32)
        self.err_exit()

    def emit(self, code, level, parms):
        """TRACEMS*/WARNMS*: msg_parm.i[k], msg_code y emit_message(cinfo, level)"""
        err = self.rd(self.ci)
        for k, v in enumerate(parms):
            self.wr(err + 0x18 + 4 * k, 4, v & M32)
        self.wr(err + 0x14, 4, code)
        self.events.append([2, level & M32, self.rd(err + 0x14), self.tag(self.ci)] + [self.rd(err + 0x18 + 4 * k) for k in range(8)])

    def mod(self, ident):
        """modulo de un argumento (cinfo): etiqueta, estado y resumen de la estructura"""
        self.events.append([ident, self.tag(self.ci), self.rd(self.ci + 0x14), self.ci_hash()])

    def fake_consume(self):
        a, ci = self.a, self.ci
        k = self.calls
        self.calls += 1
        if a[POST_ON]:
            self.wr(ci + 0x24, 4, a[POST_NC]); self.wr(ci + 0x100, 1, a[POST_JFIF]); self.wr(ci + 0x108, 1, a[POST_ADOBE]); self.wr(ci + 0x109, 1, a[POST_XF])
            if not a[COMP_BAD]:
                comp = self.rd(ci + 0xC4)
                self.wr(comp + 0x00, 4, a[POST_ID0]); self.wr(comp + 0x54, 4, a[POST_ID1]); self.wr(comp + 0xA8, 4, a[POST_ID2])
        self.events.append([10, k, self.tag(ci), self.rd(ci + 0x14), self.ci_hash()])
        if k == a[EOI_AT]:
            self.wr(self.rd(ci + 0x190) + 0x11, 1, 1)
        return 0 if k >= 40 else (a[CR0 + k] if k < 8 else a[CRDEF])

    # ---------------------------------------------------------------- default_decompress_parms (0x5CFE20)
    def default_parms(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        nc = rd(ci + 0x24)
        m = self.marks
        if nc == 1:
            m.add('cs1')
            wr(ci + 0x28, 4, 1); wr(ci + 0x2C, 4, 1)
        elif nc == 3:
            if rd(ci + 0x100, 1):
                m.add('cs3_jfif')
                wr(ci + 0x28, 4, 3); wr(ci + 0x2C, 4, 2)
            elif rd(ci + 0x108, 1):
                xf = rd(ci + 0x109, 1)
                if xf == 0:
                    m.add('cs3_adobe0')
                    wr(ci + 0x28, 4, 2); wr(ci + 0x2C, 4, 2)
                else:
                    if xf == 1:
                        m.add('cs3_adobe1')
                    else:
                        m.add('cs3_adobe_other')
                        self.emit(0x72, -1, [xf])
                    wr(ci + 0x28, 4, 3); wr(ci + 0x2C, 4, 2)
            else:
                comp = rd(ci + 0xC4)
                i0, i1, i2 = rd(comp + 0x00), rd(comp + 0x54), rd(comp + 0xA8)
                if (i0, i1, i2) == (1, 2, 3):
                    m.add('cs3_ycc')
                    wr(ci + 0x28, 4, 3); wr(ci + 0x2C, 4, 2)
                elif (i0, i1, i2) == (0x52, 0x47, 0x42):
                    m.add('cs3_rgb')
                    wr(ci + 0x28, 4, 2); wr(ci + 0x2C, 4, 2)
                else:
                    m.add('cs3_unknown')
                    self.emit(0x6F, 1, [i0, i1, i2])
                    wr(ci + 0x28, 4, 3); wr(ci + 0x2C, 4, 2)
        elif nc == 4:
            if rd(ci + 0x108, 1) == 0 or rd(ci + 0x109, 1) == 0:
                m.add('cs4_cmyk')
                wr(ci + 0x28, 4, 4); wr(ci + 0x2C, 4, 4)
            else:
                if rd(ci + 0x109, 1) == 2:
                    m.add('cs4_ycck')
                else:
                    m.add('cs4_other')
                    self.emit(0x72, -1, [rd(ci + 0x109, 1)])
                wr(ci + 0x28, 4, 5); wr(ci + 0x2C, 4, 4)
        else:
            m.add('cs_other')
            wr(ci + 0x28, 4, 0); wr(ci + 0x2C, 4, 0)
        wr(ci + 0x38, 4, 0); wr(ci + 0x40, 1, 0); wr(ci + 0x41, 1, 0); wr(ci + 0x44, 4, 0); wr(ci + 0x4A, 1, 0); wr(ci + 0x50, 1, 0); wr(ci + 0x74, 4, 0)
        wr(ci + 0x58, 1, 0); wr(ci + 0x59, 1, 0); wr(ci + 0x5A, 1, 0); wr(ci + 0x30, 4, 1); wr(ci + 0x34, 4, 1); wr(ci + 0x3C, 4, 0x3FF00000)
        wr(ci + 0x48, 1, 1); wr(ci + 0x49, 1, 1); wr(ci + 0x4C, 4, 2); wr(ci + 0x54, 4, 0x100)

    # ---------------------------------------------------------------- las funciones
    def create(self):
        a, ci = self.a, self.ci
        self.wr(ci + 4, 4, 0)
        if a[A_VER] != 0x3E:
            self.marks.add('ver_bad')
            self.errx(0x0C, 0x3E, a[A_VER])
        if a[A_SIZE] != 0x1B0:
            self.marks.add('size_bad')
            self.errx(0x15, 0x1B0, a[A_SIZE])
        if a[A_VER] == 0x3E and a[A_SIZE] == 0x1B0:
            self.marks.add('both_ok')
        err, client = self.rd(ci), self.rd(ci + 0x0C)
        for i in range(CI_SIZE):
            self.wr(ci + i, 1, 0)
        self.wr(ci + 0, 4, err); self.wr(ci + 0x0C, 4, client); self.wr(ci + 0x10, 1, 1)
        self.mod(20)
        self.wr(ci + 0x08, 4, 0); self.wr(ci + 0x18, 4, 0)
        for i in range(4):
            self.wr(ci + 0x90 + 4 * i, 4, 0)
            self.wr(ci + 0xA0 + 4 * i, 4, 0)
            self.wr(ci + 0xB0 + 4 * i, 4, 0)
        self.wr(ci + 0x10C, 4, 0)
        self.mod(21)
        self.mod(22)
        self.wr(ci + 0x14, 4, 0xC8)
        return None

    def consume(self):
        ci = self.ci
        state = self.rd(ci + 0x14)
        idx = (state - 0xC8) & M32
        # tabla de indices 0x5D005C = [0,1,2,3,3,3,3,3,3,4,3] y tabla de saltos 0x5D0048 = [5CFFD3, 5CFFEE, 5D0012, 5D001A, 5D0029]
        entry = [0, 1, 2, 3, 3, 3, 3, 3, 3, 4, 3][idx] if idx <= 10 else 4
        self.marks.add('st_%d' % (idx if idx <= 10 else 99))
        if entry == 4:
            self.errx(0x14, self.rd(ci + 0x14))
            return 0
        if entry == 2:
            return 1
        if entry == 3:
            return self.fake_consume()
        if entry == 0:
            self.mod(11)
            self.mod(12)
            self.wr(ci + 0x14, 4, 0xC9)
        r = self.fake_consume()
        if r == 1:
            self.marks.add('ret1')
            self.default_parms()
            self.wr(ci + 0x14, 4, 0xCA)
        else:
            self.marks.add('ret_not1')
        return r

    def finish(self):
        ci = self.ci
        state = self.rd(ci + 0x14)
        if state in (0xCD, 0xCE) and self.rd(ci + 0x40, 1) == 0:
            self.marks.add('st_scan')
            if self.rd(ci + 0x78) < self.rd(ci + 0x60):
                self.marks.add('too_little')
                self.errx(0x43)
            self.mod(14)
            self.wr(ci + 0x14, 4, 0xD2)
        elif state == 0xCF:
            self.marks.add('st_cf')
            self.wr(ci + 0x14, 4, 0xD2)
        elif state == 0xD2:
            self.marks.add('st_d2')
        else:
            self.marks.add('st_bad_buf' if state in (0xCD, 0xCE) else 'st_bad')
            self.errx(0x14, self.rd(ci + 0x14))
        inp = self.rd(ci + 0x190)
        while self.rd(inp + 0x11, 1) == 0:
            self.marks.add('loop')
            if self.fake_consume() == 0:
                self.marks.add('consume0')
                return 0
        self.marks.add('eoi_end')
        self.mod(13)
        self.mod(23)
        return 1

    def read_header(self):
        ci = self.ci
        state = self.rd(ci + 0x14)
        if state not in (0xC8, 0xC9):
            self.marks.add('st_bad')
            self.errx(0x14, self.rd(ci + 0x14))
        else:
            self.marks.add('st_ok')
        r = self.consume()
        if r == 1:
            self.marks.add('r1')
            return 1
        if r != 2:
            self.marks.add('r_other')
            return r
        if self.a[A_REQ] & 0xFF:
            self.marks.add('r2_req')
            self.errx(0x33)
        else:
            self.marks.add('r2_noreq')
        self.mod(23)
        return 2

    def run(self, fn):
        try:
            if fn == 0:
                r = self.create()
            elif fn == 1:
                r = self.consume()
            elif fn == 2:
                r = self.finish() & 0xFF
            else:
                r = self.read_header()
            return r, False
        except Escape:
            return None, True


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'scene_aligned', 'scene_unaligned']
CS = ['cs1', 'cs3_jfif', 'cs3_adobe0', 'cs3_adobe1', 'cs3_adobe_other', 'cs3_ycc', 'cs3_rgb', 'cs3_unknown', 'cs4_cmyk', 'cs4_ycck', 'cs4_other', 'cs_other']
EXTRA = {
    0: ['ver_bad', 'size_bad', 'both_ok', 'err_return', 'escape'],
    1: ['st_0', 'st_1', 'st_2', 'st_3', 'st_8', 'st_9', 'st_10', 'st_99', 'ret1', 'ret_not1'] + CS,
    2: ['st_scan', 'too_little', 'st_cf', 'st_d2', 'st_bad', 'st_bad_buf', 'loop', 'consume0', 'eoi_end', 'err_return', 'escape'],
    3: ['st_bad', 'st_ok', 'r1', 'r_other', 'r2_req', 'r2_noreq', 'err_return', 'escape'] + CS,
}
KEYS = {fn: COMMON + EXTRA[fn] for fn in range(len(NAMES))}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    sim = Sim(c)
    ret, escaped = sim.run(fn)
    marks = set(sim.marks)
    marks.update(['kind%d' % a[P_KIND], 'fill%d' % a[P_FILL], 'scene_unaligned' if a[P_OFF] % 4 else 'scene_aligned'])
    W = sim.W
    A, B, Cc, D = W.mem
    return {'events': sim.events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'escaped': escaped, 'marks': sorted(marks)}


def check_side(c, o, e, side_row):
    if o.get('escaped') != e['escaped']:
        return 'escaped %s != %s' % (o.get('escaped'), e['escaped'])
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        k = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (n %d vs %d, primer distinto %d) %s != %s' % (len(o['events']), len(e['events']), k, o['events'][k:k + 2], e['events'][k:k + 2])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo AL de la funcion booleana y EAX de las que devuelven int (ya comprobados como `ret`)"""
    return None


def check_row(row, e=None):
    c = row['input']
    if e is None:
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
