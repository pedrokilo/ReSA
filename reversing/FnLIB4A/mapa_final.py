"""Mapa final EXE -> biblioteca de las funciones de deflate/trees/adler32/crc32 y de escritura de libpng, con referencias en el EXE (llamadas, saltos y punteros a funcion)."""
import csv, json, struct, collections, sys
from exe_lib import *
sizes = {int(r['va'], 16): int(r['size']) for r in csv.DictReader(open('../FnLIB2A/exe_funcs.csv'))}
m = {}   # va -> (nombre, archivo, como)
src = {}
def add(va, name, f, how): m[va] = (name, f, how)
dj = json.load(open('buscar_deflate.json'))
for a, v in dj.items():
    if v: add(int(a, 16), v[0], 'trees.c' if v[0].startswith(('_tr', 'build_', 'pq', 'scan', 'send', 'compress_b', 'bi_')) else ('adler32.c' if v[0] == 'adler32' else 'deflate.c'), 'busqueda por comportamiento (bench_deflate)')
for va, n in ((0x7DF480, 'deflateInit_'), (0x7DF4B0, 'deflateInit2_'), (0x7DF7B0, 'deflateSetDictionary'), (0x7DF8B0, 'deflateReset'), (0x7DF9C0, 'deflateParams'), (0x7DFA90, 'deflate'), (0x7DFF00, 'deflateEnd'), (0x7DFFB0, 'deflateCopy')):
    add(va, n, 'deflate.c', 'API publico de deflate (bench_deflate unit)')
add(0x7D34F0, 'png_error', 'pngerror.c', 'LIB2A (validada, sin enganchar) + bench_png')
add(0x7D3510, 'png_warning', 'pngerror.c', 'LIB2A (validada, sin enganchar) + bench_png')
add(0x7DA940, 'png_write_chunk', 'pngwutil.c', 'busqueda por comportamiento (bench_png con trozos desconocidos)')
add(0x7E7A10, '_tr_align', 'trees.c', 'busqueda por comportamiento (bench_deflate con Z_PARTIAL_FLUSH)')
add(0x7E10A0, 'crc32', 'crc32.c', 'busqueda por comportamiento (bench_png: la llaman png_calculate_crc/png_reset_crc)')
pj = {}
for f in ('buscar_png.json', 'buscar_png2.json'):
    try: pj.update(json.load(open(f)))
    except Exception: pass
fi = lambda n: 'pngwrite.c' if n in ('png_write_info', 'png_write_info_before_PLTE', 'png_write_end', 'png_write_row', 'png_write_rows', 'png_write_image', 'png_write_flush', 'png_destroy_write_struct', 'png_write_destroy', 'png_create_write_struct_2', 'png_create_write_struct', 'png_set_filter', 'png_set_filter_heuristics', 'png_set_flush', 'png_write_png') or n.startswith('png_set_compression') or n == 'png_set_write_status_fn' else ('pngwio.c' if n in ('png_write_data', 'png_flush', 'png_set_write_fn') else ('pngtrans.c' if n.startswith(('png_do_', 'png_set_')) and n not in ('png_do_write_interlace',) and n not in ('png_do_pack', 'png_do_shift', 'png_do_write_swap_alpha', 'png_do_write_invert_alpha') else ('pngwtran.c' if n.startswith('png_do_') else 'pngwutil.c')))
for a, v in pj.items():
    if v:
        n = v[0]
        if a == '7DA8C0': n = 'png_save_uint_32'
        add(int(a, 16), n, fi(n), 'busqueda por comportamiento (bench_png)')
for va, n, how in ((0x7D1310, 'png_set_bgr', 'lectura del desensamblado + bench_png (API del escenario)'), (0x7D1320, 'png_set_swap', 'idem'), (0x7D1340, 'png_set_packing', 'idem'), (0x7D1360, 'png_set_packswap', 'idem'),
                   (0x7D1380, 'png_set_shift', 'idem'), (0x7D13E0, 'png_set_filler', 'idem'), (0x7D1440, 'png_set_swap_alpha', 'idem'), (0x7D1450, 'png_set_invert_alpha', 'idem'), (0x7D1460, 'png_set_invert_mono', 'idem'),
                   (0x7D1800, 'png_set_user_transform_info', 'idem'), (0x7D11E0, 'png_set_write_user_transform_fn', 'idem'), (0x7D3670, 'png_set_error_fn', 'idem'), (0x7DED30, 'png_set_mem_fn', 'idem'),
                   (0x7D1470, 'png_do_invert', 'idem'), (0x7DEED0, 'png_do_pack', 'idem'), (0x7DA8F0, 'png_save_int_32', 'idem'), (0x7DA920, 'png_save_uint_16', 'idem')):
    if va not in m: add(va, n, fi(n) if not n.startswith('png_set_error') and n != 'png_set_mem_fn' else ('pngerror.c' if 'error' in n else 'pngmem.c'), how)
# referencias
lo, hi = 0x401000, 0x825000
code = read(lo, hi - lo)
refs = collections.defaultdict(lambda: [0, 0, 0])
tg = set(m)
for i in range(len(code) - 5):
    b = code[i]
    if b in (0xE8, 0xE9):
        rel = struct.unpack_from('<i', code, i + 1)[0]; t = (lo + i + 5 + rel) & 0xffffffff
        if t in tg: refs[t][0 if b == 0xE8 else 1] += 1
    if b in (0x68, 0xB8, 0xB9, 0xBA, 0xBB, 0xBE, 0xBF) or (b in (0xC7,) ):
        v = struct.unpack_from('<I', code, i + 1)[0]
        if v in tg and b != 0xC7: refs[v][2] += 1
data = read(0x850000, 0x8E0000 - 0x850000) if False else b''
w = csv.writer(open('mapa_final.csv', 'w', newline='', encoding='utf8'))
w.writerow(['va', 'tam_exe', 'nombre', 'archivo', 'como_se_identifico', 'llamadas', 'saltos', 'punteros'])
for va in sorted(m):
    n, f, how = m[va]
    w.writerow([f'0x{va:08X}', sizes.get(va, ''), n, f, how, refs[va][0], refs[va][1], refs[va][2]])
print(len(m), 'funciones')
