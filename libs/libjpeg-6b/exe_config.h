/* Diferencias de configuracion (jmorecfg.h) entre la libjpeg 6b oficial y la que lleva gta_sa.exe (RenderWare), descubiertas con el banco LIB3A comparando
 * el EXE con la biblioteca. Se aplican desde los envoltorios wrap/w_*.c DESPUES de jpeglib.h y ANTES del .c, sin tocar la fuente oficial.
 *  - MAX_COMPONENTS = 4 (oficial: 10): cambia la disposicion de las estructuras privadas (my_upsampler, jdmainct, jdcoefct...). Evidencia: start_pass_upsample (0x5CCE60)
 *    del EXE escribe next_row_out en +0x2C (12 + 4*N + 4*N con N = 4).
 *  - RGB_PIXELSIZE = 4 (oficial: 3): los pixeles RGB tienen 4 bytes (RGBA); evidencia: rgb_ycc_convert (0x5C32C0) avanza el puntero de entrada de 4 en 4.
 *    RGB_RED/GREEN/BLUE siguen siendo 0/1/2.
 *  - Solo DCT en coma flotante: DCT_ISLOW_SUPPORTED y DCT_IFAST_SUPPORTED sin definir (oficial: definidos). Evidencia: start_pass_fdctmgr (0x5C3940) del EXE da
 *    JERR_NOT_COMPILED (48) con dct_method = JDCT_ISLOW o JDCT_IFAST. */
#ifndef JPEG6B_EXE_CONFIG_H
#define JPEG6B_EXE_CONFIG_H
#undef MAX_COMPONENTS
#define MAX_COMPONENTS 4
#undef RGB_PIXELSIZE
#define RGB_PIXELSIZE 4
#undef DCT_ISLOW_SUPPORTED
#undef DCT_IFAST_SUPPORTED
#endif
