#pragma once
// Fixtures del banco CamSphere76 (lote 76): CCamera::IsSphereVisible(const CVector& origen, float radio) (EXE 0x420D40, thiscall, ECX = camara, 2 argumentos en pila (CVector*, float como dword), `ret 8`, bool en AL).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCamera::IsSphereVisible(const CVector&, float)  0x420D40  (hook CCamera / IsSphereVisible-, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n < 16384: familia = n % 16 (1024 casos por familia); despues las familias 16 y 17 (1024 cada una).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cs76Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamX1 = 1024;        // familia 16
constexpr unsigned kFamX2 = 1024;        // familia 17
constexpr unsigned kCases = kBaseCases + kFamX1 + kFamX2;
constexpr unsigned kFamilies = 18;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La camara: +0x970 bool, +0xA4C CMatrix, +0xA94 CMatrix (hasta +0xADC); ventana de 0xB00. El origen: 3 floats (0x10).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kCamWin = 0xB00, kOrgWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kCamArena = Arena(kCamWin), kOrgArena = Arena(kOrgWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 76176u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const u32* v, std::size_t n) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < n; ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']';
        return *this;
    }
    std::string End() const { return s + "}"; }
};

// NaN con signo, carga util y tipo (QNaN/SNaN) arbitrarios; `kind`: 0 QNaN, 1 SNaN, otro = aleatorio.
inline u32 NanBits(u32 c, u32 k, u32 kind) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1);
    u32 payload;
    switch (s % 6) {
    case 0: payload = 0; break;
    case 1: payload = 1 + (r % 0xFF); break;
    case 2: payload = 0x3FFFFFu; break;
    case 3: payload = 0x200000u | (r & 0xFFu); break;
    case 4: payload = 0x1FFFFFu; break;
    default: payload = (r >> 3) & 0x3FFFFFu; break;
    }
    const bool quiet = kind == 0 ? true : kind == 1 ? false : ((r >> 30) & 1) != 0;
    if (!quiet && payload == 0) payload = 1;   // un sNaN exige carga util distinta de cero
    return ((r >> 31) << 31) | 0x7F800000u | (quiet ? 0x400000u : 0u) | payload;
}
// Patron de 32 bits "interesante" para un float: especiales, NaN de todos los tipos, desnormales, +-0, +-inf, extremos y aleatorios.
inline u32 Special(u32 c, u32 k) {
    static constexpr u32 sp[]{0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x7F7FFFFFu, 0xFF7FFFFFu,
                              0x00800000u, 0x80800000u, 0x7FC00000u, 0xFFC00000u, 0x7FA00000u, 0xFFA00000u, 0x7F800001u, 0xFF800001u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x40490FDBu, 0x3F000000u};
    const u32 r = Rand(c, k) % 10;
    if (r < 4) return sp[Rand(c, k + 1) % std::size(sp)];
    if (r == 4) return NanBits(c, k + 2, 1);
    if (r == 5) return NanBits(c, k + 3, 0);
    if (r == 6) return NanBits(c, k + 4, 2);
    if (r == 7) return B(float(i32(Rand(c, k + 5) % 2001) - 1000) / 8.0f);
    return Rand(c, k + 6);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// ------------------------------------------------------------------------------------------------ el caso
// Camara: arena rellena de palabras aleatorias con el byte +0x970 (m_bMirrorActive) fijado. El falso de 0x420C40 devuelve en AL res1 (primera llamada) y res2 (segunda), con basura en el resto de EAX.
// originKind: 0 = arena propia del origen; 1..4 = dentro de la arena de la camara en +0x0, +0x970, +0xA4C, +0xA94 (alias con lo que el falso lee).
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 mirror{}, mutate{}, mutVal{};
    u32 res1{}, res2{}, hi1{}, hi2{};
    u32 originKind{}, origin[3]{}, radius{};
};

inline C Make(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / kFamX1;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.mirror = Rand(n, 3) % 2; c.res1 = Rand(n, 4) % 2; c.res2 = Rand(n, 5) % 2;
    c.hi1 = Rand(n, 6) % 2 ? 0u : Rand(n, 8) & 0xFFFFFF00u; c.hi2 = Rand(n, 9) % 2 ? 0u : Rand(n, 10) & 0xFFFFFF00u;
    for (int i = 0; i < 3; ++i) c.origin[i] = Rand(n, 11 + i) % 2 ? Special(n, 20 + i) : B(float(i32(Rand(n, 30 + i) % 20001) - 10000) / 16.0f);
    c.radius = Rand(n, 14) % 2 ? B(float(Rand(n, 15) % 4000) / 32.0f) : Special(n, 16);
    switch (c.family) {
    case 0:   // sin espejo, la primera llamada acierta
        c.mirror = 0; c.res1 = 1;
        break;
    case 1:   // sin espejo, la primera falla
        c.mirror = 0; c.res1 = 0;
        break;
    case 2:   // espejo, la primera acierta (no se llama a la segunda)
        c.mirror = 1; c.res1 = 1;
        break;
    case 3:   // espejo, la primera falla, la segunda acierta
        c.mirror = 1; c.res1 = 0; c.res2 = 1;
        break;
    case 4:   // espejo, fallan las dos
        c.mirror = 1; c.res1 = 0; c.res2 = 0;
        break;
    case 5:   // el byte del espejo con todos los valores
        c.mirror = Rand(n, 40) % 4 == 0 ? Rand(n, 41) % 2 : Rand(n, 42) & 0xFF; c.res1 = Rand(n, 43) % 3 == 0 ? 0u : 1u + Rand(n, 48) % 255;
        break;
    case 6:   // el falso de la primera llamada cambia el byte del espejo: se relee DESPUES
        c.mutate = 1; c.res1 = 0; c.mirror = Rand(n, 44) % 2 ? 0u : 1u + Rand(n, 45) % 255; c.mutVal = Rand(n, 46) % 2 ? 0u : 1u + Rand(n, 47) % 255;
        break;
    case 7:   // radio con especiales
        c.radius = Special(n, 50); c.mirror = Rand(n, 51) % 2; c.res1 = Rand(n, 52) % 2;
        break;
    case 8:   // origen con especiales
        for (int i = 0; i < 3; ++i) c.origin[i] = Special(n, 60 + i);
        c.mirror = Rand(n, 53) % 2; c.res1 = Rand(n, 54) % 2;
        break;
    case 9:   // origen dentro de la propia camara
        c.originKind = 1 + Rand(n, 55) % 4; c.mirror = Rand(n, 56) % 2; c.res1 = Rand(n, 57) % 2;
        break;
    case 10:  // basura en el resto de EAX del falso
        c.hi1 = Rand(n, 58) | 0x100u; c.hi2 = Rand(n, 59) | 0x100u; c.mirror = Rand(n, 63) % 2; c.res1 = Rand(n, 64) % 2;
        break;
    case 11:  // radio: cero, negativo, infinito, desnormal, NaN de todos los tipos
        c.radius = Special(n, 65); c.mirror = 1; c.res1 = 0; c.res2 = Rand(n, 66) % 2;
        break;
    case 12:  // los bytes de la camara cercanos a +0x970 especiales
        c.mirror = Rand(n, 67) & 0xFF; c.res1 = Rand(n, 68) % 2;
        break;
    case 13:  // registros y semillas de pila
        c.mirror = Rand(n, 69) % 2; c.res1 = Rand(n, 70) % 2; c.eax = SeedReg(n, 71); c.edx = SeedReg(n, 72);
        break;
    case 14:  // mutacion y radio especial
        c.mutate = 1; c.mutVal = Rand(n, 73) & 0xFF; c.radius = Special(n, 74); c.res1 = 0; c.mirror = Rand(n, 75) & 0xFF;
        break;
    case 15:  // mezcla
        c.mutate = Rand(n, 76) % 4 == 0; c.mutVal = Rand(n, 77) & 0xFF; c.originKind = Rand(n, 78) % 3 == 0 ? Rand(n, 79) % 5 : 0u;
        c.mirror = Rand(n, 80) & 0xFF;
        break;
    case 16: {   // extras: byte del espejo (256 valores) x res1 x res2 x mutacion
        const u32 j = n - kBaseCases;
        c.mirror = j % 256; c.res1 = (j / 256) % 2; c.res2 = (j / 512) % 2; c.mutate = Rand(n, 81) % 8 == 0; c.mutVal = Rand(n, 82) & 0xFF;
        break;
    }
    default: {   // 17: radio con NaN de todos los tipos x espejo x resultados
        const u32 j = n - kBaseCases - kFamX1;
        c.radius = NanBits(n, 83, 2 + (j % 3)); if (j % 7 == 0) c.radius = Special(n, 85);
        c.mirror = (j / 3) % 2; c.res1 = (j / 6) % 2; c.res2 = (j / 12) % 2;
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("mirror", c.mirror).U("mutate", c.mutate).U("mut_val", c.mutVal).U("res1", c.res1).U("res2", c.res2).U("hi1", c.hi1).U("hi2", c.hi2);
    j.U("origin_kind", c.originKind).A("origin", c.origin, 3).U("radius", c.radius);
    return j.End();
}
}
