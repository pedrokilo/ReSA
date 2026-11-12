#pragma once

/**
 * Ruta de patrulla: elemento del CPatrolRoutePool (0x1A4 bytes, ver Pools.cpp). Solo se conoce el tamano; el contenido aun no esta reconstruido.
 * La clase existe por sus operator new / delete, que son las funciones del EXE 0x41B810 y 0x41B820 (tomadas del pool de rutas de patrulla 0xB744B4).
 */
class CPatrolRoute {
public:
    static void InjectHooks();

    void Destructor(); // 0x6677A0 (lote 170A, banco Fn170A)

    static void* operator new(size_t size);
    static void  operator delete(void* ptr);

private:
    uint8 m_data[0x1A4];
};
VALIDATE_SIZE(CPatrolRoute, 0x1A4);
