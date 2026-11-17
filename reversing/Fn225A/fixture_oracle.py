"""Oraculo independiente (Python) de Fn225A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 8 funciones del lote 225A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las regiones globales
(Fixtures.hpp) para conocer su imagen inicial y lee del propio gta-sa.exe canonico las tablas de saltos y la tabla de formatos de RenderWare (0x85C670).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`):
   0  0x4CC1D0  cdecl(tipo, banderas): esi = banderas & 0xFF00; tipo > 5 (sin signo) devuelve esi; tabla de saltos 0x4CC55C (0 y 4 -> 0x4CC1EF, 1 -> 0x4CC531, 2 -> 0x4CC518, 3 -> 0x4CC556, 5 -> 0x4CC3C6).
             Ver find_format().
   1  0x4CC580  cdecl(raster, banderas): [raster+0x20] = bajo(banderas) & 7; [raster+0x21] = bajo & 0xF8; eax = 0x4CC1D0(tipo, banderas completas); [raster+0x23] = byte (eax >> 8); [raster+0x14] = byte [(eax >> 8 & 0xF) * 8 + 0x85C674]; 1.
   2  0x4CC970  cdecl(): recorre la lista 0xB4E9FC (nodos {raster, siguiente}); tabla de saltos 0x4CCD50 (0 y 4 -> 0x4CC9BE, 1 -> 0x4CCB1E, 2 -> 0x4CCC11, 3 -> 0x4CCD23, 5 -> 0x4CCA29).
   3  0x4CD050  cdecl(ancho, alto, formato D3D, banderas): RwRasterCreate, comprobadores, CreateTexture y rellenado de la extension (ver el bloque CREATE de expected()).
   4  0x4CD250  cdecl(raster): imagen de 32 bits, ayudante 0x4CC0C0 (usercall), reconstruccion con 0x4CCE60.
   5  0x4CD360  cdecl(salida, raster*): estimacion de la memoria que ocupa el raster.
   6  0x746070  `mov eax,[0xC920EC]; test eax,eax; setne al; ret`: AL = dword != 0 (el EXE deja el dword en el resto de EAX; solo se compara AL).
   7  0x7460A0  cdecl(ancho, alto, profundidad): mejor frecuencia de refresco con IDirect3D9.

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["D3D9RasterFormat::FindCorrectRasterFormat", "D3D9RasterFormat::SetRasterFormat", "D3D9RasterFormat::VideoMemoryRasterListRestore", "D3D9RasterFormat::CreateRasterFromD3DFormat",
         "D3D9RasterFormat::ConvertRasterToDisplayFormat", "D3D9RasterFormat::GetRasterMemorySize", "Win::WasForegroundAppLastFrame", "Ps::GetBestRefreshRate"]
ENTRIES = (0x4CC1D0, 0x4CC580, 0x4CC970, 0x4CD050, 0x4CD250, 0x4CD360, 0x746070, 0x7460A0)
assert len(NAMES) == len(ENTRIES) == 8
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_FAKES = tuple(0x31000000 + 0x10 * i for i in range(7))
(FIND, SETFMT, RESTORE, CREATE, CONVERT, MEMSIZE, FOREGROUND, REFRESH) = range(8)
KIND = list(range(8))
EXE_PATH = 'D:/Grand Theft Auto San Andreas/gta-sa.exe'
STACK_TAG = 0x7F000000
# regiones globales (direccion, bytes): 0 tabla de formatos y desplazamiento de la extension, 1 cabeza de la lista, 2 ventana/D3D/adaptador/dispositivo/superficie, 3 divisores y globales del adaptador,
# 4 parametros de presentacion, 5 tipo de dispositivo, 6 primer plano y subsistema, 7 marca del banco
REGIONS = ((0xB4E7E0, 0x204), (0xB4E9FC, 4), (0xC97C1C, 0x14), (0xC9BEE4, 0x5C), (0xC9C040, 0x38), (0x8E2428, 4), (0xC920EC, 0x10), (0xA9B0B0, 1))
EXTOFF, TABLE, LIST = 0xB4E9E0, 0xB4E7E0, 0xB4E9FC
HWND, D3D, ADAPTER, DEV, RSURF = 0xC97C1C, 0xC97C20, 0xC97C24, 0xC97C28, 0xC97C2C
DIVX, DIVY, ADFMT, DISP, ZDEP, CAPS = 0xC9BEE4, 0xC9BEE8, 0xC9BEF0, 0xC9BEF4, 0xC9BEFC, 0xC9BF3C
PP, DEVTYPE, FORE, SEL = 0xC9C040, 0x8E2428, 0xC920EC, 0xC920F4
# regiones que toca cada funcion (en el orden del banco, la marca siempre al final)
FN_REGIONS = {FIND: (0, 3, 4, 7), SETFMT: (0, 3, 4, 7), RESTORE: (0, 1, 2, 3, 4, 7), CREATE: (0, 2, 7), CONVERT: (0, 2, 3, 5, 7), MEMSIZE: (0, 7), FOREGROUND: (6, 7), REFRESH: (6, 7)}
# ids de los objetos COM falsos
(C_DEVICE, C_D3D, C_TEXTURE, C_CUBE, C_SURF0, C_SURF1, C_SWAP) = range(7)
EV_CHKTEX, EV_CHKRT, EV_CHKDEPTHTEX, EV_CHKCUBERT, EV_CHKAUTOGEN, EV_CHKDEPTHSURF = 0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF20, 0x4CBF80, 0x4CBD80
EV_RASTERPUSH, EV_RASTERDESTROYRES, EV_RASTERREBUILD, EV_RASTERCREATE, EV_RASTERDESTROY = 0x4CB530, 0x4CBB00, 0x4CCE60, 0x7FB230, 0x7FB020
EV_IMAGECREATE, EV_IMAGEALLOC, EV_RASTERTOIMAGE, EV_RASTERFROMIMAGE, EV_IMAGEDESTROY = 0x8026E0, 0x8027A0, 0x7FF270, 0x8001E0, 0x802740
EV_COM = 0xC0A00000
(COM_RELEASE, COM_GETLEVELCOUNT, COM_GETLEVELDESC, COM_LOCKRECT, COM_UNLOCKRECT, COM_LOCKRECTCUBE, COM_UNLOCKRECTCUBE, COM_CREATETEXTURE, COM_CREATECUBE, COM_CREATEDEPTH, COM_SETPAL, COM_CREATESWAP,
 COM_GETBACKBUFFER, COM_MODECOUNT, COM_ENUMMODES, COM_CHECKDEVICEFORMAT, COM_D3DCREATE, COM_GETCLIENTRECT) = range(1, 19)
NONZERO = (1, 2, 0xFFFFFFFF, 0x80000000, 0x100)


# ---------------------------------------------------------------------------------------------- imagen del EXE canonico (solo lectura)
class Exe:
    def __init__(self, path=EXE_PATH):
        self.data = open(path, 'rb').read()
        e = struct.unpack_from('<I', self.data, 0x3C)[0]
        nsec = struct.unpack_from('<H', self.data, e + 6)[0]
        opt = struct.unpack_from('<H', self.data, e + 20)[0]
        self.base = struct.unpack_from('<I', self.data, e + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            _, vs, va, rs, ro = struct.unpack_from('<8sIIII', self.data, e + 24 + opt + i * 40)
            self.secs.append((va + self.base, vs, rs, ro))

    def rd(self, va, n):
        for v, vs, rs, ro in self.secs:
            if v <= va < v + max(vs, rs):
                return self.data[ro + va - v: ro + va - v + n] if va - v < rs else bytes(n)
        raise ValueError('direccion fuera de la imagen: %x' % va)

    def u32(self, va):
        return struct.unpack('<I', self.rd(va, 4))[0]


_TABLES = None


def tables():
    """tablas de saltos de 0x4CC1D0 (0x4CC55C) y 0x4CC970 (0x4CCD50) y profundidad de la tabla de formatos de RenderWare (0x85C674, 8 bytes por formato), leidas del EXE canonico"""
    global _TABLES
    if _TABLES is None:
        exe = Exe()
        find_jt = [exe.u32(0x4CC55C + 4 * i) for i in range(6)]
        restore_jt = [exe.u32(0x4CCD50 + 4 * i) for i in range(6)]
        assert find_jt == [0x4CC1EF, 0x4CC531, 0x4CC518, 0x4CC556, 0x4CC1EF, 0x4CC3C6], [hex(x) for x in find_jt]
        assert restore_jt == [0x4CC9BE, 0x4CCB1E, 0x4CCC11, 0x4CCD23, 0x4CC9BE, 0x4CCA29], [hex(x) for x in restore_jt]
        _TABLES = [exe.rd(0x85C674 + 8 * i, 1)[0] for i in range(16)]
    return _TABLES


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


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


class Mem:
    """arena (o region) dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, guard=GUARD):
        self.kind, self.seed, self.guard, self.m, self.r = kind, seed, guard, {}, {}
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
        return sum(self.get(self.guard + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[self.guard + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self, absolute=0):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - self.guard
            if self.m[i] != ref:
                out.append([rel + absolute, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba) y las regiones globales: lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.fakes = tuple(c.get('fakes') or DEFAULT_FAKES)
        self.mem = tuple(Mem(a[2], a[92 + i]) for i in range(4))
        self.reg = [Mem(a[2], (a[90] + r * 17) & M32, guard=0) for r in range(len(REGIONS))]

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return self.mem[i], addr - b - GUARD
        for r, (at, n) in enumerate(REGIONS):
            if at <= addr < at + n:
                return self.reg[r], addr - at
        raise ValueError('direccion fuera de las arenas y regiones: %x' % addr)

    def rd(self, addr, n=4):
        m, off = self.locate(addr & M32)
        return m.rd(off, n)

    def wr(self, addr, n, v):
        m, off = self.locate(addr & M32)
        m.wr(off, n, v)

    def bytes(self, addr, n):
        m, off = self.locate(addr & M32)
        return bytes(m.get(m.guard + off + j) for j in range(n))

    def tagv(self, v):
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def fake(self, i):
        return self.fakes[i]

    def com_id(self, ptr):
        ptr &= M32
        for i, f in enumerate(self.fakes):
            if f == ptr:
                return i
        return ptr

    def snap(self):
        for m in self.mem:
            m.snap()
        for m in self.reg:
            m.snap()


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    depth_table = tables()
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    W = World(c)
    A, B, Cc, D = W.mem
    events = []
    o = a[0]
    X = a[3]
    this = W.raw(0, o)
    tg = W.tagv
    st = {'hr': 0, 'chk': 0, 'var': 0}
    W.reg[7].wr(0, 1, 0x5A)
    ar = (A, B, Cc, D)

    def ev(kind, *vals):
        events.append([kind] + [v & M32 for v in vals])

    def ok(hr):
        return signed(hr) >= 0

    def next_hr():
        v = a[80 + (st['hr'] & 7)]
        st['hr'] += 1
        return v

    def chk(mask, addr, fmt):
        """comprobador falso: evento [direccion, formato]; el bit (formato & 31) de la mascara decide; los no nulos rotan segun la llamada"""
        n = st['chk']
        st['chk'] += 1
        ev(addr, fmt)
        if not ((mask >> (fmt & 31)) & 1):
            return 0
        return NONZERO[((st['var'] + 7 * n) & M32) % 5]

    def dev_id():
        return W.com_id(W.rd(DEV, 4))

    def create_texture(dev, w, h, levels, usage, fmt, pool, out):
        ev(EV_COM + COM_CREATETEXTURE, W.com_id(dev), w, h, levels, usage, fmt, pool, tg(out), 0)
        hr = next_hr()
        W.wr(out, 4, W.fake(C_TEXTURE) if ok(hr) else 0)
        return hr

    def create_cube(dev, edge, levels, usage, fmt, pool, out):
        ev(EV_COM + COM_CREATECUBE, W.com_id(dev), edge, levels, usage, fmt, pool, tg(out), 0)
        hr = next_hr()
        W.wr(out, 4, W.fake(C_CUBE) if ok(hr) else 0)
        return hr

    def get_client_rect(hwnd):
        ev(EV_COM + COM_GETCLIENTRECT, hwnd, STACK_TAG)
        return [a[25], a[26], a[27], a[28]]

    def tbl(index, j):
        """byte j de la entrada (D3DFORMAT) `index` de la tabla de formatos de pixel 0xB4E7E0 (4 bytes por entrada; el indice puede ser negativo: la direccion se calcula modulo 2^32)"""
        return W.rd((TABLE + 4 * (index & M32) + j) & M32, 1)

    ret = None
    if k in (FIND, SETFMT):
        st['var'] = a[16]
        masks = a[10:16]
        W.wr(CAPS, 1, a[6]); W.wr(DISP, 4, a[7]); W.wr(ZDEP, 4, a[8]); W.wr(PP + 8, 4, a[9])

        def tex(fmt):
            return chk(masks[0], EV_CHKTEX, fmt)

        def rt(fmt):
            return chk(masks[1], EV_CHKRT, fmt)

        def find_format(typ, flags):
            esi = flags & 0xFF00
            if typ > 5:
                marks.append('type_gt5')
                return esi
            marks.append('type%d' % typ)
            caps_neg = (W.rd(CAPS, 1) & 0x80) != 0
            if typ in (0, 4):
                L = None
                if (esi & 0xF00) == 0:
                    marks.append('n_pf0')
                    if esi & 0x2000:
                        if tex(0x29):
                            esi |= 0x500 if caps_neg else 0x600
                            marks.append('n_pal8_ok')
                        else:
                            esi &= ~0x2000 & M32
                        if esi & 0x2000:
                            return esi
                    if tex(0x15):
                        return (esi | 0x500) & M32
                    L = '4CC320'
                else:
                    if esi & 0x4000:
                        esi = (esi & ~0x4000 & M32) | 0x2000
                        marks.append('n_pal4')
                    if esi & 0x2000:
                        marks.append('n_pal')
                        if tex(0x29):
                            if (esi & 0xF00) != 0x500:
                                return esi
                            if caps_neg:
                                return esi
                        dd = signed(W.rd(DISP, 4))
                        esi &= ~0x2000 & M32
                        if (esi & 0xF00) == 0x600:
                            if dd <= 0x10:
                                L = '4CC351'
                            elif tex(0x16):
                                return (esi | 0x600) & M32
                            else:
                                L = '4CC351'
                        else:
                            if dd <= 0x10:
                                L = '4CC31A'
                            elif tex(0x15):
                                return (esi | 0x500) & M32
                            else:
                                L = '4CC31A'
                    else:
                        pf = esi & 0xF00
                        if pf == 0x500:
                            marks.append('n_p500')
                            if tex(0x15):
                                return esi
                            L = '4CC31A'
                        elif pf == 0x600:
                            marks.append('n_p600')
                            if tex(0x16):
                                return esi
                            L = '4CC351'
                        elif pf == 0x300:
                            marks.append('n_p300')
                            if tex(0x1A):
                                return esi
                            esi &= ~0x200 & M32
                            L = '4CC380'
                        elif pf == 0x400:
                            marks.append('n_p400')
                            if tex(0x32):
                                return esi
                            esi &= ~0x400 & M32
                            if tex(0x16):
                                return (esi | 0x16) & M32
                            return (esi | 0x200) & M32
                        else:
                            marks.append('n_other')
                            return esi
                marks.append('L' + L)
                if L == '4CC31A':
                    esi &= ~0x500 & M32
                    L = '4CC320'
                if L == '4CC320':
                    if tex(0x1A):
                        return (esi | 0x300) & M32
                    L = '4CC380'
                if L == '4CC380':
                    return (esi | 0x100) & M32
                # 4CC351
                esi &= ~0x400 & M32
                return (esi | 0x200) & M32
            if typ == 1:
                if esi & 0xF00:
                    return esi
                zd = W.rd(ZDEP, 4)
                if zd == 0x10:
                    return 0x700
                if zd == 0x20:
                    return 0x900
                return esi
            if typ == 2:
                if esi & 0xF00:
                    return esi
                return W.rd(TABLE + 4 * W.rd(PP + 8, 4) + 2, 2)
            if typ == 3:
                return esi
            # textura de camara (5)
            zd = W.rd(ZDEP, 4)
            ecx = esi & 0xF00
            if ecx == 0:
                if zd == 0x10:
                    return (esi | 0x200) & M32
                if rt(0x16):
                    return (esi | 0x600) & M32
                return (esi | 0x200) & M32
            dpt = depth_table[(esi >> 8) & 0xF]
            if zd == 0x10:
                marks.append('c_z16')
                if dpt == 0x20:
                    if ecx == 0x500:
                        esi = (esi & ~0x400 & M32) | 0x300
                    elif ecx == 0x600:
                        esi = (esi & ~0x400 & M32) | 0x200
                    else:
                        esi = 0x200
                if (esi & 0xF00) == 0x300:
                    if not rt(0x1A):
                        esi = (esi & ~0x200 & M32) | 0x100
                if (esi & 0xF00) != 0x100:
                    return esi
                if rt(0x19):
                    return esi
                esi &= ~0x100 & M32
                return (esi | 0x200) & M32
            marks.append('c_zother')
            if dpt == 0x10:
                if ecx == 0x300:
                    esi = (esi & ~0x200 & M32) | 0x500
                elif ecx == 0x100:
                    esi |= 0x500
                elif ecx == 0x200:
                    esi |= 0x600
                else:
                    esi = 0x600
            if (esi & 0xF00) == 0x500:
                if not rt(0x15):
                    esi = (esi & ~0x100 & M32) | 0x600
            return esi

        W.snap()
        if k == FIND:
            ret = find_format(a[4], a[5])
        else:
            low = a[5] & 0xFF
            typ = low & 7
            A.wr(o + 0x20, 1, typ)
            A.wr(o + 0x21, 1, low & 0xF8)
            res = find_format(typ, a[5])
            A.wr(o + 0x23, 1, (res >> 8) & 0xFF)
            A.wr(o + 0x14, 4, depth_table[(res >> 8) & 0xF])
            ret = 1
    elif k == RESTORE:
        st['var'] = a[16]
        W.wr(EXTOFF, 4, X); W.wr(ZDEP, 4, a[5]); W.wr(PP + 8, 4, a[6]); W.wr(DIVX, 4, a[7]); W.wr(DIVY, 4, a[8]); W.wr(PP, 4, a[9]); W.wr(PP + 4, 4, a[10])
        W.wr(PP + 0x10, 4, a[11]); W.wr(PP + 0x14, 4, a[12]); W.wr(HWND, 4, a[13]); W.wr(RSURF, 4, a[14]); W.wr(DEV, 4, W.fake(C_DEVICE))
        count = a[4]
        for n in range(count):
            b = 32 + 14 * n
            R, H = ar[n], ar[n + 1]
            ro = a[b + 0]
            self_ptr = W.raw(n, ro)
            parent = self_ptr if a[b + 1] == 0 else (W.raw((n + 1) % 4, 0x10) if a[b + 1] == 1 else a[b + 12])
            R.wr(ro, 4, parent); R.wr(ro + 0xC, 4, a[b + 4]); R.wr(ro + 0x10, 4, a[b + 5]); R.wr(ro + 0x20, 1, a[b + 2]); R.wr(ro + 0x23, 1, a[b + 3])
            e = ro + X
            R.wr(e + 0, 4, 0 if a[b + 7] == 0 else (W.fake(C_SURF0) if a[b + 7] == 1 else 0x0BADF00D))
            R.wr(e + 4, 4, W.raw(n + 1, a[b + 13]) if a[b + 8] else 0)
            R.wr(e + 9, 1, a[b + 9]); R.wr(e + 0xA, 1, a[b + 10]); R.wr(e + 0x18, 4, a[b + 6]); R.wr(e + 0x1C, 4, a[b + 11]); R.wr(e + 0x20, 4, a[b + 12])
            H.wr(a[74 + n], 4, self_ptr)
            H.wr(a[74 + n] + 4, 4, W.raw(n + 2, a[74 + n + 1]) if n + 1 < count else 0)
        W.wr(LIST, 4, W.raw(1, a[74]) if count else 0)
        W.snap()
        node = W.rd(LIST, 4)
        marks.append('nodes%d' % count)
        if node == 0:
            ret = 1
        else:
            result, ebx = 1, 0
            while True:
                raster = W.rd(node, 4)
                nxt = W.rd(node + 4, 4)
                ext = (raster + W.rd(EXTOFF, 4)) & M32
                if W.rd(raster, 4) == raster:
                    t = W.rd(raster + 0x20, 1)
                    if t <= 5:
                        marks.append('t%d' % t)
                        if t in (0, 4):
                            cf = W.rd(raster + 0x23, 1)
                            if cf & 0x60 == 0:
                                fmt = signed(W.rd(ext + 0x18, 4))
                                if 0x46 <= fmt <= 0x50:
                                    ebx = create_texture(W.rd(DEV, 4), W.rd(raster + 0xC, 4), W.rd(raster + 0x10, 4), 1, 2, fmt, 0, ext)
                                    marks.append('depth_texture')
                            else:
                                pal = W.rd(ext + 4, 4)
                                if pal:
                                    number = W.rd(pal + 0x400, 4)
                                    ev(EV_COM + COM_SETPAL, dev_id(), number, tg(pal), fnv(W.bytes(pal, 0x400)))
                                    ebx = next_hr()
                                    marks.append('palette')
                        elif t == 1:
                            if W.rd(ext, 4) == 0:
                                zd = W.rd(ZDEP, 4)
                                if zd == 0x10:
                                    r = chk(a[15], EV_CHKDEPTHSURF, 0x49)
                                    W.wr(ext + 0x18, 4, 0x49 if r else 0x50)
                                elif zd == 0x20:
                                    if chk(a[15], EV_CHKDEPTHSURF, 0x4B):
                                        W.wr(ext + 0x18, 4, 0x4B)
                                    elif chk(a[15], EV_CHKDEPTHSURF, 0x4F):
                                        W.wr(ext + 0x18, 4, 0x4F)
                                    else:
                                        r = chk(a[15], EV_CHKDEPTHSURF, 0x47)
                                        W.wr(ext + 0x18, 4, 0x47 if r else 0x4D)
                                height = signed(W.rd(raster + 0x10, 4))
                                width = signed(W.rd(raster + 0xC, 4))
                                height = 1 if height < 1 else height
                                width = 1 if width < 1 else width
                                fmt = W.rd(ext + 0x18, 4)
                                ev(EV_COM + COM_CREATEDEPTH, dev_id(), width, height, fmt, W.rd(PP + 0x10, 4), W.rd(PP + 0x14, 4), 0, tg(ext), 0)
                                ebx = next_hr()
                                W.wr(ext, 4, W.fake(C_SURF0) if ok(ebx) else 0)
                                marks.append('zbuffer_new')
                            else:
                                rect = get_client_rect(W.rd(HWND, 4))
                                W.wr(raster + 0xC, 4, rect[2]); W.wr(raster + 0x10, 4, rect[3])
                                W.wr(ext, 4, W.rd(RSURF, 4)); W.wr(ext + 0x18, 4, W.rd(PP + 0x28, 4))
                                marks.append('zbuffer_adopt')
                        elif t == 2:
                            if W.rd(ext + 0x1C, 4) != 0:
                                window = W.rd(ext + 0x20, 4)
                                pp = [W.rd(PP + 4 * i, 4) for i in range(14)]
                                rect = get_client_rect(window)
                                pp[7] = window
                                W.wr(ext + 0x1C, 4, 0)
                                pp[0] = 1 if rect[2] == 0 else 0
                                pp[1] = 1 if rect[3] == 0 else 0
                                pp[9] = 0
                                ev(EV_COM + COM_CREATESWAP, dev_id(), STACK_TAG, tg(ext + 0x1C), *pp)
                                hr2 = next_hr()
                                W.wr(ext + 0x1C, 4, W.fake(C_SWAP) if ok(hr2) else 0)
                                if hr2 == 0:
                                    sc = W.rd(ext + 0x1C, 4)
                                    ev(EV_COM + COM_GETBACKBUFFER, W.com_id(sc), 0, 0, tg(ext))
                                    ebx = next_hr()
                                    W.wr(ext, 4, W.fake(C_SURF1))
                                    ev(EV_COM + COM_RELEASE, W.com_id(W.rd(ext, 4)))
                                    W.wr(ext + 0x18, 4, pp[2])
                                    marks.append('swap_ok')
                                else:
                                    marks.append('swap_fail')
                            else:
                                divx = W.rd(DIVX, 4)
                                if divx != 0:
                                    w = W.rd(raster + 0xC, 4)
                                    back = W.rd(PP, 4)
                                    if w != back:
                                        W.wr(raster + 0xC, 4, ((w * back) & M32) // divx)
                                divy = W.rd(DIVY, 4)
                                if divy != 0:
                                    h = W.rd(raster + 0x10, 4)
                                    back = W.rd(PP + 4, 4)
                                    if h != back:
                                        W.wr(raster + 0x10, 4, ((h * back) & M32) // divy)
                                bb = W.rd(PP + 8, 4)
                                W.wr(raster + 0x14, 4, tbl(bb, 1)); W.wr(raster + 0x23, 1, tbl(bb, 3))
                                W.wr(ext + 0x18, 4, bb); W.wr(ext + 8, 1, tbl(bb, 0))
                                marks.append('camera_plain')
                        elif t == 5:
                            if W.rd(ext, 4) == 0:
                                cf = W.rd(raster + 0x23, 1)
                                levels = (~(cf >> 7)) & 1
                                usage = 0x400 if (W.rd(ext + 0xA, 1) & 0xF) else 0
                                cube = (W.rd(ext + 9, 1) & 0xF) != 0

                                def make(fmt, rt_bit):
                                    # primer intento (0x4CCA69): `or edx,1` (D3DUSAGE_RENDERTARGET); reintento con el back buffer (0x4CCACB / 0x4CCAE1): neg dl; sbb edx,edx; and edx,400h SIN `or edx,1`
                                    if cube:
                                        return create_cube(W.rd(DEV, 4), W.rd(raster + 0xC, 4), levels, usage | rt_bit, fmt, 0, ext)
                                    return create_texture(W.rd(DEV, 4), W.rd(raster + 0xC, 4), W.rd(raster + 0x10, 4), levels, usage | rt_bit, fmt, 0, ext)

                                ebx = make(W.rd(ext + 0x18, 4), 1)
                                marks.append('cube_target' if cube else 'texture_target')
                                if not ok(ebx) and ebx != 0x8876017C:
                                    bb = W.rd(PP + 8, 4)
                                    ebx = make(bb, 0)
                                    W.wr(raster + 0x23, 1, tbl(bb, 3)); W.wr(raster + 0x14, 4, tbl(bb, 1))
                                    W.wr(ext + 0x18, 4, bb); W.wr(ext + 8, 1, tbl(bb, 0))
                                    marks.append('retry')
                                elif not ok(ebx):
                                    marks.append('out_of_video_memory')
                if signed(ebx) < 0:
                    result = 0
                node = nxt
                if node == 0:
                    break
            ret = result
            marks.append('result%d' % result)
    elif k == CREATE:
        st['var'] = a[16]
        W.wr(EXTOFF, 4, X); W.wr(DEV, 4, W.fake(C_DEVICE))
        A.wr(o + 0xC, 4, a[9]); A.wr(o + 0x10, 4, a[10])
        W.snap()
        fmt, fl = a[6], a[7]
        if (fl & 7) in (0, 1):
            fl &= 0xFFFF6FFF
        ev(EV_RASTERCREATE, a[4], a[5], 0, (fl | 0x80) & M32)
        raster = 0 if a[8] else this
        if raster == 0:
            ret = 0
            marks.append('create_null')
        else:
            ext = (raster + W.rd(EXTOFF, 4)) & M32
            failed = False
            if (fl & 0x9000) == 0x9000:
                if chk(a[14], EV_CHKAUTOGEN, fmt):
                    W.wr(ext + 0xA, 1, (W.rd(ext + 0xA, 1) & 0xF1) | 1)
                    marks.append('autogen')
            usage = 0x400 if (W.rd(ext + 0xA, 1) & 0xF) else 0
            if (fl & 5) == 5:
                pool = 0
                marks.append('pool_rt')
                if not chk(a[12], EV_CHKRT, fmt):
                    failed = True
                else:
                    usage |= 1
            elif 0x46 <= signed(fmt) <= 0x50:
                pool = 0
                marks.append('pool_depth')
                if not chk(a[13], EV_CHKDEPTHTEX, fmt):
                    failed = True
                else:
                    usage |= 2
            else:
                pool = 1
                marks.append('pool_managed')
                if not chk(a[11], EV_CHKTEX, fmt):
                    failed = True
            if not failed:
                levels = (~(fl >> 15)) & 1
                hr = create_texture(W.rd(DEV, 4), W.rd(raster + 0xC, 4), W.rd(raster + 0x10, 4), levels, usage, fmt, pool, ext)
                if not ok(hr):
                    failed = True
                    marks.append('create_fail')
            if failed:
                ev(EV_RASTERDESTROY, tg(raster))
                ret = 0
                marks.append('failed')
            else:
                if pool == 0:
                    ev(EV_RASTERPUSH, tg(raster))
                W.wr(raster + 0x21, 1, W.rd(raster + 0x21, 1) & 0x7F)
                W.wr(raster + 0x23, 1, W.rd(raster + 0x23, 1) & 0xF0)
                W.wr(ext + 0x18, 4, fmt)
                sf = signed(fmt)
                if sf < 0x31545844 or sf > 0x35545844:
                    W.wr(ext + 0xA, 1, W.rd(ext + 0xA, 1) & 0xF)
                    if sf < 0x80:
                        W.wr(ext + 8, 1, tbl(fmt, 0))
                        W.wr(raster + 0x14, 4, tbl(fmt, 1))
                        W.wr(raster + 0x23, 1, W.rd(raster + 0x23, 1) | tbl(fmt, 3))
                        marks.append('table_format')
                    else:
                        marks.append('format_ge80')
                else:
                    W.wr(ext + 0xA, 1, (W.rd(ext + 0xA, 1) & 0xF) | 0x10)
                    if fmt == 0x31545844:
                        W.wr(ext + 8, 1, 0)
                        W.wr(raster + 0x23, 1, W.rd(raster + 0x23, 1) | 2)
                        marks.append('dxt1')
                    else:
                        W.wr(ext + 8, 1, 1)
                        W.wr(raster + 0x23, 1, W.rd(raster + 0x23, 1) | 3)
                        marks.append('dxt2_5')
                    W.wr(raster + 0x14, 4, 0x10)
                ret = tg(raster)
                marks.append('created')
    elif k == CONVERT:
        st['var'] = a[18]
        W.wr(EXTOFF, 4, X); W.wr(D3D, 4, W.fake(C_D3D)); W.wr(ADAPTER, 4, a[12]); W.wr(DEVTYPE, 4, a[13]); W.wr(ADFMT, 4, a[11]); W.wr(DISP, 4, a[10])
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5])
        A.wr(o, 4, this if a[6] == 0 else (W.raw(1, a[21]) if a[6] == 1 else a[22]))
        A.wr(o + 0x20, 1, a[7]); A.wr(o + X + 8, 1, a[8]); A.wr(o + X + 9, 1, a[9])
        image = 0 if a[14] else W.raw(1, a[15])
        W.snap()
        ev(EV_IMAGECREATE, A.rd(o + 0xC, 4), A.rd(o + 0x10, 4), 0x20)
        if image == 0:
            marks.append('image_null')
        else:
            ext = (this + W.rd(EXTOFF, 4)) & M32
            ev(EV_IMAGEALLOC, tg(image))
            ev(EV_RASTERTOIMAGE, tg(image), tg(A.rd(o, 4)), 0)
            alpha = W.rd(ext + 8, 1)
            disp = signed(W.rd(DISP, 4))
            ctype = W.rd(this + 0x20, 1)

            def helper(is_cube, fmt):
                """0x4CC0C0 (EAX = es cubo, EDX = formato, ECX = tipo): tipo & 7 = 0 o 4 -> CheckDeviceFormat (resultado >= 0); 5 -> 0x4CBF20 (cubo) o 0x4CBE20 (valor crudo); otro -> 0"""
                t = ctype & 7
                if t in (0, 4):
                    ev(EV_COM + COM_CHECKDEVICEFORMAT, W.com_id(W.rd(D3D, 4)), W.rd(ADAPTER, 4), W.rd(DEVTYPE, 4), W.rd(ADFMT, 4), 0, 5 if is_cube else 3, fmt)
                    return 1 if ok(next_hr()) else 0
                if t == 5:
                    return chk(a[16], EV_CHKCUBERT, fmt) if is_cube else chk(a[17], EV_CHKRT, fmt)
                return 0

            cube = W.rd(ext + 9, 1) & 0xF
            newfmt = None
            if alpha != 0:
                marks.append('alpha')
                if disp > 0x10:
                    if helper(cube, 0x15):
                        newfmt = 0x500
                if newfmt is None:
                    newfmt = 0x300 if helper(cube, 0x1A) else 0x100
            elif disp > 0x10:
                newfmt = 0x600 if helper(cube, 0x16) else 0x200
            else:
                newfmt = 0x200
            marks.append('fmt%X' % newfmt)
            ev(EV_RASTERDESTROYRES, 0, tg(this), 0)
            ev(EV_RASTERREBUILD, 0, tg(this), (W.rd(this + 0x20, 1) & 7) | newfmt | 0x9000)
            ev(EV_RASTERFROMIMAGE, tg(this), tg(image), 0)
            ev(EV_IMAGEDESTROY, tg(image))
        ret = None
    elif k == MEMSIZE:
        W.wr(EXTOFF, 4, X)
        B.wr(a[5], 4, 0 if a[6] else this)
        A.wr(o + 0x23, 1, a[7])
        cube = (a[8] & 0xF) != 0
        A.wr(o + X, 4, W.fake(C_CUBE) if cube else W.fake(C_TEXTURE))
        A.wr(o + X + 9, 1, a[8]); A.wr(o + X + 0xA, 1, a[9])
        W.snap()
        out = W.raw(2, a[4])
        raster = B.rd(a[5], 4)
        if raster == 0:
            W.wr(out, 4, 0x54)
            marks.append('raster_null')
        else:
            size = 0x64
            cf = W.rd(raster + 0x23, 1)
            if cf & 0x40:
                size = 0xE4
            elif cf & 0x20:
                size = 0x464
            ext = (raster + W.rd(EXTOFF, 4)) & M32
            tid = W.com_id(W.rd(ext, 4))
            is_cube = (W.rd(ext + 9, 1) & 0xF) != 0
            if W.rd(ext + 0xA, 1) & 0xF:
                levels = 1
            else:
                ev(EV_COM + COM_GETLEVELCOUNT, tid)
                levels = a[11]
            total = 0
            if levels != 0:
                while True:
                    levels -= 1
                    ev(EV_COM + COM_GETLEVELDESC, tid, levels, STACK_TAG)
                    height = (a[14] + levels * a[15]) & M32
                    pitch = (a[12] + levels * a[13]) & M32
                    if is_cube:
                        ev(EV_COM + COM_LOCKRECTCUBE, tid, 0, levels, STACK_TAG, 0, 0x8010)
                        ev(EV_COM + COM_UNLOCKRECTCUBE, tid, 0, levels)
                    else:
                        ev(EV_COM + COM_LOCKRECT, tid, levels, STACK_TAG, 0, 0x8010)
                        ev(EV_COM + COM_UNLOCKRECT, tid, levels)
                    nbytes = (((height >> 2) * pitch) if (W.rd(ext + 0xA, 1) & 0xF0) else (pitch * height)) & M32
                    total = (total + nbytes + 4) & M32
                    if levels == 0:
                        break
            size = (size + 6 * total) & M32 if is_cube else (size + total) & M32
            W.wr(out, 4, size)
            marks.append('cube' if is_cube else 'texture')
            marks.append('compressed' if (W.rd(ext + 0xA, 1) & 0xF0) else 'plain')
        ret = 1
    elif k == FOREGROUND:
        W.wr(FORE, 4, a[4])
        W.snap()
        ret = 1 if W.rd(FORE, 4) != 0 else 0
        marks.append('foreground_set' if ret else 'foreground_clear')
        if ret and (a[4] & 0xFF) == 0:
            marks.append('high_bits_only')
    elif k == REFRESH:
        W.wr(SEL, 4, a[7])
        W.snap()
        depth = a[6]
        fmt = 0x16 if depth == 0x20 else (0x14 if depth == 0x18 else 0x17)
        marks.append('depth32' if depth == 0x20 else ('depth24' if depth == 0x18 else 'depth_other'))
        ev(EV_COM + COM_D3DCREATE, 31)
        sel = W.rd(SEL, 4)
        d3d_id = W.com_id(W.fake(C_D3D))
        ev(EV_COM + COM_MODECOUNT, d3d_id, sel, fmt)
        count = signed(a[8])
        best = 0x7FFFFFFF
        done = False
        for i in range(count if count > 0 else 0):
            ev(EV_COM + COM_ENUMMODES, d3d_id, sel, fmt, i, STACK_TAG)
            w, h, rate, mfmt = a[16 + 4 * i: 20 + 4 * i]
            if w != a[4] or h != a[5] or mfmt != fmt:
                continue
            marks.append('mode_match')
            if rate == 0:
                ev(EV_COM + COM_RELEASE, d3d_id)
                ret = 0
                done = True
                marks.append('rate_zero')
                break
            if rate < best and rate >= 60:
                best = rate
        if not done:
            ev(EV_COM + COM_RELEASE, d3d_id)
            ret = 0xFFFFFFFF if best == 0x7FFFFFFF else best
            marks.append('none' if best == 0x7FFFFFFF else 'found')
        if count <= 0:
            marks.append('no_modes')
    gch = []
    for r in FN_REGIONS[k]:
        gch += W.reg[r].changes(absolute=REGIONS[r][0])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'vptrs': [], 'gchanges': gch, 'ret': ret, 'threw': False, 'marks': marks}


KEYS = [[] for _ in NAMES]
_FIND_KEYS = ['type0', 'type1', 'type2', 'type3', 'type4', 'type5', 'type_gt5', 'n_pal', 'n_pal4', 'n_p300', 'n_p400', 'n_p500', 'n_p600', 'n_pf0', 'n_other', 'c_z16', 'c_zother', 'L4CC31A', 'L4CC320', 'L4CC351', 'L4CC380']
KEYS[FIND] = _FIND_KEYS
KEYS[SETFMT] = _FIND_KEYS
KEYS[RESTORE] = ['nodes0', 'nodes1', 'nodes2', 'nodes3', 't0', 't1', 't2', 't3', 't4', 't5', 'result0', 'result1', 'camera_plain', 'cube_target', 'texture_target', 'depth_texture', 'palette', 'retry',
                 'swap_ok', 'swap_fail', 'zbuffer_adopt', 'zbuffer_new', 'out_of_video_memory']
KEYS[CREATE] = ['create_null', 'created', 'failed', 'create_fail', 'pool_rt', 'pool_depth', 'pool_managed', 'dxt1', 'dxt2_5', 'autogen', 'table_format', 'format_ge80']
KEYS[CONVERT] = ['image_null', 'alpha', 'fmt100', 'fmt200', 'fmt300', 'fmt500', 'fmt600']
KEYS[MEMSIZE] = ['raster_null', 'cube', 'texture', 'compressed', 'plain']
KEYS[FOREGROUND] = ['foreground_set', 'foreground_clear', 'high_bits_only']
KEYS[REFRESH] = ['depth32', 'depth24', 'depth_other', 'mode_match', 'none', 'found', 'no_modes', 'rate_zero']


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado; de WasForegroundAppLastFrame solo AL)"""
    return None


def check_row(row):
    c = row['input']
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
