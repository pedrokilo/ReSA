#pragma once

/**
 * Rutinas de la API de compresion de la libjpeg 6b que lleva el EXE (jcapimin.c: jpeg_CreateCompress,
 * jpeg_suppress_tables y jpeg_finish_compress, zona 0x5CF960..0x5CFBC3). El listado las atribuia a
 * FxEmitterBP_c por cercania de direcciones; no pertenecen a ninguna clase del juego. Trabajan sobre el
 * jpeg_compress_struct del EXE (no el de libjpeg-turbo de la DLL), por lo que acceden a sus campos por
 * desplazamiento.
 */
class LibJpegCompressApi {
public:
    static void InjectHooks();
};
