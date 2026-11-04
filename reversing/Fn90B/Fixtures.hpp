#pragma once
// Fixtures del banco Fn90B (lote piloto 90B, categoria "pequenas sin FPU"): ocho funciones de juego pequenas, 4096 casos cada una (32768 pares). Solo C++ estandar (sin tipos del juego).
//
//   0 CNodeAddress::operator==                  0x420920  thiscall (this en ECX, otra direccion), `ret 4`; AL = (area == area') && (nodo == nodo')           (hook CNodeAddress / operator==, tipo S)
//   1 CNodeAddress::operator!=                  0x420950  thiscall, `ret 4`; AL = !(igual)                                                                   (hook CNodeAddress / operator!=, tipo S)
//   2 CNodeAddress::operator<                   0x420980  thiscall, `ret 4`; AL = orden lexicografico SIN signo (area y luego nodo)                          (hook CNodeAddress / operator<, tipo S)
//   3 CWorld::AdvanceCurrentScanCode            0x4072E0  cdecl sin argumentos, `ret`; si el word 0xB7CD78 >= 0xFFFF llama a ClearScanCodes (0x563470) y lo deja en 1; si no, lo incrementa  (hook CWorld / AdvanceCurrentScanCode, tipo S)
//   4 CTaskSimpleScratchHead::CTaskSimpleScratchHead   0x48DF30  thiscall (objeto en ECX), `ret`; devuelve this en EAX                                       (hook CTaskSimpleScratchHead / Constructor, tipo S)
//   5 CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading 0x667E20  thiscall (objeto en ECX, rumbo, ritmo, tolerancia), `ret 0Ch`; devuelve this         (hook CTaskSimpleAchieveHeading / Constructor, tipo S)
//   6 CColModel::CColModel                      0x40FB60  thiscall (objeto en ECX), `ret`; devuelve this                                                     (hook CColModel / Constructor, tipo S)
//   7 CEventDeath::CEventDeath(bool)            0x4ADDF0  thiscall (objeto en ECX, bool), `ret 4`; devuelve this; lee CTimer::m_snTimeInMilliseconds (0xB7CB84)  (hook CEventDeath / Constructor, tipo S)
//
// Familia = n % 16 (256 casos por familia).
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace F90Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;       // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 8;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x40;      // ventana del objeto (los constructores escriben como mucho 0x30 bytes: lo de mas se vigila)
constexpr std::size_t kArena = kGuard + kWin + kGuard;

constexpr u32 kScanAddr = 0xB7CD78;     // CWorld::ms_nCurrentScanCode (word; el word alto es otro dato)
constexpr u32 kTimeAddr = 0xB7CB84;     // CTimer::m_snTimeInMilliseconds

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 90001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno del objeto por modo: 0 ruido, 1 ceros, 2 unos, 3 0xCD, 4 0xA5, 5 ruido con ceros dispersos
inline void FillMode(u8* dst, std::size_t bytes, u32 seed, u32 mode) {
    switch (mode % 6) {
    case 1: std::memset(dst, 0x00, bytes); break;
    case 2: std::memset(dst, 0xFF, bytes); break;
    case 3: std::memset(dst, 0xCD, bytes); break;
    case 4: std::memset(dst, 0xA5, bytes); break;
    case 5: FillWords(dst, bytes, seed); for (std::size_t i = 0; i < bytes; ++i) if (Rand(seed, u32(i) + 500u) % 3 == 0) dst[i] = 0; break;
    default: FillWords(dst, bytes, seed); break;
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
};

inline constexpr std::array<u32, 12> kU16{0u, 1u, 2u, 0x7FFFu, 0x8000u, 0x8001u, 0xFFFEu, 0xFFFFu, 0x00FFu, 0x0100u, 0xFF00u, 0x1234u};
inline constexpr std::array<u32, 14> kSpecialF{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7FC12345u, 0x7FA00000u, 0xFFBFFFFFu, 0x7F7FFFFFu, 0xFF7FFFFFu};
inline constexpr std::array<u32, 6> kScanNear{0x7FFFu, 0x8000u, 0x8001u, 0xFFFFu, 0xFFFEu, 0x00FFu};
inline constexpr std::array<u32, 5> kTimes{1u, 0x80000000u, 0x7FFFFFFFu, 0x00010000u, 0x000000FFu};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }

// El caso (los campos de cada funcion son independientes)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 fillA{}, fillSeed{};          // modo de relleno del objeto (arena A)
    u32 a{}, b{};                     // 0..2: (area | nodo << 16) de this y de la otra; alias = la otra es this
    u32 alias{};
    u32 scan{};                       // dword inicial de 0xB7CD78 (word bajo = codigo de escaneo)
    u32 time{};                       // dword inicial de 0xB7CB84
    u32 f0{}, f1{}, f2{};             // 5: rumbo, ritmo, tolerancia (patrones de 32 bits)
    u32 drown{};                      // 7: argumento bool (dword apilado)
};

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    c.fillSeed = Rand(n, 11 + fn);
    c.fillA = Rand(n, 12 + fn) % 6;
    c.scan = Rand(n, 13 + fn); c.time = Rand(n, 14 + fn);
    const u32 f = c.family;
    u32 kk = 100 + fn * 50;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto U16 = [&]() -> u32 { return P(55) ? Pick(kU16, n, nk()) : R(0x10000); };
    switch (fn) {
    case 0: case 1: case 2: {   // comparaciones de CNodeAddress
        u32 a0 = U16(), a1 = U16(), b0 = a0, b1 = a1;
        if (f < 4) {                    // iguales
        } else if (f < 6) {             // misma area, nodo distinto (+-1 o cualquiera)
            b1 = P(50) ? ((a1 + (P(50) ? 1u : 0xFFFFu)) & 0xFFFFu) : U16();
        } else if (f < 8) {             // mismo nodo, area distinta
            b0 = P(50) ? ((a0 + (P(50) ? 1u : 0xFFFFu)) & 0xFFFFu) : U16();
        } else if (f < 10) {            // distintos en ambos
            b0 = U16(); b1 = U16();
        } else if (f == 10) {           // la otra es this
            c.alias = 1;
        } else if (f == 11) {           // frontera del signo: 0x7FFF / 0x8000 en area o nodo (el orden sin signo difiere del orden con signo)
            const u32 lo = P(50) ? 0x7FFFu : 0x0000u, hi = P(50) ? 0x8000u : 0xFFFFu;
            const bool swap = P(50);
            if (P(50)) { a0 = swap ? hi : lo; b0 = swap ? lo : hi; a1 = U16(); b1 = P(50) ? a1 : U16(); }
            else { a1 = swap ? hi : lo; b1 = swap ? lo : hi; b0 = a0; }
        } else if (f == 12) {           // area con diferencia 1 y nodos cualesquiera
            b0 = (a0 + 1u) & 0xFFFFu; a1 = U16(); b1 = U16();
        } else if (f == 13) {           // misma area, nodos con orden contrario
            b1 = U16(); if (P(50)) std::swap(a1, b1);
        } else if (f == 14) {           // todo aleatorio de 16 bits
            a0 = R(0x10000); a1 = R(0x10000); b0 = R(0x10000); b1 = R(0x10000);
        } else {                        // extremos: 0, 0xFFFF y 0x8000 combinados
            const u32 e[3]{0u, 0xFFFFu, 0x8000u};
            a0 = e[R(3)]; a1 = e[R(3)]; b0 = e[R(3)]; b1 = e[R(3)];
        }
        c.a = a0 | (a1 << 16); c.b = b0 | (b1 << 16);
        if (c.alias) c.b = c.a;
        break;
    }
    case 3: {   // CWorld::AdvanceCurrentScanCode
        u32 w;
        if (f < 4) w = 0xFFFFu;
        else if (f < 6) w = 0xFFFEu;
        else if (f < 8) w = 0xFFFFu - R(4);
        else if (f < 10) w = R(3);
        else if (f < 12) w = Pick(kScanNear, n, nk());
        else w = R(0x10000);
        const u32 hi = P(40) ? 0u : (P(50) ? 0xFFFFu : R(0x10000));
        c.scan = w | (hi << 16);
        break;
    }
    case 4: case 6: break;           // constructores sin argumentos: solo cambia el relleno del objeto y los registros
    case 5: {   // CTaskSimpleAchieveHeading(rumbo, ritmo, tolerancia)
        auto Fl = [&]() -> u32 {
            if (f >= 8 && P(45)) return Pick(kSpecialF, n, nk());
            if (P(8)) return Pick(kSpecialF, n, nk());
            return B((float(R(20001)) - 10000.0f) / 1000.0f * (P(20) ? 100.0f : 1.0f));
        };
        c.f0 = Fl(); c.f1 = P(40) ? B(0.5f) : Fl(); c.f2 = P(40) ? B(0.2f) : Fl();
        break;
    }
    default: {  // 7 CEventDeath(bool)
        const u32 bit = P(50) ? 1u : 0u;
        c.drown = f < 12 ? bit : ((R(0x1000000)) << 8 | bit);   // las familias 12..15 apilan basura en los tres bytes altos
        if (f < 4) c.time = 0;
        else if (f < 6) c.time = 0xFFFFFFFFu;
        else if (f < 8) c.time = Pick(kTimes, n, nk());
        break;
    }
    }
    return c;
}

// JSON de la entrada sin llave final: CaseRun anade la ventana inicial del objeto y cierra.
inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("fill", c.fillA).U("scan", c.scan);
    switch (c.fn) {
    case 0: case 1: case 2: j.U("a", c.a).U("b", c.b).U("alias", c.alias); break;
    case 5: j.U("h", c.f0).U("rate", c.f1).U("tol", c.f2); break;
    case 7: j.U("drown", c.drown); break;
    default: break;
    }
    j.U("time", c.time);
    return j.s;
}
}
