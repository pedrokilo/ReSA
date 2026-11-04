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

// Pila de texturas D3D9 reutilizables de un mismo formato y tamano (cache de D3DResourceSystem). Los metodos con direccion son codigo del EXE
// reconstruido (0x72FE80..0x730AD0); las texturas son objetos COM que se usan por su vtable (D3DResourceCom, en D3DResourceSystem.h).
class D3DTextureBuffer {
public:
#ifdef _D3D9_H_
    using Texture = IDirect3DTexture9;
#else
    using Texture = void;
#endif
    uint32 m_nFormat;
    uint32 m_nWidth;
    int32  m_nLevels; // -1 - not defined, 0 - full-chain, 1 - one level
    uint32 m_nCapcacity;
    uint32 m_nNumTexturesInBuffer; // contador de entregas/devoluciones (no es el numero de texturas guardadas)
    uint32 m_nSize;                // numero de texturas guardadas en la pila
    Texture** m_apTextures;

    static void InjectHooks();

    void Clear();
    void Destroy();
    uint32 GetTotalDataSize();
    void Resize(uint32 newCapacity);
    void Setup(uint32 format, int32 width, int32 levels, uint32 capacity);
    Texture* Pop();
    Texture* Pop(uint32 format, int32 width, int32 height, int32 levels);
    bool Push(Texture* texture);
    bool PushWithoutIncreasingCounter(Texture* texture);
};

VALIDATE_SIZE(D3DTextureBuffer, 0x1C);