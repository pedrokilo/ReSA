"""Oraculo independiente del banco Dependencies25D: repite las formulas de Fixtures.hpp (fixtures por caso) y calcula, a partir de la fixture y de la ventana inicial del arena
registrada por el banco (campo `before`), el resultado que DEBE tener cada una de las 20 funciones segun el desensamblado del EXE: valor de retorno, ventana final del arena,
traza de colaboradores y estado de los seis globales del motor. No usa el codigo C++ de la DLL ni de los bancos; `python -B fixture_oracle.py` imprime la cobertura declarada."""
import json
import sys

M = 0xFFFFFFFF
CASES = 1024
ARENA = 0x300


def mix(x):
    x &= M
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M
    x ^= x >> 15
    x = (x * 0x846CA68B) & M
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M)


SPECIAL = [0x00000000, 0x00000001, 0x00000002, 0x0000007F, 0x000000FF, 0x00000100, 0x0000FFFF, 0x00010000,
           0x00FF0000, 0x01010101, 0x7FFFFFFF, 0x80000000, 0x80000001, 0xFFFFFFFF, 0xFFFFFFFE, 0xDEADBEEF,
           0xCDCDCDCD, 0xA5A5A5A5, 0x5A5A5A5A, 0x12345678, 0x7F7FFFFF, 0xFF800000, 0x7FC00001, 0x3F800000,
           0x00800000, 0x00000003, 0x0000000F, 0x40000000, 0x0FFFFFFF, 0x10000000, 0xC0000000, 0x00008000]
HR = [0x00000000, 0x00000001, 0x00000002, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x80004005, 0x8876086C,
      0x88760868, 0x00010000, 0x80070057, 0x8007000E, 0x80004001, 0x8000FFFF, 0xFFFF0000, 0xFFFFFFFE]
RET8 = [0x00000000, 0x00000001, 0x00000002, 0xFFFFFFFF, 0x80000000, 0x00000100, 0x0000FF00, 0x7FFFFFFF]
ENG = [0x10, 0x20]
TEXBASE = [0x100, 0x140, 0x180]
RASBASE = [0x120, 0x150, 0x180]
VBPOOL = [0x11110000, 0x22220000, 0x33330000, 0x00000000]
SURFACE = [0x00000000, 0x00A00010, 0x00B00020, 0xFFFFFFFF]
DYN_STRIDE, DYN_BASE = 0x1C, 0x100
FRAME_PREV, FRAME_NEXT, FRAME_OBJ = 0x100, 0x110, 0x130
JUNK = 0xC0DE0000
REG_TEXTURE, REG_TEXDICT, REG_RASTER = 0x8E23CC, 0x8E23E4, 0x8E2518


def special(c, k):
    return SPECIAL[rand(c, k) % 32]


def seeds(c):
    return [rand(c, 900), rand(c, 901), rand(c, 902)]


# ---------------------------------------------------------------- nombres y orden (identico a kSpecs de Scenes.hpp)
NAMES = ['RwCore::RwTextureGetMipmapping', 'RwCore::RwTextureGetAutoMipmapping', 'RwCore::RwTextureRegisterPlugin', 'RwCore::RwTexDictionaryRegisterPlugin',
         'RwCore::RwTextureRasterGenerateMipmaps', 'RwCore::RwD3D9DynamicVertexBufferDestroy', 'RwCore::RwD3D9DynamicVertexBufferUnlock', 'RwCore::_rwD3D9SetDepthStencilSurface',
         'RwCore::_rwDeviceRegisterPlugin', 'RwCore::RwD3D9DeleteVertexShader', 'RwCore::RwD3D9CreatePixelShader', 'RwCore::RwD3D9DeletePixelShader',
         'RwCore::RwRasterGetCurrentContext', 'RwCore::RwRasterRegisterPlugin', 'RwCore::RwRasterLockPalette', 'RwPlCore::RwFreeListCreate',
         'RwPlCore::RwFreeListCreateAndPreallocateSpace', 'RwCore::RwRGBAToPixel', 'RwCore::RwTextureRegisterPluginStream', 'RwCore::_rwObjectHasFrameReleaseFrame']
ENTRIES = [0x7F3550, 0x7F3580, 0x7F3BB0, 0x7F3C10, 0x7F3CD0, 0x7F5AE0, 0x7F5C90, 0x7F5EF0, 0x7F5F60, 0x7FAC90, 0x7FACC0, 0x7FACF0, 0x7FAE60, 0x7FB0B0, 0x7FB0E0, 0x801980,
           0x801B70, 0x803740, 0x804550, 0x804F40]
VOID = {5, 7, 9, 11, 19}
(MIP, AUTO, TEXREG, DICTREG, GEN, DYN, UNLOCK, DEPTH, DEVREG, DELVS, CREATEPS, DELPS, RASCTX, RASREG, LOCKPAL, FREELIST, FREEPRE, RGBA, STREAM, FRAME) = range(20)
WINDOW = {MIP: (0x100, 0xC0), AUTO: (0x100, 0xC0), GEN: (0x100, 0xC0), RASCTX: (0x100, 0xC0), TEXREG: (0x100, 8), DICTREG: (0x100, 8), RASREG: (0x100, 8), STREAM: (0x100, 8),
          DYN: (0x100, 0xC0), UNLOCK: (0x100, 0x30), DELVS: (0x100, 0x30), DELPS: (0x100, 0x30), DEPTH: (0x100, 0x10), CREATEPS: (0x100, 0x40), LOCKPAL: (0x100, 8),
          RGBA: (0x100, 0x10), FREELIST: (0x100, 8), FREEPRE: (0x100, 8), DEVREG: (0x100, 8), FRAME: (0x100, 0x50)}


# ---------------------------------------------------------------- fixtures (diccionarios con las mismas claves que `input` de la escena C++)
def fx_tex(c, kind):
    f = {'case': c, 'eng': ENG[c % 2], 'tex_base': TEXBASE[c // 2 % 3]}
    if kind == GEN:
        f.update(raster=rand(c, 4), image=rand(c, 5), rv=0 if c % 4 == 0 else special(c, 6))
    else:
        f.update({'mip': special(c, 1), 'auto': special(c, 2), 'cb': special(c, 3)})
    return f


def fx_ras(c):
    return {'case': c, 'eng': ENG[c % 2], 'ras_base': RASBASE[c // 2 % 3], 'sp': c // 6 % 18 - 4}


def fx_plugin(c, stream):
    f = {'case': c, 'id': special(c, 2), 'f1': rand(c, 3), 'f2': rand(c, 4), 'f3': rand(c, 5), 'ret': special(c, 6)}
    if not stream:
        f['size'] = special(c, 1)
    return f


def fx_dyn(c):
    n, mul, sel = c % 7, c // 7 % 6 + 1, c // 42 % 6
    arg = VBPOOL[sel] if sel < 4 else (0x44440000 if sel == 4 else 0xFFFFFFFF)
    return {'case': c, 'n': n, 'mul': mul, 'arg_sel': sel, 'vb_sel': [rand(c, 10 + i) % 4 for i in range(6)], 'arg': arg}


def fx_obj(c):
    return {'case': c, 'sel': c // 16 % 2, 'hr': HR[c % 16], 'cnt0': rand(c, 1), 'cnt1': rand(c, 2)}


def fx_depth(c):
    return {'case': c, 'cur': SURFACE[c % 4], 'arg': SURFACE[c // 4 % 4], 'mut': c // 16 % 3, 'hr': HR[c // 48 % 16]}


def fx_pixel(c):
    return {'case': c, 'func': 0 if c % 7 == 0 else rand(c, 1), 'token': rand(c, 2), 'mode': c // 16 % 3, 'slot': c // 48 % 4, 'hr': HR[c % 16]}


def fx_std(c, rgba):
    f = {'case': c, 'eng': ENG[c // 24 % 2], 'ret': RET8[c % 8], 'wmode': c // 8 % 3, 'token': rand(c, 3)}
    if rgba:
        f.update(format=special(c, 2), rgba=rand(c, 4))
    else:
        f.update(raster=rand(c, 1), mode=special(c, 2))
    return f


def fx_free(c):
    return {'case': c, 'a': [special(c, 1 + i) for i in range(4)] + [rand(c, 5), rand(c, 6)], 'ret': rand(c, 7)}


def fx_devreg(c):
    return {'case': c, 'ret': special(c, 1)}


def fx_frame(c):
    sel = c // 8 % 5
    parent = [0, 1, 0xFFFFFFFF, 0x80000000, 0x12345678][sel]
    return {'case': c, 'topo': c % 8, 'parent': parent, 'junk': [rand(c, 40 + i) for i in range(6)]}


def fixture(fn, c):
    if fn in (MIP, AUTO):
        return fx_tex(c, MIP)
    if fn == GEN:
        return fx_tex(c, GEN)
    if fn == RASCTX:
        return fx_ras(c)
    if fn in (TEXREG, DICTREG, RASREG):
        return fx_plugin(c, False)
    if fn == STREAM:
        return fx_plugin(c, True)
    if fn == DYN:
        return fx_dyn(c)
    if fn in (UNLOCK, DELVS, DELPS):
        return fx_obj(c)
    if fn == DEPTH:
        return fx_depth(c)
    if fn == CREATEPS:
        return fx_pixel(c)
    if fn == LOCKPAL:
        return fx_std(c, False)
    if fn == RGBA:
        return fx_std(c, True)
    if fn in (FREELIST, FREEPRE):
        return fx_free(c)
    if fn == DEVREG:
        return fx_devreg(c)
    return fx_frame(c)


def dyn_node(f, i):
    return DYN_BASE + DYN_STRIDE * ((i * f['mul']) % 7)


def input_extras(fn, c, f, base):
    """Claves que anade el banco al `input` de la escena: ventana, globales de entrada, semillas y los seis argumentos de pila."""
    lo, n = WINDOW[fn]
    eng = {MIP: f.get('eng'), AUTO: f.get('eng'), GEN: f.get('eng'), RASCTX: f.get('eng'), LOCKPAL: f.get('eng'), RGBA: f.get('eng')}.get(fn, 0x10)
    g = {'eng': eng, 'tex': (0x100 - eng) & M, 'ras': (0x140 - eng) & M, 'dyn': 0, 'ds': rand(c, 950), 'dev': 0xE0}
    a = [JUNK] * 6
    if fn in (MIP, AUTO, GEN):
        g['tex'] = (f['tex_base'] - eng) & M
    if fn == GEN:
        a[0], a[1] = f['raster'], f['image']
    if fn == RASCTX:
        g['ras'] = (f['ras_base'] - eng) & M
    if fn in (TEXREG, DICTREG, RASREG):
        a[:5] = [f['size'], f['id'], f['f1'], f['f2'], f['f3']]
    if fn == STREAM:
        a[:4] = [f['id'], f['f1'], f['f2'], f['f3']]
    if fn == DYN:
        g['dyn'] = dyn_node(f, 0) if f['n'] else 0
        a[0] = f['arg']
    if fn in (UNLOCK, DELVS, DELPS):
        a[0] = (base + (0x120 if f['sel'] else 0x100)) & M
    if fn == DEPTH:
        g['dev'], g['ds'] = 0x100, f['cur']
        a[0] = f['arg']
    if fn == CREATEPS:
        g['dev'] = 0x100
        a[0], a[1] = f['func'], (base + 0x120 + 4 * f['slot']) & M
    if fn == LOCKPAL:
        a[0], a[1] = f['raster'], f['mode']
    if fn == RGBA:
        a[0], a[1] = (base + 0x100) & M, f['format']
    if fn in (FREELIST, FREEPRE):
        a[:6] = f['a']
    if fn == FRAME:
        a[0] = (base + FRAME_OBJ) & M
    return {'window_lo': lo, 'window_len': n, 'globals_in': g, 'seeds': seeds(c), 'args': a}


# ---------------------------------------------------------------- oraculo de resultados
class Mem:
    def __init__(self, base, lo, hexs):
        self.base, self.lo, self.b = base, lo, bytearray(bytes.fromhex(hexs))

    def idx(self, off):
        i = off - self.lo
        if i < 0 or i + 4 > len(self.b):
            raise RuntimeError('acceso fuera de la ventana: %#x' % off)
        return i

    def rd(self, off):
        return int.from_bytes(self.b[self.idx(off):self.idx(off) + 4], 'little')

    def wr(self, off, v):
        self.b[self.idx(off):self.idx(off) + 4] = (v & M).to_bytes(4, 'little')

    def rd_abs(self, a):
        return self.rd((a - self.base) & M)

    def wr_abs(self, a, v):
        self.wr((a - self.base) & M, v)


def enc(base, v):
    return (v - base) & M if base <= v < base + ARENA else v


def signed(v):
    return v - (1 << 32) if v & 0x80000000 else v


def expect(fn, f, before_hex, base):
    """Devuelve dict(eax, after_hex, trace, globals_after) esperado."""
    lo, n = WINDOW[fn]
    mem = Mem(base, lo, before_hex)
    ex = input_extras(fn, f['case'], f, base)
    g = dict(ex['globals_in'])
    trace, eax = [], None
    if fn == MIP:
        eax = mem.rd(f['tex_base'] + 0x1C)
    elif fn == AUTO:
        eax = mem.rd(f['tex_base'] + 0x20)
    elif fn in (TEXREG, DICTREG, RASREG):
        reg = {TEXREG: REG_TEXTURE, DICTREG: REG_TEXDICT, RASREG: REG_RASTER}[fn]
        trace = [1, reg, f['size'], f['id'], enc(base, f['f1']), enc(base, f['f2']), enc(base, f['f3'])]
        eax = f['ret']
    elif fn == STREAM:
        trace = [2, REG_TEXTURE, f['id'], enc(base, f['f1']), enc(base, f['f2']), enc(base, f['f3'])]
        eax = f['ret']
    elif fn == GEN:
        trace = [10, enc(base, f['raster']), enc(base, f['image'])]
        eax = 1 if f['rv'] != 0 else 0
    elif fn == DYN:
        for i in range(f['n']):
            if VBPOOL[f['vb_sel'][i]] == f['arg']:
                node = dyn_node(f, i)
                mem.wr(node + 4, 0)
                mem.wr(node + 0xC, 0)
                break
    elif fn in (UNLOCK, DELVS, DELPS):
        obj = 0x120 if f['sel'] else 0x100
        trace = [20 if fn == UNLOCK else 21, obj]
        mem.wr(obj + 4, mem.rd(obj + 4) + (1 if fn == UNLOCK else -1))
        if fn == UNLOCK:
            eax = 1 if signed(f['hr']) >= 0 else 0
    elif fn == DEPTH:
        if f['cur'] != f['arg']:
            trace = [22, 0x100, enc(base, f['arg']), f['arg']]
            mem.wr(0x104, mem.rd(0x104) + 1)
            g['ds'] = 0xBAD0BAD0 if f['mut'] == 1 else f['arg']
            if f['mut'] == 2:
                g['dev'] = 0
    elif fn == DEVREG:
        trace = [4]
        eax = f['ret']
    elif fn == CREATEPS:
        slot = 0x120 + 4 * f['slot']
        trace = [23, 0x100, enc(base, f['func']), slot]
        mem.wr(0x104, mem.rd(0x104) + 1)
        if f['mode'] == 1 or (f['mode'] == 0 and signed(f['hr']) >= 0):
            mem.wr(slot, f['token'])
        eax = 1 if signed(f['hr']) >= 0 else 0
    elif fn == RASCTX:
        eax = mem.rd(f['ras_base'] + 4 * f['sp'])
    elif fn == LOCKPAL:
        trace = [30, enc(base, f['raster']), f['mode'], f['mode']]   # la ranura de salida arranca con el valor de lockMode
        wrote = f['wmode'] == 0 or (f['wmode'] == 2 and f['ret'] != 0)
        eax = 0 if f['ret'] == 0 else (f['token'] if wrote else f['mode'])
    elif fn == RGBA:
        trace = [31, 0x100, f['format'], f['format'], f['rgba']]
        wrote = f['wmode'] == 0 or (f['wmode'] == 2 and f['ret'] != 0)
        eax = (((f['rgba'] ^ f['format']) + f['token']) & M) if wrote else f['format']
    elif fn == FREELIST:
        a = f['a']
        trace = [3, a[0], a[1], a[2], 1, 0, a[3]]
        eax = f['ret']
    elif fn == FREEPRE:
        a = f['a']
        trace = [3, a[0], a[1], a[2], a[3], enc(base, a[4]), a[5]]
        eax = f['ret']
    elif fn == FRAME:
        obj = FRAME_OBJ
        if mem.rd(obj + 4) != 0:
            nxt, prv = mem.rd(obj + 8), mem.rd(obj + 0xC)
            mem.wr_abs(prv, nxt)
            nxt2, prv2 = mem.rd(obj + 8), mem.rd(obj + 0xC)
            mem.wr_abs(nxt2 + 4, prv2)
    return {'eax': eax, 'after': mem.b.hex(), 'trace': trace, 'globals': g}


def globals_json(g):
    return {'eng': g['eng'], 'tex': g['tex'], 'ras': g['ras'], 'dyn': g['dyn'], 'ds': g['ds'], 'dev': g['dev']}


def coverage_declared():
    """Cobertura de las fixtures (contada aqui, sin el banco): clases de estres por funcion."""
    cov = {}

    def count(key, cond, rng=range(CASES)):
        cov[key] = sum(1 for c in rng if cond(c))
    count('gen_rv_zero', lambda c: fx_tex(c, GEN)['rv'] == 0)
    count('gen_rv_nonzero', lambda c: fx_tex(c, GEN)['rv'] != 0)
    count('ras_sp_in_stack', lambda c: 0 <= fx_ras(c)['sp'] < 10)
    count('ras_sp_out_of_stack', lambda c: not 0 <= fx_ras(c)['sp'] < 10)
    count('dyn_empty', lambda c: fx_dyn(c)['n'] == 0)
    count('dyn_match', lambda c: any(VBPOOL[fx_dyn(c)['vb_sel'][i]] == fx_dyn(c)['arg'] for i in range(fx_dyn(c)['n'])))
    count('dyn_no_match', lambda c: fx_dyn(c)['n'] > 0 and not any(VBPOOL[fx_dyn(c)['vb_sel'][i]] == fx_dyn(c)['arg'] for i in range(fx_dyn(c)['n'])))
    count('dyn_duplicate_vb', lambda c: sum(1 for i in range(fx_dyn(c)['n']) if VBPOOL[fx_dyn(c)['vb_sel'][i]] == fx_dyn(c)['arg']) > 1)
    count('dyn_null_vb_match', lambda c: fx_dyn(c)['arg'] == 0 and any(VBPOOL[fx_dyn(c)['vb_sel'][i]] == 0 for i in range(fx_dyn(c)['n'])))
    count('hr_negative', lambda c: signed(fx_obj(c)['hr']) < 0)
    count('hr_nonnegative', lambda c: signed(fx_obj(c)['hr']) >= 0)
    count('depth_equal', lambda c: fx_depth(c)['cur'] == fx_depth(c)['arg'])
    count('depth_call', lambda c: fx_depth(c)['cur'] != fx_depth(c)['arg'])
    count('depth_mut1_call', lambda c: fx_depth(c)['cur'] != fx_depth(c)['arg'] and fx_depth(c)['mut'] == 1)
    count('depth_mut2_call', lambda c: fx_depth(c)['cur'] != fx_depth(c)['arg'] and fx_depth(c)['mut'] == 2)
    for m in range(3):
        count('pixel_mode%d' % m, lambda c, m=m: fx_pixel(c)['mode'] == m)
    count('pixel_null_func', lambda c: fx_pixel(c)['func'] == 0)
    count('pixel_hr_negative', lambda c: signed(fx_pixel(c)['hr']) < 0)
    count('lock_ret_zero', lambda c: fx_std(c, False)['ret'] == 0)
    count('lock_wrote', lambda c: fx_std(c, False)['ret'] != 0 and (fx_std(c, False)['wmode'] == 0 or fx_std(c, False)['wmode'] == 2))
    count('lock_not_wrote_nonzero', lambda c: fx_std(c, False)['ret'] != 0 and fx_std(c, False)['wmode'] == 1)
    count('rgba_wrote', lambda c: fx_std(c, True)['wmode'] == 0 or (fx_std(c, True)['wmode'] == 2 and fx_std(c, True)['ret'] != 0))
    count('rgba_not_wrote', lambda c: not (fx_std(c, True)['wmode'] == 0 or (fx_std(c, True)['wmode'] == 2 and fx_std(c, True)['ret'] != 0)))
    for t in range(8):
        count('frame_topo%d_parent' % t, lambda c, t=t: fx_frame(c)['topo'] == t and fx_frame(c)['parent'] != 0)
    count('frame_no_parent', lambda c: fx_frame(c)['parent'] == 0)
    count('plugin_ret_big', lambda c: fx_plugin(c, False)['ret'] & 0x80000000 != 0)
    return cov


if __name__ == '__main__':
    print(json.dumps(coverage_declared(), indent=1))
    if len(sys.argv) > 1 and sys.argv[1] == '--dump':
        out = {}
        for fn in range(20):
            out[NAMES[fn]] = [dict(fixture(fn, c), **{k: v for k, v in input_extras(fn, c, fixture(fn, c), 0x10000000).items() if k in ('window_lo', 'window_len', 'seeds')}) for c in range(CASES)]
        json.dump(out, open(sys.argv[2], 'w'))
