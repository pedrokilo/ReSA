/* Envoltorio generado: incluye jdphuff.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdphuff.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdphuff[] = {
    {"start_pass_phuff_decoder", (void*)start_pass_phuff_decoder},
    {"process_restart", (void*)process_restart},
    {"decode_mcu_DC_first", (void*)decode_mcu_DC_first},
    {"decode_mcu_AC_first", (void*)decode_mcu_AC_first},
    {"decode_mcu_DC_refine", (void*)decode_mcu_DC_refine},
    {"decode_mcu_AC_refine", (void*)decode_mcu_AC_refine},
    {"jinit_phuff_decoder", (void*)jinit_phuff_decoder},
    {0, 0}
};
