#pragma once

/**
 * Rutinas internas de descompresion de la libjpeg 6b que lleva el EXE (modulos jdhuff, jdinput y jdmainct,
 * zona 0x5C9790..0x5CB000). El listado las atribuia a FxEmitterBP_c por cercania de direcciones; no
 * pertenecen a ninguna clase del juego. Trabajan sobre el jpeg_decompress_struct del EXE (no el de
 * libjpeg-turbo de la DLL), por lo que acceden a sus campos por desplazamiento.
 */
class LibJpegDecompress {
public:
    static void InjectHooks();
};
