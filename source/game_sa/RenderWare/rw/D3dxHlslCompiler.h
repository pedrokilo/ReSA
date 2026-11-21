#pragma once

// Objeto auxiliar del compilador HLSL de D3DX9 enlazado en el EXE (lote NEWDEL, ver reversing/FnNEWDEL/README.md). Solo lo usan las rutas D3DXCompileShader* (que el
// juego no llama). __thiscall con `this` en ECX. Layout parcial: +0x04 y +0x08 objetos con destructor virtual (bandera 1), +0x18 bloque propio (operator delete).
struct D3dxHlslCompiler {
    uint32 f00;       // +0x00
    void*  obj04;     // +0x04
    void*  obj08;     // +0x08
    uint32 pad0C[3];  // +0x0C..+0x17
    void*  block18;   // +0x18
    uint32 pad1C[6];  // +0x1C..+0x33
    uint32 f34;       // +0x34
    uint32 f38;       // +0x38
    uint32 pad3C[5];  // +0x3C..+0x4F
    uint32 f50;       // +0x50

    // 0x773022: pone a cero los campos +0, +4, +8, +0x18, +0x34, +0x38 y +0x50; devuelve this
    D3dxHlslCompiler* Constructor();
    // 0x77303B: borra +4 y +8 con su destructor virtual y libera +0x18
    void Destructor();

    static void InjectHooks();
};
static_assert(offsetof(D3dxHlslCompiler, block18) == 0x18);
static_assert(offsetof(D3dxHlslCompiler, f50) == 0x50);

// 0x77EE77 (__stdcall, ret 0x20, 8 argumentos): cuerpo de D3DXCompileShader*; pone a cero *a6 y *a7 si no son nulos, devuelve D3DERR_INVALIDCALL si a2 es nulo y si no
// llama a 0x77EB3C (11 argumentos: a0, a1, a2, 0, 0, 0, a3, a4, a5, a6, a7).
int32 __stdcall D3dxCompileShaderEntry(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uint32* a6, uint32* a7);
