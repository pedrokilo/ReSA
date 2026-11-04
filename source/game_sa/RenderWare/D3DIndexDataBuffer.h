/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#ifdef _DX9_SDK_INSTALLED
#include "d3d9.h"
#endif

// Pila de index buffers D3D9 reutilizables (cache de D3DResourceSystem; uno por grupo de 100 indices y uno generico). Los metodos con direccion son codigo
// del EXE reconstruido (0x730270..0x7303B0); los index buffers son objetos COM que se usan por su vtable (D3DResourceCom, en D3DResourceSystem.h).
class D3DIndexDataBuffer {
public:
#ifdef _D3D9_H_
    using IndexData = IDirect3DIndexBuffer9;
#else
    using IndexData = void;
#endif
    uint32 m_nFormat;
    int32  field_4;
    uint32 m_nCapcacity;
    uint32 m_nNumDatasInBuffer; // contador de entregas/devoluciones (no es el numero de buffers guardados)
    uint32 m_nSize;             // numero de index buffers guardados en la pila
    IndexData** m_apIndexData;

    static void InjectHooks();

    void Clear();
    void Destroy();
    uint32 GetTotalDataSize();
    void Resize(uint32 newCapacity);
    void Setup(uint32 format, int32 _f4, uint32 capacity);
    IndexData* Pop();
    IndexData* Pop(uint32 indexCount);
    bool Push(IndexData* indexBuffer);
    bool PushWithoutIncreasingCounter(IndexData* indexBuffer);
};

VALIDATE_SIZE(D3DIndexDataBuffer, 0x18);