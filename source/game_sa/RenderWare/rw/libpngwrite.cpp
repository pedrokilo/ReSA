// Lote LIB4A: escritura de libpng 1.0.12 del EXE (pngwrite/pngwutil/pngwtran/pngwio y los ajustes de pngtrans), sustituida por la biblioteca oficial (libs/libpng-1.0.12).
// Archivo GENERADO por reversing/FnLIB4A/gen_hooks.py desde reversing/FnLIB4A/confirmacion.csv; no editar a mano. Ver reversing/FnLIB4A/README.md.
// Con GTASA_WITH_ZLIB_LIBPNG cada cuerpo reenvia a la biblioteca oficial (libs/); sin la opcion no se engancha nada y el EXE original sigue ejecutando su codigo.
#include "StdInc.h"

#include "libpngwrite.h"

#ifdef GTASA_WITH_ZLIB_LIBPNG
extern "C" {
    void png_write_info_before_PLTE(void* png, void* info);
    void png_write_info(void* png, void* info);
    void png_write_end(void* png, void* info);
    void* png_create_write_struct_2(const char* ver, void* error_ptr, void* error_fn, void* warn_fn, void* mem_ptr, void* malloc_fn, void* free_fn);
    void png_write_rows(void* png, void* row, uint32 num_rows);
    void png_write_image(void* png, void* image);
    void png_write_row(void* png, void* row);
    void png_set_flush(void* png, int32 nrows);
    void png_write_flush(void* png);
    void png_destroy_write_struct(void* png_ptr_ptr, void* info_ptr_ptr);
    void png_write_destroy(void* png);
    void png_set_filter(void* png, int32 method, int32 filters);
    void png_set_filter_heuristics(void* png, int32 heuristic_method, int32 num_weights, void* filter_weights, void* filter_costs);
    void png_set_compression_level(void* png, int32 level);
    void png_set_compression_mem_level(void* png, int32 mem_level);
    void png_set_compression_strategy(void* png, int32 strategy);
    void png_set_compression_window_bits(void* png, int32 window_bits);
    void png_set_compression_method(void* png, int32 method);
    void png_set_write_status_fn(void* png, void* write_row_fn);
    void png_set_write_user_transform_fn(void* png, void* write_user_transform_fn);
    void png_set_bgr(void* png);
    void png_set_swap(void* png);
    void png_set_packing(void* png);
    void png_set_packswap(void* png);
    void png_set_shift(void* png, void* true_bits);
    int32 png_set_interlace_handling(void* png);
    void png_set_filler(void* png, uint32 filler, int32 filler_loc);
    void png_set_swap_alpha(void* png);
    void png_set_invert_alpha(void* png);
    void png_set_invert_mono(void* png);
    void png_do_invert(void* row_info, void* row);
    void png_do_swap(void* row_info, void* row);
    void png_do_packswap(void* row_info, void* row);
    void png_do_strip_filler(void* row_info, void* row, uint32 flags);
    void png_do_bgr(void* row_info, void* row);
    void png_set_user_transform_info(void* png, void* user_transform_ptr, int32 user_transform_depth, int32 user_transform_channels);
    void png_write_data(void* png, void* data, uint32 length);
    void png_flush(void* png);
    void png_set_write_fn(void* png, void* io_ptr, void* write_data_fn, void* output_flush_fn);
    void png_error(void* png, const char* message);
    void png_warning(void* png, const char* message);
    void png_set_error_fn(void* png, void* error_ptr, void* error_fn, void* warning_fn);
    void png_save_uint_32(void* buf, uint32 i);
    void png_write_chunk(void* png, void* chunk_name, void* data, uint32 length);
    void png_write_sig(void* png);
    void png_write_IHDR(void* png, uint32 width, uint32 height, int32 bit_depth, int32 color_type, int32 compression_type, int32 filter_type, int32 interlace_type);
    void png_write_PLTE(void* png, void* palette, uint32 num_pal);
    void png_write_IDAT(void* png, void* data, uint32 length);
    void png_write_IEND(void* png);
    void png_write_gAMA(void* png, double file_gamma);
    void png_write_sRGB(void* png, int32 srgb_intent);
    void png_write_iCCP(void* png, void* name, int32 compression_type, void* profile, int32 profile_len);
    int32 png_text_compress(void* png, void* text, uint32 text_len, int32 compression, void* comp);
    void png_write_compressed_data_out(void* png, void* comp);
    void png_write_sPLT(void* png, void* spalette);
    void png_write_sBIT(void* png, void* sbit, int32 color_type);
    void png_write_cHRM(void* png, double white_x, double white_y, double red_x, double red_y, double green_x, double green_y, double blue_x, double blue_y);
    void png_write_tRNS(void* png, void* trans, void* tran, int32 num_trans, int32 color_type);
    void png_write_bKGD(void* png, void* back, int32 color_type);
    void png_write_hIST(void* png, void* hist, int32 num_hist);
    uint32 png_check_keyword(void* png, void* key, void* new_key);
    void png_write_tEXt(void* png, void* key, void* text, uint32 text_len);
    void png_write_zTXt(void* png, void* key, void* text, uint32 text_len, int32 compression);
    void png_write_oFFs(void* png, int32 x_offset, int32 y_offset, int32 unit_type);
    void png_write_pCAL(void* png, void* purpose, int32 X0, int32 X1, int32 type, int32 nparams, void* units, void* params);
    void png_write_sCAL_s(void* png, int32 unit, void* width, void* height);
    void png_write_pHYs(void* png, uint32 x_pixels_per_unit, uint32 y_pixels_per_unit, int32 unit_type);
    void png_write_start_row(void* png);
    void png_write_finish_row(void* png);
    void png_do_write_interlace(void* row_info, void* row, int32 pass);
    void png_write_find_filter(void* png, void* row_info);
    void png_write_filtered_row(void* png, void* filtered_row);
    void png_set_mem_fn(void* png, void* mem_ptr, void* malloc_fn, void* free_fn);
    void png_do_write_transformations(void* png);
    void png_do_pack(void* row_info, void* row, uint32 bit_depth);
    void png_do_shift(void* row_info, void* row, void* bit_depth);
    void png_do_write_swap_alpha(void* row_info, void* row);
    void png_do_write_invert_alpha(void* row_info, void* row);
}

namespace {
void __cdecl PngWriteInfoBeforePLTE(void* png, void* info) { ::png_write_info_before_PLTE(png, info); } // 0x007CFE00 (414 bytes)
void __cdecl PngWriteInfo(void* png, void* info) { ::png_write_info(png, info); } // 0x007CFFA0 (677 bytes)
void __cdecl PngWriteEnd(void* png, void* info) { ::png_write_end(png, info); } // 0x007D0250 (297 bytes)
void* __cdecl PngCreateWriteStruct2(const char* ver, void* error_ptr, void* error_fn, void* warn_fn, void* mem_ptr, void* malloc_fn, void* free_fn) { return ::png_create_write_struct_2(ver, error_ptr, error_fn, warn_fn, mem_ptr, malloc_fn, free_fn); } // 0x007D03B0 (341 bytes)
void __cdecl PngWriteRows(void* png, void* row, uint32 num_rows) { ::png_write_rows(png, row, num_rows); } // 0x007D0690 (41 bytes)
void __cdecl PngWriteImage(void* png, void* image) { ::png_write_image(png, image); } // 0x007D06C0 (85 bytes)
void __cdecl PngWriteRow(void* png, void* row) { ::png_write_row(png, row); } // 0x007D0720 (508 bytes)
void __cdecl PngSetFlush(void* png, int32 nrows) { ::png_set_flush(png, nrows); } // 0x007D0940 (25 bytes)
void __cdecl PngWriteFlush(void* png) { ::png_write_flush(png); } // 0x007D0960 (211 bytes)
void __cdecl PngDestroyWriteStruct(void* png_ptr_ptr, void* info_ptr_ptr) { ::png_destroy_write_struct(png_ptr_ptr, info_ptr_ptr); } // 0x007D0A40 (152 bytes)
void __cdecl PngWriteDestroy(void* png) { ::png_write_destroy(png); } // 0x007D0AE0 (265 bytes)
void __cdecl PngSetFilter(void* png, int32 method, int32 filters) { ::png_set_filter(png, method, filters); } // 0x007D0BF0 (479 bytes)
void __cdecl PngSetFilterHeuristics(void* png, int32 heuristic_method, int32 num_weights, void* filter_weights, void* filter_costs) { ::png_set_filter_heuristics(png, heuristic_method, num_weights, filter_weights, filter_costs); } // 0x007D0DF0 (737 bytes)
void __cdecl PngSetCompressionLevel(void* png, int32 level) { ::png_set_compression_level(png, level); } // 0x007D10E0 (24 bytes)
void __cdecl PngSetCompressionMemLevel(void* png, int32 mem_level) { ::png_set_compression_mem_level(png, mem_level); } // 0x007D1100 (24 bytes)
void __cdecl PngSetCompressionStrategy(void* png, int32 strategy) { ::png_set_compression_strategy(png, strategy); } // 0x007D1120 (24 bytes)
void __cdecl PngSetCompressionWindowBits(void* png, int32 window_bits) { ::png_set_compression_window_bits(png, window_bits); } // 0x007D1140 (82 bytes)
void __cdecl PngSetCompressionMethod(void* png, int32 method) { ::png_set_compression_method(png, method); } // 0x007D11A0 (46 bytes)
void __cdecl PngSetWriteStatusFn(void* png, void* write_row_fn) { ::png_set_write_status_fn(png, write_row_fn); } // 0x007D11D0 (15 bytes)
void __cdecl PngSetWriteUserTransformFn(void* png, void* write_user_transform_fn) { ::png_set_write_user_transform_fn(png, write_user_transform_fn); } // 0x007D11E0 (24 bytes)
void __cdecl PngSetBgr(void* png) { ::png_set_bgr(png); } // 0x007D1310 (9 bytes)
void __cdecl PngSetSwap(void* png) { ::png_set_swap(png); } // 0x007D1320 (21 bytes)
void __cdecl PngSetPacking(void* png) { ::png_set_packing(png); } // 0x007D1340 (30 bytes)
void __cdecl PngSetPackswap(void* png) { ::png_set_packswap(png); } // 0x007D1360 (21 bytes)
void __cdecl PngSetShift(void* png, void* true_bits) { ::png_set_shift(png, true_bits); } // 0x007D1380 (36 bytes)
int32 __cdecl PngSetInterlaceHandling(void* png) { return ::png_set_interlace_handling(png); } // 0x007D13B0 (35 bytes)
void __cdecl PngSetFiller(void* png, uint32 filler, int32 filler_loc) { ::png_set_filler(png, filler, filler_loc); } // 0x007D13E0 (88 bytes)
void __cdecl PngSetSwapAlpha(void* png) { ::png_set_swap_alpha(png); } // 0x007D1440 (12 bytes)
void __cdecl PngSetInvertAlpha(void* png) { ::png_set_invert_alpha(png); } // 0x007D1450 (12 bytes)
void __cdecl PngSetInvertMono(void* png) { ::png_set_invert_mono(png); } // 0x007D1460 (9 bytes)
void __cdecl PngDoInvert(void* row_info, void* row) { ::png_do_invert(row_info, row); } // 0x007D1470 (40 bytes)
void __cdecl PngDoSwap(void* row_info, void* row) { ::png_do_swap(row_info, row); } // 0x007D14A0 (47 bytes)
void __cdecl PngDoPackswap(void* row_info, void* row) { ::png_do_packswap(row_info, row); } // 0x007D14D0 (77 bytes)
void __cdecl PngDoStripFiller(void* row_info, void* row, uint32 flags) { ::png_do_strip_filler(row_info, row, flags); } // 0x007D1520 (518 bytes)
void __cdecl PngDoBgr(void* row_info, void* row) { ::png_do_bgr(row_info, row); } // 0x007D1730 (194 bytes)
void __cdecl PngSetUserTransformInfo(void* png, void* user_transform_ptr, int32 user_transform_depth, int32 user_transform_channels) { ::png_set_user_transform_info(png, user_transform_ptr, user_transform_depth, user_transform_channels); } // 0x007D1800 (26 bytes)
void __cdecl PngWriteData(void* png, void* data, uint32 length) { ::png_write_data(png, data, length); } // 0x007D2BE0 (32 bytes)
void __cdecl PngFlush(void* png) { ::png_flush(png); } // 0x007D2C00 (21 bytes)
void __cdecl PngSetWriteFn(void* png, void* io_ptr, void* write_data_fn, void* output_flush_fn) { ::png_set_write_fn(png, io_ptr, write_data_fn, output_flush_fn); } // 0x007D2C20 (70 bytes)
void __cdecl PngError(void* png, const char* message) { ::png_error(png, message); } // 0x007D34F0 (83 bytes)
void __cdecl PngWarning(void* png, const char* message) { ::png_warning(png, message); } // 0x007D3510 (51 bytes)
void __cdecl PngSetErrorFn(void* png, void* error_ptr, void* error_fn, void* warning_fn) { ::png_set_error_fn(png, error_ptr, error_fn, warning_fn); } // 0x007D3670 (26 bytes)
void __cdecl PngSaveUint32(void* buf, uint32 i) { ::png_save_uint_32(buf, i); } // 0x007DA8C0 (35 bytes)
void __cdecl PngWriteChunk(void* png, void* chunk_name, void* data, uint32 length) { ::png_write_chunk(png, chunk_name, data, length); } // 0x007DA940 (174 bytes)
void __cdecl PngWriteSig(void* png) { ::png_write_sig(png); } // 0x007DAAC0 (102 bytes)
void __cdecl PngWriteIHDR(void* png, uint32 width, uint32 height, int32 bit_depth, int32 color_type, int32 compression_type, int32 filter_type, int32 interlace_type) { ::png_write_IHDR(png, width, height, bit_depth, color_type, compression_type, filter_type, interlace_type); } // 0x007DAB30 (922 bytes)
void __cdecl PngWritePLTE(void* png, void* palette, uint32 num_pal) { ::png_write_PLTE(png, palette, num_pal); } // 0x007DAF10 (303 bytes)
void __cdecl PngWriteIDAT(void* png, void* data, uint32 length) { ::png_write_IDAT(png, data, length); } // 0x007DB040 (186 bytes)
void __cdecl PngWriteIEND(void* png) { ::png_write_IEND(png); } // 0x007DB100 (133 bytes)
void __cdecl PngWriteGAMA(void* png, double file_gamma) { ::png_write_gAMA(png, file_gamma); } // 0x007DB190 (247 bytes)
void __cdecl PngWriteSRGB(void* png, int32 srgb_intent) { ::png_write_sRGB(png, srgb_intent); } // 0x007DB350 (182 bytes)
void __cdecl PngWriteICCP(void* png, void* name, int32 compression_type, void* profile, int32 profile_len) { ::png_write_iCCP(png, name, compression_type, profile, profile_len); } // 0x007DB410 (348 bytes)
int32 __cdecl PngTextCompress(void* png, void* text, uint32 text_len, int32 compression, void* comp) { return ::png_text_compress(png, text, text_len, compression, comp); } // 0x007DB570 (670 bytes)
void __cdecl PngWriteCompressedDataOut(void* png, void* comp) { ::png_write_compressed_data_out(png, comp); } // 0x007DB810 (243 bytes)
void __cdecl PngWriteSPLT(void* png, void* spalette) { ::png_write_sPLT(png, spalette); } // 0x007DB910 (546 bytes)
void __cdecl PngWriteSBIT(void* png, void* sbit, int32 color_type) { ::png_write_sBIT(png, sbit, color_type); } // 0x007DBB40 (366 bytes)
void __cdecl PngWriteCHRM(void* png, double white_x, double white_y, double red_x, double red_y, double green_x, double green_y, double blue_x, double blue_y) { ::png_write_cHRM(png, white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y); } // 0x007DBCB0 (1411 bytes)
void __cdecl PngWriteTRNS(void* png, void* trans, void* tran, int32 num_trans, int32 color_type) { ::png_write_tRNS(png, trans, tran, num_trans, color_type); } // 0x007DC4F0 (611 bytes)
void __cdecl PngWriteBKGD(void* png, void* back, int32 color_type) { ::png_write_bKGD(png, back, color_type); } // 0x007DC760 (588 bytes)
void __cdecl PngWriteHIST(void* png, void* hist, int32 num_hist) { ::png_write_hIST(png, hist, num_hist); } // 0x007DC9B0 (246 bytes)
uint32 __cdecl PngCheckKeyword(void* png, void* key, void* new_key) { return ::png_check_keyword(png, key, new_key); } // 0x007DCAB0 (406 bytes)
void __cdecl PngWriteTEXt(void* png, void* key, void* text, uint32 text_len) { ::png_write_tEXt(png, key, text, text_len); } // 0x007DCC50 (306 bytes)
void __cdecl PngWriteZTXt(void* png, void* key, void* text, uint32 text_len, int32 compression) { ::png_write_zTXt(png, key, text, text_len, compression); } // 0x007DCD90 (641 bytes)
void __cdecl PngWriteOFFs(void* png, int32 x_offset, int32 y_offset, int32 unit_type) { ::png_write_oFFs(png, x_offset, y_offset, unit_type); } // 0x007DD020 (258 bytes)
void __cdecl PngWritePCAL(void* png, void* purpose, int32 X0, int32 X1, int32 type, int32 nparams, void* units, void* params) { ::png_write_pCAL(png, purpose, X0, X1, type, nparams, units, params); } // 0x007DD130 (610 bytes)
void __cdecl PngWriteSCALS(void* png, int32 unit, void* width, void* height) { ::png_write_sCAL_s(png, unit, width, height); } // 0x007DD3A0 (339 bytes)
void __cdecl PngWritePHYs(void* png, uint32 x_pixels_per_unit, uint32 y_pixels_per_unit, int32 unit_type) { ::png_write_pHYs(png, x_pixels_per_unit, y_pixels_per_unit, unit_type); } // 0x007DD500 (258 bytes)
void __cdecl PngWriteStartRow(void* png) { ::png_write_start_row(png); } // 0x007DD610 (402 bytes)
void __cdecl PngWriteFinishRow(void* png) { ::png_write_finish_row(png); } // 0x007DD7B0 (793 bytes)
void __cdecl PngDoWriteInterlace(void* row_info, void* row, int32 pass) { ::png_do_write_interlace(row_info, row, pass); } // 0x007DDAD0 (575 bytes)
void __cdecl PngWriteFindFilter(void* png, void* row_info) { ::png_write_find_filter(png, row_info); } // 0x007DDD10 (3041 bytes)
void __cdecl PngWriteFilteredRow(void* png, void* filtered_row) { ::png_write_filtered_row(png, filtered_row); } // 0x007DE900 (376 bytes)
void __cdecl PngSetMemFn(void* png, void* mem_ptr, void* malloc_fn, void* free_fn) { ::png_set_mem_fn(png, mem_ptr, malloc_fn, free_fn); } // 0x007DED30 (35 bytes)
void __cdecl PngDoWriteTransformations(void* png) { ::png_do_write_transformations(png); } // 0x007DED70 (343 bytes)
void __cdecl PngDoPack(void* row_info, void* row, uint32 bit_depth) { ::png_do_pack(row_info, row, bit_depth); } // 0x007DEED0 (272 bytes)
void __cdecl PngDoShift(void* row_info, void* row, void* bit_depth) { ::png_do_shift(row_info, row, bit_depth); } // 0x007DEFE0 (629 bytes)
void __cdecl PngDoWriteSwapAlpha(void* row_info, void* row) { ::png_do_write_swap_alpha(row_info, row); } // 0x007DF260 (257 bytes)
void __cdecl PngDoWriteInvertAlpha(void* row_info, void* row) { ::png_do_write_invert_alpha(row_info, row); } // 0x007DF370 (263 bytes)
}

void LibPngWriteInjectHooks() {
    RH_ScopedNamespaceName("LibPngWrite");
    RH_ScopedCategory("Plugins");
    RH_ScopedGlobalInstall(PngWriteInfoBeforePLTE, 0x7CFE00);
    RH_ScopedGlobalInstall(PngWriteInfo, 0x7CFFA0);
    RH_ScopedGlobalInstall(PngWriteEnd, 0x7D0250);
    RH_ScopedGlobalInstall(PngCreateWriteStruct2, 0x7D03B0);
    RH_ScopedGlobalInstall(PngWriteRows, 0x7D0690);
    RH_ScopedGlobalInstall(PngWriteImage, 0x7D06C0);
    RH_ScopedGlobalInstall(PngWriteRow, 0x7D0720);
    RH_ScopedGlobalInstall(PngSetFlush, 0x7D0940);
    RH_ScopedGlobalInstall(PngWriteFlush, 0x7D0960);
    RH_ScopedGlobalInstall(PngDestroyWriteStruct, 0x7D0A40);
    RH_ScopedGlobalInstall(PngWriteDestroy, 0x7D0AE0);
    RH_ScopedGlobalInstall(PngSetFilter, 0x7D0BF0);
    RH_ScopedGlobalInstall(PngSetFilterHeuristics, 0x7D0DF0);
    RH_ScopedGlobalInstall(PngSetCompressionLevel, 0x7D10E0);
    RH_ScopedGlobalInstall(PngSetCompressionMemLevel, 0x7D1100);
    RH_ScopedGlobalInstall(PngSetCompressionStrategy, 0x7D1120);
    RH_ScopedGlobalInstall(PngSetCompressionWindowBits, 0x7D1140);
    RH_ScopedGlobalInstall(PngSetCompressionMethod, 0x7D11A0);
    RH_ScopedGlobalInstall(PngSetWriteStatusFn, 0x7D11D0);
    RH_ScopedGlobalInstall(PngSetWriteUserTransformFn, 0x7D11E0);
    RH_ScopedGlobalInstall(PngSetBgr, 0x7D1310);
    RH_ScopedGlobalInstall(PngSetSwap, 0x7D1320);
    RH_ScopedGlobalInstall(PngSetPacking, 0x7D1340);
    RH_ScopedGlobalInstall(PngSetPackswap, 0x7D1360);
    RH_ScopedGlobalInstall(PngSetShift, 0x7D1380);
    RH_ScopedGlobalInstall(PngSetInterlaceHandling, 0x7D13B0);
    RH_ScopedGlobalInstall(PngSetFiller, 0x7D13E0);
    RH_ScopedGlobalInstall(PngSetSwapAlpha, 0x7D1440);
    RH_ScopedGlobalInstall(PngSetInvertAlpha, 0x7D1450);
    RH_ScopedGlobalInstall(PngSetInvertMono, 0x7D1460);
    RH_ScopedGlobalInstall(PngDoInvert, 0x7D1470);
    RH_ScopedGlobalInstall(PngDoSwap, 0x7D14A0);
    RH_ScopedGlobalInstall(PngDoPackswap, 0x7D14D0);
    RH_ScopedGlobalInstall(PngDoStripFiller, 0x7D1520);
    RH_ScopedGlobalInstall(PngDoBgr, 0x7D1730);
    RH_ScopedGlobalInstall(PngSetUserTransformInfo, 0x7D1800);
    RH_ScopedGlobalInstall(PngWriteData, 0x7D2BE0);
    RH_ScopedGlobalInstall(PngFlush, 0x7D2C00);
    RH_ScopedGlobalInstall(PngSetWriteFn, 0x7D2C20);
    RH_ScopedGlobalInstall(PngError, 0x7D34F0);
    RH_ScopedGlobalInstall(PngWarning, 0x7D3510);
    RH_ScopedGlobalInstall(PngSetErrorFn, 0x7D3670);
    RH_ScopedGlobalInstall(PngSaveUint32, 0x7DA8C0);
    RH_ScopedGlobalInstall(PngWriteChunk, 0x7DA940);
    RH_ScopedGlobalInstall(PngWriteSig, 0x7DAAC0);
    RH_ScopedGlobalInstall(PngWriteIHDR, 0x7DAB30);
    RH_ScopedGlobalInstall(PngWritePLTE, 0x7DAF10);
    RH_ScopedGlobalInstall(PngWriteIDAT, 0x7DB040);
    RH_ScopedGlobalInstall(PngWriteIEND, 0x7DB100);
    RH_ScopedGlobalInstall(PngWriteGAMA, 0x7DB190);
    RH_ScopedGlobalInstall(PngWriteSRGB, 0x7DB350);
    RH_ScopedGlobalInstall(PngWriteICCP, 0x7DB410);
    RH_ScopedGlobalInstall(PngTextCompress, 0x7DB570);
    RH_ScopedGlobalInstall(PngWriteCompressedDataOut, 0x7DB810);
    RH_ScopedGlobalInstall(PngWriteSPLT, 0x7DB910);
    RH_ScopedGlobalInstall(PngWriteSBIT, 0x7DBB40);
    RH_ScopedGlobalInstall(PngWriteCHRM, 0x7DBCB0);
    RH_ScopedGlobalInstall(PngWriteTRNS, 0x7DC4F0);
    RH_ScopedGlobalInstall(PngWriteBKGD, 0x7DC760);
    RH_ScopedGlobalInstall(PngWriteHIST, 0x7DC9B0);
    RH_ScopedGlobalInstall(PngCheckKeyword, 0x7DCAB0);
    RH_ScopedGlobalInstall(PngWriteTEXt, 0x7DCC50);
    RH_ScopedGlobalInstall(PngWriteZTXt, 0x7DCD90);
    RH_ScopedGlobalInstall(PngWriteOFFs, 0x7DD020);
    RH_ScopedGlobalInstall(PngWritePCAL, 0x7DD130);
    RH_ScopedGlobalInstall(PngWriteSCALS, 0x7DD3A0);
    RH_ScopedGlobalInstall(PngWritePHYs, 0x7DD500);
    RH_ScopedGlobalInstall(PngWriteStartRow, 0x7DD610);
    RH_ScopedGlobalInstall(PngWriteFinishRow, 0x7DD7B0);
    RH_ScopedGlobalInstall(PngDoWriteInterlace, 0x7DDAD0);
    RH_ScopedGlobalInstall(PngWriteFindFilter, 0x7DDD10);
    RH_ScopedGlobalInstall(PngWriteFilteredRow, 0x7DE900);
    RH_ScopedGlobalInstall(PngSetMemFn, 0x7DED30);
    RH_ScopedGlobalInstall(PngDoWriteTransformations, 0x7DED70);
    RH_ScopedGlobalInstall(PngDoPack, 0x7DEED0);
    RH_ScopedGlobalInstall(PngDoShift, 0x7DEFE0);
    RH_ScopedGlobalInstall(PngDoWriteSwapAlpha, 0x7DF260);
    RH_ScopedGlobalInstall(PngDoWriteInvertAlpha, 0x7DF370);
}
#else
void LibPngWriteInjectHooks() {}
#endif
