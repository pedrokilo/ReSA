/* Envoltorio generado: incluye jdcoefct.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdcoefct.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdcoefct[] = {
    {"start_iMCU_row", (void*)start_iMCU_row},
    {"start_input_pass", (void*)start_input_pass},
    {"start_output_pass", (void*)start_output_pass},
    {"decompress_onepass", (void*)decompress_onepass},
    {"dummy_consume_data", (void*)dummy_consume_data},
    {"consume_data", (void*)consume_data},
    {"decompress_data", (void*)decompress_data},
    {"smoothing_ok", (void*)smoothing_ok},
    {"decompress_smooth_data", (void*)decompress_smooth_data},
    {"jinit_d_coef_controller", (void*)jinit_d_coef_controller},
    {0, 0}
};
