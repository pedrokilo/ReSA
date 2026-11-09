#pragma once
// Fixtures del banco Fn138A (lote 138A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                      forma
//   0   0x4B17C0   CEventCopCarBeingStolen::GetEventType            thiscall() `ret`: mov eax,10h        (ranura 1 de la vtable 0x85B548)
//   1   0x4B17D0   CEventCopCarBeingStolen::GetLifeTime             thiscall() `ret`: xor eax,eax        (ranura 3)
//   2   0x4B17E0   CEventCopCarBeingStolen::GetEventPriority        thiscall() `ret`: mov eax,26h        (ranura 2)
//   3   0x4B1D20   CEventCarUpsideDown::GetEventType               thiscall() `ret`: mov eax,2Dh        (vtable 0x85B660, ranura 1)
//   4   0x4B1D30   CEventCarUpsideDown::GetLifeTime                thiscall() `ret`: xor eax,eax        (ranura 3)
//   5   0x4B1D40   CEventCarUpsideDown::GetEventPriority           thiscall() `ret`: mov eax,12h        (ranura 2)
//   6   0x4B20F0   CEventSeenPanickedPed::GetEventType             thiscall() `ret`: mov eax,41h        (vtable 0x85B778, ranura 1)
//   7   0x4B2100   CEventSeenPanickedPed::GetLifeTime              thiscall() `ret`: xor eax,eax        (ranura 3)
//   8   0x4B2110   CEventSeenPanickedPed::GetSourceEntity          thiscall() `ret`: mov eax,[ecx+14h]  (ranura 10)
//   9   0x4B2120   CEventSeenPanickedPed::GetEventPriority         thiscall() `ret`: mov eax,0Dh        (ranura 2)
//  10   0x4B2440   CEventPlayerCommandToGroup::GetLifeTime         thiscall() `ret`: xor eax,eax        (vtable 0x85B800, ranura 3)
//  11   0x4B2450   CEventPlayerCommandToGroup::AffectsPed          thiscall(ped) `ret 4`: xor al,al     (ranura 5)
//  12   0x4B2460   CEventPlayerCommandToGroup::GetSourceEntity     thiscall() `ret`: mov eax,[ecx+18h]  (ranura 10)
//  13   0x4B2520   CEventPlayerCommandToGroup::TakesPriorityOver   thiscall(event&) `ret 4`: mov al,1    (ranura 11)
//  14   0x634F20   CTaskComplexUseGoggles::Destructor (cuerpo)     thiscall() `ret`: mov [ecx],86E4A8h; jmp 61A3D0 (~CTaskComplex)
//  15   0x634F30   CTaskComplexUseGoggles::MakeAbortable           thiscall(ped, prioridad, evento) `ret 0Ch`: mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]  (ranura 6, vtable 0x86E4A8)
//  16   0x678150   CTaskComplexGetUpAndStandStill::GetTaskType     thiscall() `ret`: mov eax,0CEh       (vtable 0x870404, ranura 4)
//  17   0x678160   CTaskComplexGetUpAndStandStill::Destructor      thiscall() `ret`: mov [ecx],870404h; jmp 61A3D0
//  18   0x6790A0   CTaskComplexFallToDeath::GetTaskType            thiscall() `ret`: mov eax,115h       (vtable 0x870520, ranura 4)
//  19   0x6790B0   CTaskComplexFallToDeath::Destructor             thiscall() `ret`: mov [ecx],870520h; jmp 61A3D0
//  20   0x72F4A0   CMemoryMgr::PushMemId                           cdecl `ret`: no hace nada
//  21   0x72F4B0   CMemoryMgr::PopMemId                            cdecl `ret`: no hace nada
//  22   0x72F500   CMemoryMgr::MoveMemory (un puntero)             cdecl(ptr) `ret`: mov eax,[esp+4]
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta, memoria global del juego (se guarda y se restaura tras cada lado).
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

namespace F138AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanSeenSrc = 0x18;    // GetSourceEntity de CEventSeenPanickedPed lee +0x14
constexpr u32 kSpanCmdSrc = 0x1C;     // GetSourceEntity de CEventPlayerCommandToGroup lee +0x18
constexpr u32 kSpanGoggles = 0x0C;    // MakeAbortable lee la subtarea en +8

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

// Indices: 0..13 eventos, 14..19 tareas, 20..22 CMemoryMgr (ver la cabecera)
inline constexpr unsigned kIdxGoggleDtor = 14, kIdxGoggleAbort = 15, kIdxGetUpDtor = 17, kIdxFallDtor = 19, kIdxPush = 20, kIdxPop = 21, kIdxMove = 22;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; el resto por funcion
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case 8:     // CEventSeenPanickedPed::GetSourceEntity: a[3] = dword en +0x14 (m_ped)
        c.a[0] = Offset(g, kind, kSpanSeenSrc);
        c.a[3] = Raw(g);
        break;
    case 12:    // CEventPlayerCommandToGroup::GetSourceEntity: a[3] = dword en +0x18 (m_target)
        c.a[0] = Offset(g, kind, kSpanCmdSrc);
        c.a[3] = Raw(g);
        break;
    case 11: case 13:   // AffectsPed (ped) / TakesPriorityOver (evento): a[3] = argumento crudo (no se desreferencia), a[4] = dword que ocupa [this]
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kIdxGoggleDtor: case kIdxGetUpDtor: case kIdxFallDtor:   // destructores: a[3] = vtable previa en [this]
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x86E4A8u, 0x870404u, 0x870520u, 0x86D4CCu, 0x86D48Cu, 0x86E4A9u, 0x870405u, 0x870521u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);   // gRet del falso de 0x61A3D0
        break;
    case kIdxGoggleAbort: {   // a[3], a[4], a[5] = ped, prioridad, evento; a[6] = subtarea (desplazamiento en B); a[7] = vtable (desplazamiento en C); a[8] = EAX del falso de la ranura 6
        c.a[0] = Offset(g, kind, kSpanGoggles);
        c.a[3] = Raw(g); c.a[4] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, g.W(), 0x80000000u}); c.a[5] = Raw(g);
        c.a[6] = Offset(g, g.R(4), 4);
        c.a[7] = 4 * g.R((win - 0x1C) / 4 + 1);
        c.a[8] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFFFFu, g.W(), g.W(), 0x00BADC00u + g.R(256), 0x80u});
        break;
    }
    case kIdxPush:   // a[3] = numero de argumentos que se empujan (0 o 1; el EXE no lee ninguno), a[4] = el argumento
        c.a[3] = g.R(2);
        c.a[4] = g.Pk({0u, 1u, 7u, 32u, 33u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W()});
        break;
    case kIdxPop:    // idem
        c.a[3] = g.R(2);
        c.a[4] = Raw(g);
        break;
    case kIdxMove:   // a[3] = puntero recibido
        c.a[3] = Raw(g);
        break;
    default:         // constantes (0..7, 9, 10, 16, 18): `this` en la arena A
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);   // dword en [this] (el EXE no lo lee)
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
