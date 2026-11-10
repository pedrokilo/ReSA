#pragma once
// Fixtures del banco Fn145A (lote 145A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                           forma
//    0   0x502BF0   CAEWMADecoder::Shutdown                                                 cdecl() `ret`: if [0B6BAD8h] { FreeLibrary([0B6BAD4h]) por la IAT 858110h; [0B6BAD8h]=0 }
//    1   0x502C40   CAEWMADecoder::ScalarDeletingDestructor (vtable 0x862D90, ranura 6)     thiscall(flags) `ret 4`: call 502760; test byte [esp+8],1; operator delete 8214BDh; eax=this
//  2..21 las 20 clases de tareas (ranura 0 de su vtable): thiscall(flags) `ret 4`: push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je +; push esi; call 61A5B0 (CTask::operator delete); add esp,4; mov eax,esi; pop esi
//        (ver kClass en Fixtures.hpp / classes.py: 64C930 CTaskComplexCarSlowBeDraggedOutAndStandUp, 64C950 CTaskSimpleBikeJacked, 65D0C0 CTaskComplexWanderFlee, 65D0E0 CTaskComplexFleePoint,
//         671610 CTaskComplexFollowPointRoute, 671730 CTaskComplexFollowNodeRoute, 638160 CTaskComplexHitByGunResponse, 638180 CTaskComplexSunbathe, 6275E0 CTaskSimpleGangDriveBy,
//         628780 CTaskComplexDestroyCar, 639310 CTaskSimplePlayerOnFire, 639330 CTaskSimpleIKManager, 664A80 CTaskComplexGangJoinRespond, 664AA0 CTaskComplexProstituteSolicit,
//         638900 CTaskComplexWaitForDryWeather, 638920 CTaskComplexWaitForBus, 638F00 CTaskComplexAttractorPartnerWait, 639F90 CTaskComplexUseSequence, 48E700 CTaskComplexWanderCriminal,
//         670400 CTaskComplexGoToAttractor)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, en Shutdown, memoria global del juego (0xB6BAD4..0xB6BADB, se guarda y se restaura tras cada lado) y la IAT
// (FreeLibrary falso solo alrededor de la llamada, en el EXE y en la DLL).
// Familia = n % 16: tipo = familia % 4 (destructores: posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio; Shutdown: manejador) y relleno de las arenas / puntero del lector = familia / 4.
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

namespace F145AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
constexpr unsigned kTasks = 20;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable

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

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Indices (ver la cabecera)
inline constexpr unsigned kIdxShutdown = 0, kIdxWmaDelete = 1, kIdxFirstTask = 2;
constexpr bool IsScalarDeleting(u32 fn) { return fn >= 1; }
// Cuerpo del destructor y operator delete de cada destructor con borrado (idx 1..21) y vtable de cada clase (informativa: se usa en los dwords previos de [this])
inline constexpr std::array<u32, 21> kBodyExe{0x502760, 0x648690, 0x648C40, 0x65B370, 0x65B410, 0x66E050, 0x66EB70, 0x631E30, 0x632050, 0x6218C0, 0x621CB0, 0x6335B0, 0x633830, 0x661720, 0x661AF0, 0x632830, 0x632930, 0x633290, 0x6396C0, 0x48E720, 0x66B6A0};
inline constexpr std::array<u32, 21> kDeleteExe{0x8214BD, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0};
inline constexpr std::array<u32, 21> kVtable{0x862D90, 0x86EF80, 0x86EFAC, 0x86F690, 0x86F6CC, 0x870078, 0x8700A8, 0x86E06C, 0x86E0AC, 0x86D944, 0x86D968, 0x86E334, 0x86E358, 0x86FB5C, 0x86FB88, 0x86E160, 0x86E18C, 0x86E284, 0x86E518, 0x85A23C, 0x86FF3C};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A (destructores), a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global;
//   destructores con borrado: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [ECX] (gDtorWrite); a[6] = banderas
//   Shutdown: a[3] = manejador [0xB6BAD4]; a[4] = puntero [0xB6BAD8] (WMCreateSyncReader); a[5] = BOOL que devuelve el FreeLibrary falso (gRet)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    if (c.fn == kIdxShutdown) {
        const u32 h = Raw(g), r = Raw(g), r2 = g.W() | 1u;
        c.a[0] = 0;
        c.a[3] = kind == 0 ? h : kind == 1 ? 0u : kind == 2 ? 0xFFFFFFFFu : 0x00400000u + g.R(0x800000);
        const u32 rk = f / 4;
        c.a[4] = rk == 0 ? 0u : rk == 1 ? r2 : rk == 2 ? r : g.Pk({1u, 0xFFFFFFFFu, 0x80000000u, 0x00400000u + g.R(0x800000), 0x7FFFFFFFu});
        if (rk == 2 && g.P(25)) c.a[4] = 0;
        c.a[5] = Raw(g);
        return;
    }
    // destructores con borrado
    c.a[0] = Offset(g, kind, kSpanThis);
    const u32 mine = kVtable[c.fn - 1];
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, mine, mine, mine + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, mine + 1, g.W(), g.W(), 0xDEADBEEFu});
    c.a[4] = Raw(g);
    c.a[5] = Raw(g);
    c.a[6] = FlagsDword(g);
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
