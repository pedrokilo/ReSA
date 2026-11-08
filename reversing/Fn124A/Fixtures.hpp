#pragma once
// Fixtures del banco Fn124A (lote 124A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Trece funciones diminutas del EXE (de 12 a 27 bytes). Las clases del listado de pendientes se atribuian por cercania: las reales salen del desensamblado, de la vtable que escribe cada una y de sus
// llamadores (ver README.md y fixture_oracle.py).
//    0 BoneNode_c constructor de elemento      0x617850  thiscall, ret this; `ListItem_c::ListItem_c` (0x4A8DB0, real) pone a 0 [this] y [this+4]
//    1 CZone::GetTranslatedName                0x5720C0  thiscall; CText::Get(&TheText, this + 8); devuelve su resultado (colaborador 0x6A0050 con hook, falso)
//    2 GetNameOfSavedGame                      0x5D0F40  cdecl (int slot); 0xC16368 + slot * 0x104 en 32 bits
//    3 CTaskComplexBeStill constructor         0x5F6660  thiscall, ret this; CTaskComplex::Constructor (0x61A3B0, con hook, falso) y vtable 0x86C7BC
//    4 ceilf del CRT (CObject::CeilFloat)      0x538AD0  cdecl (float); ST0 = ceil((double)x) del CRT 0x823820 (real)
//    5 CTaskSimpleCarWaitToSlowDown::SetPedPosition 0x646AB0  thiscall (CPed*), `ret 4`, virtual (ranura 8, hook V); si [this+8] != 0 llama CPed::SetPedPositionInCar (0x5DF910, con hook, falso); AL = 1
//    6 CVector2D::Magnitude (fuera de linea)   0x420860  thiscall, ST0 = sqrt(x*x + y*y) en la x87 de 24 bits
//    7 GetOSWPerformanceTime                   0x5617A0  cdecl sin argumentos; QueryPerformanceCounter (IAT 0x8580C8; falso en la IAT del EXE y en la de la DLL); EDX:EAX = contador
//    8 BreakManager_c constructor              0x59E620  thiscall, ret this; 64 x BreakObject_c (0x59D170 real: bytes +0, +1 y dword +8 a 0) con el iterador CRT 0x821D30 (real)
//    9 CAEVorbisDecoder destructor con borrado 0x502700  thiscall, `ret 4`; 0x5023F0 (con hook, falso) y operator delete 0x8214BD (falso) si el bit 0 de las banderas
//   10 CAEWaveDecoder destructor con borrado   0x503430  igual con 0x503290
//   11 CWanted destructor con borrado          0x56F850  igual con 0x56F200 (real) -> 0x4E6E00 (con hook, falso; ECX = this + 0x21C)
//   12 CRealTimeShadow destructor con borrado  0x706850  igual con 0x706440 (con hook, falso)
//
// Cada caso se ejecuta por la entrada del EXE. `this` apunta a una arena propia (guardas de 0x40) en un desplazamiento alineado, desalineado o al borde de la ventana (la escritura entra en la guarda).
// Familia = n % 16: tipo = familia % 4 (0 alineado, 1 desalineado, 2 al borde de la ventana, 3 alineado con datos "interesantes") y relleno = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD);
// las funciones 2, 4 y 7 (sin `this`) y la 6 (tipo de valor) definen ademas sus propias 16 familias.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F124AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 13;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x1000, kAWin = 0x80;   // arena del objeto (this) y arena secundaria (ped / vehiculo / centinela)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kSlotNamesAddr = 0xC16368u;   // region vigilada de la funcion 2 (8 ranuras de 0x104 bytes)
constexpr std::size_t kSlotNamesBytes = 0x820;

// Extremo de lo que cada funcion lee o escribe a partir de this (bytes) y desplazamiento de ECX respecto a this en el colaborador
inline constexpr std::array<u32, kFunctions> kSpan{8u, 0x10u, 0u, 12u, 0u, 12u, 8u, 0u, 0x7ECu, 0x28u, 0x3Cu, 0x220u, 4u};
inline constexpr std::array<u32, kFunctions> kEcxAdj{0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0x21Cu, 0u};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

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

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de this en la arena (funciones con this) o el dato principal (2: ranura, 4: bits del float), a[1] = tipo (familia % 4), a[2] = relleno de la arena (0 aleatorio, 1 ceros,
//   2 unos, 3 0xCD), a[3] = 0 (reservado), a[4] = ventana de la arena del objeto, a[5] = extremo de lo que toca la funcion a partir de this (bytes), a[6..11] = datos por funcion:
//     0: a[6], a[7] = dwords previos en this y this+4                         1: a[6], a[7] = clave (this+8, this+12); a[8] = EAX que devuelve el falso de CText::Get
//     3: a[6..8] = dwords previos en this, this+4, this+8                      5: a[6] = modo del vehiculo ([this+8]: 0 nulo, 1 puntero a la arena secundaria, 2 crudo = a[7]); a[8] = modo del ped
//     6: a[6], a[7] = bits de x (this) y de y (this+4)                           (argumento: 0 nulo, 1 arena secundaria + a[10], 2 crudo = a[9]); a[11] = EAX que devuelve el falso de SetPedPositionInCar
//     7: a[6], a[7] = parte baja y alta del contador; a[8] = BOOL que devuelve el falso de QueryPerformanceCounter
//     8: a[6], a[7], a[8] = multiplicadores de los campos previos (byte +0, byte +1, dword +8); a[9], a[10] = mascara (64 bits) de elementos con campos previos no nulos; a[11] = numero de elementos activos
//     9..12: a[6] = dword previo en [ECX] (ECX = this + kEcxAdj), a[7] = banderas (argumento de pila)
inline u32 Interesting(Gen& g, u32 own, u32 other) {
    // aleatorio / ceros / unos / 0xDEADBEEF / la propia / la de otra funcion / la propia con un byte distinto / con dos bytes distintos / un byte igual
    switch (g.Pk({0u, 0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 7u})) {
    case 0: return g.W();
    case 1: return 0u;
    case 2: return 0xFFFFFFFFu;
    case 3: return 0xDEADBEEFu;
    case 4: return own;
    case 5: return other;
    case 6: return own ^ (0xFFu << (8 * g.R(4)));
    case 7: return own ^ (0xFFFFu << (8 * g.R(3)));
    default: return (g.W() & ~(0xFFu << (8 * g.R(4)))) | (own & 0xFFu);
    }
}
// Dword "interesante" sin vtable propia: aleatorio, cero, unos, 0xDEADBEEF, 1, o con un solo byte distinto de cero
inline u32 Interesting1(Gen& g) {
    switch (g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 5u})) {
    case 0: return g.W();
    case 1: return 0u;
    case 2: return 0xFFFFFFFFu;
    case 3: return 0xDEADBEEFu;
    case 4: return 1u;
    default: return 0xFFu << (8 * g.R(4));
    }
}

// float con signo, exponente y mantisa elegidos
constexpr u32 FBits(u32 sign, u32 exp, u32 man) { return (sign << 31) | ((exp & 0xFFu) << 23) | (man & 0x7FFFFFu); }

inline u32 FloatCase(Gen& g, u32 family) {
    const u32 sign = g.R(2);
    switch (family) {
    case 0: return g.W();
    case 1: return std::bit_cast<u32>(float(int(g.R(201)) - 100));
    case 2: return std::bit_cast<u32>(float(int(g.R(101)) - 50) + 0.5f);
    case 3: { const float v = float(int(g.R(2001)) - 1000); return std::bit_cast<u32>(v) + (g.P(50) ? 1u : u32(-1)); }
    case 4: return FBits(sign, 150 + g.R(10), g.W());
    case 5: return FBits(sign, 0, g.W() | (g.P(20) ? 1u : 0u));
    case 6: return g.P(50) ? (sign << 31) : FBits(sign, 1 + g.R(30), g.W());
    case 7: return g.P(70) ? FBits(sign, 255, 0) : FBits(sign, 254, 0x7FFFFFu);
    case 8: return FBits(sign, 255, 0x400000u | g.R(0x400000u));
    case 9: return FBits(sign, 255, 1u + g.R(0x3FFFFFu));
    case 10: return std::bit_cast<u32>(g.P(50) ? -(float(1 + g.R(1000)) / 1024.0f) : (float(1 + g.R(1024)) / 1024.0f));
    case 11: return FBits(sign, g.R(255), g.W());
    case 12: return std::bit_cast<u32>(float(int(g.R(1u << 24)) - (1 << 23)));
    case 13: return std::bit_cast<u32>((float(g.R(200001)) - 100000.0f) / 1000.0f);
    case 14: { const float v = float(int(g.R(2001)) - 1000); return std::bit_cast<u32>(v) - 1u; }
    default: return FloatCase(g, g.R(15));
    }
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 fn = c.fn;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[3] = 0;
    c.a[4] = u32(kObjWin);
    c.a[5] = kSpan[fn];
    if (fn == 2) {
        // ranura: enteros con signo en familias propias
        u32 v;
        switch (f) {
        case 0: v = g.R(8); break;
        case 1: v = u32(-1 - int(g.R(8))); break;
        case 2: v = 8 + g.R(57); break;
        case 3: v = g.W(); break;
        case 4: v = 16519105u * g.R(4) + u32(int(g.R(9)) - 4); break;                // cerca de los multiplos de 2^32 / 0x104 (el producto da la vuelta)
        case 5: v = g.Pk({0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0x7FFFFFFEu}); break;
        case 6: v = (1u << g.R(32)) ^ (g.P(30) ? 0xFFFFFFFFu : 0u); break;
        case 7: v = u32(-int(g.R(1000)) - 1); break;
        case 8: v = u32(int(g.R(2000001)) - 1000000); break;
        case 9: v = g.W() % 0x1000000u; break;
        case 10: v = g.R(0x10000); break;
        case 11: v = 0x00FF0000u | g.R(0x10000); break;
        case 12: v = g.W() & 0xFFFFFF00u; break;
        case 13: v = g.W() | 0x80000000u; break;
        case 14: v = g.W() & 0x0000FFFFu; break;
        default: v = g.W() >> g.R(32); break;
        }
        c.a[0] = v;
        return;
    }
    if (fn == 4) { c.a[0] = FloatCase(g, f); return; }
    if (fn == 7) {
        // contador de 64 bits y BOOL del falso: tipo de valor = familia % 4, BOOL = familia / 4
        const u32 vk = f % 4, bk = f / 4;
        switch (vk) {
        case 0: c.a[6] = g.W(); c.a[7] = g.W(); break;
        case 1: c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, g.W()}); c.a[7] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u}); break;
        case 2: c.a[6] = g.W(); c.a[7] = g.R(4); break;                            // contadores tipicos
        default: c.a[6] = g.W() | 0x80000000u; c.a[7] = 0x7FFFFFFFu - g.R(3); break;
        }
        c.a[8] = bk == 0 ? 1u : bk == 1 ? 0u : bk == 2 ? g.W() : g.Pk({1u, 0u, 2u, 0x100u, 0x80000000u});
        return;
    }
    // funciones con this: desplazamiento segun el tipo (la ventana es de kObjWin bytes y la funcion toca [off, off + span))
    const u32 win = u32(kObjWin), span = c.a[5];
    switch (kind) {
    case 0: c.a[0] = 4 * g.R((win - span) / 4 + 1); break;
    case 1: c.a[0] = 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3); break;
    case 2: c.a[0] = win - span - 4 + g.R(8); break;                           // desde 4 bytes antes del borde hasta 3 bytes dentro de la guarda posterior
    default: c.a[0] = 4 * g.R((win - span) / 4 + 1); break;
    }
    switch (fn) {
    case 0:
        c.a[6] = Interesting1(g); c.a[7] = Interesting1(g);
        break;
    case 1:
        // clave de texto: aleatoria, ceros, texto alfanumerico terminado en cero, unos o dword interesante
        switch (g.R(5)) {
        case 0: c.a[6] = g.W(); c.a[7] = g.W(); break;
        case 1: c.a[6] = 0; c.a[7] = 0; break;
        case 2: c.a[6] = 0x41414141u + (g.R(26) * 0x01010101u); c.a[7] = 0x00000041u + g.R(26) + (g.R(2) ? 0u : 0x00410000u); break;
        case 3: c.a[6] = 0xFFFFFFFFu; c.a[7] = 0xFFFFFFFFu; break;
        default: c.a[6] = Interesting1(g); c.a[7] = Interesting1(g); break;
        }
        c.a[8] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W(), 0x00BADC00u + g.R(256)});
        break;
    case 3:
        c.a[6] = Interesting(g, 0x86C7BCu, 0x86D4CCu);
        c.a[7] = Interesting1(g); c.a[8] = Interesting1(g);
        break;
    case 5:
        // vehiculo (+8): 0 / puntero a la arena secundaria / crudo; ped: 0 / puntero a la arena secundaria / crudo (los crudos no caen nunca en una arena)
        c.a[6] = g.Pk({0u, 1u, 1u, 2u, 2u});
        c.a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFF0000u + g.R(0x10000), 0x00BADC00u + g.R(256), 0x10u});
        c.a[8] = g.Pk({0u, 1u, 1u, 2u, 2u});
        c.a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFF0000u + g.R(0x10000), 0x00BADC00u + g.R(256), 0x20u});
        c.a[10] = 4 * g.R(0x10);
        c.a[11] = g.Pk({0u, 1u, g.W(), g.W(), 0xFFFFFFFFu, 0x100u});
        break;
    case 6: {
        const u32 t = g.R(16) == 0 ? 99u : f;
        auto sign = [&] { return g.R(2); };
        switch (t) {
        case 0: c.a[6] = g.W(); c.a[7] = g.W(); break;
        case 1: c.a[6] = std::bit_cast<u32>((float(g.R(2000001)) - 1000000.0f) / 1000.0f); c.a[7] = std::bit_cast<u32>((float(g.R(2000001)) - 1000000.0f) / 1000.0f); break;
        case 2: c.a[6] = FBits(sign(), 1 + g.R(40), g.W()); c.a[7] = FBits(sign(), 1 + g.R(40), g.W()); break;
        case 3: c.a[6] = FBits(sign(), g.P(50) ? 254u : 240u + g.R(14), g.W()); c.a[7] = FBits(sign(), g.P(50) ? 254u : 240u + g.R(14), g.W()); break;
        case 4: c.a[6] = FBits(sign(), 0, g.W()); c.a[7] = FBits(sign(), g.P(50) ? 0 : 1 + g.R(100), g.W()); break;
        case 5: c.a[6] = g.P(50) ? (sign() << 31) : g.W(); c.a[7] = g.P(50) ? (sign() << 31) : FBits(sign(), 100 + g.R(50), g.W()); if (g.P(30)) c.a[7] = sign() << 31; break;
        case 6: c.a[6] = FBits(sign(), 255, 0); c.a[7] = g.Pk({FBits(sign(), 255, 0), FBits(sign(), 120 + g.R(10), g.W()), FBits(sign(), 255, 0x400000u | (1u + g.R(0x3FFFFFu)))}); break;
        case 7: c.a[6] = FBits(sign(), 255, g.P(50) ? (0x400000u | g.R(0x400000u)) : 1u + g.R(0x3FFFFFu)); c.a[7] = FBits(sign(), 100 + g.R(40), g.W()); break;
        case 8: c.a[7] = FBits(sign(), 255, g.P(50) ? (0x400000u | g.R(0x400000u)) : 1u + g.R(0x3FFFFFu)); c.a[6] = FBits(sign(), 100 + g.R(40), g.W()); break;
        case 9: {
            // dos NaN con significandos (ya silenciados) distintos: gana el mayor
            const u32 mx = 1u + g.R(0x3FFFFFu) + (g.P(50) ? 0x400000u : 0u);
            u32 my = 1u + g.R(0x3FFFFFu) + (g.P(50) ? 0x400000u : 0u);
            if ((my | 0x400000u) == (mx | 0x400000u)) my ^= 1u;
            c.a[6] = FBits(sign(), 255, mx); c.a[7] = FBits(sign(), 255, my); break;
        }
        case 10: {
            static constexpr float tri[3][2]{{3.0f, 4.0f}, {5.0f, 12.0f}, {8.0f, 15.0f}};
            const auto& p = tri[g.R(3)];
            const float scale = std::bit_cast<float>(FBits(0, 100 + g.R(55), 0));
            const bool swap = g.P(50);
            c.a[6] = std::bit_cast<u32>(p[swap ? 1 : 0] * scale * (g.P(50) ? -1.0f : 1.0f)); c.a[7] = std::bit_cast<u32>(p[swap ? 0 : 1] * scale * (g.P(50) ? -1.0f : 1.0f)); break;
        }
        case 11: { const u32 b = FBits(0, 100 + g.R(55), g.W()); c.a[6] = b | (sign() << 31); c.a[7] = b | (sign() << 31); break; }
        case 12: { const u32 v = FBits(sign(), 100 + g.R(55), g.W()); if (g.P(50)) { c.a[6] = v; c.a[7] = sign() << 31; } else { c.a[6] = sign() << 31; c.a[7] = v; } break; }
        case 13: c.a[6] = FBits(1, 100 + g.R(55), g.W()); c.a[7] = FBits(1, 100 + g.R(55), g.W()); break;
        case 14: c.a[6] = FBits(sign(), 120 + g.R(15), g.W()); c.a[7] = FBits(sign(), 120 + g.R(15), g.W()); break;
        default: c.a[6] = FBits(sign(), g.R(255), g.W()); c.a[7] = FBits(sign(), g.R(255), g.W()); break;
        }
        break;
    }
    case 8: {
        c.a[6] = g.P(10) ? 0u : g.W() | 1u; c.a[7] = g.P(10) ? 0u : g.W() | 1u; c.a[8] = g.P(10) ? 0u : g.W() | 1u;
        // elementos con campos previos distintos de cero: mascara de 64 bits con 0, 1, 2, 3, 4, 6, 8, 16, 32 o 64 elementos activos
        static constexpr u32 counts[]{0, 1, 1, 2, 3, 4, 6, 8, 16, 32, 64};
        const u32 count = counts[g.R(11)];
        u64 mask = 0; u32 placed = 0;
        if (count >= 64) mask = ~u64(0);
        else while (placed < count) { const u32 e = g.R(64); if (!((mask >> e) & 1)) { mask |= u64(1) << e; ++placed; } }
        c.a[9] = u32(mask); c.a[10] = u32(mask >> 32); c.a[11] = count;
        break;
    }
    default: {
        // destructores con borrado: dword previo en [ECX] y banderas
        c.a[6] = fn == 11 ? Interesting1(g) : Interesting(g, fn == 9 ? 0x862D70u : fn == 10 ? 0x862DE8u : 0x85F368u, 0x862D70u);
        switch (g.R(10)) {
        case 0: c.a[7] = 0; break;
        case 1: c.a[7] = 1; break;
        case 2: c.a[7] = g.Pk({2u, 4u, 0x80u, 0xFEu, 0x100u, 0x10000u, 0x80000000u, 0xFFFFFF00u, 0xFFFFFE00u}); break;
        case 3: c.a[7] = g.Pk({3u, 5u, 0x81u, 0xFFu, 0x101u, 0x10001u, 0x80000001u, 0xFFFFFF01u, 0xFFFFFF03u, 0x12345679u}); break;
        case 4: c.a[7] = g.W() & ~1u; break;
        case 5: c.a[7] = g.W() | 1u; break;
        case 6: c.a[7] = 0xFFFFFFFFu; break;
        default: c.a[7] = g.W(); break;
        }
        break;
    }
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}
