#pragma once
// Fixtures del banco D3DResource63 (lote 63): D3DResourceSystem::GetTotalPixelsSize (0x730660) y GetTotalIndexDataSize (0x7307F0), 4096 casos cada una. Solo C++ estandar
// (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   D3DResourceSystem::GetTotalPixelsSize     0x730660  cdecl sin argumentos, uint32 en EAX: suma D3DTextureBuffer::GetTotalDataSize (0x7300A0) de los NumTextureBuffers buffers
//   D3DResourceSystem::GetTotalIndexDataSize  0x7307F0  cdecl sin argumentos, uint32 en EAX: suma D3DIndexDataBuffer::GetTotalDataSize (0x7303B0) de los 16 buffers y del generico
//
// El caso especifica el contador (palabra 0 del bloque 0xC87C60), los valores que devuelve el falso del colaborador (modo + semilla, ver RetVal) y una reentrada opcional:
// el k-esimo falso escribe una palabra del bloque de globales (el EXE lee el contador una sola vez; la DLL tampoco debe releerlo).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace D3D63Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 2;
constexpr u32 kPixels = 0, kIndex = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

// Bloque de globales del EXE: 0xC87C60 + 4 * palabra (palabra 0 = NumTextureBuffers).
constexpr u32 kRegionBase = 0xC87C60;
constexpr u32 kRegionWords = 226;            // 0xC87C60 .. 0xC87FE8
constexpr u32 kTexBufferBase = 0xC87C68;     // TextureBuffers[0]
constexpr u32 kTexStride = 0x1C;             // sizeof(D3DTextureBuffer)
constexpr u32 kIbBufferBase = 0xC87E48;      // IndexDataBuffers[0]
constexpr u32 kIbStride = 0x18;              // sizeof(D3DIndexDataBuffer)
constexpr u32 kIbBuffers = 16;
constexpr u32 kGenericIb = 0xC87FC8;         // IndexDataBuffer (generico)
constexpr u32 kGenWord = (kGenericIb - kRegionBase) / 4;   // 218
static_assert(kGenWord == 218 && kIbBufferBase + kIbBuffers * kIbStride == kGenericIb, "disposicion de los index buffers");
static_assert(kRegionBase + 4 * kRegionWords == 0xC87FE8, "tamano del bloque");
constexpr u32 kMaxEvents = 520;              // maximo de llamadas al falso por caso (ntex <= 512)
constexpr u32 kGarbageSeed = 0x6A09E667u;

inline u32 Garbage(u32 n, u32 k) {   // basura que nunca coincide con un valor que escriban las funciones (dos bits altos a 1, y nunca 0xFFFFFFFF)
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// Valor que devuelve el falso en su llamada k-esima (k desde 0), segun el modo del caso.
inline u32 RetVal(u32 mode, u32 seed, u32 k) {
    const u32 r = Rand(seed, k + 1);
    switch (mode) {
    case 0: return 0;
    case 1: return 1;
    case 2: return r & 0xFFFu;                       // pequenos: la suma no desborda
    case 3: return r;                                // 32 bits aleatorios: la suma da la vuelta
    case 4: return 0xFFFFFFFFu;                      // todo unos
    case 5: return 0x80000000u;                      // un bit: la suma da la vuelta a 0
    case 6: return (k & 1) ? 0xFFFFFFFFu : 1u;       // alterna
    default: return (r & 3) == 0 ? 0u : (r >> 4);    // mezcla
    }
}

struct Spec {
    u32 fn{};
    u32 ntex{};                          // contenido de NumTextureBuffers (solo lo lee GetTotalPixelsSize)
    u32 mode{}, seed{};                  // valores del falso
    u32 reK{0xFFFFFFFFu}, reKind{}, reVal{};   // reentrada: en la llamada reK (de 0) se escribe reVal en: 1 = palabra 0 (contador); 2 = campo size del generico; 3 = palabra 100
    u32 stack{}, eax{}, ecx{}, edx{};
    u32 family{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

constexpr u32 kFamilies = 12;

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + fn * 0x20000u);
    s.family = n % kFamilies;
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    s.seed = g.R();
    s.mode = g.P(8);
    switch (s.family) {
    case 0: s.ntex = 0; break;
    case 1: s.ntex = 1; break;
    case 2: s.ntex = 2 + g.P(15); break;                  // 2..16
    case 3: s.ntex = 17; break;                           // todos los buffers del arreglo
    case 4: s.ntex = 18 + g.P(47); break;                 // fuera del arreglo (18..64)
    case 5: s.ntex = 65 + g.P(448); break;                // 65..512
    case 6: s.ntex = g.P(18); s.mode = 3; break;          // sumas con vuelta
    case 7: s.ntex = 1 + g.P(17); s.mode = 4 + g.P(3); break;
    case 8: s.ntex = 1 + g.P(24); break;                  // con reentrada del contador
    case 9: s.ntex = 1 + g.P(24); break;                  // con reentrada del tamano del generico / otra palabra
    case 10: s.ntex = g.P(5); s.mode = 6; break;
    default: s.ntex = g.C(50) ? 17u : g.P(40); break;
    }
    if (s.family == 8 || s.family == 9 || (s.family == 11 && g.C(50))) {
        s.reKind = s.family == 8 ? 1u : (g.C(50) ? 2u : 3u);
        s.reK = g.P(s.fn == kPixels ? (s.ntex ? s.ntex : 1u) : 17u);
        s.reVal = g.C(25) ? s.ntex : (g.C(50) ? 0u : 1u + g.P(1000));
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonSpec(const Spec& s) {
    return "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"ntex\":" + U(s.ntex) + ",\"mode\":" + U(s.mode) + ",\"seed\":" + U(s.seed) + ",\"re\":[" + U(s.reK) + "," + U(s.reKind) + "," +
           U(s.reVal) + "],\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.ecx) + "," + U(s.edx) + "]}";
}
}  // namespace D3D63Fix
