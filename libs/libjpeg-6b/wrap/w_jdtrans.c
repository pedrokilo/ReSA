/* Envoltorio generado: incluye jdtrans.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdtrans.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdtrans[] = {
    {"jpeg_read_coefficients", (void*)jpeg_read_coefficients},
    {"transdecode_master_selection", (void*)transdecode_master_selection},
    {0, 0}
};
