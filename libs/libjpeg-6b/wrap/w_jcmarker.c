/* Envoltorio generado: incluye jcmarker.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcmarker.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcmarker[] = {
    {"emit_byte", (void*)emit_byte},
    {"emit_marker", (void*)emit_marker},
    {"emit_2bytes", (void*)emit_2bytes},
    {"emit_dqt", (void*)emit_dqt},
    {"emit_dht", (void*)emit_dht},
    {"emit_dac", (void*)emit_dac},
    {"emit_dri", (void*)emit_dri},
    {"emit_sof", (void*)emit_sof},
    {"emit_sos", (void*)emit_sos},
    {"emit_jfif_app0", (void*)emit_jfif_app0},
    {"emit_adobe_app14", (void*)emit_adobe_app14},
    {"write_marker_header", (void*)write_marker_header},
    {"write_marker_byte", (void*)write_marker_byte},
    {"write_file_header", (void*)write_file_header},
    {"write_frame_header", (void*)write_frame_header},
    {"write_scan_header", (void*)write_scan_header},
    {"write_file_trailer", (void*)write_file_trailer},
    {"write_tables_only", (void*)write_tables_only},
    {"jinit_marker_writer", (void*)jinit_marker_writer},
    {0, 0}
};
