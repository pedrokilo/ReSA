/* Envoltorio generado: incluye jdcolor.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdcolor.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdcolor[] = {
    {"build_ycc_rgb_table", (void*)build_ycc_rgb_table},
    {"ycc_rgb_convert", (void*)ycc_rgb_convert},
    {"null_convert", (void*)null_convert},
    {"grayscale_convert", (void*)grayscale_convert},
    {"gray_rgb_convert", (void*)gray_rgb_convert},
    {"ycck_cmyk_convert", (void*)ycck_cmyk_convert},
    {"start_pass_dcolor", (void*)start_pass_dcolor},
    {"jinit_color_deconverter", (void*)jinit_color_deconverter},
    {0, 0}
};
