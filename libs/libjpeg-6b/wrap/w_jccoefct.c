/* Envoltorio generado: incluye jccoefct.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jccoefct.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jccoefct[] = {
    {"start_iMCU_row", (void*)start_iMCU_row},
    {"start_pass_coef", (void*)start_pass_coef},
    {"compress_data", (void*)compress_data},
    {"compress_first_pass", (void*)compress_first_pass},
    {"compress_output", (void*)compress_output},
    {"jinit_c_coef_controller", (void*)jinit_c_coef_controller},
    {0, 0}
};
