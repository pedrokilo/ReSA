#pragma once

// D3DX9 enlazado estaticamente en el EXE (matematicas con variantes 3DNow! y SSE). Lote R4A (sin validar).
// Todas son __stdcall como la API publica de D3DX9 (el EXE las cierra con `ret 8` / `ret 0Ch`).
// No son codigo del juego ni de RenderWare; ver reversing/PendienteValidar/R4A.md.

void D3DX9InjectHooks();

// 0x7C1D80: D3DXFloat16To32Array, variante 3DNow!/MMX. Convierte `count` medios flotantes a float; devuelve `out`.
float* __stdcall D3DXFloat16To32Array_3DNow(float* out, const uint16* in, uint32 count);

// 0x7C203B: D3DXVec2Transform, variante 3DNow!. out(4 floats) = (x, y, 0, 1) * matriz 4x4. Devuelve `out`.
float* __stdcall D3DXVec2Transform_3DNow(float* out, const float* v, const float* matrix);

// 0x7C2090: D3DXVec2TransformNormal, variante 3DNow!. out(2 floats) = (x, y) * matriz 2x2 superior izquierda. Devuelve `out`.
float* __stdcall D3DXVec2TransformNormal_3DNow(float* out, const float* v, const float* matrix);

// 0x7C2AAE: acos de 4 floats alineados a 16 (SSE). `in` y `out` deben estar alineados a 16 bytes.
void __stdcall D3DX9AcosPS(const float* in, float* out);

// 0x7C2CA4: 2 * acos(valor) escalar (SSE, mismo polinomio que 0x7C2AAE; escribe un solo float en `out`).
void __stdcall D3DX9TwoAcosScalar(float value, float* out);

// 0x7C307E y 0x7C381F: seno y coseno de 4 floats alineados a 16 (SSE; el primero con enteros por memoria, el segundo con SSE2). Devuelven `outSin`.
float* __stdcall D3DX9SinCosPS(const float* in, float* outSin, float* outCos);
float* __stdcall D3DX9SinCosPS_SSE2(const float* in, float* outSin, float* outCos);

// Lote R21A (sin validar): ensamblador y compilador de shaders de D3DX9 enlazados en el EXE (0x7652AE..0x7654B8). Todas son __stdcall como la API publica de D3DX9 y devuelven un HRESULT
// (0x88760B59 si el objeto de errores no queda vacio). Nombres de la API publica inferidos del numero de argumentos y de las constantes (0 = ANSI, 1 = Unicode en 0x76DF03).
// 0x7652AE: D3DXAssembleShader(pSrcData, SrcDataLen, pDefines, pInclude, Flags, ppShader, ppErrorMsgs).
int32 __stdcall D3DXAssembleShader(const void* srcData, uint32 srcDataLen, const void* defines, void* include, uint32 flags, void** shader, void** errorMsgs);
// 0x76534D: D3DXCompileShaderFromFileA(pSrcFile, pDefines, pInclude, pFunctionName, pTarget, Flags, ppShader, ppErrorMsgs, ppConstantTable).
int32 __stdcall D3DXCompileShaderFromFileA(const char* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable);
// 0x765402: D3DXCompileShaderFromFileW (los mismos argumentos con la ruta Unicode).
int32 __stdcall D3DXCompileShaderFromFileW(const wchar_t* srcFile, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable);
// 0x7654B8: D3DXCompileShaderFromResourceA(hSrcModule, pSrcResource, pDefines, pInclude, pFunctionName, pTarget, Flags, ppShader, ppErrorMsgs, ppConstantTable).
int32 __stdcall D3DXCompileShaderFromResourceA(void* srcModule, const char* srcResource, const void* defines, void* include, const char* functionName, const char* target, uint32 flags, void** shader, void** errorMsgs, void** constantTable);
