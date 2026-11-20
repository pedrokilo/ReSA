/* Envoltorio generado: incluye jcdctmgr.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcdctmgr.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcdctmgr[] = {
    {"start_pass_fdctmgr", (void*)start_pass_fdctmgr},
    {"forward_DCT_float", (void*)forward_DCT_float},
    {"jinit_forward_dct", (void*)jinit_forward_dct},
    {0, 0}
};
