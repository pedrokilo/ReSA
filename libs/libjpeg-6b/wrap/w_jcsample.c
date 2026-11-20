/* Envoltorio generado: incluye jcsample.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jcsample.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jcsample[] = {
    {"start_pass_downsample", (void*)start_pass_downsample},
    {"expand_right_edge", (void*)expand_right_edge},
    {"sep_downsample", (void*)sep_downsample},
    {"int_downsample", (void*)int_downsample},
    {"fullsize_downsample", (void*)fullsize_downsample},
    {"h2v1_downsample", (void*)h2v1_downsample},
    {"h2v2_downsample", (void*)h2v2_downsample},
#ifdef INPUT_SMOOTHING_SUPPORTED
    {"h2v2_smooth_downsample", (void*)h2v2_smooth_downsample},
#endif
#ifdef INPUT_SMOOTHING_SUPPORTED
    {"fullsize_smooth_downsample", (void*)fullsize_smooth_downsample},
#endif
    {"jinit_downsampler", (void*)jinit_downsampler},
    {0, 0}
};
