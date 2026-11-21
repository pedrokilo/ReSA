"""Genera las envolturas de LIB4A (zlibdeflate.cpp y libpngwrite.cpp) y las filas de docs/hooks.csv a partir de confirmacion.csv (solo las funciones cuya sustitucion salio
identica y que se ejecutan en los escenarios). Uso: python gen_hooks.py   (desde reversing/FnLIB4A, con el python del proyecto)."""
import csv, re
from pathlib import Path
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
V, I, U, D, P = 'void', 'int32', 'uint32', 'double', 'void*'
S = {
 # zlib deflate
 'deflateInit_': (I, [(P,'strm'),(I,'level'),('const char*','version'),(I,'stream_size')]),
 'deflateInit2_': (I, [(P,'strm'),(I,'level'),(I,'method'),(I,'windowBits'),(I,'memLevel'),(I,'strategy'),('const char*','version'),(I,'stream_size')]),
 'deflateSetDictionary': (I, [(P,'strm'),('const uint8*','dictionary'),(U,'dictLength')]),
 'deflateReset': (I, [(P,'strm')]), 'deflateParams': (I, [(P,'strm'),(I,'level'),(I,'strategy')]), 'deflate': (I, [(P,'strm'),(I,'flush')]),
 'deflateEnd': (I, [(P,'strm')]), 'deflateCopy': (I, [(P,'dest'),(P,'source')]),
 'fill_window': (V, [(P,'s')]), 'deflate_stored': (I, [(P,'s'),(I,'flush')]), 'deflate_fast': (I, [(P,'s'),(I,'flush')]), 'deflate_slow': (I, [(P,'s'),(I,'flush')]),
 'longest_match': (U, [(P,'s'),(U,'cur_match')]),
 '_tr_init': (V, [(P,'s')]), '_tr_stored_block': (V, [(P,'s'),(P,'buf'),(U,'stored_len'),(I,'eof')]), '_tr_align': (V, [(P,'s')]),
 '_tr_flush_block': (V, [(P,'s'),(P,'buf'),(U,'stored_len'),(I,'eof')]),
 'build_tree': (V, [(P,'s'),(P,'desc')]), 'pqdownheap': (V, [(P,'s'),(P,'tree'),(I,'k')]), 'scan_tree': (V, [(P,'s'),(P,'tree'),(I,'max_code')]),
 'send_tree': (V, [(P,'s'),(P,'tree'),(I,'max_code')]), 'compress_block': (V, [(P,'s'),(P,'ltree'),(P,'dtree')]), 'bi_windup': (V, [(P,'s')]),
 'adler32': (U, [(U,'adler'),('const uint8*','buf'),(U,'len')]), 'crc32': (U, [(U,'crc'),('const uint8*','buf'),(U,'len')]),
 # libpng escritura
 'png_create_write_struct_2': (P, [('const char*','ver'),(P,'error_ptr'),(P,'error_fn'),(P,'warn_fn'),(P,'mem_ptr'),(P,'malloc_fn'),(P,'free_fn')]),
 'png_write_info_before_PLTE': (V, [(P,'png'),(P,'info')]), 'png_write_info': (V, [(P,'png'),(P,'info')]),
 'png_write_row': (V, [(P,'png'),(P,'row')]), 'png_write_rows': (V, [(P,'png'),(P,'row'),(U,'num_rows')]), 'png_write_image': (V, [(P,'png'),(P,'image')]),
 'png_write_end': (V, [(P,'png'),(P,'info')]), 'png_write_flush': (V, [(P,'png')]), 'png_set_flush': (V, [(P,'png'),(I,'nrows')]),
 'png_destroy_write_struct': (V, [(P,'png_ptr_ptr'),(P,'info_ptr_ptr')]), 'png_write_destroy': (V, [(P,'png')]),
 'png_set_filter': (V, [(P,'png'),(I,'method'),(I,'filters')]),
 'png_set_filter_heuristics': (V, [(P,'png'),(I,'heuristic_method'),(I,'num_weights'),(P,'filter_weights'),(P,'filter_costs')]),
 'png_set_compression_level': (V, [(P,'png'),(I,'level')]), 'png_set_compression_mem_level': (V, [(P,'png'),(I,'mem_level')]),
 'png_set_compression_strategy': (V, [(P,'png'),(I,'strategy')]), 'png_set_compression_window_bits': (V, [(P,'png'),(I,'window_bits')]),
 'png_set_compression_method': (V, [(P,'png'),(I,'method')]), 'png_set_write_status_fn': (V, [(P,'png'),(P,'write_row_fn')]),
 'png_set_write_user_transform_fn': (V, [(P,'png'),(P,'write_user_transform_fn')]),
 'png_write_data': (V, [(P,'png'),(P,'data'),(U,'length')]), 'png_flush': (V, [(P,'png')]),
 'png_set_write_fn': (V, [(P,'png'),(P,'io_ptr'),(P,'write_data_fn'),(P,'output_flush_fn')]),
 'png_set_bgr': (V, [(P,'png')]), 'png_set_swap': (V, [(P,'png')]), 'png_set_packing': (V, [(P,'png')]), 'png_set_packswap': (V, [(P,'png')]),
 'png_set_shift': (V, [(P,'png'),(P,'true_bits')]), 'png_set_interlace_handling': (I, [(P,'png')]), 'png_set_filler': (V, [(P,'png'),(U,'filler'),(I,'filler_loc')]),
 'png_set_swap_alpha': (V, [(P,'png')]), 'png_set_invert_alpha': (V, [(P,'png')]), 'png_set_invert_mono': (V, [(P,'png')]),
 'png_set_user_transform_info': (V, [(P,'png'),(P,'user_transform_ptr'),(I,'user_transform_depth'),(I,'user_transform_channels')]),
 'png_do_invert': (V, [(P,'row_info'),(P,'row')]), 'png_do_swap': (V, [(P,'row_info'),(P,'row')]), 'png_do_packswap': (V, [(P,'row_info'),(P,'row')]),
 'png_do_strip_filler': (V, [(P,'row_info'),(P,'row'),(U,'flags')]), 'png_do_bgr': (V, [(P,'row_info'),(P,'row')]),
 'png_set_error_fn': (V, [(P,'png'),(P,'error_ptr'),(P,'error_fn'),(P,'warning_fn')]), 'png_set_mem_fn': (V, [(P,'png'),(P,'mem_ptr'),(P,'malloc_fn'),(P,'free_fn')]),
 'png_save_uint_32': (V, [(P,'buf'),(U,'i')]), 'png_save_int_32': (V, [(P,'buf'),(I,'i')]), 'png_save_uint_16': (V, [(P,'buf'),(U,'i')]),
 'png_error': (V, [(P,'png'),('const char*','message')]), 'png_warning': (V, [(P,'png'),('const char*','message')]),
 'png_write_chunk': (V, [(P,'png'),(P,'chunk_name'),(P,'data'),(U,'length')]),
 'png_write_sig': (V, [(P,'png')]), 'png_write_IHDR': (V, [(P,'png'),(U,'width'),(U,'height'),(I,'bit_depth'),(I,'color_type'),(I,'compression_type'),(I,'filter_type'),(I,'interlace_type')]),
 'png_write_PLTE': (V, [(P,'png'),(P,'palette'),(U,'num_pal')]), 'png_write_IDAT': (V, [(P,'png'),(P,'data'),(U,'length')]), 'png_write_IEND': (V, [(P,'png')]),
 'png_write_gAMA': (V, [(P,'png'),(D,'file_gamma')]), 'png_write_sRGB': (V, [(P,'png'),(I,'srgb_intent')]),
 'png_write_iCCP': (V, [(P,'png'),(P,'name'),(I,'compression_type'),(P,'profile'),(I,'profile_len')]),
 'png_text_compress': (I, [(P,'png'),(P,'text'),(U,'text_len'),(I,'compression'),(P,'comp')]), 'png_write_compressed_data_out': (V, [(P,'png'),(P,'comp')]),
 'png_write_sPLT': (V, [(P,'png'),(P,'spalette')]), 'png_write_sBIT': (V, [(P,'png'),(P,'sbit'),(I,'color_type')]),
 'png_write_cHRM': (V, [(P,'png')] + [(D,n) for n in ('white_x','white_y','red_x','red_y','green_x','green_y','blue_x','blue_y')]),
 'png_write_tRNS': (V, [(P,'png'),(P,'trans'),(P,'tran'),(I,'num_trans'),(I,'color_type')]), 'png_write_bKGD': (V, [(P,'png'),(P,'back'),(I,'color_type')]),
 'png_write_hIST': (V, [(P,'png'),(P,'hist'),(I,'num_hist')]), 'png_check_keyword': (U, [(P,'png'),(P,'key'),(P,'new_key')]),
 'png_write_tEXt': (V, [(P,'png'),(P,'key'),(P,'text'),(U,'text_len')]), 'png_write_zTXt': (V, [(P,'png'),(P,'key'),(P,'text'),(U,'text_len'),(I,'compression')]),
 'png_write_oFFs': (V, [(P,'png'),(I,'x_offset'),(I,'y_offset'),(I,'unit_type')]),
 'png_write_pCAL': (V, [(P,'png'),(P,'purpose'),(I,'X0'),(I,'X1'),(I,'type'),(I,'nparams'),(P,'units'),(P,'params')]),
 'png_write_sCAL_s': (V, [(P,'png'),(I,'unit'),(P,'width'),(P,'height')]), 'png_write_pHYs': (V, [(P,'png'),(U,'x_pixels_per_unit'),(U,'y_pixels_per_unit'),(I,'unit_type')]),
 'png_write_start_row': (V, [(P,'png')]), 'png_write_finish_row': (V, [(P,'png')]), 'png_do_write_interlace': (V, [(P,'row_info'),(P,'row'),(I,'pass')]),
 'png_write_find_filter': (V, [(P,'png'),(P,'row_info')]), 'png_write_filtered_row': (V, [(P,'png'),(P,'filtered_row')]),
 'png_do_write_transformations': (V, [(P,'png')]), 'png_do_pack': (V, [(P,'row_info'),(P,'row'),(U,'bit_depth')]), 'png_do_shift': (V, [(P,'row_info'),(P,'row'),(P,'bit_depth')]),
 'png_do_write_swap_alpha': (V, [(P,'row_info'),(P,'row')]), 'png_do_write_invert_alpha': (V, [(P,'row_info'),(P,'row')]),
}
def camel(n):
    parts = [p for p in re.split(r'_+', n) if p]
    return ''.join(p[0].upper() + p[1:] for p in parts)
rows = [r for r in csv.DictReader(open(HERE / 'confirmacion.csv', encoding='utf8'))]
ok = [r for r in rows if r['sustitucion_en_banco'] == 'identica' and r['control_trampa'] == 'se_ejecuta']
DEF_FILES = ('deflate.c', 'trees.c', 'adler32.c', 'crc32.c')
NL = chr(10)
def emit(header_name, cls, fn_hook, desc, names):
    ext = []; wr = []; inst = []
    for r in names:
        n = r['nombre']; rt, args = S[n]
        cargs = ', '.join(f'{t} {a}' for t, a in args)
        ext.append(f'    {rt} {n}({cargs});')
        call = ', '.join(a for _, a in args)
        w = camel(n)
        wr.append(f'{rt} __cdecl {w}({cargs}) {{ {"" if rt == "void" else "return "}::{n}({call}); }} // {r["va"]} ({r["tam_exe"]} bytes)')
        inst.append(f'    RH_ScopedGlobalInstall({w}, 0x{int(r["va"], 16):X});')
    txt = f'''// Lote LIB4A: {desc}
// Archivo GENERADO por reversing/FnLIB4A/gen_hooks.py desde reversing/FnLIB4A/confirmacion.csv; no editar a mano. Ver reversing/FnLIB4A/README.md.
// Con GTASA_WITH_ZLIB_LIBPNG cada cuerpo reenvia a la biblioteca oficial (libs/); sin la opcion no se engancha nada y el EXE original sigue ejecutando su codigo.
#include "StdInc.h"

#include "{header_name}.h"

#ifdef GTASA_WITH_ZLIB_LIBPNG
extern "C" {{
{NL.join(ext)}
}}

namespace {{
{NL.join(wr)}
}}

void {fn_hook}() {{
    RH_ScopedNamespaceName("{cls}");
    RH_ScopedCategory("Plugins");
{NL.join(inst)}
}}
#else
void {fn_hook}() {{}}
#endif
'''
    d = ROOT / 'source/game_sa/RenderWare/rw'
    (d / (header_name + '.cpp')).write_text(txt, encoding='utf-8', newline=NL)
    (d / (header_name + '.h')).write_text(f'#pragma once{NL}{NL}// Lote LIB4A: {desc}{NL}void {fn_hook}();{NL}', encoding='utf-8', newline=NL)
zl = [r for r in ok if r['archivo'] in DEF_FILES]
pn = [r for r in ok if r['archivo'] not in DEF_FILES]
emit('zlibdeflate', 'ZlibDeflate', 'ZlibDeflateInjectHooks', 'compresion deflate de zlib 1.1.3 del EXE (deflate.c, trees.c) y adler32/crc32, sustituida por la biblioteca oficial (libs/zlib-1.1.3).', zl)
emit('libpngwrite', 'LibPngWrite', 'LibPngWriteInjectHooks', 'escritura de libpng 1.0.12 del EXE (pngwrite/pngwutil/pngwtran/pngwio y los ajustes de pngtrans), sustituida por la biblioteca oficial (libs/libpng-1.0.12).', pn)
hp = ROOT / 'docs/hooks.csv'
text = hp.read_text(encoding='utf-8-sig')
have = set(l.split(',')[2] for l in text.splitlines()[1:])
new = []
for cls, lst in (('ZlibDeflate', zl), ('LibPngWrite', pn)):
    for r in lst:
        a = f'0x{int(r["va"], 16):08X}'
        if a not in have: new.append(f'{cls},{camel(r["nombre"])},{a},1,0,0,S')
if new:
    with hp.open('a', encoding='utf-8', newline='') as f:
        f.write(('' if text.endswith(NL) else NL) + NL.join(new) + NL)
print(len(zl), 'de zlib,', len(pn), 'de libpng; filas nuevas en hooks.csv:', len(new))
