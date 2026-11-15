#pragma once

/**
 * Rutinas internas de descompresion de la libjpeg 6b que lleva el EXE (modulos jdpostct y jdsample, zona
 * 0x5CCDF0..0x5CD52C). El listado las atribuia a FxEmitterBP_c por cercania de direcciones; no pertenecen a
 * ninguna clase del juego. Trabajan sobre el jpeg_decompress_struct del EXE (no el de libjpeg-turbo de la DLL),
 * por lo que acceden a sus campos por desplazamiento.
 */
class LibJpegDecompressUpsample {
public:
    static void InjectHooks();
};
