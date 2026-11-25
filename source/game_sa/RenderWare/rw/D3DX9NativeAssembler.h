#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// FnD3DX9C: ensamblador de vertex shaders propio para la gramatica que genera el juego (_rpD3D9VertexShaderCreate, 0x75F0B0): cabecera vs_2_0 / vs_2_x, declaraciones dcl_*,
// y las instrucciones mov, mova, add, mul, mad, dp3, dp4, rsq, rcp, expp, max, min, slt, sge, sub, m3x3, m4x3, m4x4 con registros v/r/c/a0, c[N], c[a0.x + N], c[A + B],
// negacion, mascaras de escritura y permutaciones. Produce el mismo bytecode que D3DXAssembleShader de d3dx9_43.dll (banco: reversing/FnD3DX9C/banco_ensamblador.cpp).
// Cualquier otra cosa (vs_1_1, vs_3_0, ps_*, def, comentarios, macros, registros fuera de rango...) devuelve Unsupported para que el llamador use la DLL (D3DX9System).
namespace D3DX9Native {
enum class AssembleResult { Ok, Unsupported };
// `bytecode` recibe los DWORD del shader (version, instrucciones, fin 0x0000FFFF). `reason` explica un Unsupported (diagnostico).
AssembleResult AssembleVertexShader(const char* source, size_t length, std::vector<uint32_t>& bytecode, std::string& reason);
} // namespace D3DX9Native
