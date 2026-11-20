/* Envoltorio generado: incluye jdatadst.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdatadst.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdatadst[] = {
    {"init_destination", (void*)init_destination},
    {"empty_output_buffer", (void*)empty_output_buffer},
    {"term_destination", (void*)term_destination},
    {"jpeg_stdio_dest", (void*)jpeg_stdio_dest},
    {0, 0}
};
