/* Envoltorio generado: incluye jcmaster.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcmaster.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcmaster[] = {
    {"initial_setup", (void*)initial_setup},
    {"validate_script", (void*)validate_script},
    {"select_scan_parameters", (void*)select_scan_parameters},
    {"per_scan_setup", (void*)per_scan_setup},
    {"prepare_for_pass", (void*)prepare_for_pass},
    {"pass_startup", (void*)pass_startup},
    {"finish_pass_master", (void*)finish_pass_master},
    {"jinit_c_master_control", (void*)jinit_c_master_control},
    {0, 0}
};
