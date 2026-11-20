/* Envoltorio generado: incluye jidctred.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jidctred.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jidctred[] = {
#ifdef IDCT_SCALING_SUPPORTED
    {"jpeg_idct_4x4", (void*)jpeg_idct_4x4},
#endif
#ifdef IDCT_SCALING_SUPPORTED
    {"jpeg_idct_2x2", (void*)jpeg_idct_2x2},
#endif
#ifdef IDCT_SCALING_SUPPORTED
    {"jpeg_idct_1x1", (void*)jpeg_idct_1x1},
#endif
    {0, 0}
};
