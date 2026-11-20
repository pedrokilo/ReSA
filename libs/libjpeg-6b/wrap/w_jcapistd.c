/* Envoltorio generado: incluye jcapistd.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcapistd.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcapistd[] = {
    {"jpeg_start_compress", (void*)jpeg_start_compress},
    {"jpeg_write_scanlines", (void*)jpeg_write_scanlines},
    {"jpeg_write_raw_data", (void*)jpeg_write_raw_data},
    {0, 0}
};
