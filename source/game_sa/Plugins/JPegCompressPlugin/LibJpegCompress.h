#pragma once

/**
 * Rutinas internas de compresion de la libjpeg 6b que lleva el EXE (modulos jcapistd, jcmaster, jccolor,
 * jcdctmgr, jchuff, jcmainct y jccoefct, zona 0x5C26B0..0x5C5560). El listado las atribuia a FxEmitterBP_c por
 * cercania de direcciones; no pertenecen a ninguna clase del juego. Trabajan sobre el jpeg_compress_struct
 * del EXE (no el de libjpeg-turbo de la DLL), por lo que acceden a sus campos por desplazamiento.
 */
class LibJpegCompress {
public:
    static void InjectHooks();
};
