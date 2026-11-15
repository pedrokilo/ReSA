#pragma once

#include "Vector.h"

/**
 * Ruta de patrulla: elemento del CPatrolRoutePool (0x1A4 bytes, ver Pools.cpp). Solo se conoce el tamano; el contenido aun no esta reconstruido.
 * La clase existe por sus operator new / delete, que son las funciones del EXE 0x41B810 y 0x41B820 (tomadas del pool de rutas de patrulla 0xB744B4).
 * Lote 195A (sin validar): se conocen ya los campos que toca Reverse (0x66D550).
 */
class CPatrolRoute {
public:
    static void InjectHooks();

    void Destructor(); // 0x6677A0 (lote 170A, banco Fn170A)
    CPatrolRoute* Constructor();                    // 0x66D440 (lote 175A, banco Fn175A)
    void          Assign(const CPatrolRoute* src);  // 0x66D4B0 (lote 175A, banco Fn175A)
    void Reverse();    // 0x66D550 (lote 195A, sin validar): invierte el orden de los nodos (nombres y puntos)

    static void* operator new(size_t size);
    static void  operator delete(void* ptr);

    // Disposicion tomada de 0x66D550 (y del constructor 0x66D440): contador en +0, 8 nodos de dos cadenas (0x18 y 0x10 bytes, 0x28 en total) desde +4 y 8 puntos (CVector) desde +0x144.
    struct Node {
        char m_szName[0x18];
        char m_szName2[0x10];
    };
    VALIDATE_SIZE(Node, 0x28);

private:
    uint8 m_data[0x1A4]; // contador (uint32) en +0, Node[8] desde +4, CVector[8] desde +0x144
};
VALIDATE_SIZE(CPatrolRoute, 0x1A4);
