#pragma once
// Fixtures del banco Fn238A (lote 238A): tipos y utilidades C++ estandar compartidas por Core.hpp y LiveComparison.hpp.
// Los casos NO se generan aqui: los genera fixture_cases.py (Python) en un archivo de lineas JSON (fn238a-cases.jsonl, junto al EXE de pruebas) que el banco lee tal cual; el oraculo
// independiente (fixture_oracle.py) parte de esas mismas lineas. Dos funciones del EXE:
//   idx 0  0x4012E9 ExeProtectionStub::MD5Transform(estado*, bloque*)           cdecl, sin retorno
//   idx 1  0x496E00 CRunningScript::ProcessCommands1700To1799(this, comando)    thiscall (ECX = script), un argumento de pila, `ret 4`, resultado en AL
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

namespace F238AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}
constexpr unsigned kFunctions = 2;
constexpr unsigned kFamilies = 16;
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
// Especificacion de puntero a una arena: 0xFA000000 | (arena << 16) | desplazamiento (un float de ese patron seria ~ -1.7e35: el generador nunca lo produce)
constexpr bool IsPS(u32 v) { return (v >> 24) == 0xFAu && ((v >> 16) & 0xFFu) < 4u; }
constexpr u32 kCases = 16384;      // por funcion
}
