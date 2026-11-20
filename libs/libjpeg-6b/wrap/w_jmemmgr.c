/* Envoltorio generado: incluye jmemmgr.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jmemmgr.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jmemmgr[] = {
    {"out_of_memory", (void*)out_of_memory},
    {"alloc_small", (void*)alloc_small},
    {"alloc_large", (void*)alloc_large},
    {"alloc_sarray", (void*)alloc_sarray},
    {"alloc_barray", (void*)alloc_barray},
    {"request_virt_sarray", (void*)request_virt_sarray},
    {"request_virt_barray", (void*)request_virt_barray},
    {"realize_virt_arrays", (void*)realize_virt_arrays},
    {"do_sarray_io", (void*)do_sarray_io},
    {"do_barray_io", (void*)do_barray_io},
    {"access_virt_sarray", (void*)access_virt_sarray},
    {"access_virt_barray", (void*)access_virt_barray},
    {"free_pool", (void*)free_pool},
    {"self_destruct", (void*)self_destruct},
    {"jinit_memory_mgr", (void*)jinit_memory_mgr},
    {0, 0}
};
