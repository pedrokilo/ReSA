/* Envoltorio generado: incluye jdatasrc.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdatasrc.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdatasrc[] = {
    {"init_source", (void*)init_source},
    {"fill_input_buffer", (void*)fill_input_buffer},
    {"skip_input_data", (void*)skip_input_data},
    {"term_source", (void*)term_source},
    {"jpeg_stdio_src", (void*)jpeg_stdio_src},
    {0, 0}
};
