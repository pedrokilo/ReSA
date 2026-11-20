/* Envoltorio generado: incluye jdmaster.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdmaster.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdmaster[] = {
    {"use_merged_upsample", (void*)use_merged_upsample},
    {"jpeg_calc_output_dimensions", (void*)jpeg_calc_output_dimensions},
    {"prepare_range_limit_table", (void*)prepare_range_limit_table},
    {"master_selection", (void*)master_selection},
    {"prepare_for_output_pass", (void*)prepare_for_output_pass},
    {"finish_output_pass", (void*)finish_output_pass},
    {"jpeg_new_colormap", (void*)jpeg_new_colormap},
    {"jinit_master_decompress", (void*)jinit_master_decompress},
    {0, 0}
};
