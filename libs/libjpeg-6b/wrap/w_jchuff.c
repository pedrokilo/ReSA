/* Envoltorio generado: incluye jchuff.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jchuff.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jchuff[] = {
    {"start_pass_huff", (void*)start_pass_huff},
    {"jpeg_make_c_derived_tbl", (void*)jpeg_make_c_derived_tbl},
    {"dump_buffer", (void*)dump_buffer},
    {"emit_bits", (void*)emit_bits},
    {"flush_bits", (void*)flush_bits},
    {"encode_one_block", (void*)encode_one_block},
    {"emit_restart", (void*)emit_restart},
    {"encode_mcu_huff", (void*)encode_mcu_huff},
    {"finish_pass_huff", (void*)finish_pass_huff},
    {"htest_one_block", (void*)htest_one_block},
    {"encode_mcu_gather", (void*)encode_mcu_gather},
    {"jpeg_gen_optimal_table", (void*)jpeg_gen_optimal_table},
    {"finish_pass_gather", (void*)finish_pass_gather},
    {"jinit_huff_encoder", (void*)jinit_huff_encoder},
    {0, 0}
};
