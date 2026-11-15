#pragma once

/**
 * Gestor de errores de la libjpeg 6b que lleva el EXE (jerror.c: error_exit, output_message y jpeg_std_error,
 * zona 0x5CD6B0..0x5CD85B). El listado las atribuia a FxEmitterBP_c por cercania de direcciones; no pertenecen a
 * ninguna clase del juego. Trabajan sobre el jpeg_error_mgr del EXE por desplazamientos.
 */
class LibJpegError {
public:
    static void InjectHooks();
};
