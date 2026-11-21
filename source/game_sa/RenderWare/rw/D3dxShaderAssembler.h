#pragma once

#include "D3dxShaderSource.h"

// Ensamblador de shaders (vs_1_x/vs_2_x) de D3DX9 enlazado en el EXE (lote NEWDEL, ver reversing/FnNEWDEL/README.md). Objeto de la pila de D3DXAssembleShader
// (0x7652AE); __thiscall con `this` en ECX. 0x76FAEB lo inicializa, 0x772BB1 ejecuta el ensamblado y 0x771F63 lo destruye. Banderas del EXE: 1 y 4 (4 = no usar el
// validador de d3d9.dll; el juego pasa 4). Los colaboradores no reconstruidos se llaman por direccion (ver D3dxShaderAssembler.cpp).
struct D3dxShaderAssembler {
    D3dxErrorLog*     errorLog;   // +0x00 = fuente + 4
    D3dxShaderSource* source;     // +0x04
    void*             validator;  // +0x08 IDirect3DShaderValidator9 (solo con la bandera 4 apagada)
    uint32            pad0C;      // +0x0C
    uint32            errorCode;  // +0x10 (lo escriben los colaboradores; 9 = hay que releer la version)
    uint32            pad14;      // +0x14
    uint32            version;    // +0x18 version del shader (0xFFFE0101 = vs_1_1, 0xFFFE0200 = vs_2_0, ...)
    uint32            pad1C[4];   // +0x1C..+0x2B
    uint32            pad2C;      // +0x2C
    uint32            f30;        // +0x30
    void*             f34;        // +0x34 objeto con destructor virtual (bandera 1)
    int32             mode;       // +0x38 indice de version (0..0xE), -1 sin fijar
    uint32            flags;      // +0x3C
    uint32            pad40[3];   // +0x40..+0x4B
    uint32            failed;     // +0x4C
    uint32            f50;        // +0x50
    uint32            f54;        // +0x54
    void*             f58;        // +0x58 bloque propio (operator delete)
    uint32            f5C;        // +0x5C
    uint32            f60;        // +0x60
    uint32            f64;        // +0x64
    uint32            f68;        // +0x68
    uint32            offset;     // +0x6C
    uint32            length;     // +0x70
    uint32            f74;        // +0x74
    void*             f78;        // +0x78 objeto de 0x64 bytes (ctor 0x7716DD, destructor 0x77194F)
    uint32            f7C;        // +0x7C

    // 0x76FAEB: devuelve this
    D3dxShaderAssembler* Constructor();
    // 0x771F63
    void Destructor();
    // 0x772BB1 (ret 0x10): ensambla la fuente; `unkC` (distinto de cero: activa la ruta con f78/f7C) siempre es 0 en D3DXAssembleShader.
    // Escribe el ID3DXBuffer del bytecode en *ppShader. Devuelve HRESULT (D3DERR_INVALIDCALL 0x8876086C con banderas o puntero invalidos).
    int32 Assemble(D3dxShaderSource* src, uint32 flags, uint32 unkC, void** ppShader);

    static void InjectHooks();
};
static_assert(sizeof(D3dxShaderAssembler) == 0x80);
static_assert(offsetof(D3dxShaderAssembler, errorCode) == 0x10);
static_assert(offsetof(D3dxShaderAssembler, version) == 0x18);
static_assert(offsetof(D3dxShaderAssembler, f34) == 0x34);
static_assert(offsetof(D3dxShaderAssembler, flags) == 0x3C);
static_assert(offsetof(D3dxShaderAssembler, failed) == 0x4C);
static_assert(offsetof(D3dxShaderAssembler, f58) == 0x58);
static_assert(offsetof(D3dxShaderAssembler, offset) == 0x6C);
static_assert(offsetof(D3dxShaderAssembler, f78) == 0x78);
