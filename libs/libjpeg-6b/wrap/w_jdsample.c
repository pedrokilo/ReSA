/* Envoltorio generado: incluye jdsample.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdsample.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdsample[] = {
    {"start_pass_upsample", (void*)start_pass_upsample},
    {"sep_upsample", (void*)sep_upsample},
    {"fullsize_upsample", (void*)fullsize_upsample},
    {"noop_upsample", (void*)noop_upsample},
    {"int_upsample", (void*)int_upsample},
    {"h2v1_upsample", (void*)h2v1_upsample},
    {"h2v2_upsample", (void*)h2v2_upsample},
    {"h2v1_fancy_upsample", (void*)h2v1_fancy_upsample},
    {"h2v2_fancy_upsample", (void*)h2v2_fancy_upsample},
    {"jinit_upsampler", (void*)jinit_upsampler},
    {0, 0}
};
