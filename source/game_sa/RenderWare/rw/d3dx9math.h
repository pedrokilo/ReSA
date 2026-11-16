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
