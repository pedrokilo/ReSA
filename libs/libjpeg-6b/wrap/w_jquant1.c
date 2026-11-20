/* Envoltorio generado: incluye jquant1.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jquant1.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jquant1[] = {
    {"select_ncolors", (void*)select_ncolors},
    {"output_value", (void*)output_value},
    {"largest_input_value", (void*)largest_input_value},
    {"create_colormap", (void*)create_colormap},
    {"create_colorindex", (void*)create_colorindex},
    {"make_odither_array", (void*)make_odither_array},
    {"create_odither_tables", (void*)create_odither_tables},
    {"color_quantize", (void*)color_quantize},
    {"color_quantize3", (void*)color_quantize3},
    {"quantize_ord_dither", (void*)quantize_ord_dither},
    {"quantize3_ord_dither", (void*)quantize3_ord_dither},
    {"quantize_fs_dither", (void*)quantize_fs_dither},
    {"alloc_fs_workspace", (void*)alloc_fs_workspace},
    {"start_pass_1_quant", (void*)start_pass_1_quant},
    {"finish_pass_1_quant", (void*)finish_pass_1_quant},
    {"new_color_map_1_quant", (void*)new_color_map_1_quant},
    {"jinit_1pass_quantizer", (void*)jinit_1pass_quantizer},
    {0, 0}
};
