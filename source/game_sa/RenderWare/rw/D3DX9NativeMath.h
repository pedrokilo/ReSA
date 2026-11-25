#pragma once

#include <cstdint>

// FnD3DX9C: matematica de D3DX9 en C++ propio (sin d3dx9_43.dll). Sustituye a las cinco funciones que el juego usaba a traves de la DLL del SDK de 2010
// (D3DXFloat16To32Array, D3DXVec3TransformCoord, D3DXVec3TransformNormal, D3DXMatrixMultiply, D3DXMatrixTranspose); D3DXMatrixMultiplyTranspose ya era propia
// (D3DX9MatrixMultiplyTransposeSse.h). Contrato: mismos bits que la DLL (ver reversing/FnD3DX9C/README.md, banco contra d3dx9_43.dll).
// Matrices 4x4 de 16 floats en filas (convencion D3DX, vector fila). Todas leen la entrada completa antes de escribir: `out` puede coincidir con una entrada.
namespace D3DX9Native {
float* Float16To32Array(float* out, const uint16_t* in, uint32_t count);
void*  Vec3TransformCoord(void* out, const void* v, const void* matrix);
void*  Vec3TransformNormal(void* out, const void* v, const void* matrix);
void*  MatrixMultiply(void* out, const void* a, const void* b);
void*  MatrixTranspose(void* out, const void* in);
} // namespace D3DX9Native
