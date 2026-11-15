#pragma once

/**
 * Gestor de memoria de la libjpeg 6b que lleva el EXE (jmemmgr.c, jmemnobs.c y jutils.c, zona 0x5CEBC0..0x5CF960).
 * El listado las atribuia a FxEmitterBP_c por cercania de direcciones; no pertenecen a ninguna clase del juego.
 * Trabajan sobre el my_memory_mgr del EXE (no el de libjpeg-turbo de la DLL), por lo que acceden a sus campos por
 * desplazamiento.
 */
class LibJpegMemory {
public:
    static void InjectHooks();
};
