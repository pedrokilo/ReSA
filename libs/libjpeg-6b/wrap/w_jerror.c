/* Envoltorio generado: incluye jerror.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jerror.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jerror[] = {
    {"error_exit", (void*)error_exit},
    {"output_message", (void*)output_message},
    {"emit_message", (void*)emit_message},
    {"format_message", (void*)format_message},
    {"reset_error_mgr", (void*)reset_error_mgr},
    {"jpeg_std_error", (void*)jpeg_std_error},
    {0, 0}
};
