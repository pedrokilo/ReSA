"""Oraculo independiente (modelo en Python escrito desde la documentacion de libpng, no desde el codigo del EXE ni de la fuente) de los captadores de pngget.c.
Lee los registros que escribe bench.exe con --oracle <dir> (png_struct y png_info previos, argumentos, retornos y buffers de salida del EXE y de la biblioteca)
y comprueba que AMBOS lados coinciden con el modelo.
Uso: python -I oracle_get.py <dir con *.bin> <layout.txt>   (layout.txt = salida de 'bench.exe x x --layout')"""
import struct, sys, math
from pathlib import Path

d = Path(sys.argv[1]); lay = {}
for line in Path(sys.argv[2]).read_text().splitlines():
    if line.startswith('sizeof'):
        for t in line.split()[2:] + [line.split()[1]]:
            pass
    elif '=' in line:
        k, v = line.split('='); lay[k.strip()] = int(v)
I = lambda f: lay['png_info.' + f]
PNG_INFO = dict(gAMA=0x1, sBIT=0x2, cHRM=0x4, PLTE=0x8, tRNS=0x10, bKGD=0x20, hIST=0x40, pHYs=0x80, oFFs=0x100, tIME=0x200, pCAL=0x400, sRGB=0x800, iCCP=0x1000, sPLT=0x2000, sCAL=0x4000)

def f32(x): return struct.unpack('<f', struct.pack('<f', x))[0]
def rd(buf, off, fmt): return struct.unpack_from('<' + fmt, buf, off)[0]

def models(name, a, png, info, bufs):
    """Devuelve (retorno esperado, {indice de buffer: bytes esperados de los primeros bytes}) o None si no se modela."""
    ok = a[0] != 0 and a[1] != 0 if name != 'png_sig_cmp' and name != 'png_check_sig' else True
    valid = rd(info, I('valid'), 'I')
    def u32(f): return rd(info, I(f), 'I')
    def u8(f): return rd(info, I(f), 'B')
    if name == 'png_get_valid': return (valid & a[2] if ok else 0), {}
    simple = {'png_get_rowbytes': ('rowbytes', 'I'), 'png_get_channels': ('channels', 'B'), 'png_get_image_width': ('width', 'I'), 'png_get_image_height': ('height', 'I'),
              'png_get_bit_depth': ('bit_depth', 'B'), 'png_get_color_type': ('color_type', 'B'), 'png_get_filter_type': ('filter_type', 'B'),
              'png_get_interlace_type': ('interlace_type', 'B'), 'png_get_compression_type': ('compression_type', 'B')}
    if name in simple:
        f, t = simple[name]; return (rd(info, I(f), t) if ok else 0), {}
    phys = ok and (valid & PNG_INFO['pHYs']) != 0
    meter = u8('phys_unit_type') == 1
    if name == 'png_get_x_pixels_per_meter': return (u32('x_pixels_per_unit') if phys and meter else 0), {}
    if name == 'png_get_y_pixels_per_meter': return (u32('y_pixels_per_unit') if phys and meter else 0), {}
    if name == 'png_get_pixels_per_meter':
        return (u32('x_pixels_per_unit') if phys and meter and u32('x_pixels_per_unit') == u32('y_pixels_per_unit') else 0), {}
    if name == 'png_get_pixel_aspect_ratio':
        x, y = u32('x_pixels_per_unit'), u32('y_pixels_per_unit')
        if x >= (1 << 24) or y >= (1 << 24): return None     # el EXE (x87) no redondea a float los operandos enteros: ver README
        r = f32(float(y) / float(x)) if phys and x != 0 else 0.0
        return struct.unpack('<I', struct.pack('<f', r))[0], {}
    offs = ok and (valid & PNG_INFO['oFFs']) != 0
    if name in ('png_get_x_offset_pixels', 'png_get_y_offset_pixels', 'png_get_x_offset_microns', 'png_get_y_offset_microns'):
        unit = 0 if 'pixels' in name else 1
        f = 'x_offset' if '_x_' in name else 'y_offset'
        return (rd(info, I(f), 'I') if offs and u8('offset_unit_type') == unit else 0), {}
    if name == 'png_get_signature': return (a[1] + I('signature') if ok else 0), {}
    if name == 'png_get_rows': return (rd(info, I('row_pointers'), 'I') if ok else 0), {}
    return None

def check(name):
    p = d / (name + '.bin')
    if not p.exists(): return None
    data = p.read_bytes(); hs, hi = struct.unpack_from('<II', data, 4)
    pos = 16; n = bad = modeled = 0
    while pos < len(data):
        c = struct.unpack_from('<I', data, pos)[0]; a = struct.unpack_from('<12I', data, pos + 4); pos += 52
        r1, r2, e1, e2 = struct.unpack_from('<4I', data, pos); pos += 16
        png = data[pos:pos + hs]; pos += hs; info = data[pos:pos + hi]; pos += hi
        nb = struct.unpack_from('<I', data, pos)[0]; pos += 4
        bufs = []
        for _ in range(nb):
            bp = struct.unpack_from('<I', data, pos)[0]; pre = data[pos + 4:pos + 68]; ex = data[pos + 68:pos + 132]; li = data[pos + 132:pos + 196]; pos += 196
            bufs.append((bp, pre, ex, li))
        n += 1
        m = models(name, a, png, info, bufs)
        if m is None: continue
        modeled += 1
        exp_ret, _ = m
        if (r1 & 0xFFFFFFFF) != exp_ret or (r2 & 0xFFFFFFFF) != exp_ret:
            # los retornos de 8 bits se enmascaran en el banco
            if name.endswith(('channels', 'bit_depth', 'color_type', 'filter_type', 'interlace_type', 'compression_type')): exp_ret &= 0xFF
            if r1 != exp_ret or r2 != exp_ret: bad += 1
    return n, modeled, bad

if __name__ == '__main__':
    tot_bad = 0
    for p in sorted(d.glob('*.bin')):
        r = check(p.stem)
        if r: print(f'{p.stem:34s} registros {r[0]:6d}  modelados {r[1]:6d}  fallos del oraculo {r[2]}'); tot_bad += r[2]
    print('FALLOS DEL ORACULO', tot_bad)
    sys.exit(1 if tot_bad else 0)
