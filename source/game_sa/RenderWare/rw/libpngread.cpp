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
}
#else
void LibPngReadInjectHooks() {}
#endif
