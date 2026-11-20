/* Envoltorio generado: incluye jdmarker.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jdmarker.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jdmarker[] = {
    {"get_soi", (void*)get_soi},
    {"get_sof", (void*)get_sof},
    {"get_sos", (void*)get_sos},
    {"get_dht", (void*)get_dht},
    {"get_dqt", (void*)get_dqt},
    {"get_dri", (void*)get_dri},
    {"examine_app0", (void*)examine_app0},
    {"examine_app14", (void*)examine_app14},
    {"get_interesting_appn", (void*)get_interesting_appn},
    {"save_marker", (void*)save_marker},
    {"skip_variable", (void*)skip_variable},
    {"next_marker", (void*)next_marker},
    {"first_marker", (void*)first_marker},
    {"read_markers", (void*)read_markers},
    {"read_restart_marker", (void*)read_restart_marker},
    {"jpeg_resync_to_restart", (void*)jpeg_resync_to_restart},
    {"reset_marker_reader", (void*)reset_marker_reader},
    {"jinit_marker_reader", (void*)jinit_marker_reader},
    {"jpeg_save_markers", (void*)jpeg_save_markers},
    {"jpeg_set_marker_processor", (void*)jpeg_set_marker_processor},
    {0, 0}
};
