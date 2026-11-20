/* Envoltorio generado: incluye jmemnobs.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */
#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"
#include "../exe_config.h"
#include "../jmemnobs.c"
#include "../jpeg6b_export.h"
const jpeg6b_export jpeg6b_exports_jmemnobs[] = {
    {"jpeg_get_small", (void*)jpeg_get_small},
    {"jpeg_free_small", (void*)jpeg_free_small},
    {"jpeg_get_large", (void*)jpeg_get_large},
    {"jpeg_free_large", (void*)jpeg_free_large},
    {"jpeg_mem_available", (void*)jpeg_mem_available},
    {"jpeg_open_backing_store", (void*)jpeg_open_backing_store},
    {"jpeg_mem_init", (void*)jpeg_mem_init},
    {"jpeg_mem_term", (void*)jpeg_mem_term},
    {0, 0}
};
