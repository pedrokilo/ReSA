/* Envoltorio generado: incluye jcapimin.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcapimin.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcapimin[] = {
    {"jpeg_CreateCompress", (void*)jpeg_CreateCompress},
    {"jpeg_destroy_compress", (void*)jpeg_destroy_compress},
    {"jpeg_abort_compress", (void*)jpeg_abort_compress},
    {"jpeg_suppress_tables", (void*)jpeg_suppress_tables},
    {"jpeg_finish_compress", (void*)jpeg_finish_compress},
    {"jpeg_write_marker", (void*)jpeg_write_marker},
    {"jpeg_write_m_header", (void*)jpeg_write_m_header},
    {"jpeg_write_m_byte", (void*)jpeg_write_m_byte},
    {"jpeg_write_tables", (void*)jpeg_write_tables},
    {0, 0}
};
