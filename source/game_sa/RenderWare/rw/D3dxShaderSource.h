#pragma once

// Objeto "fuente" del ensamblador de shaders de D3DX9 enlazado estaticamente en el EXE (lote NEWDEL, ver reversing/FnNEWDEL/README.md).
// Lo crea D3DXAssembleShader (0x7652AE) en su pila (0x8C bytes, sin constructor de C++): 0x76B89B lo inicializa, 0x76DFD1 / 0x76DF03 / 0x76DF6C
// lo cargan (memoria / fichero / recurso), 0x772BB1 (D3dxShaderAssembler::Assemble) lo consume, 0x76E5FB copia el texto de errores a un ID3DXBuffer y
// 0x76B940 lo destruye. Todos son __thiscall con `this` en ECX. Los colaboradores que no se han reconstruido se llaman por direccion.
//
// Disposicion (solo los campos que tocan estas rutinas; el resto lo escriben los colaboradores):
//   +0x00 bloques de memoria encadenados (0x76E390 lo pone a 0; 0x76E396 libera la cadena)
//   +0x04 registro de errores (D3dxErrorLog, 0x20 bytes)
//   +0x24..+0x4C parametros de analisis (se inicializan en el constructor); +0x50 lector (0x70 bytes, ctor 0x76B567), +0x54 copia del lector, +0x58 argumento del cargador
//   +0x80 uno; +0x84 configuracion regional guardada (cadena duplicada); +0x88 palabra de control de coma flotante guardada

#include "D3dxErrorLog.h"

struct D3dxShaderSource {
    void*        pool;         // +0x00
    D3dxErrorLog errors;       // +0x04
    uint32       f24;          // +0x24
    uint32       f28;          // +0x28
    uint32       f2C;          // +0x2C
    uint32       f30;          // +0x30
    uint32       f34;          // +0x34
    uint32       f38;          // +0x38
    uint32       f3C;          // +0x3C
    void*        f40;          // +0x40 objeto con destructor virtual (borrado con bandera 1)
    void*        f44;          // +0x44 ídem
    void*        f48;          // +0x48 objeto con destructor 0x76B87F
    void*        f4C;          // +0x4C objeto con destructor 0x76B4FD
    void*        reader;       // +0x50 lector (0x70 bytes); destructor 0x76B5EF
    void*        readerCopy;   // +0x54
    void*        loaderArg;    // +0x58
    uint32       pad5C[9];     // +0x5C..+0x7F
    uint32       f80;          // +0x80
    char*        savedLocale;  // +0x84 setlocale(LC_NUMERIC, 0) duplicado
    uint32       savedFpCtrl;  // +0x88 _controlfp(0, 0)

    // 0x76B89B: devuelve this
    D3dxShaderSource* Constructor();
    // 0x76B940
    void Destructor();
    // 0x76DFD1 (ret 0x10): carga desde memoria. Devuelve HRESULT.
    int32 InitFromMemory(uintptr_t data, uintptr_t size, uintptr_t defines, uintptr_t include);
    // 0x76DF03 (ret 0x10): carga desde fichero (usa 0x76B60B). Devuelve HRESULT.
    int32 InitFromFile(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3);
    // 0x76DF6C (ret 0x14): carga desde recurso (usa 0x76B7B2). Devuelve HRESULT.
    int32 InitFromResource(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4);
    static void InjectHooks();
};
static_assert(sizeof(D3dxShaderSource) == 0x8C);
static_assert(offsetof(D3dxShaderSource, errors) == 0x04);
static_assert(offsetof(D3dxShaderSource, f24) == 0x24);
static_assert(offsetof(D3dxShaderSource, reader) == 0x50);
static_assert(offsetof(D3dxShaderSource, loaderArg) == 0x58);
static_assert(offsetof(D3dxShaderSource, f80) == 0x80);
static_assert(offsetof(D3dxShaderSource, savedFpCtrl) == 0x88);
