"""Oraculo independiente (Python) de Fn95C: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de cada funcion del lote 95C y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Funciones (todas cdecl; el EXE las llama sin conservar ECX/EDX):
  0  GetFirstTextureCallback 0x734930 (textura, dato*): *dato = textura; devuelve 0.
  1  RwTexDictionaryGtaStreamRead1 0x731070 (stream): [0x8D6088] = 0; FindChunk(stream, 1, &len, &ver) (fallo -> 0); Read(stream, marco de 12 bytes, len) y compara con la longitud RELEIDA del marco (una lectura de
     5 a 12 bytes pisa la longitud, que vive en +4); Create (nulo -> 0); cuenta = word bajo de la palabra 0; mitad = cuenta >> 1; [0x8D6088] = mitad; si cuenta > mitad: do { textura = RwTextureGtaStreamRead
     (0x730E60); nula -> ForAllTextures(txd, 0x730E50, 0), Destroy(txd), 0; AddTexture; cuenta = (cuenta - 1) & 0xFFFF } while (cuenta > [0x8D6088] con signo, RELEIDO). Al final [0xC87FE8] = [stream + 0xC],
     [0x8D6088] = cuenta restante y devuelve el diccionario.
  2  RwTexDictionaryGtaStreamRead2 0x731150 (stream, txd): Skip(stream, [0xC87FE8] - [stream + 0xC]); si [0x8D6088] != 0: do { [0x8D6088] -= 1; textura = leer; nula -> ForAllTextures/Destroy, 0; AddTexture }
     while ([0x8D6088] != 0); luego [0x8D6088] -= 1 (queda en -1) y devuelve txd.
  3  RpClumpGtaStreamRead1 0x72E570 (stream) -> AL: FindChunk(1, &len, &ver); Read(stream, 0xC87AE8, 12) == 12; FindChunk(0xE, 0, &ver); _rwFrameListStreamRead(stream, 0xC87AF4) (nulo -> false SIN liberar nada);
     FindChunk(0x1A, 0, &ver) o la lista de geometrias fase 1 fallan -> _rwFrameListDeinitialize(0xC87AF4) y false. Fase 1 de la lista (0x72E140): [0x8D607C] = 0; FindChunk(1, &len, &ver); Read(stream, &contador, 4) == 4;
     [0x8D607C] = contador / 2 (con signo, hacia cero); [0xC87B04] = contador; con contador > 0: Malloc(contador * 4, 0) (nulo -> fallo), se pone a cero y [0xC87B00] = vector; si no, [0xC87B00] = 0; luego
     para i < [0x8D607C]: FindChunk(0xF, 0, &ver); geometria = RpGeometryStreamRead; vector[i] = geometria; nula -> fallo. Con exito [0xC87AFC] = [stream + 0xC] y true.
  4  RpClumpGtaStreamRead2 0x72E620 (stream) -> clump: RpClumpCreate (nulo -> 0); Skip(stream, [0xC87AFC] - [stream + 0xC]); fase 2 de la lista (0x72E220): i = [0x8D607C]; mientras i < [0xC87B04] (con signo):
     FindChunk(0xF, 0, &ver); vector[i] = RpGeometryStreamRead; nula -> fallo; i++. clump+4 = frames[0]. Por cada atomico (i < [0xC87AE8] con signo, relido): FindChunk(0x14, 0, &ver) y 0x72E270: FindChunk(1, &len, &ver),
     Read(stream, 16 bytes {frame, geometria, flags, 0}, len) == len (relida), RpAtomicCreate; atomico+2 = byte bajo de flags; con [0xC87AF8] != 0: RpAtomicSetFrame(atomico, frames[frame]); con [0xC87B04] != 0:
     RpAtomicSetGeometry(atomico, vector[geometria], 0) y listo; si no FindChunk(0xF, 0, &ver), RpGeometryStreamRead, SetGeometry y RpGeometryDestroy (con fallo: RpAtomicDestroy y nulo); RpClumpAddAtomic.
     Cualquier fallo: lista de geometrias liberada (0x72E360: con contador != 0, RpGeometryDestroy de cada entrada no nula y Free(vector); contador = 0), _rwFrameListDeinitialize(0xC87AF4), RpClumpDestroy(clump) y 0.
     Con exito: 0x72E360 y Deinitialize, devuelve el clump.
  5  RpClumpGtaCancelStream 0x72E700 (): 0x72E360 sobre 0xC87B00; _rwFrameListDeinitialize(0xC87AF4); [0xC87AF8] = 0.

Modelo de los falsos (los mismos en el EXE y en el cuerpo de la DLL): ver Fixtures.hpp (scripts por tipo de llamada) y LiveComparison.hpp. Etiquetas de puntero: arena i, desplazamiento d -> 0x10000000 + (i << 20) + 0x40 + d
(0 stream, 1 clump, 2..9 atomicos, 10 vector de frames, 11 vector de geometrias, 12 dato); un puntero a la pila -> 0x7F000000; el resto en crudo.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
A_STREAM, A_CLUMP, A_ATOM0, A_FRAMES, A_GEOMS, A_DATA = 0, 1, 2, 10, 11, 12
STACK = 0x7F000000
FL_ADDR = 0xC87AF4
HDR_ADDR = 0xC87AE8
KFC, KRD, KTEX, KGEO, KATOMS = 32, 8, 24, 24, 8


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + 0x40 + off


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def b4(v):
    return list((v & M32).to_bytes(4, 'little'))


def ptr(v, arena):
    """valor de un puntero de la entrada: 1 = arena propia, el resto crudo"""
    return tag(arena, 0) if v == 1 else v


class Fail(Exception):
    pass


class Model:
    def __init__(self, c):
        self.c = c
        self.ev = []
        self.marks = []
        self.fc = self.rd = self.tx = self.ge = self.at = 0
        self.pos = c.get('spos', 0) & M32
        g = c.get('glob', [0] * 11)
        self.g607c, self.g6088, self.gfe8 = g[0] & M32, g[1] & M32, g[2] & M32
        self.hdr = [g[3] & M32, g[4] & M32, g[5] & M32]
        self.frames = ptr(g[6], A_FRAMES)
        self.fln = g[7] & M32
        self.gafc = g[8] & M32
        self.glarr = ptr(g[9], A_GEOMS)
        self.glnum = g[10] & M32
        self.ga = list(c.get('ga', [0] * 32))
        self.ga_init = list(self.ga)
        self.atomflags = {}
        self.clumpparent = None

    def step(self, k):
        self.pos = (self.pos + self.c['step'][k]) & M32

    # ---- falsos
    def find(self, typ, has_len, has_ver):
        k = self.fc
        self.fc += 1
        self.ev.append([1, tag(A_STREAM), typ, int(has_len), int(has_ver)])
        self.step(0)
        val = self.c['fcval'][k] if k < len(self.c['fcval']) else 0
        ln = self.c['fclen'][k] if has_len and 'fclen' in self.c and k < len(self.c['fclen']) else 0
        return val & M32, ln

    def read(self, buf, length):
        k = self.rd
        self.rd += 1
        self.ev.append([2, tag(A_STREAM), buf, length & M32])
        self.step(1)
        mode = self.c['rdmode'][k] if k < len(self.c['rdmode']) else 0
        pay = self.c['rdpay'][4 * k:4 * k + 4] if 4 * (k + 1) <= len(self.c['rdpay']) else [0, 0, 0, 0]
        raw = b''.join(w.to_bytes(4, 'little') for w in pay)
        r = {0: length, 1: length - 1, 2: length + 1, 3: 0, 4: M32, 5: pay[1]}[mode] & M32
        return r, raw[:min(length, 16)], pay

    def skip(self, off):
        self.ev.append([3, tag(A_STREAM), off & M32])

    def texread(self):
        k = self.tx
        self.tx += 1
        self.ev.append([5, tag(A_STREAM)])
        self.step(2)
        if k < 32 and (self.c['pokemask'] >> k) & 1:
            self.g6088 = self.c['pokeval'][k] & M32
        return (self.c['tex'][k] if k < len(self.c['tex']) else 0) & M32

    def georead(self):
        k = self.ge
        self.ge += 1
        self.ev.append([11, tag(A_STREAM)])
        self.step(3)
        return (self.c['geo'][k] if k < len(self.c['geo']) else 0) & M32

    def atomcreate(self):
        k = self.at
        self.at += 1
        self.ev.append([13])
        if k >= KATOMS or (self.c['atomnull'] >> k) & 1:
            return 0, k
        return tag(A_ATOM0 + k), k

    # ---- ayudantes de la fase de clump
    def deinit(self):
        self.ev.append([21, FL_ADDR])

    def geomlist_destroy(self):
        if self.glnum != 0:
            for i in range(max(0, s32(self.glnum))):
                w = self.ga[i]
                if w:
                    self.ev.append([12, w])
            self.ev.append([10, self.glarr])
        self.glnum = 0


def txd_cleanup(m, txd):
    m.ev.append([7, txd, 0x730E50, 0])
    m.ev.append([8, txd])


def oracle_cb(c):
    m = Model(c)
    changes = []
    old, new = b4(c['data']), b4(c['tex'])
    for i in range(4):
        if old[i] != new[i]:
            changes.append([A_DATA, i, new[i]])
    return dict(ret=0, ev=[], changes=changes, glob=None, marks=['cb'])


def spos_changes(m):
    if 'spos' not in m.c:
        return []
    old, new = b4(m.c['spos']), b4(m.pos)
    return [[A_STREAM, 0xC + i, new[i]] for i in range(4) if old[i] != new[i]]


def ga_changes(m):
    out = []
    for i in range(32):
        old, new = b4(m.ga_init[i]), b4(m.ga[i])
        for j in range(4):
            if old[j] != new[j]:
                out.append([A_GEOMS, 4 * i + j, new[j]])
    return out


def globals_list(m):
    return [m.g607c, m.g6088, m.gfe8, m.hdr[0], m.hdr[1], m.hdr[2], m.frames, m.fln, m.gafc, m.glarr, m.glnum]


def finish(m, ret, extra_changes=()):
    ch = spos_changes(m) + list(extra_changes)
    ch.sort(key=lambda x: (x[0], x[1]))
    return dict(ret=ret & M32, ev=m.ev, changes=ch, glob=[v & M32 for v in globals_list(m)], marks=m.marks)


def oracle_txd1(c):
    m = Model(c)
    m.g6088 = 0
    v, ln = m.find(1, True, True)
    if not v:
        m.marks.append('fc_fail')
        return finish(m, 0)
    r, raw, pay = m.read(STACK, ln)
    n = min(ln, 12)
    frame = bytearray(12)
    frame[4:8] = ln.to_bytes(4, 'little')
    frame[:n] = raw[:n]
    slot = int.from_bytes(frame[4:8], 'little')
    if ln >= 8:
        m.marks.append('slot_overwritten')
    if r != slot:
        m.marks.append('read_mismatch')
        return finish(m, 0)
    m.ev.append([4])
    txd = c['createval'] & M32
    if txd == 0:
        m.marks.append('create_null')
        return finish(m, 0)
    remaining = int.from_bytes(frame[0:2], 'little')
    half = remaining >> 1
    m.g6088 = half
    m.marks.append('count0' if remaining == 0 else ('count1' if remaining == 1 else 'count_many'))
    if c['pokemask']:
        m.marks.append('poke')
    if remaining > half:
        while True:
            tex = m.texread()
            if tex == 0:
                m.marks.append('tex_fail')
                txd_cleanup(m, txd)
                return finish(m, 0)
            m.ev.append([6, txd, tex])
            remaining = (remaining - 1) & 0xFFFF
            if not (remaining > s32(m.g6088)):
                break
    m.marks.append('success')
    m.gfe8 = m.pos
    m.g6088 = remaining
    return finish(m, txd)


def oracle_txd2(c):
    m = Model(c)
    txd = c['txdarg'] & M32
    m.skip((m.gfe8 - m.pos) & M32)
    if m.g6088 != 0:
        m.marks.append('loop')
        while True:
            m.g6088 = (m.g6088 - 1) & M32
            tex = m.texread()
            if tex == 0:
                m.marks.append('tex_fail')
                txd_cleanup(m, txd)
                return finish(m, 0)
            m.ev.append([6, txd, tex])
            if m.g6088 == 0:
                break
    else:
        m.marks.append('empty')
    if c['pokemask']:
        m.marks.append('poke')
    m.marks.append('success')
    m.g6088 = (m.g6088 - 1) & M32
    return finish(m, txd)


def oracle_clump1(c):
    m = Model(c)
    fail_deinit = False

    def bad():
        m.deinit()
        return finish(m, 0, ga_changes(m))
    v, _ = m.find(1, True, True)
    if not v:
        m.marks.append('fc0_fail')
        return finish(m, 0)
    r, raw, pay = m.read(HDR_ADDR, 12)
    m.hdr = [pay[0] & M32, pay[1] & M32, pay[2] & M32]
    if r != 12:
        m.marks.append('rd0_bad')
        return finish(m, 0, ga_changes(m))
    v, _ = m.find(0xE, False, True)
    if not v:
        m.marks.append('fc1_fail')
        return finish(m, 0)
    k = 0
    m.ev.append([20, tag(A_STREAM), FL_ADDR])
    m.step(4)
    mode = c['flmode']
    m.frames = {0: tag(A_FRAMES), 1: 0}.get(mode, 0xC0DE0000)
    m.fln = c['flnum'] & M32
    if not c['flret']:
        m.marks.append('fl_fail')
        return finish(m, 0)
    v, _ = m.find(0x1A, False, True)
    if not v:
        m.marks.append('fc2_fail')
        return bad()
    # fase 1 de la lista de geometrias
    m.g607c = 0
    v, _ = m.find(1, True, True)
    if not v:
        m.marks.append('fc3_fail')
        return bad()
    r, raw, pay = m.read(STACK, 4)
    if r != 4:
        m.marks.append('rd1_bad')
        return bad()
    count = pay[0] & M32
    sc = s32(count)
    m.g607c = (int(sc / 2) if sc >= 0 else -int((-sc) / 2)) & M32
    m.glnum = count
    if sc > 0:
        size = (count * 4) & M32
        m.ev.append([9, size, 0])
        if c['mallocnull']:
            m.marks.append('malloc_null')
            m.glarr = 0
            return bad()
        m.glarr = tag(A_GEOMS)
        nz = size
        for i in range(min(nz, 128)):
            m.ga[i // 4] &= ~(0xFF << (8 * (i % 4)))
        m.marks.append('count_pos')
    else:
        m.glarr = 0
        m.marks.append('count_nonpos')
    i = 0
    while i < s32(m.g607c):
        v, _ = m.find(0xF, False, True)
        if not v:
            m.marks.append('gfc_fail')
            return bad()
        geo = m.georead()
        m.ga[i] = geo
        if geo == 0:
            m.marks.append('geo_fail')
            return bad()
        i += 1
    if s32(m.g607c) > 0:
        m.marks.append('geoms_read')
    m.gafc = m.pos
    m.marks.append('success')
    return finish(m, 1, ga_changes(m))


def oracle_clump2(c):
    m = Model(c)
    clump = tag(A_CLUMP)
    m.ev.append([17])
    if c['clumpnull']:
        m.marks.append('clump_null')
        return finish(m, 0)
    m.skip((m.gafc - m.pos) & M32)
    fa = c['fa']
    extra = []

    def result(ret):
        ch = ga_changes(m) + extra
        return finish(m, ret, ch)

    def fail():
        m.geomlist_destroy()
        m.deinit()
        m.ev.append([19, clump])
        return result(0)
    # fase 2 de la lista
    i = s32(m.g607c)
    if i < s32(m.glnum):
        m.marks.append('geoms2')
        while True:
            v, _ = m.find(0xF, False, True)
            if not v:
                m.marks.append('gfc_fail')
                return fail()
            geo = m.georead()
            m.ga[i] = geo
            if geo == 0:
                m.marks.append('geo_fail')
                return fail()
            i += 1
            if not (i < s32(m.glnum)):
                break
    # padre del clump = frames[0]
    old, new = b4(c['clumpparent0']), b4(fa[0])
    for j in range(4):
        if old[j] != new[j]:
            extra.append([A_CLUMP, 4 + j, new[j]])
    i = 0
    while i < s32(m.hdr[0]):
        v, _ = m.find(0x14, False, True)
        if not v:
            m.marks.append('atomic_fc_fail')
            return fail()
        atom = atomread(m, c, fa, extra)
        if atom == 0:
            m.marks.append('atom_fail')
            return fail()
        m.ev.append([18, clump, atom])
        i += 1
    if s32(m.hdr[0]) > 0:
        m.marks.append('atoms')
    m.geomlist_destroy()
    m.deinit()
    m.marks.append('success')
    return result(clump)


def atomread(m, c, fa, extra):
    v, ln = m.find(1, True, True)
    if not v:
        m.marks.append('astruct_fc_fail')
        return 0
    k = m.rd
    r, raw, pay = m.read(STACK, ln)
    if r != ln:
        m.marks.append('ard_bad')
        return 0
    atom, ak = m.atomcreate()
    if atom == 0:
        m.marks.append('acreate_null')
        return 0
    flags = pay[2] & 0xFF
    if flags != c['atomflags0'][ak]:
        extra.append([A_ATOM0 + ak, 2, flags])
    if m.fln != 0:
        m.ev.append([14, atom, fa[s32(pay[0])]])
        m.marks.append('setframe')
    if m.glnum != 0:
        m.ev.append([15, atom, m.ga[s32(pay[1])], 0])
        m.marks.append('setgeom_list')
        return atom
    v, _ = m.find(0xF, False, True)
    if not v:
        m.marks.append('agfc_fail')
        m.ev.append([16, atom])
        return 0
    geo = m.georead()
    if geo == 0:
        m.marks.append('ageo_fail')
        m.ev.append([16, atom])
        return 0
    m.ev.append([15, atom, geo, 0])
    m.ev.append([12, geo])
    m.marks.append('setgeom_stream')
    return atom


def oracle_cancel(c):
    m = Model(c)
    m.marks.append('num0' if m.glnum == 0 else ('neg' if s32(m.glnum) < 0 else 'num'))
    m.geomlist_destroy()
    m.deinit()
    m.fln = 0
    return finish(m, 0)


def expected(c):
    fn = c['fn']
    if fn == 0:
        return oracle_cb(c)
    return [None, oracle_txd1, oracle_txd2, oracle_clump1, oracle_clump2, oracle_cancel][fn](c)


def check_side(c, o, e):
    if e['ret'] is not None and c['fn'] != 5 and o.get('ret') != e['ret']:
        return 'ret: %s != %s' % (o.get('ret'), e['ret'])
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['ev']:
        n = next((i for i in range(min(len(o['events']), len(e['ev']))) if o['events'][i] != e['ev'][i]), min(len(o['events']), len(e['ev'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['ev']), n, o['events'][n:n + 1], e['ev'][n:n + 1])
    if e['glob'] is not None and o.get('globals') != e['glob']:
        return 'globales: %s != %s' % (o.get('globals'), e['glob'])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
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
