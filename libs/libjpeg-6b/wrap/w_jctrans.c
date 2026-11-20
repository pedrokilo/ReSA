/* Envoltorio generado: incluye jctrans.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jctrans.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jctrans[] = {
    {"jpeg_write_coefficients", (void*)jpeg_write_coefficients},
    {"jpeg_copy_critical_parameters", (void*)jpeg_copy_critical_parameters},
    {"transencode_master_selection", (void*)transencode_master_selection},
    {"start_iMCU_row", (void*)start_iMCU_row},
    {"start_pass_coef", (void*)start_pass_coef},
    {"compress_output", (void*)compress_output},
    {"transencode_coef_controller", (void*)transencode_coef_controller},
    {0, 0}
};
