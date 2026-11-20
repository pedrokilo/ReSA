/* Envoltorio generado: incluye jcomapi.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcomapi.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcomapi[] = {
    {"jpeg_abort", (void*)jpeg_abort},
    {"jpeg_destroy", (void*)jpeg_destroy},
    {"jpeg_alloc_quant_table", (void*)jpeg_alloc_quant_table},
    {"jpeg_alloc_huff_table", (void*)jpeg_alloc_huff_table},
    {0, 0}
};
