/* Envoltorio generado: incluye jcparam.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcparam.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcparam[] = {
    {"jpeg_add_quant_table", (void*)jpeg_add_quant_table},
    {"jpeg_set_linear_quality", (void*)jpeg_set_linear_quality},
    {"jpeg_quality_scaling", (void*)jpeg_quality_scaling},
    {"jpeg_set_quality", (void*)jpeg_set_quality},
    {"add_huff_table", (void*)add_huff_table},
    {"std_huff_tables", (void*)std_huff_tables},
    {"jpeg_set_defaults", (void*)jpeg_set_defaults},
    {"jpeg_default_colorspace", (void*)jpeg_default_colorspace},
    {"jpeg_set_colorspace", (void*)jpeg_set_colorspace},
    {"fill_a_scan", (void*)fill_a_scan},
    {"fill_scans", (void*)fill_scans},
    {"fill_dc_scans", (void*)fill_dc_scans},
    {"jpeg_simple_progression", (void*)jpeg_simple_progression},
    {0, 0}
};
