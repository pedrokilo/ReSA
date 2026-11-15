#pragma once

/**
 * Rutinas auxiliares del bloque de arranque/proteccion del EXE (0x401000..0x403FFF, anterior a CBuilding).
 * El listado las atribuia a CEntity; no pertenecen a ninguna clase del juego.
 */
class CProtectionStub {
public:
    static void InjectHooks();
};
