/* Envoltorio generado: incluye jcprepct.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcprepct.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcprepct[] = {
    {"start_pass_prep", (void*)start_pass_prep},
    {"expand_bottom_edge", (void*)expand_bottom_edge},
    {"pre_process_data", (void*)pre_process_data},
#ifdef INPUT_SMOOTHING_SUPPORTED
    {"pre_process_context", (void*)pre_process_context},
#endif
#ifdef INPUT_SMOOTHING_SUPPORTED
    {"create_context_buffer", (void*)create_context_buffer},
#endif
    {"jinit_c_prep_controller", (void*)jinit_c_prep_controller},
    {0, 0}
};
