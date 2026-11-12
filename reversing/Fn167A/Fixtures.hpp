#pragma once
// Fixtures del banco Fn167A (lote 167A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 7 asignadas: 7 reconstruidas.
//   idx  direccion  nombre real                                                                     forma
//    0   0x4C5750   CClumpModelInfo cuerpo del destructor                                           thiscall() `ret`: mov dword [ecx],85BB9Ch (sin colaboradores)
//    1   0x4C5760   CWeaponModelInfo cuerpo del constructor (el listado decia CClumpModelInfo)      thiscall() `ret`: call 4C4A60; [this] = 85BD78h; eax = this
//    2   0x4DC2A0   CAEDataStream::Close                                                            thiscall() `ret`: eax = [this+4]; [this+0Ch] = 0 (byte); call [CloseHandle IAT 858078]; eax = el de CloseHandle
//    3   0x4DC550   CAEDataStream destructor con borrado (sin referencias estaticas)                 thiscall(flags) `ret 4`: [this] = 85F0BCh; CloseHandle si byte +0Ch; delete [this+8]; delete this si bit 0
//    4   0x6889F0   CTaskSimpleSwim::GetTaskType (vtable 0x870920, ranura 4)                         thiscall() `ret`: mov eax,518h
//    5   0x68B680   CTaskSimpleArrestPed::GetTaskType (vtable 0x870984, ranura 4; el listado decia Swim)  thiscall() `ret`: mov eax,44Ch
//    6   0x68B690   CTaskSimpleArrestPed cuerpo del destructor (marco SEH; el listado decia Swim)   thiscall() `ret`: vtable 870984h, SetFinishCallback 4CEBE0, CleanUpOldReference 571A00, IsPlayer 5DF8F0, SetPedState 5E4500, ~CTask 61A3A0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL; CloseHandle en la casilla
// de la IAT del EXE y en la de la DLL; operator delete del CRT 0x8214BD en la direccion del EXE).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F167AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 7;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

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
    std::array<u32, 24> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B o C), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

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

// Clases de funcion (forma)
enum Kind : u32 { kConst, kVoid, kCtorBody, kClose, kAeSdd, kArrestBody };
inline constexpr std::array<Kind, kFunctions> kKind{kVoid, kCtorBody, kClose, kAeSdd, kConst, kConst, kArrestBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4C5750, 0x4C5760, 0x4DC2A0, 0x4DC550, 0x6889F0, 0x68B680, 0x68B690};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85BD30, 0x85BD78, 0x85F0BC, 0x85F0BC, 0x870920, 0x870984, 0x870984};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 0x10, 0x10, 4, 4, 0x14};
// Un ped sintetico ocupa 0x540 bytes en la arena B (se leen +0x46C y +0x530)
inline constexpr u32 kPedSpan = 0x540;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee)
//   kVoid: a[3] = dword previo en [this]
//   kCtorBody: a[3] = dword previo en [this]; a[4] = EAX que devuelve el falso del constructor base (se ignora); a[5] = dword que el falso deja en [this]
//   kClose: a[3] = dword previo en [this]; a[4] = HANDLE (+4, crudo; nunca se usa de verdad); a[5] = nombre (+8, crudo; no se lee); a[6] = dword en +0Ch (solo se escribe el byte bajo); a[7] = BOOL que devuelve el falso de CloseHandle
//   kAeSdd: a[3] = dword previo en [this]; a[4] = HANDLE (+4); a[5] = modo del nombre (+8) (0 nulo, 1 en B a[6]); a[6] = desplazamiento en B; a[7] = dword en +0Ch (solo cuenta el byte bajo: m_bIsOpen);
//         a[8] = BOOL que devuelve el falso de CloseHandle; a[9] = banderas
//   kArrestBody: a[3] = dword previo en [this]; a[4] = modo del ped (+8) (0 nulo, 1 en B a[5], 2 crudo a[6]); a[7] = modo de la animacion (+0x10) (0 nula, 1 en C a[8], 2 cruda a[9]);
//         a[10] = dword en +0x46C del ped (bit 8 = bInVehicle); a[11] = dword en +0x530 del ped (estado); a[16] = EAX del falso de IsPlayer (solo AL; con ped crudo se fuerza AL != 0);
//         a[17] = dword que el falso de ~CTask deja en [this]; a[18] = EAX del falso de ~CTask; a[19] = dword en +0xC
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x85BB9Cu}); };
    switch (kKind[c.fn]) {
    case kConst:
        c.a[3] = Raw(g);
        break;
    case kVoid:
        c.a[3] = prev();
        break;
    case kCtorBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kClose:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, 0xFFFFFF00u, 0xFFFFFF01u, 0x01010101u, 0xFFFFFFFFu, g.W(), g.W(), g.W() | 1u});
        c.a[7] = g.Pk({0u, 1u, 1u, 0xFFFFFFFFu, 0x100u, g.W(), g.R(4)});
        break;
    case kAeSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = g.Pk({0u, 1u, 1u});
        c.a[6] = AuxOffset(g, 0x20);
        c.a[7] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, 0xFFFFFF00u, 0xFFFFFF01u, 0x01010101u, 0xFFFFFFFFu, g.W(), g.W(), g.W() | 1u});
        c.a[8] = g.Pk({0u, 1u, 1u, 0xFFFFFFFFu, 0x100u, g.W(), g.R(4)});
        c.a[9] = FlagsDword(g);
        break;
    case kArrestBody: {
        c.a[3] = prev();
        c.a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 2u});
        c.a[5] = AuxOffset(g, kPedSpan);
        c.a[6] = Raw(g);
        if (c.a[6] == 0) c.a[6] = 0x12345678u;
        c.a[7] = g.Pk({0u, 1u, 1u, 2u});
        c.a[8] = AuxOffset(g, 0x40);
        c.a[9] = Raw(g);
        const u32 w1 = g.W(), w2 = g.W();
        c.a[10] = g.Pk({0x100u, 0x100u, 0x100u, 0u, 0xFFFFFFFFu, 0xFFFFFEFFu, w1 | 0x100u, w1 & ~0x100u, 0x80000100u, 0x200u});
        c.a[11] = g.Pk({0x3Fu, 0x3Fu, 0x3Fu, 0x3Fu, 0x32u, 0u, 0x3Eu, 0x40u, 0xFFFFFFFFu, 0x3Fu | 0x100u, w2});
        c.a[16] = g.Pk({0u, 0u, 0u, 1u, 0xFFu, 0x100u, 0xFFFFFF00u, 0x80000000u, g.W(), g.W() | 1u});
        c.a[17] = Raw(g);
        c.a[18] = Raw(g);
        c.a[19] = Raw(g);
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
    j.H("s", c.s);
    return j.End();
}
}
