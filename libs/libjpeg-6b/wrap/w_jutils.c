/* Envoltorio generado: incluye jutils.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jutils.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jutils[] = {
    {"jdiv_round_up", (void*)jdiv_round_up},
    {"jround_up", (void*)jround_up},
    {"jcopy_sample_rows", (void*)jcopy_sample_rows},
    {"jcopy_block_row", (void*)jcopy_block_row},
    {"jzero_far", (void*)jzero_far},
    {0, 0}
};
