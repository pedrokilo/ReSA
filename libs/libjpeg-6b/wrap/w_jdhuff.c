/* Envoltorio generado: incluye jdhuff.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdhuff.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdhuff[] = {
    {"start_pass_huff_decoder", (void*)start_pass_huff_decoder},
    {"jpeg_make_d_derived_tbl", (void*)jpeg_make_d_derived_tbl},
    {"jpeg_fill_bit_buffer", (void*)jpeg_fill_bit_buffer},
    {"jpeg_huff_decode", (void*)jpeg_huff_decode},
    {"process_restart", (void*)process_restart},
    {"decode_mcu", (void*)decode_mcu},
    {"jinit_huff_decoder", (void*)jinit_huff_decoder},
    {0, 0}
};
