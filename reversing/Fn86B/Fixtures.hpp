#pragma once
// Fixtures del banco Fn86B (lote 86B): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPopulation::CreateSunbatheObject                        0x6EABA0  cdecl (const CVector* posicion, int32 tipo); devuelve EAX = objeto o 0   (hook CPopulation / CreateSunbatheObject, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F86BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr unsigned kModels = 9;         // words de modelo: 0x8CD638, 0x8CD67C, 0x8CD680, 0x8CD684, 0x8CD688, 0x8CD68C, 0x8CD690, 0x8CD694, 0x8CD698
inline constexpr std::array<std::uintptr_t, kModels> kModelAddr{0x8CD638, 0x8CD67C, 0x8CD680, 0x8CD684, 0x8CD688, 0x8CD68C, 0x8CD690, 0x8CD694, 0x8CD698};

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x180, kMatWin = 0x50, kRwWin = 0x10, kFrameWin = 0x60, kPosWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kObjArena = Arena(kObjWin), kMatArena = Arena(kMatWin), kRwArena = Arena(kRwWin), kFrameArena = Arena(kFrameWin), kPosArena = Arena(kPosWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 86008u); }
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

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN silencioso (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, ecx{}, stack{};
    u32 type{};                         // argumento 2 (int32)
    u32 pos[3]{};                       // posicion pedida (floats)
    u32 count{};                        // word de CObject::nNoTempObjects (0xBB4A70)
    u32 time{};                         // CTimer::m_snTimeInMilliseconds (0xB7CB84)
    u32 models[kModels]{};              // words de modelo (0..65535)
    u32 randv{};                        // eax que devuelve el rand() falso
    u32 newnull{};                      // la reserva falla (operator new devuelve nulo)
    u32 hasmat{};                       // el objeto construido tiene matriz (+0x14)
    u32 hasrw{};                        // el objeto construido tiene objeto RW (+0x18)
    u32 gate{};
};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Pk = [&](std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); };
    for (u32 k = 0; k < 3; ++k) c.pos[k] = B(float(int(R(200001)) - 100000) / 64.0f);
    c.count = P(15) ? Pk({0u, 1u, 143u, 144u, 145u, 146u, 149u, 150u, 151u, 0xFFFFu, 0x8000u}) : R(160);
    c.time = P(20) ? Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFFFFFFu - 43200000u, 0xFFFFFFFFu - 43199999u, 0xFFFFFFFFu - 43200001u, 0x80000000u, 43200000u}) : Rand(n, nk());
    for (u32 i = 0; i < kModels; ++i) c.models[i] = (Rand(n, nk()) & 0xFFFFu);
    c.randv = Rand(n, nk());
    c.hasmat = R(2); c.hasrw = R(2);
    c.newnull = P(4) ? 1u : 0u;
    c.type = 5u;
    switch (f) {
    case 0: c.type = 5u; c.count = R(145); break;
    case 1: c.type = 11u; c.count = R(145); break;
    case 2: c.type = 11u; c.count = 140u + R(14); break;
    case 3: c.type = P(50) ? 5u : 6u; c.count = Pk({146u, 147u, 148u, 149u, 150u, 151u, 152u, 0xFFFFu, 0x8000u, 149u, 150u}); break;
    case 4: c.type = 1u + R(10); c.count = R(150); break;
    case 5: c.type = Pk({0u, 0xFFFFFFFFu, 12u, 13u, 100u, 0x80000000u, 0x7FFFFFFFu, 0x80000005u, 0x8000000Bu, 0xFFFFFFFAu, 0xFFFFFFF5u}); c.count = R(150); break;
    case 6: c.newnull = P(70) ? 1u : 0u; c.type = Pk({5u, 11u, 6u, 2u, 7u, 1u, 10u, 11u, 5u}); c.count = R(145); break;
    case 7: c.hasmat = 1; c.hasrw = 1; c.type = Pk({5u, 11u, 6u, 3u, 8u}); c.count = R(145); break;
    case 8: c.hasmat = 0; c.hasrw = 1; c.type = Pk({5u, 11u, 6u, 3u, 8u}); c.count = R(145); break;
    case 9: c.hasmat = 1; c.hasrw = 0; c.type = Pk({5u, 11u, 6u, 3u, 8u}); c.count = R(145); break;
    case 10: c.hasmat = 0; c.hasrw = 0; c.type = Pk({5u, 11u, 6u, 3u, 8u}); c.count = R(145); break;
    case 11:
        c.type = Pk({5u, 11u, 6u, 2u, 7u}); c.count = R(145);
        for (u32 k = 0; k < 3; ++k) if (P(45)) c.pos[k] = Special(n, nk());
        break;
    case 12:
        c.type = Pk({5u, 11u, 6u, 2u, 3u, 4u, 7u, 8u, 9u, 10u}); c.count = R(145);
        for (u32 i = 0; i < kModels; ++i) if (P(40)) c.models[i] = Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, 0x00FFu, 0x0100u});
        break;
    case 13:
        c.type = Pk({5u, 11u, 6u, 2u, 10u}); c.count = R(145);
        c.time = Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFFFFFFu - 43200000u, 0xFFFFFFFFu - 43199999u, 0xFFFFFFFFu - 43200001u, 0x80000000u, 43200000u, 0x7FFFFFFFu, 0xFD6CD200u});
        break;
    case 14: c.type = Pk({5u, 11u, 6u, 2u, 3u, 4u, 7u, 8u, 9u, 10u, 1u}); c.count = R(150); c.ecx = Pk({0u, 0xFFFFFFFFu, c.ecx}); c.edx = Pk({0u, 0xFFFFFFFFu, c.edx}); break;
    default:
        c.type = P(60) ? Pk({5u, 11u, 6u, 2u, 3u, 4u, 7u, 8u, 9u, 10u, 1u}) : R(16);
        break;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("type", c.type).A("pos", std::vector<u32>(c.pos, c.pos + 3)).U("count", c.count).U("time", c.time).A("models", std::vector<u32>(c.models, c.models + kModels));
    j.U("randv", c.randv).U("newnull", c.newnull).U("hasmat", c.hasmat).U("hasrw", c.hasrw).U("gate", c.gate);
    return j.End();
}
}
