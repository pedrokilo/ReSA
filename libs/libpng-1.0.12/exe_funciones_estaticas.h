/* Lote LIB4A: inclusion forzada (/FI) SOLO para pngwutil.c. Incluye antes las cabeceras del CRT que png.h/pngconf.h/zlib.h traerian despues y
 * deja `static` vacio: asi png_text_compress y png_write_compressed_data_out (static en la fuente oficial, funciones propias en el EXE) tienen enlace externo. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include <malloc.h>
#define static
