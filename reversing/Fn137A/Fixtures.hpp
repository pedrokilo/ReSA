#pragma once
// Fixtures del banco Fn137A (lote 137A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado se atribuian por cercania: las reales salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                      forma
//   0   0x406450   CdStreamGetLastPosn                              cdecl() `ret`: EAX = dword [0x8E4898]
//   1   0x406D10   CMaths::AbsFloat (fabs de un float)              cdecl(float) `ret`: fld dword [esp+4]; fabs (resultado en ST0)
//   2   0x420E90   CEvent::IsCriminalEvent                          thiscall() `ret` (vtable de CEvent, ranura 7): xor al, al
//   3   0x420EA0   CEvent::ReportCriminalEvent                      thiscall(ped) `ret 4` (ranura 8): vacia (el banco vigila 0x8E4898 para comprobar que no escribe memoria global)
//   4   0x420EB0   CEvent::GetLocalSoundLevel                       thiscall() `ret` (ranura 12): fld dword [0x858B50] (0.0f en ST0)
//   5   0x420EC0   CEvent::DoInformVehicleOccupants                 thiscall(ped) `ret 4` (ranura 13): xor al, al
//   6   0x4ACA40   CEventPedCollisionWithPed::GetEventType          thiscall() `ret` (vtable 0x85AC68, ranura 1): mov eax, 2
//   7   0x4ACA50   CEventPedCollisionWithPed::GetLifeTime           (ranura 3): xor eax, eax
//   8   0x4ACA60   CEventPedCollisionWithPed::GetEventPriority      (ranura 2): mov eax, 0x3C
//   9   0x4AE760   CEventPotentialWalkIntoPed::GetEventType         (vtable 0x85AF60, ranura 1): mov eax, 0xD
//  10   0x4AE770   CEventPotentialWalkIntoPed::GetLifeTime          (ranura 3): xor eax, eax
//  11   0x4AE780   CEventPotentialWalkIntoPed::GetSourceEntity      (ranura 10): mov eax, [ecx+0x20]
//  12   0x4AE790   CEventPotentialWalkIntoPed::GetEventPriority     (ranura 2): mov eax, 0x1C
//  13   0x4AF6D0   CEventVehicleToSteal::GetEventType               (vtable 0x85B210, ranura 1): mov eax, 0x1C
//  14   0x4AF6E0   CEventVehicleToSteal::GetLifeTime                (ranura 3): xor eax, eax
//  15   0x4AF6F0   CEventVehicleToSteal::GetEventPriority           (ranura 2): mov eax, 7
//  16   0x4B0CD0   CEventInAir::GetEventType                        (vtable 0x85B3C0, ranura 1): mov eax, 0x21
//  17   0x4B0CE0   CEventInAir::GetLifeTime                         (ranura 3): xor eax, eax
//  18   0x4B0CF0   CEventInAir::GetEventPriority                    (ranura 2): mov eax, 0x3D
//
// Ninguna llama a otra funcion: el banco no necesita falsos de colaboradores. Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, en CdStreamGetLastPosn, con la
// memoria global real del juego (se guarda y se restaura tras cada lado).
// Familia = n % 16: tipo = familia % 4 (ver cada funcion) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F137AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 19;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que las funciones tocan a partir de `this` (bytes): GetSourceEntity lee el dword +0x20; las demas no leen
constexpr u32 kSpanEvent = 0x24;

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
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
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

// Bits de un float segun el tipo de la familia: 0 magnitudes ordinarias (con signo), 1 valores especiales (cero con signo, uno, extremos, infinitos, NaN), 2 NaN con cargas utiles distintas
// (silenciosos y senalizantes, con cualquier signo), 3 denormales, limites del exponente y potencias de dos
inline u32 FloatBitsAbs(Gen& g, u32 kind) {
    switch (kind) {
    case 0: {
        const float scale = float(g.Pk({1u, 10u, 100u, 1000u, 100000u, 1000000000u}));
        const float f = (float(g.W()) / 4294967296.0f - 0.5f) * 2.0f * scale;
        u32 b; std::memcpy(&b, &f, 4); return b;
    }
    case 1:
        return g.Pk({0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x00000001u, 0x80000001u, 0x00800000u, 0x80800000u, 0x007FFFFFu, 0x807FFFFFu, 0x7F7FFFFFu,
                     0xFF7FFFFFu, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFA00000u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x3D4CCCCDu, 0xBD4CCCCDu});
    case 2: {
        const u32 payload = g.P(25) ? (g.R(4) == 0 ? 1u : g.Pk({0x7FFFFFu, 0x400000u, 0x3FFFFFu, 0x200000u})) : (g.W() & 0x7FFFFFu);
        const u32 signBit = g.P(50) ? 0x80000000u : 0u;
        const u32 mant = payload ? payload : 1u;      // payload 0 seria infinito
        return signBit | 0x7F800000u | mant;
    }
    default:
        switch (g.R(4)) {
        case 0: return (g.P(50) ? 0x80000000u : 0u) | (g.W() & 0x7FFFFFu);                                   // denormal (o cero)
        case 1: return (g.P(50) ? 0x80000000u : 0u) | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu);         // cualquier exponente finito
        case 2: return (g.P(50) ? 0x80000000u : 0u) | ((1u + g.R(254)) << 23);                               // potencia de dos
        default: return (g.P(50) ? 0x80000000u : 0u) | g.Pk({0x00800000u, 0x7F7FFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F000000u, 0x01000000u});
        }
    }
}

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (el dword leido entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global;
//   a[3] = dato principal (valor del global, bits del float, argumento `ped`, dword de +0x20), a[4] = modo del dato, a[5] = desplazamiento en la arena B cuando el dato apunta a ella.
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpanEvent);
    switch (c.fn) {
    case 0: {   // CdStreamGetLastPosn: a[3] = valor de gLastCdStreamPosn (0x8E4898)
        switch (kind) {
        case 0: c.a[3] = g.W(); break;
        case 1: c.a[3] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00FFFFFFu, 0x01000000u, 0xFF000000u, 0xFEFFFFFFu, 0xDEADBEEFu}); break;
        case 2: c.a[3] = (g.R(32) << 24) | (g.W() & 0xFFFFFFu); break;         // FileID en el byte alto y desplazamiento en sectores
        default: c.a[3] = g.P(50) ? (g.W() & 0xFFu) : (g.W() | 0x80000000u); break;
        }
        break;
    }
    case 1:     // CMaths::AbsFloat: a[3] = bits del float
        c.a[3] = FloatBitsAbs(g, kind);
        break;
    case 3: case 5: case 11: {   // ReportCriminalEvent / DoInformVehicleOccupants (a[3] = argumento `ped`) y GetSourceEntity (a[3] = dword de this+0x20)
        c.a[4] = g.R(4);         // modo: 0 crudo, 1 puntero a la arena B + a[5], 2 nulo, 3 -1
        c.a[5] = Offset(g, g.R(2), 4);
        c.a[3] = c.a[4] == 0 ? g.Pk({g.W(), g.W(), 0x00400000u + g.R(0x800000), 0x7FFFFFFFu, 0x80000000u, 1u}) : (c.a[4] == 2 ? 0u : 0xFFFFFFFFu);
        c.a[6] = g.W() | 1u;     // ReportCriminalEvent (funcion vacia): contenido de gLastCdStreamPosn (0x8E4898), memoria global vigilada que no debe cambiar
        break;
    }
    default:    // el resto no lee ni escribe nada: solo cambian `this`, las semillas de los registros y la pila
        break;
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
    j.H("s", c.s);
    return j.End();
}
}
