/* Envoltorio generado: incluye jddctmgr.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
/* jpegint.h declara jpeg_inverse_dct con inverse_DCT[MAX_COMPONENTS]; MAX_COMPONENTS (4 en el EXE) debe aplicarse antes: se renombra la estructura oficial y se redeclara con la disposicion del EXE (start_pass_idctmgr 0x5C9510: cur_method en +0x14) */
#define jpeg_inverse_dct jpeg_inverse_dct_oficial
#include "../jpeglib.h"
#undef jpeg_inverse_dct
#include "../exe_config.h"
struct jpeg_inverse_dct { JMETHOD(void, start_pass, (j_decompress_ptr cinfo)); inverse_DCT_method_ptr inverse_DCT[MAX_COMPONENTS]; };
#include "../jddctmgr.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jddctmgr[] = {
    {"start_pass", (void*)start_pass},
    {"jinit_inverse_dct", (void*)jinit_inverse_dct},
    {0, 0}
};
