/* Envoltorio generado: incluye jdmerge.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdmerge.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdmerge[] = {
    {"build_ycc_rgb_table", (void*)build_ycc_rgb_table},
    {"start_pass_merged_upsample", (void*)start_pass_merged_upsample},
    {"merged_2v_upsample", (void*)merged_2v_upsample},
    {"merged_1v_upsample", (void*)merged_1v_upsample},
    {"h2v1_merged_upsample", (void*)h2v1_merged_upsample},
    {"h2v2_merged_upsample", (void*)h2v2_merged_upsample},
    {"jinit_merged_upsampler", (void*)jinit_merged_upsampler},
    {0, 0}
};
