/* Envoltorio generado: incluye jquant2.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG6B_KEEP_2PASS
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jquant2.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jquant2[] = {
    {"prescan_quantize", (void*)prescan_quantize},
    {"find_biggest_color_pop", (void*)find_biggest_color_pop},
    {"find_biggest_volume", (void*)find_biggest_volume},
    {"update_box", (void*)update_box},
    {"median_cut", (void*)median_cut},
    {"compute_color", (void*)compute_color},
    {"select_colors", (void*)select_colors},
    {"find_nearby_colors", (void*)find_nearby_colors},
    {"find_best_colors", (void*)find_best_colors},
    {"fill_inverse_cmap", (void*)fill_inverse_cmap},
    {"pass2_no_dither", (void*)pass2_no_dither},
    {"pass2_fs_dither", (void*)pass2_fs_dither},
    {"init_error_limit", (void*)init_error_limit},
    {"finish_pass1", (void*)finish_pass1},
    {"finish_pass2", (void*)finish_pass2},
    {"start_pass_2_quant", (void*)start_pass_2_quant},
    {"new_color_map_2_quant", (void*)new_color_map_2_quant},
    {"jinit_2pass_quantizer", (void*)jinit_2pass_quantizer},
    {0, 0}
};
