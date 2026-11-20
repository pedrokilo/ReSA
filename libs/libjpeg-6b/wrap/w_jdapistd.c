/* Envoltorio generado: incluye jdapistd.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdapistd.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdapistd[] = {
    {"jpeg_start_decompress", (void*)jpeg_start_decompress},
    {"output_pass_setup", (void*)output_pass_setup},
    {"jpeg_read_scanlines", (void*)jpeg_read_scanlines},
    {"jpeg_read_raw_data", (void*)jpeg_read_raw_data},
    {"jpeg_start_output", (void*)jpeg_start_output},
    {"jpeg_finish_output", (void*)jpeg_finish_output},
    {0, 0}
};
