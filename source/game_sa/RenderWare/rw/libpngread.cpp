// Lote LIB2A: funciones de lectura de libpng 1.0.12 del EXE (0x7D0000..0x7E9400), sustituidas por la biblioteca oficial (libs/libpng-1.0.12) detras de la opcion CMake
// GTASA_WITH_ZLIB_LIBPNG (APAGADA por defecto: con la opcion apagada este archivo no engancha nada y el EXE original sigue ejecutando su propia libpng).
// Cada cuerpo solo reenvia a la biblioteca; el banco par a par (reversing/FnLIB2A, EXE original vs biblioteca, FPU a 24 bits, /arch:IA32) comprobo que cada una se comporta igual que la del EXE.
// Archivo GENERADO por reversing/FnLIB2A/gen_hooks.py desde reversing/FnLIB2A/enganchables.txt; no editar a mano.
#include "StdInc.h"

#include "libpngread.h"

#ifdef GTASA_WITH_ZLIB_LIBPNG
extern "C" {
    uint32 png_get_valid(void* png_ptr, void* info_ptr, uint32 flag);
    uint32 png_get_rowbytes(void* png_ptr, void* info_ptr);
    void* png_get_rows(void* png_ptr, void* info_ptr);
    uint32 png_get_image_width(void* png_ptr, void* info_ptr);
    uint32 png_get_image_height(void* png_ptr, void* info_ptr);
    uint8 png_get_bit_depth(void* png_ptr, void* info_ptr);
    uint8 png_get_color_type(void* png_ptr, void* info_ptr);
    uint8 png_get_filter_type(void* png_ptr, void* info_ptr);
    uint8 png_get_interlace_type(void* png_ptr, void* info_ptr);
    uint8 png_get_compression_type(void* png_ptr, void* info_ptr);
    uint32 png_get_x_pixels_per_meter(void* png_ptr, void* info_ptr);
    uint32 png_get_y_pixels_per_meter(void* png_ptr, void* info_ptr);
    uint32 png_get_pixels_per_meter(void* png_ptr, void* info_ptr);
    int32 png_get_x_offset_microns(void* png_ptr, void* info_ptr);
    int32 png_get_y_offset_microns(void* png_ptr, void* info_ptr);
    int32 png_get_x_offset_pixels(void* png_ptr, void* info_ptr);
    int32 png_get_y_offset_pixels(void* png_ptr, void* info_ptr);
    uint8 png_get_channels(void* png_ptr, void* info_ptr);
    void* png_get_signature(void* png_ptr, void* info_ptr);
    uint32 png_get_bKGD(void* png_ptr, void* info_ptr, void* background);
    uint32 png_get_cHRM(void* png_ptr, void* info_ptr, void* white_x, void* white_y, void* red_x, void* red_y, void* green_x, void* green_y, void* blue_x, void* blue_y);
    uint32 png_get_cHRM_fixed(void* png_ptr, void* info_ptr, void* int_white_x, void* int_white_y, void* int_red_x, void* int_red_y, void* int_green_x, void* int_green_y, void* int_blue_x, void* int_blue_y);
    uint32 png_get_gAMA(void* png_ptr, void* info_ptr, void* file_gamma);
    uint32 png_get_gAMA_fixed(void* png_ptr, void* info_ptr, void* int_file_gamma);
    uint32 png_get_sRGB(void* png_ptr, void* info_ptr, void* intent);
    uint32 png_get_sPLT(void* png_ptr, void* info_ptr, void* entries);
    uint32 png_get_hIST(void* png_ptr, void* info_ptr, void* hist);
    uint32 png_get_IHDR(void* png_ptr, void* info_ptr, void* width, void* height, void* bit_depth, void* color_type, void* interlace_method, void* compression_method, void* filter_method);
    uint32 png_get_oFFs(void* png_ptr, void* info_ptr, void* offset_x, void* offset_y, void* unit_type);
    uint32 png_get_pCAL(void* png_ptr, void* info_ptr, void* purpose, void* X0, void* X1, void* type, void* nparams, void* units, void* params);
    uint32 png_get_sCAL(void* png_ptr, void* info_ptr, void* unit, void* width, void* height);
    uint32 png_get_pHYs(void* png_ptr, void* info_ptr, void* res_x, void* res_y, void* unit_type);
    uint32 png_get_PLTE(void* png_ptr, void* info_ptr, void* palette, void* num_palette);
    uint32 png_get_sBIT(void* png_ptr, void* info_ptr, void* sig_bit);
    uint32 png_get_text(void* png_ptr, void* info_ptr, void* text_ptr, void* num_text);
    uint32 png_get_tIME(void* png_ptr, void* info_ptr, void* mod_time);
    uint32 png_get_tRNS(void* png_ptr, void* info_ptr, void* trans, void* num_trans, void* trans_values);
    uint32 png_get_unknown_chunks(void* png_ptr, void* info_ptr, void* entries);
    uint32 png_get_compression_buffer_size(void* png_ptr);
    int32 png_sig_cmp(void* sig, uint32 start, uint32 num_to_check);
    void png_set_sig_bytes(void* png_ptr, int32 num_bytes);
    void png_set_bKGD(void* png_ptr, void* info_ptr, void* background);
    void png_set_cHRM(void* png_ptr, void* info_ptr, double white_x, double white_y, double red_x, double red_y, double green_x, double green_y, double blue_x, double blue_y);
    void png_set_cHRM_fixed(void* png_ptr, void* info_ptr, int32 int_white_x, int32 int_white_y, int32 int_red_x, int32 int_red_y, int32 int_green_x, int32 int_green_y, int32 int_blue_x, int32 int_blue_y);
    void png_set_gAMA(void* png_ptr, void* info_ptr, double file_gamma);
    void png_set_gAMA_fixed(void* png_ptr, void* info_ptr, int32 int_file_gamma);
    void png_set_hIST(void* png_ptr, void* info_ptr, void* hist);
    void png_set_IHDR(void* png_ptr, void* info_ptr, uint32 width, uint32 height, int32 bit_depth, int32 color_type, int32 interlace_method, int32 compression_method, int32 filter_method);
    void png_set_oFFs(void* png_ptr, void* info_ptr, int32 offset_x, int32 offset_y, int32 unit_type);
    void png_set_pCAL(void* png_ptr, void* info_ptr, void* purpose, int32 X0, int32 X1, int32 type, int32 nparams, void* units, void* params);
    void png_set_sCAL(void* png_ptr, void* info_ptr, int32 unit, double width, double height);
    void png_set_pHYs(void* png_ptr, void* info_ptr, uint32 res_x, uint32 res_y, int32 unit_type);
    void png_set_PLTE(void* png_ptr, void* info_ptr, void* palette, int32 num_palette);
    void png_set_sBIT(void* png_ptr, void* info_ptr, void* sig_bit);
    void png_set_sRGB(void* png_ptr, void* info_ptr, int32 intent);
    void png_set_sRGB_gAMA_and_cHRM(void* png_ptr, void* info_ptr, int32 intent);
    void png_set_iCCP(void* png_ptr, void* info_ptr, void* name, int32 compression_type, void* profile, uint32 proflen);
    void png_set_text(void* png_ptr, void* info_ptr, void* text_ptr, int32 num_text);
    void png_set_tIME(void* png_ptr, void* info_ptr, void* mod_time);
    void png_set_tRNS(void* png_ptr, void* info_ptr, void* trans, int32 num_trans, void* trans_values);
    void png_set_unknown_chunks(void* png_ptr, void* info_ptr, void* unknowns, int32 num_unknowns);
    void png_set_unknown_chunk_location(void* png_ptr, void* info_ptr, int32 chunk, int32 location);
    void png_set_keep_unknown_chunks(void* png_ptr, int32 keep, void* chunk_list, int32 num_chunks);
    void png_set_read_user_chunk_fn(void* png_ptr, void* user_chunk_ptr, uint32 read_user_chunk_fn);
    void png_set_rows(void* png_ptr, void* info_ptr, void* row_pointers);
    void png_set_compression_buffer_size(void* png_ptr, uint32 size);
    void png_set_invalid(void* png_ptr, void* info_ptr, int32 mask);
    void* png_create_info_struct(void* png_ptr);
    void png_destroy_info_struct(void* png_ptr, void* info_ptr_ptr);
    void png_info_destroy(void* png_ptr, void* info_ptr);
    void png_free_data(void* png_ptr, void* info_ptr, uint32 free_me, int32 num);
    uint32 png_zalloc(uint32 png_ptr, uint32 items, uint32 size);
    void png_zfree(uint32 png_ptr, uint32 ptr);
    void png_reset_crc(void* png_ptr);
    void png_calculate_crc(void* png_ptr, void* ptr, uint32 length);
    int32 png_handle_as_unknown(void* png_ptr, void* chunk_name);
    void* png_get_copyright(void* png_ptr);
    void* png_get_header_ver(void* png_ptr);
    void* png_get_header_version(void* png_ptr);
    void* png_get_io_ptr(void* png_ptr);
    int32 png_reset_zstream(void* png_ptr);
    void* png_malloc(void* png_ptr, uint32 size);
    void png_free(void* png_ptr, void* ptr);
    void* png_memcpy_check(void* png_ptr, void* s1, void* s2, uint32 size);
    void* png_memset_check(void* png_ptr, void* s1, int32 value, uint32 size);
    void* png_create_struct_2(int32 type, uint32 malloc_fn, void* mem_ptr);
    void png_destroy_struct_2(void* struct_ptr, uint32 free_fn, void* mem_ptr);
    void png_read_data(void* png_ptr, void* data, uint32 length);
    void png_set_read_fn(void* png_ptr, void* io_ptr, uint32 read_data_fn);
    void png_crc_read(void* png_ptr, void* buf, uint32 length);
    int32 png_crc_finish(void* png_ptr, uint32 skip);
    int32 png_crc_error(void* png_ptr);
    void png_check_chunk_name(void* png_ptr, void* chunk_name);
    void png_read_filter_row(void* png_ptr, void* row_info, void* row, void* prev_row, int32 filter);
    void png_read_finish_row(void* png_ptr);
    void png_set_read_status_fn(void* png_ptr, uint32 read_row_fn);
    void png_set_expand(void* png_ptr);
    void png_set_gray_1_2_4_to_8(void* png_ptr);
    void png_set_palette_to_rgb(void* png_ptr);
    void png_set_crc_action(void* png_ptr, int32 crit_action, int32 ancil_action);
    void png_set_rgb_to_gray_fixed(void* png_ptr, int32 error_action, int32 red, int32 green);
    void png_set_read_user_transform_fn(void* png_ptr, uint32 read_user_transform_fn);
    void png_do_gray_to_rgb(void* row_info, void* row);
    void png_do_read_transformations(void* png_ptr);
}

namespace {
uint32 __cdecl LpRd_png_get_valid(void* png_ptr, void* info_ptr, uint32 flag) { return ::png_get_valid(png_ptr, info_ptr, flag); }
uint32 __cdecl LpRd_png_get_rowbytes(void* png_ptr, void* info_ptr) { return ::png_get_rowbytes(png_ptr, info_ptr); }
void* __cdecl LpRd_png_get_rows(void* png_ptr, void* info_ptr) { return ::png_get_rows(png_ptr, info_ptr); }
uint32 __cdecl LpRd_png_get_image_width(void* png_ptr, void* info_ptr) { return ::png_get_image_width(png_ptr, info_ptr); }
uint32 __cdecl LpRd_png_get_image_height(void* png_ptr, void* info_ptr) { return ::png_get_image_height(png_ptr, info_ptr); }
uint8 __cdecl LpRd_png_get_bit_depth(void* png_ptr, void* info_ptr) { return ::png_get_bit_depth(png_ptr, info_ptr); }
uint8 __cdecl LpRd_png_get_color_type(void* png_ptr, void* info_ptr) { return ::png_get_color_type(png_ptr, info_ptr); }
uint8 __cdecl LpRd_png_get_filter_type(void* png_ptr, void* info_ptr) { return ::png_get_filter_type(png_ptr, info_ptr); }
uint8 __cdecl LpRd_png_get_interlace_type(void* png_ptr, void* info_ptr) { return ::png_get_interlace_type(png_ptr, info_ptr); }
uint8 __cdecl LpRd_png_get_compression_type(void* png_ptr, void* info_ptr) { return ::png_get_compression_type(png_ptr, info_ptr); }
uint32 __cdecl LpRd_png_get_x_pixels_per_meter(void* png_ptr, void* info_ptr) { return ::png_get_x_pixels_per_meter(png_ptr, info_ptr); }
uint32 __cdecl LpRd_png_get_y_pixels_per_meter(void* png_ptr, void* info_ptr) { return ::png_get_y_pixels_per_meter(png_ptr, info_ptr); }
uint32 __cdecl LpRd_png_get_pixels_per_meter(void* png_ptr, void* info_ptr) { return ::png_get_pixels_per_meter(png_ptr, info_ptr); }
int32 __cdecl LpRd_png_get_x_offset_microns(void* png_ptr, void* info_ptr) { return ::png_get_x_offset_microns(png_ptr, info_ptr); }
int32 __cdecl LpRd_png_get_y_offset_microns(void* png_ptr, void* info_ptr) { return ::png_get_y_offset_microns(png_ptr, info_ptr); }
int32 __cdecl LpRd_png_get_x_offset_pixels(void* png_ptr, void* info_ptr) { return ::png_get_x_offset_pixels(png_ptr, info_ptr); }
int32 __cdecl LpRd_png_get_y_offset_pixels(void* png_ptr, void* info_ptr) { return ::png_get_y_offset_pixels(png_ptr, info_ptr); }
uint8 __cdecl LpRd_png_get_channels(void* png_ptr, void* info_ptr) { return ::png_get_channels(png_ptr, info_ptr); }
void* __cdecl LpRd_png_get_signature(void* png_ptr, void* info_ptr) { return ::png_get_signature(png_ptr, info_ptr); }
uint32 __cdecl LpRd_png_get_bKGD(void* png_ptr, void* info_ptr, void* background) { return ::png_get_bKGD(png_ptr, info_ptr, background); }
uint32 __cdecl LpRd_png_get_cHRM(void* png_ptr, void* info_ptr, void* white_x, void* white_y, void* red_x, void* red_y, void* green_x, void* green_y, void* blue_x, void* blue_y) { return ::png_get_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y); }
uint32 __cdecl LpRd_png_get_cHRM_fixed(void* png_ptr, void* info_ptr, void* int_white_x, void* int_white_y, void* int_red_x, void* int_red_y, void* int_green_x, void* int_green_y, void* int_blue_x, void* int_blue_y) { return ::png_get_cHRM_fixed(png_ptr, info_ptr, int_white_x, int_white_y, int_red_x, int_red_y, int_green_x, int_green_y, int_blue_x, int_blue_y); }
uint32 __cdecl LpRd_png_get_gAMA(void* png_ptr, void* info_ptr, void* file_gamma) { return ::png_get_gAMA(png_ptr, info_ptr, file_gamma); }
uint32 __cdecl LpRd_png_get_gAMA_fixed(void* png_ptr, void* info_ptr, void* int_file_gamma) { return ::png_get_gAMA_fixed(png_ptr, info_ptr, int_file_gamma); }
uint32 __cdecl LpRd_png_get_sRGB(void* png_ptr, void* info_ptr, void* intent) { return ::png_get_sRGB(png_ptr, info_ptr, intent); }
uint32 __cdecl LpRd_png_get_sPLT(void* png_ptr, void* info_ptr, void* entries) { return ::png_get_sPLT(png_ptr, info_ptr, entries); }
uint32 __cdecl LpRd_png_get_hIST(void* png_ptr, void* info_ptr, void* hist) { return ::png_get_hIST(png_ptr, info_ptr, hist); }
uint32 __cdecl LpRd_png_get_IHDR(void* png_ptr, void* info_ptr, void* width, void* height, void* bit_depth, void* color_type, void* interlace_method, void* compression_method, void* filter_method) { return ::png_get_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, interlace_method, compression_method, filter_method); }
uint32 __cdecl LpRd_png_get_oFFs(void* png_ptr, void* info_ptr, void* offset_x, void* offset_y, void* unit_type) { return ::png_get_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type); }
uint32 __cdecl LpRd_png_get_pCAL(void* png_ptr, void* info_ptr, void* purpose, void* X0, void* X1, void* type, void* nparams, void* units, void* params) { return ::png_get_pCAL(png_ptr, info_ptr, purpose, X0, X1, type, nparams, units, params); }
uint32 __cdecl LpRd_png_get_sCAL(void* png_ptr, void* info_ptr, void* unit, void* width, void* height) { return ::png_get_sCAL(png_ptr, info_ptr, unit, width, height); }
uint32 __cdecl LpRd_png_get_pHYs(void* png_ptr, void* info_ptr, void* res_x, void* res_y, void* unit_type) { return ::png_get_pHYs(png_ptr, info_ptr, res_x, res_y, unit_type); }
uint32 __cdecl LpRd_png_get_PLTE(void* png_ptr, void* info_ptr, void* palette, void* num_palette) { return ::png_get_PLTE(png_ptr, info_ptr, palette, num_palette); }
uint32 __cdecl LpRd_png_get_sBIT(void* png_ptr, void* info_ptr, void* sig_bit) { return ::png_get_sBIT(png_ptr, info_ptr, sig_bit); }
uint32 __cdecl LpRd_png_get_text(void* png_ptr, void* info_ptr, void* text_ptr, void* num_text) { return ::png_get_text(png_ptr, info_ptr, text_ptr, num_text); }
uint32 __cdecl LpRd_png_get_tIME(void* png_ptr, void* info_ptr, void* mod_time) { return ::png_get_tIME(png_ptr, info_ptr, mod_time); }
uint32 __cdecl LpRd_png_get_tRNS(void* png_ptr, void* info_ptr, void* trans, void* num_trans, void* trans_values) { return ::png_get_tRNS(png_ptr, info_ptr, trans, num_trans, trans_values); }
uint32 __cdecl LpRd_png_get_unknown_chunks(void* png_ptr, void* info_ptr, void* entries) { return ::png_get_unknown_chunks(png_ptr, info_ptr, entries); }
uint32 __cdecl LpRd_png_get_compression_buffer_size(void* png_ptr) { return ::png_get_compression_buffer_size(png_ptr); }
int32 __cdecl LpRd_png_sig_cmp(void* sig, uint32 start, uint32 num_to_check) { return ::png_sig_cmp(sig, start, num_to_check); }
void __cdecl LpRd_png_set_sig_bytes(void* png_ptr, int32 num_bytes) { ::png_set_sig_bytes(png_ptr, num_bytes); }
void __cdecl LpRd_png_set_bKGD(void* png_ptr, void* info_ptr, void* background) { ::png_set_bKGD(png_ptr, info_ptr, background); }
void __cdecl LpRd_png_set_cHRM(void* png_ptr, void* info_ptr, double white_x, double white_y, double red_x, double red_y, double green_x, double green_y, double blue_x, double blue_y) { ::png_set_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y); }
void __cdecl LpRd_png_set_cHRM_fixed(void* png_ptr, void* info_ptr, int32 int_white_x, int32 int_white_y, int32 int_red_x, int32 int_red_y, int32 int_green_x, int32 int_green_y, int32 int_blue_x, int32 int_blue_y) { ::png_set_cHRM_fixed(png_ptr, info_ptr, int_white_x, int_white_y, int_red_x, int_red_y, int_green_x, int_green_y, int_blue_x, int_blue_y); }
void __cdecl LpRd_png_set_gAMA(void* png_ptr, void* info_ptr, double file_gamma) { ::png_set_gAMA(png_ptr, info_ptr, file_gamma); }
void __cdecl LpRd_png_set_gAMA_fixed(void* png_ptr, void* info_ptr, int32 int_file_gamma) { ::png_set_gAMA_fixed(png_ptr, info_ptr, int_file_gamma); }
void __cdecl LpRd_png_set_hIST(void* png_ptr, void* info_ptr, void* hist) { ::png_set_hIST(png_ptr, info_ptr, hist); }
void __cdecl LpRd_png_set_IHDR(void* png_ptr, void* info_ptr, uint32 width, uint32 height, int32 bit_depth, int32 color_type, int32 interlace_method, int32 compression_method, int32 filter_method) { ::png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, interlace_method, compression_method, filter_method); }
void __cdecl LpRd_png_set_oFFs(void* png_ptr, void* info_ptr, int32 offset_x, int32 offset_y, int32 unit_type) { ::png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type); }
void __cdecl LpRd_png_set_pCAL(void* png_ptr, void* info_ptr, void* purpose, int32 X0, int32 X1, int32 type, int32 nparams, void* units, void* params) { ::png_set_pCAL(png_ptr, info_ptr, purpose, X0, X1, type, nparams, units, params); }
void __cdecl LpRd_png_set_sCAL(void* png_ptr, void* info_ptr, int32 unit, double width, double height) { ::png_set_sCAL(png_ptr, info_ptr, unit, width, height); }
void __cdecl LpRd_png_set_pHYs(void* png_ptr, void* info_ptr, uint32 res_x, uint32 res_y, int32 unit_type) { ::png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_type); }
void __cdecl LpRd_png_set_PLTE(void* png_ptr, void* info_ptr, void* palette, int32 num_palette) { ::png_set_PLTE(png_ptr, info_ptr, palette, num_palette); }
void __cdecl LpRd_png_set_sBIT(void* png_ptr, void* info_ptr, void* sig_bit) { ::png_set_sBIT(png_ptr, info_ptr, sig_bit); }
void __cdecl LpRd_png_set_sRGB(void* png_ptr, void* info_ptr, int32 intent) { ::png_set_sRGB(png_ptr, info_ptr, intent); }
void __cdecl LpRd_png_set_sRGB_gAMA_and_cHRM(void* png_ptr, void* info_ptr, int32 intent) { ::png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, intent); }
void __cdecl LpRd_png_set_iCCP(void* png_ptr, void* info_ptr, void* name, int32 compression_type, void* profile, uint32 proflen) { ::png_set_iCCP(png_ptr, info_ptr, name, compression_type, profile, proflen); }
void __cdecl LpRd_png_set_text(void* png_ptr, void* info_ptr, void* text_ptr, int32 num_text) { ::png_set_text(png_ptr, info_ptr, text_ptr, num_text); }
void __cdecl LpRd_png_set_tIME(void* png_ptr, void* info_ptr, void* mod_time) { ::png_set_tIME(png_ptr, info_ptr, mod_time); }
void __cdecl LpRd_png_set_tRNS(void* png_ptr, void* info_ptr, void* trans, int32 num_trans, void* trans_values) { ::png_set_tRNS(png_ptr, info_ptr, trans, num_trans, trans_values); }
void __cdecl LpRd_png_set_unknown_chunks(void* png_ptr, void* info_ptr, void* unknowns, int32 num_unknowns) { ::png_set_unknown_chunks(png_ptr, info_ptr, unknowns, num_unknowns); }
void __cdecl LpRd_png_set_unknown_chunk_location(void* png_ptr, void* info_ptr, int32 chunk, int32 location) { ::png_set_unknown_chunk_location(png_ptr, info_ptr, chunk, location); }
void __cdecl LpRd_png_set_keep_unknown_chunks(void* png_ptr, int32 keep, void* chunk_list, int32 num_chunks) { ::png_set_keep_unknown_chunks(png_ptr, keep, chunk_list, num_chunks); }
void __cdecl LpRd_png_set_read_user_chunk_fn(void* png_ptr, void* user_chunk_ptr, uint32 read_user_chunk_fn) { ::png_set_read_user_chunk_fn(png_ptr, user_chunk_ptr, read_user_chunk_fn); }
void __cdecl LpRd_png_set_rows(void* png_ptr, void* info_ptr, void* row_pointers) { ::png_set_rows(png_ptr, info_ptr, row_pointers); }
void __cdecl LpRd_png_set_compression_buffer_size(void* png_ptr, uint32 size) { ::png_set_compression_buffer_size(png_ptr, size); }
void __cdecl LpRd_png_set_invalid(void* png_ptr, void* info_ptr, int32 mask) { ::png_set_invalid(png_ptr, info_ptr, mask); }
void* __cdecl LpRd_png_create_info_struct(void* png_ptr) { return ::png_create_info_struct(png_ptr); }
void __cdecl LpRd_png_destroy_info_struct(void* png_ptr, void* info_ptr_ptr) { ::png_destroy_info_struct(png_ptr, info_ptr_ptr); }
void __cdecl LpRd_png_info_destroy(void* png_ptr, void* info_ptr) { ::png_info_destroy(png_ptr, info_ptr); }
void __cdecl LpRd_png_free_data(void* png_ptr, void* info_ptr, uint32 free_me, int32 num) { ::png_free_data(png_ptr, info_ptr, free_me, num); }
uint32 __cdecl LpRd_png_zalloc(uint32 png_ptr, uint32 items, uint32 size) { return ::png_zalloc(png_ptr, items, size); }
void __cdecl LpRd_png_zfree(uint32 png_ptr, uint32 ptr) { ::png_zfree(png_ptr, ptr); }
void __cdecl LpRd_png_reset_crc(void* png_ptr) { ::png_reset_crc(png_ptr); }
void __cdecl LpRd_png_calculate_crc(void* png_ptr, void* ptr, uint32 length) { ::png_calculate_crc(png_ptr, ptr, length); }
int32 __cdecl LpRd_png_handle_as_unknown(void* png_ptr, void* chunk_name) { return ::png_handle_as_unknown(png_ptr, chunk_name); }
void* __cdecl LpRd_png_get_copyright(void* png_ptr) { return ::png_get_copyright(png_ptr); }
void* __cdecl LpRd_png_get_header_ver(void* png_ptr) { return ::png_get_header_ver(png_ptr); }
void* __cdecl LpRd_png_get_header_version(void* png_ptr) { return ::png_get_header_version(png_ptr); }
void* __cdecl LpRd_png_get_io_ptr(void* png_ptr) { return ::png_get_io_ptr(png_ptr); }
int32 __cdecl LpRd_png_reset_zstream(void* png_ptr) { return ::png_reset_zstream(png_ptr); }
void* __cdecl LpRd_png_malloc(void* png_ptr, uint32 size) { return ::png_malloc(png_ptr, size); }
void __cdecl LpRd_png_free(void* png_ptr, void* ptr) { ::png_free(png_ptr, ptr); }
void* __cdecl LpRd_png_memcpy_check(void* png_ptr, void* s1, void* s2, uint32 size) { return ::png_memcpy_check(png_ptr, s1, s2, size); }
void* __cdecl LpRd_png_memset_check(void* png_ptr, void* s1, int32 value, uint32 size) { return ::png_memset_check(png_ptr, s1, value, size); }
void* __cdecl LpRd_png_create_struct_2(int32 type, uint32 malloc_fn, void* mem_ptr) { return ::png_create_struct_2(type, malloc_fn, mem_ptr); }
void __cdecl LpRd_png_destroy_struct_2(void* struct_ptr, uint32 free_fn, void* mem_ptr) { ::png_destroy_struct_2(struct_ptr, free_fn, mem_ptr); }
void __cdecl LpRd_png_read_data(void* png_ptr, void* data, uint32 length) { ::png_read_data(png_ptr, data, length); }
void __cdecl LpRd_png_set_read_fn(void* png_ptr, void* io_ptr, uint32 read_data_fn) { ::png_set_read_fn(png_ptr, io_ptr, read_data_fn); }
void __cdecl LpRd_png_crc_read(void* png_ptr, void* buf, uint32 length) { ::png_crc_read(png_ptr, buf, length); }
int32 __cdecl LpRd_png_crc_finish(void* png_ptr, uint32 skip) { return ::png_crc_finish(png_ptr, skip); }
int32 __cdecl LpRd_png_crc_error(void* png_ptr) { return ::png_crc_error(png_ptr); }
void __cdecl LpRd_png_check_chunk_name(void* png_ptr, void* chunk_name) { ::png_check_chunk_name(png_ptr, chunk_name); }
void __cdecl LpRd_png_read_filter_row(void* png_ptr, void* row_info, void* row, void* prev_row, int32 filter) { ::png_read_filter_row(png_ptr, row_info, row, prev_row, filter); }
void __cdecl LpRd_png_read_finish_row(void* png_ptr) { ::png_read_finish_row(png_ptr); }
void __cdecl LpRd_png_set_read_status_fn(void* png_ptr, uint32 read_row_fn) { ::png_set_read_status_fn(png_ptr, read_row_fn); }
void __cdecl LpRd_png_set_expand(void* png_ptr) { ::png_set_expand(png_ptr); }
void __cdecl LpRd_png_set_gray_1_2_4_to_8(void* png_ptr) { ::png_set_gray_1_2_4_to_8(png_ptr); }
void __cdecl LpRd_png_set_palette_to_rgb(void* png_ptr) { ::png_set_palette_to_rgb(png_ptr); }
void __cdecl LpRd_png_set_crc_action(void* png_ptr, int32 crit_action, int32 ancil_action) { ::png_set_crc_action(png_ptr, crit_action, ancil_action); }
void __cdecl LpRd_png_set_rgb_to_gray_fixed(void* png_ptr, int32 error_action, int32 red, int32 green) { ::png_set_rgb_to_gray_fixed(png_ptr, error_action, red, green); }
void __cdecl LpRd_png_set_read_user_transform_fn(void* png_ptr, uint32 read_user_transform_fn) { ::png_set_read_user_transform_fn(png_ptr, read_user_transform_fn); }
void __cdecl LpRd_png_do_gray_to_rgb(void* row_info, void* row) { ::png_do_gray_to_rgb(row_info, row); }
void __cdecl LpRd_png_do_read_transformations(void* png_ptr) { ::png_do_read_transformations(png_ptr); }
}

void LibPngReadInjectHooks() {
    RH_ScopedNamespaceName("LibPngRead");
    RH_ScopedCategory("Plugins");
    RH_ScopedGlobalInstall(LpRd_png_get_valid, 0x7D4B80);
    RH_ScopedGlobalInstall(LpRd_png_get_rowbytes, 0x7D4BA0);
    RH_ScopedGlobalInstall(LpRd_png_get_rows, 0x7D4BC0);
    RH_ScopedGlobalInstall(LpRd_png_get_image_width, 0x7D4BE0);
    RH_ScopedGlobalInstall(LpRd_png_get_image_height, 0x7D4C00);
    RH_ScopedGlobalInstall(LpRd_png_get_bit_depth, 0x7D4C20);
    RH_ScopedGlobalInstall(LpRd_png_get_color_type, 0x7D4C40);
    RH_ScopedGlobalInstall(LpRd_png_get_filter_type, 0x7D4C60);
    RH_ScopedGlobalInstall(LpRd_png_get_interlace_type, 0x7D4C80);
    RH_ScopedGlobalInstall(LpRd_png_get_compression_type, 0x7D4CA0);
    RH_ScopedGlobalInstall(LpRd_png_get_x_pixels_per_meter, 0x7D4CC0);
    RH_ScopedGlobalInstall(LpRd_png_get_y_pixels_per_meter, 0x7D4CF0);
    RH_ScopedGlobalInstall(LpRd_png_get_pixels_per_meter, 0x7D4D20);
    RH_ScopedGlobalInstall(LpRd_png_get_x_offset_microns, 0x7D4DA0);
    RH_ScopedGlobalInstall(LpRd_png_get_y_offset_microns, 0x7D4DD0);
    RH_ScopedGlobalInstall(LpRd_png_get_x_offset_pixels, 0x7D4E00);
    RH_ScopedGlobalInstall(LpRd_png_get_y_offset_pixels, 0x7D4E30);
    RH_ScopedGlobalInstall(LpRd_png_get_channels, 0x7D4E60);
    RH_ScopedGlobalInstall(LpRd_png_get_signature, 0x7D4E80);
    RH_ScopedGlobalInstall(LpRd_png_get_bKGD, 0x7D4EA0);
    RH_ScopedGlobalInstall(LpRd_png_get_cHRM, 0x7D4ED0);
    RH_ScopedGlobalInstall(LpRd_png_get_cHRM_fixed, 0x7D4F80);
    RH_ScopedGlobalInstall(LpRd_png_get_gAMA, 0x7D5030);
    RH_ScopedGlobalInstall(LpRd_png_get_gAMA_fixed, 0x7D5060);
    RH_ScopedGlobalInstall(LpRd_png_get_sRGB, 0x7D5090);
    RH_ScopedGlobalInstall(LpRd_png_get_sPLT, 0x7D5130);
    RH_ScopedGlobalInstall(LpRd_png_get_hIST, 0x7D5160);
    RH_ScopedGlobalInstall(LpRd_png_get_IHDR, 0x7D5190);
    RH_ScopedGlobalInstall(LpRd_png_get_oFFs, 0x7D5280);
    RH_ScopedGlobalInstall(LpRd_png_get_pCAL, 0x7D52D0);
    RH_ScopedGlobalInstall(LpRd_png_get_sCAL, 0x7D5390);
    RH_ScopedGlobalInstall(LpRd_png_get_pHYs, 0x7D53F0);
    RH_ScopedGlobalInstall(LpRd_png_get_PLTE, 0x7D5440);
    RH_ScopedGlobalInstall(LpRd_png_get_sBIT, 0x7D5480);
    RH_ScopedGlobalInstall(LpRd_png_get_text, 0x7D54B0);
    RH_ScopedGlobalInstall(LpRd_png_get_tIME, 0x7D5500);
    RH_ScopedGlobalInstall(LpRd_png_get_tRNS, 0x7D5530);
    RH_ScopedGlobalInstall(LpRd_png_get_unknown_chunks, 0x7D55B0);
    RH_ScopedGlobalInstall(LpRd_png_get_compression_buffer_size, 0x7D5620);
    RH_ScopedGlobalInstall(LpRd_png_sig_cmp, 0x7D2CA0);
    RH_ScopedGlobalInstall(LpRd_png_set_sig_bytes, 0x7D2C70);
    RH_ScopedGlobalInstall(LpRd_png_set_bKGD, 0x7D1830);
    RH_ScopedGlobalInstall(LpRd_png_set_cHRM, 0x7D1870);
    RH_ScopedGlobalInstall(LpRd_png_set_cHRM_fixed, 0x7D1AE0);
    RH_ScopedGlobalInstall(LpRd_png_set_gAMA, 0x7D1BE0);
    RH_ScopedGlobalInstall(LpRd_png_set_gAMA_fixed, 0x7D1C70);
    RH_ScopedGlobalInstall(LpRd_png_set_hIST, 0x7D1CC0);
    RH_ScopedGlobalInstall(LpRd_png_set_IHDR, 0x7D1D60);
    RH_ScopedGlobalInstall(LpRd_png_set_oFFs, 0x7D1F30);
    RH_ScopedGlobalInstall(LpRd_png_set_pCAL, 0x7D1F60);
    RH_ScopedGlobalInstall(LpRd_png_set_sCAL, 0x7D20B0);
    RH_ScopedGlobalInstall(LpRd_png_set_pHYs, 0x7D2100);
    RH_ScopedGlobalInstall(LpRd_png_set_PLTE, 0x7D2130);
    RH_ScopedGlobalInstall(LpRd_png_set_sBIT, 0x7D21C0);
    RH_ScopedGlobalInstall(LpRd_png_set_sRGB, 0x7D21F0);
    RH_ScopedGlobalInstall(LpRd_png_set_sRGB_gAMA_and_cHRM, 0x7D2220);
    RH_ScopedGlobalInstall(LpRd_png_set_iCCP, 0x7D2330);
    RH_ScopedGlobalInstall(LpRd_png_set_text, 0x7D2410);
    RH_ScopedGlobalInstall(LpRd_png_set_tIME, 0x7D25F0);
    RH_ScopedGlobalInstall(LpRd_png_set_tRNS, 0x7D2630);
    RH_ScopedGlobalInstall(LpRd_png_set_unknown_chunks, 0x7D2830);
    RH_ScopedGlobalInstall(LpRd_png_set_unknown_chunk_location, 0x7D2970);
    RH_ScopedGlobalInstall(LpRd_png_set_keep_unknown_chunks, 0x7D29E0);
    RH_ScopedGlobalInstall(LpRd_png_set_read_user_chunk_fn, 0x7D2B00);
    RH_ScopedGlobalInstall(LpRd_png_set_rows, 0x7D2B20);
    RH_ScopedGlobalInstall(LpRd_png_set_compression_buffer_size, 0x7D2B70);
    RH_ScopedGlobalInstall(LpRd_png_set_invalid, 0x7D2BC0);
    RH_ScopedGlobalInstall(LpRd_png_create_info_struct, 0x7D2E80);
    RH_ScopedGlobalInstall(LpRd_png_destroy_info_struct, 0x7D2EC0);
    RH_ScopedGlobalInstall(LpRd_png_info_destroy, 0x7D33D0);
    RH_ScopedGlobalInstall(LpRd_png_free_data, 0x7D3010);
    RH_ScopedGlobalInstall(LpRd_png_zalloc, 0x7D2DA0);
    RH_ScopedGlobalInstall(LpRd_png_zfree, 0x7D2E00);
    RH_ScopedGlobalInstall(LpRd_png_reset_crc, 0x7D2E10);
    RH_ScopedGlobalInstall(LpRd_png_calculate_crc, 0x7D2E30);
    RH_ScopedGlobalInstall(LpRd_png_handle_as_unknown, 0x7D3480);
    RH_ScopedGlobalInstall(LpRd_png_get_copyright, 0x7D3440);
    RH_ScopedGlobalInstall(LpRd_png_get_header_ver, 0x7D3450);
    RH_ScopedGlobalInstall(LpRd_png_get_header_version, 0x7D3470);
    RH_ScopedGlobalInstall(LpRd_png_get_io_ptr, 0x7D3430);
    RH_ScopedGlobalInstall(LpRd_png_reset_zstream, 0x7D34D0);
    RH_ScopedGlobalInstall(LpRd_png_malloc, 0x7DEBD0);
    RH_ScopedGlobalInstall(LpRd_png_free, 0x7DEC70);
    RH_ScopedGlobalInstall(LpRd_png_memcpy_check, 0x7DECC0);
    RH_ScopedGlobalInstall(LpRd_png_memset_check, 0x7DECF0);
    RH_ScopedGlobalInstall(LpRd_png_create_struct_2, 0x7DEAD0);
    RH_ScopedGlobalInstall(LpRd_png_destroy_struct_2, 0x7DEB80);
    RH_ScopedGlobalInstall(LpRd_png_read_data, 0x7DA850);
    RH_ScopedGlobalInstall(LpRd_png_set_read_fn, 0x7DA870);
    RH_ScopedGlobalInstall(LpRd_png_crc_read, 0x7E1A70);
    RH_ScopedGlobalInstall(LpRd_png_crc_finish, 0x7E1AA0);
    RH_ScopedGlobalInstall(LpRd_png_crc_error, 0x7E1BF0);
    RH_ScopedGlobalInstall(LpRd_png_check_chunk_name, 0x7E6810);
    RH_ScopedGlobalInstall(LpRd_png_read_filter_row, 0x7E6FC0);
    RH_ScopedGlobalInstall(LpRd_png_read_finish_row, 0x7E71A0);
    RH_ScopedGlobalInstall(LpRd_png_set_read_status_fn, 0x7D4980);
    RH_ScopedGlobalInstall(LpRd_png_set_expand, 0x7D5FE0);
    RH_ScopedGlobalInstall(LpRd_png_set_gray_1_2_4_to_8, 0x7D6000);
    RH_ScopedGlobalInstall(LpRd_png_set_palette_to_rgb, 0x7D5FF0);
    RH_ScopedGlobalInstall(LpRd_png_set_crc_action, 0x7D5640);
    RH_ScopedGlobalInstall(LpRd_png_set_rgb_to_gray_fixed, 0x7D60C0);
    RH_ScopedGlobalInstall(LpRd_png_set_read_user_transform_fn, 0x7D6180);
    RH_ScopedGlobalInstall(LpRd_png_do_gray_to_rgb, 0x7D7C00);
    RH_ScopedGlobalInstall(LpRd_png_do_read_transformations, 0x7D6FF0);
}
#else
void LibPngReadInjectHooks() {}
#endif
