/* Envoltorio generado: incluye jdapimin.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdapimin.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdapimin[] = {
    {"jpeg_CreateDecompress", (void*)jpeg_CreateDecompress},
    {"jpeg_destroy_decompress", (void*)jpeg_destroy_decompress},
    {"jpeg_abort_decompress", (void*)jpeg_abort_decompress},
    {"default_decompress_parms", (void*)default_decompress_parms},
    {"jpeg_read_header", (void*)jpeg_read_header},
    {"jpeg_consume_input", (void*)jpeg_consume_input},
    {"jpeg_input_complete", (void*)jpeg_input_complete},
    {"jpeg_has_multiple_scans", (void*)jpeg_has_multiple_scans},
    {"jpeg_finish_decompress", (void*)jpeg_finish_decompress},
    {0, 0}
};
