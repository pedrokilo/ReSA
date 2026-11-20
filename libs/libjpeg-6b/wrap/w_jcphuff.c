/* Envoltorio generado: incluye jcphuff.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcphuff.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcphuff[] = {
    {"start_pass_phuff", (void*)start_pass_phuff},
    {"dump_buffer", (void*)dump_buffer},
    {"emit_bits", (void*)emit_bits},
    {"flush_bits", (void*)flush_bits},
    {"emit_symbol", (void*)emit_symbol},
    {"emit_buffered_bits", (void*)emit_buffered_bits},
    {"emit_eobrun", (void*)emit_eobrun},
    {"emit_restart", (void*)emit_restart},
    {"encode_mcu_DC_first", (void*)encode_mcu_DC_first},
    {"encode_mcu_AC_first", (void*)encode_mcu_AC_first},
    {"encode_mcu_DC_refine", (void*)encode_mcu_DC_refine},
    {"encode_mcu_AC_refine", (void*)encode_mcu_AC_refine},
    {"finish_pass_phuff", (void*)finish_pass_phuff},
    {"finish_pass_gather_phuff", (void*)finish_pass_gather_phuff},
    {"jinit_phuff_encoder", (void*)jinit_phuff_encoder},
    {0, 0}
};
