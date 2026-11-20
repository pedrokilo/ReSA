/* Envoltorio generado: incluye jdinput.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdinput.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdinput[] = {
    {"initial_setup", (void*)initial_setup},
    {"per_scan_setup", (void*)per_scan_setup},
    {"latch_quant_tables", (void*)latch_quant_tables},
    {"start_input_pass", (void*)start_input_pass},
    {"finish_input_pass", (void*)finish_input_pass},
    {"consume_markers", (void*)consume_markers},
    {"reset_input_controller", (void*)reset_input_controller},
    {"jinit_input_controller", (void*)jinit_input_controller},
    {0, 0}
};
