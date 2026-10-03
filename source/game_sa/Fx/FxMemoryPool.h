/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"

/**
 * Arena lineal de memoria para blueprints, partículas y auxiliares FX.
 *
 * GetMem() sólo incrementa un cursor; Reset() invalida conceptualmente todas las
 * reservas previas y permite reutilizar el bloque completo.
 */
class FxMemoryPool_c {
public:
    uint8* m_pData;     //!< Inicio del bloque propiedad del pool.
    size_t m_nSize;     //!< Capacidad actual en bytes.
    size_t m_nPosition; //!< Siguiente offset libre.

public:
    static void InjectHooks();

    FxMemoryPool_c() = default; // 0x4A9C10
    ~FxMemoryPool_c() = default; // 0x4A9C20

    bool  Init();
    void  Exit();
    void  Reset();
    void* GetMem(int32 size, int32 align = sizeof(void*));
    void  Optimise();
};

VALIDATE_SIZE(FxMemoryPool_c, 0xC);
