/* Envoltorio generado: incluye jdmainct.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdmainct.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdmainct[] = {
    {"alloc_funny_pointers", (void*)alloc_funny_pointers},
    {"make_funny_pointers", (void*)make_funny_pointers},
    {"set_wraparound_pointers", (void*)set_wraparound_pointers},
    {"set_bottom_pointers", (void*)set_bottom_pointers},
    {"start_pass_main", (void*)start_pass_main},
    {"process_data_simple_main", (void*)process_data_simple_main},
    {"process_data_context_main", (void*)process_data_context_main},
    {"process_data_crank_post", (void*)process_data_crank_post},
    {"jinit_d_main_controller", (void*)jinit_d_main_controller},
    {0, 0}
};
