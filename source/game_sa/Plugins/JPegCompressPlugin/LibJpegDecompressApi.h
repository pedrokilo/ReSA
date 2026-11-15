#pragma once

/**
 * API de descompresion de la libjpeg 6b que lleva el EXE (modulo jdapimin, zona 0x5CFD20..0x5D0240). El listado las atribuia a
 * FxEmitterBP_c por cercania de direcciones; no pertenecen a ninguna clase del juego. Lote 236A (sin validar). Trabajan sobre el
 * jpeg_decompress_struct del EXE (no el de libjpeg-turbo de la DLL), por lo que acceden a sus campos por desplazamiento.
 */
class LibJpegDecompressApi {
public:
    static void InjectHooks();
};
