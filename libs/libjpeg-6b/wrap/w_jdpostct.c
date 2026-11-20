/* Envoltorio generado: incluye jdpostct.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdpostct.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdpostct[] = {
    {"start_pass_dpost", (void*)start_pass_dpost},
    {"post_process_1pass", (void*)post_process_1pass},
    {"post_process_prepass", (void*)post_process_prepass},
    {"post_process_2pass", (void*)post_process_2pass},
    {"jinit_d_post_controller", (void*)jinit_d_post_controller},
    {0, 0}
};
