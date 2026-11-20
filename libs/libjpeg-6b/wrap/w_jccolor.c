/* Envoltorio generado: incluye jccolor.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jccolor.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jccolor[] = {
    {"rgb_ycc_start", (void*)rgb_ycc_start},
    {"rgb_ycc_convert", (void*)rgb_ycc_convert},
    {"rgb_gray_convert", (void*)rgb_gray_convert},
    {"cmyk_ycck_convert", (void*)cmyk_ycck_convert},
    {"grayscale_convert", (void*)grayscale_convert},
    {"null_convert", (void*)null_convert},
    {"null_method", (void*)null_method},
    {"jinit_color_converter", (void*)jinit_color_converter},
    {0, 0}
};
