#pragma once
// Fixtures del banco Fn114A (lote 114A): 14 funciones x 16384 casos (229376 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlaceable::GetRightVector                  0x41CC70  thiscall (ECX = placeable; destino en pila), `ret 4`; devuelve el destino en EAX                (hook S)
//   1 CPlaceable::GetUpVector                     0x50E420  thiscall (ECX = placeable; destino en pila), `ret 4`; devuelve el destino en EAX                (hook S)
//   2 CPhysical::ApplyMoveForce(x, y, z)          0x4ABBA0  thiscall (ECX = fisico; x, y, z en pila), `ret 0Ch`; void                                      (hook S)
//   3 CPedIntelligence::GetPedFOVRange            0x4ABEA0  thiscall (ECX = inteligencia), float en ST0                                                    (hook S)
//   4 CPedGroups::RegisterKillByPlayer            0x5F7E30  cdecl sin argumentos; void                                                                     (hook S)
//   5 CMissionCleanup::AddEntityToList(handle, tipo) 0x4637E0  thiscall (ECX = lista), `ret 8`; void                                                       (hook S)
//   6 CEventDamage::IsSameEventForAI              0x4ADBF0  thiscall (ECX = evento; otro evento en pila), `ret 4`; bool en AL                              (hook S)
//   7 CEntryExitManager::WeAreInInteriorTransition 0x43E400 cdecl sin argumentos; bool en AL                                                               (hook S)
//   8 CPointRoute::AddPoint                       0x48E180  thiscall (ECX = ruta; &punto en pila), `ret 4`; bool en AL                                     (hook S)
//   9 CPointRoute::RemovePoint                    0x5F13A0  thiscall (ECX = ruta; indice en pila), `ret 4`; void                                           (hook S)
//  10 CPedGroupIntelligence::GetTask              0x5F7660  thiscall (ECX sin usar; ped y &pares en pila), `ret 8`; devuelve la tarea en EAX            (hook S)
//  11 CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent 0x4AE100 thiscall (ECX = evento de grupo; otro en pila), `ret 4`; bool en AL               (hook S)
//  12 CEvent::TakesPriorityOverBase               0x4ABC30  thiscall (ECX = evento; otro en pila), `ret 4`; bool en AL                                   (hook S)
//  13 IsPedPointerValid                           0x5E8A30  cdecl (ped); bool en AL                                                                        (hook S)
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

namespace F114AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 14;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kObjWin = 0x260, kOutWin = 0x20, kMatWin = 0x40, kObj2Win = 0x40, kEvWin = 0x20, kPairsWin = 0xA0, kPoolWin = 0x20, kFlagsWin = 0x10, kStoreWin = 0x3800;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 114001u); }
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

// ------------------------------------------------------------------------------------------------ el caso
// Cada funcion interpreta `a` como se documenta en su generador; todos los valores viajan como u32 (los floats, como su patron de bits).
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    std::array<u32, 24> a{};
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
    u32 W() { return Rand(n, nk()); }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 8> kNans{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7FC12345u, 0xFFFFFFFFu};
// Un float "especial": cero, desnormal, infinito, NaN (cualquier patron), enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return kNans[g.R(8)];
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
inline u32 Raw(Gen& g) { return g.P(60) ? g.W() : Special(g); }   // palabra cruda: cualquier patron

// Valor inicial determinista del dword k de la entrada j de una ruta (funcion 9; el oraculo lo reproduce con la misma mezcla)
inline u32 RouteWord(u32 seed, int j, int k) { return Rand(seed, u32(50 + (j + 5) * 3 + k)); }

// ------------------------------------------------------------------------------------------------ funcion 0: CPlaceable::GetRightVector (0x41CC70)
//   a[0] = el placeable tiene matriz (+0x14), a[1] = rumbo (+0x10), a[2..4] = fila derecha de la matriz (+0 .. +8), a[5..7] = valor inicial del destino
constexpr float kPi = 3.14159265358979f;
inline constexpr std::array<u32, 12> kHeadSpecial{0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x80800000u, 0x3F800000u, 0xBF800000u, 0x007FFFFFu, 0x2EDBE6FFu, 0xAEDBE6FFu, 0x3F490FDBu};
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 mat = 0, head;
    const auto row = [&](bool raw) {
        for (u32 k = 0; k < 3; ++k) c.a[2 + k] = raw ? (g.P(70) ? g.W() : Special(g)) : B(g.Fl(-1.0f, 1.0f));
    };
    head = B(g.Fl(-4.0f, 4.0f));
    row(false);
    switch (f) {
    case 0: break;
    case 1: mat = 1; break;
    case 2: head = kHeadSpecial[g.R(12)]; break;
    case 3: mat = 1; row(true); break;
    case 4: head = B(g.Fl(-8.0f, 8.0f)); break;
    case 5: head = g.Pk({0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x7F800001u, 0xFFA00000u, 0x7FFFFFFFu}); break;
    case 6: mat = 1; head = Special(g); row(g.P(50)); break;
    case 7: head = B(g.Fl(-1.0e-3f, 1.0e-3f)); break;
    case 8: head = NextUp(B(float(int(g.R(9)) - 4) * (kPi / 2.0f)), int(g.R(7)) - 3); break;      // cerca de multiplos de pi/2
    case 9: head = B(g.Fl(-0.1f, 0.1f)); break;
    case 10: mat = 1; row(false); break;
    case 11: head = B(g.Fl(-3.2f, 3.2f)); break;
    case 12: head = B(g.Fl(-1.0e-6f, 1.0e-6f)); break;
    case 13: head = g.Pk({0x80000000u, 0x80000001u, 0x80800000u, 0xB0000000u, 0xA0000000u}); break;
    case 14: mat = 1; head = g.W(); row(true); break;
    default: if (g.P(30)) mat = 1; head = g.P(60) ? B(g.Fl(-6.0f, 6.0f)) : Special(g); break;
    }
    if (!mat) {   // sin matriz solo cabe un rumbo finito pequeno (fsin de valores grandes depende de la reduccion interna de la x87, fuera del modelo) o no finito
        const float hv = F(head);
        if (std::isfinite(hv) && std::fabs(hv) > 8.0f) head = B(g.Fl(-8.0f, 8.0f));
    }
    c.a[0] = mat; c.a[1] = head;
    for (u32 k = 0; k < 3; ++k) c.a[5 + k] = g.P(55) ? g.W() : g.Pk({0u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0x3F800000u});
}

// ------------------------------------------------------------------------------------------------ funcion 1: CPlaceable::GetUpVector (0x50E420)
//   a[0] = el placeable tiene matriz (+0x14), a[1..3] = fila arriba de la matriz (+0x20 .. +0x28), a[4..6] = valor inicial del destino, a[7] = rumbo (+0x10, no se lee)
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 mat = g.P(50);
    for (u32 k = 0; k < 3; ++k) c.a[1 + k] = g.P(50) ? g.W() : B(g.Fl(-1.0f, 1.0f));
    for (u32 k = 0; k < 3; ++k) c.a[4 + k] = g.P(50) ? g.W() : g.Pk({0u, 0x3F800000u, 0x80000000u, 0x7FC00000u});
    c.a[7] = g.W();
    switch (f) {
    case 0: mat = 0; break;
    case 1: mat = 1; break;
    case 2: mat = 1; for (u32 k = 0; k < 3; ++k) c.a[1 + k] = g.W(); break;
    case 3: mat = 1; for (u32 k = 0; k < 3; ++k) c.a[1 + k] = Special(g); break;
    case 4: mat = 1; for (u32 k = 0; k < 3; ++k) c.a[1 + k] = kNans[g.R(8)]; break;
    case 5: mat = 0; c.a[4] = 0; c.a[5] = 0; c.a[6] = 0x3F800000u; break;                    // destino ya igual al resultado sin matriz: sin cambios
    case 6: mat = 0; c.a[4] = 0; c.a[5] = 0; c.a[6] = g.W(); break;
    case 7: mat = 1; c.a[4] = c.a[1]; c.a[5] = c.a[2]; c.a[6] = c.a[3]; break;                // destino ya igual a la fila: sin cambios
    case 8: mat = 1; c.a[4] = c.a[1]; c.a[6] = c.a[3]; break;
    case 9: mat = 1; for (u32 k = 0; k < 3; ++k) c.a[1 + k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u}); break;
    case 10: mat = 0; c.a[7] = Special(g); break;                                              // sin matriz el rumbo no influye
    case 11: mat = 1; for (u32 k = 0; k < 3; ++k) c.a[1 + k] = g.Pk({0x00000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u}); break;
    case 12: mat = 1; c.a[1] = 0x7F800001u; c.a[2] = 0xFF800001u; c.a[3] = 0x7FFFFFFFu; break;   // NaN senalizantes: la copia es en crudo
    case 13: mat = g.P(30); break;
    default: break;
    }
    c.a[0] = mat;
}

// ------------------------------------------------------------------------------------------------ funcion 2: CPhysical::ApplyMoveForce(x, y, z) (0x4ABBA0)
//   a[0..2] = x, y, z (patrones de 32 bits cualesquiera: el EXE y la DLL los pasan en crudo a ApplyMoveForce(CVector) 0x5429F0)
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 3; ++k) c.a[k] = B(g.Fl(-100.0f, 100.0f));
    switch (f) {
    case 0: break;
    case 1: for (u32 k = 0; k < 3; ++k) c.a[k] = g.W(); break;
    case 2: for (u32 k = 0; k < 3; ++k) c.a[k] = Special(g); break;
    case 3: for (u32 k = 0; k < 3; ++k) c.a[k] = kNans[g.R(8)]; break;
    case 4: c.a[g.R(3)] = kNans[g.R(8)]; break;
    case 5: for (u32 k = 0; k < 3; ++k) c.a[k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u}); break;
    case 6: for (u32 k = 0; k < 3; ++k) c.a[k] = g.Pk({0x00000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80000001u}); break;
    case 7: for (u32 k = 0; k < 3; ++k) c.a[k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u}); break;
    case 8: for (u32 k = 0; k < 3; ++k) c.a[k] = B(g.Fl(-3000.0f, 3000.0f)); break;
    case 9: c.a[0] = c.a[1]; c.a[2] = c.a[1]; break;
    case 10: for (u32 k = 0; k < 3; ++k) c.a[k] = g.Pk({0x7F800000u, 0xFF800000u}); break;
    case 11: c.a[0] = g.W(); break;
    case 12: c.a[1] = g.W(); break;
    case 13: c.a[2] = g.W(); break;
    case 14: for (u32 k = 0; k < 3; ++k) c.a[k] = g.P(50) ? g.W() : Special(g); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 3: CPedIntelligence::GetPedFOVRange (0x4ABEA0)
//   a[0] = alcance de oido (+0xBC), a[1] = alcance de vista (+0xC0)
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = B(g.Fl(0.0f, 100.0f)); c.a[1] = B(g.Fl(0.0f, 100.0f));
    switch (f) {
    case 0: c.a[0] = B(g.Fl(50.0f, 100.0f)); c.a[1] = B(g.Fl(0.0f, 50.0f)); break;      // oido mayor
    case 1: c.a[0] = B(g.Fl(0.0f, 50.0f)); c.a[1] = B(g.Fl(50.0f, 100.0f)); break;      // vista mayor
    case 2: c.a[1] = c.a[0]; break;                                                         // iguales
    case 3: c.a[0] = g.Pk({0u, 0x80000000u}); c.a[1] = g.Pk({0u, 0x80000000u}); break;     // ceros con signo
    case 4: c.a[0] = kNans[g.R(8)]; break;
    case 5: c.a[1] = kNans[g.R(8)]; break;
    case 6: c.a[0] = kNans[g.R(8)]; c.a[1] = kNans[g.R(8)]; break;
    case 7: c.a[0] = g.Pk({0x7F800000u, 0xFF800000u}); c.a[1] = g.P(50) ? c.a[0] : B(g.Fl(0.0f, 100.0f)); break;
    case 8: c.a[1] = g.Pk({0x7F800000u, 0xFF800000u}); break;
    case 9: c.a[0] = g.Pk({1u, 0x007FFFFFu, 0x80000001u, 0x00800000u}); c.a[1] = g.Pk({0u, 1u, 0x007FFFFFu, 0x00800001u, 0x80000000u}); break;
    case 10: c.a[0] = g.W(); c.a[1] = g.W(); break;
    case 11: c.a[0] = Special(g); c.a[1] = Special(g); break;
    case 12: c.a[0] = NextUp(c.a[1], int(g.R(5)) - 2); break;                                // a pocos ulps
    case 13: c.a[0] = B(-g.Fl(0.0f, 10.0f)); c.a[1] = B(-g.Fl(0.0f, 10.0f)); break;
    case 14: c.a[0] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F800000u}); c.a[1] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F800000u}); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 4: CPedGroups::RegisterKillByPlayer (0x5F7E30)
//   a[0] = dword en 0xC098E8 (el byte bajo es ms_bIsPlayerOnAMission), a[1] = dword en 0xC098EC (contador de muertes)
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(2) | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u);
    c.a[1] = g.R(1000);
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 1; break;
    case 2: c.a[0] = g.Pk({2u, 0x7Fu, 0x80u, 0xFFu, 0x81u}); break;
    case 3: c.a[0] = g.W() & 0xFFFFFF00u; break;                                              // byte bajo cero con basura arriba
    case 4: c.a[0] = g.W() | 1u; break;
    case 5: c.a[1] = g.Pk({0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu}); c.a[0] = 1; break;
    case 6: c.a[1] = g.W(); c.a[0] = g.R(2); break;
    case 7: c.a[1] = 0; c.a[0] = g.R(2); break;
    case 8: c.a[1] = g.Pk({0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu}); c.a[0] = 0; break;
    case 9: c.a[0] = g.Pk({0x100u, 0x200u, 0x01000000u, 0x80000000u}); break;                  // byte bajo cero
    case 10: c.a[0] = g.W(); c.a[1] = g.W(); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 5: CMissionCleanup::AddEntityToList(handle, tipo) (0x4637E0)
//   a[0] = handle, a[1] = tipo (dword: el EXE guarda solo el byte bajo), a[2] = indice del primer hueco libre (0..75; 75 = lleno), a[3] = contador inicial (byte +0x258; el resto del dword es basura),
//   a[4..7] = mascara de ocupacion de los huecos posteriores al primero libre (bit s % 32 de a[4 + s / 32]; 1 = ocupado), a[8] = handle inicial del primer hueco libre, a[9] = semilla de los demas
inline constexpr unsigned kCleanSlots = 75;
inline bool CleanOccupied(const C& c, unsigned s) {
    if (s < c.a[2]) return true;
    if (s == c.a[2]) return false;
    return (c.a[4 + s / 32] >> (s % 32)) & 1u;
}
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(50) ? g.R(2000) : g.W();
    c.a[1] = g.Pk({1u, 2u, 3u, 4u, 5u, 7u, 8u, 9u, 10u, 11u});
    c.a[2] = g.R(kCleanSlots + 1);
    c.a[3] = g.P(50) ? g.R(kCleanSlots) : g.W();
    for (u32 k = 0; k < 4; ++k) c.a[4 + k] = g.W();
    c.a[8] = g.P(50) ? 0u : g.W();
    c.a[9] = g.W();
    switch (f) {
    case 0: c.a[2] = 0; break;
    case 1: c.a[2] = kCleanSlots; break;                                                     // lleno: no hace nada
    case 2: c.a[2] = kCleanSlots - 1; break;                                                 // ultimo hueco
    case 3: c.a[2] = 1 + g.R(4); break;
    case 4: c.a[1] = g.R(256); break;                                                         // tipos de un byte (0 = deja el hueco libre)
    case 5: c.a[1] = g.W(); break;                                                            // basura en los bytes altos del tipo
    case 6: c.a[1] = (g.W() & 0xFFFFFF00u); break;                                            // byte bajo 0: guarda 0
    case 7: c.a[0] = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu}); break;
    case 8: c.a[3] = g.Pk({0u, 0x7Fu, 0x80u, 0xFFu, 74u, 75u}); break;                        // el contador (byte) da la vuelta
    case 9: c.a[8] = c.a[0]; break;                                                           // el handle inicial ya es el nuevo
    case 10: c.a[2] = g.Pk({0u, 1u, 73u, 74u, 75u}); c.a[4] = 0; c.a[5] = 0; c.a[6] = 0; c.a[7] = 0; break;
    case 11: c.a[4] = 0; c.a[5] = 0; c.a[6] = 0; c.a[7] = 0; break;                           // todos los demas libres
    case 12: c.a[4] = 0xFFFFFFFFu; c.a[5] = 0xFFFFFFFFu; c.a[6] = 0xFFFFFFFFu; c.a[7] = 0xFFFFFFFFu; break;
    case 13: c.a[1] = (g.W() & 0xFFFFFF00u) | g.Pk({1u, 2u, 3u}); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 6: CEventDamage::IsSameEventForAI (0x4ADBF0)
//   a[0] = fuente del propio evento (+0x14), a[1] = arma (+0x1C), a[2] = fuente del otro, a[3] = arma del otro, a[4] = el otro es el mismo objeto
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.W(); c.a[1] = g.R(60); c.a[2] = g.W(); c.a[3] = g.R(60); c.a[4] = g.P(8);
    switch (f) {
    case 0: c.a[2] = c.a[0]; c.a[3] = c.a[1]; c.a[4] = 0; break;                            // iguales
    case 1: c.a[2] = c.a[0]; c.a[4] = 0; break;                                              // misma fuente, otra arma (probable)
    case 2: c.a[3] = c.a[1]; c.a[4] = 0; break;                                              // misma arma, otra fuente
    case 3: c.a[4] = 1; break;
    case 4: c.a[0] = 0; c.a[2] = 0; c.a[4] = 0; break;                                       // ambas fuentes nulas
    case 5: c.a[2] = c.a[0]; c.a[3] = c.a[1] ^ 0x100u; c.a[4] = 0; break;                     // armas que solo difieren en un bit alto
    case 6: c.a[2] = c.a[0] ^ 0x80000000u; c.a[3] = c.a[1]; c.a[4] = 0; break;                // fuentes que solo difieren en el bit alto
    case 7: c.a[2] = c.a[0]; c.a[3] = c.a[1] ^ 1u; c.a[4] = 0; break;
    case 8: c.a[1] = g.W(); c.a[3] = c.a[1]; c.a[2] = c.a[0]; c.a[4] = 0; break;              // armas de 32 bits cualesquiera iguales
    case 9: c.a[1] = g.W(); c.a[3] = g.P(50) ? c.a[1] : g.W(); c.a[2] = c.a[0]; c.a[4] = 0; break;
    case 10: c.a[1] = g.Pk({0xFFFFFFFFu, 0u, 0x80000000u, 0x7FFFFFFFu, 0x12u}); c.a[3] = g.Pk({0xFFFFFFFFu, 0u, 0x80000000u, 0x7FFFFFFFu, 0x12u}); c.a[2] = c.a[0]; c.a[4] = 0; break;
    case 11: c.a[0] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u}); c.a[2] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u}); c.a[3] = c.a[1]; c.a[4] = 0; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 7: CEntryExitManager::WeAreInInteriorTransition (0x43E400)
//   a[0] = dword en 0x96A7CC (ms_exitEnterState)
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(5);
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 1 + g.R(4); break;
    case 2: c.a[0] = g.W(); break;
    case 3: c.a[0] = g.Pk({0x100u, 0x10000u, 0x01000000u, 0x80000000u, 0xFFFFFF00u}); break;
    case 4: c.a[0] = g.Pk({0xFFFFFFFFu, 1u, 0x7FFFFFFFu, 0xFFu}); break;
    case 5: c.a[0] = g.P(50) ? 0u : g.W(); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 8: CPointRoute::AddPoint (0x48E180)
//   a[0] = contador (dword; con signo), a[1..3] = el punto (patrones crudos), a[4..6] = valor inicial de la entrada contador (3 dwords; solo si cabe)
inline void Make8(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    int count = int(g.R(8));
    c.a[1] = Raw(g); c.a[2] = Raw(g); c.a[3] = Raw(g);
    for (u32 k = 0; k < 3; ++k) c.a[4 + k] = g.P(60) ? g.W() : 0u;
    switch (f) {
    case 0: count = 0; break;
    case 1: count = 7; break;
    case 2: count = 8; break;
    case 3: count = 9 + int(g.R(100)); break;
    case 4: count = g.Pk({0x7FFFFFFFu, 100u, 0x10000u, 0x7FFFFFF0u, 1000u}); break;
    case 5: count = -1 - int(g.R(5)); break;                                                 // negativo: escribe antes de la ruta (guarda de la arena)
    case 6: count = int(g.R(8)); for (u32 k = 0; k < 3; ++k) c.a[1 + k] = B(g.Fl(-3000.0f, 3000.0f)); break;
    case 7: count = int(g.R(8)); for (u32 k = 0; k < 3; ++k) c.a[1 + k] = kNans[g.R(8)]; break;
    case 8: count = int(g.R(8)); for (u32 k = 0; k < 3; ++k) c.a[4 + k] = c.a[1 + k]; break;   // el valor inicial ya es el punto
    case 9: count = int(g.R(9)); break;
    case 10: count = -1; break;
    case 11: count = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u}); break;
    case 12: count = g.P(50) ? 8 : 7; break;
    default: break;
    }
    c.a[0] = u32(count);
}

// ------------------------------------------------------------------------------------------------ funcion 9: CPointRoute::RemovePoint (0x5F13A0)
//   a[0] = contador (dword; con signo), a[1] = indice (con signo), a[2] = semilla del contenido inicial de las entradas (RouteWord: entradas -5..10; el contador pisa el dword 0)
inline void Make9(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    int count = int(g.R(9)) ;
    int idx = int(g.R(9));
    c.a[2] = g.W();
    switch (f) {
    case 0: idx = count - 1; break;                                                          // quita la ultima
    case 1: idx = 0; break;
    case 2: idx = count; break;                                                              // indice == contador
    case 3: idx = count + 1 + int(g.R(3)); break;                                            // indice mayor que el contador
    case 4: count = -2 + int(g.R(2)); idx = int(g.R(10)); break;                             // contador <= 0
    case 5: count = 9 + int(g.R(2)); idx = int(g.R(10)); break;                              // contador 9 o 10 (la 8 queda fuera del array)
    case 6: count = 1 + int(g.R(8)); idx = count - 2; break;                                 // penultima
    case 7: count = 8; idx = int(g.R(9)); break;
    case 8: count = int(g.R(11)); idx = 12 - int(g.R(3)); break;
    case 9: count = int(g.R(11)) - 2; idx = -1 - int(g.R(4)); break;                         // indice negativo (se corrige abajo si habria pasadas)
    case 10: count = 2; idx = int(g.R(3)); break;
    case 11: count = int(g.R(11)); idx = int(g.R(11)); break;
    case 12: count = 1; idx = int(g.R(3)) - 1; if (idx < 0) idx = 1 + int(g.R(2)); break;
    default: break;
    }
    // un indice negativo escribiria sobre el contador (entrada -1) o cambiaria las pasadas: solo se prueba cuando no hay ninguna pasada (indice >= contador - 1)
    if (idx < 0 && idx < count - 1) idx = count - 1;
    if (idx < -4) idx = -4;
    c.a[0] = u32(count); c.a[1] = u32(idx);
}

// ------------------------------------------------------------------------------------------------ funcion 10: CPedGroupIntelligence::GetTask (0x5F7660)
//   a[0] = ped buscado (0 = ped arena, 1 = nulo, 2 = valor crudo), a[1] = indice del primer par que lo contiene (0..7; 8 = ninguno), a[2] = indice de un segundo par que tambien lo contiene (0..7; 8 = ninguno),
//   a[3] = semilla del contenido de los pares (ped y tarea de cada par; palabras crudas), a[4] = mascara de pares con el ped nulo (bit j) cuando se busca un ped nulo
inline void Make10(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? 0u : g.R(3);
    c.a[1] = g.R(9); c.a[2] = g.P(30) ? g.R(9) : 8u;
    c.a[3] = g.W(); c.a[4] = g.W() & 0xFFu;
    switch (f) {
    case 0: c.a[0] = 0; c.a[1] = 0; c.a[2] = 8; break;
    case 1: c.a[0] = 0; c.a[1] = 7; c.a[2] = 8; break;
    case 2: c.a[0] = 0; c.a[1] = 8; c.a[2] = 8; break;                                       // sin coincidencia
    case 3: c.a[0] = 0; c.a[1] = 1 + g.R(3); c.a[2] = 4 + g.R(4); break;                      // dos coincidencias: gana la primera
    case 4: c.a[0] = 1; c.a[4] = 0; break;                                                    // ped nulo sin pares nulos
    case 5: c.a[0] = 1; c.a[4] = 1u << g.R(8); break;                                         // ped nulo con un par nulo
    case 6: c.a[0] = 1; c.a[4] = g.W() & 0xFFu; break;                                        // ped nulo con varios
    case 7: c.a[0] = 2; break;
    case 8: c.a[0] = 0; c.a[1] = g.R(8); c.a[2] = 8; break;
    case 9: c.a[0] = 0; c.a[1] = 8; c.a[2] = 8; c.a[3] = g.W(); break;
    case 10: c.a[1] = 0; c.a[2] = 7; break;
    case 11: c.a[1] = 3; c.a[2] = 3; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 11: CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent (0x4AE100)
//   a[0] = tipo que devuelve GetEventType (ranura 1) del evento propio, a[1] = tipo del evento del otro, a[2] = lo que devuelve TakesPriorityOver (ranura 11) (dword),
//   a[3] = el evento del grupo del otro apunta al mismo evento que el propio (el otro evento grupo es el mismo objeto de evento)
inline u32 PriorityTypes(Gen& g) { return g.Pk({0x36u, 0x37u, 0x39u, 0x47u, 0x4Cu, 0x07u, 0x4Du}); }
inline u32 PlainTypes(Gen& g) {
    for (;;) { const u32 t = g.R(0x60); if (t != 0x36 && t != 0x37 && t != 0x39 && t != 0x47 && t != 0x4C && t != 7 && t != 0x4D) return t; }
}
inline void Make11(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(50) ? PriorityTypes(g) : PlainTypes(g);
    c.a[1] = g.P(50) ? PriorityTypes(g) : PlainTypes(g);
    c.a[2] = g.P(50) ? g.R(2) : g.W();
    c.a[3] = 0;
    switch (f) {
    case 0: c.a[0] = PriorityTypes(g); c.a[1] = PriorityTypes(g); break;
    case 1: c.a[0] = PriorityTypes(g); c.a[1] = PlainTypes(g); break;
    case 2: c.a[0] = PlainTypes(g); c.a[1] = PriorityTypes(g); break;
    case 3: c.a[0] = PlainTypes(g); c.a[1] = PlainTypes(g); break;                            // llama a TakesPriorityOver
    case 4: c.a[0] = PlainTypes(g); c.a[1] = PlainTypes(g); c.a[2] = 0; break;
    case 5: c.a[0] = PlainTypes(g); c.a[1] = PlainTypes(g); c.a[2] = 1; break;
    case 6: c.a[0] = PlainTypes(g); c.a[1] = PlainTypes(g); c.a[2] = g.Pk({0x100u, 0x80000000u, 0xFFFFFF00u, 2u, 0xFFu}); break;   // solo cuenta AL
    case 7: c.a[0] = PlainTypes(g); c.a[1] = c.a[0]; c.a[3] = 1; break;                       // mismo evento
    case 8: c.a[0] = g.Pk({0x35u, 0x38u, 0x3Au, 0x46u, 0x48u, 0x4Bu, 0x4Eu, 6u, 8u}); c.a[1] = g.Pk({0x35u, 0x38u, 0x3Au, 0x46u, 0x48u, 0x4Bu, 0x4Eu, 6u, 8u}); break;   // vecinos de los tipos de prioridad
    case 9: c.a[0] = g.Pk({0u, 0x7Fu, 0x100u, 0xFFFFFFFFu, 0x80000036u, 0x00000136u}); c.a[1] = g.Pk({0u, 0x7Fu, 0x100u, 0xFFFFFFFFu, 0x80000047u}); break;
    case 10: c.a[0] = g.W(); c.a[1] = g.W(); break;
    case 11: c.a[0] = PriorityTypes(g); c.a[1] = c.a[0]; c.a[3] = 1; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 12: CEvent::TakesPriorityOverBase (0x4ABC30)
//   a[0] = prioridad (ranura 2) del propio evento, a[1] = prioridad del otro (int32 con signo), a[2] = el otro es el mismo objeto
inline void Make12(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(100); c.a[1] = g.R(100); c.a[2] = g.P(8);
    switch (f) {
    case 0: c.a[0] = 50 + g.R(50); c.a[1] = g.R(50); break;                                   // mayor
    case 1: c.a[0] = g.R(50); c.a[1] = 50 + g.R(50); break;                                   // menor
    case 2: c.a[1] = c.a[0]; break;                                                            // igual
    case 3: c.a[0] = g.Pk({0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0u, 1u}); c.a[1] = g.Pk({0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0u, 1u}); break;   // frontera del signo
    case 4: c.a[0] = u32(-int(1 + g.R(50))); c.a[1] = u32(-int(1 + g.R(50))); break;
    case 5: c.a[0] = g.W(); c.a[1] = g.W(); break;
    case 6: c.a[2] = 1; break;
    case 7: c.a[0] = 0x80000000u; c.a[1] = 0x7FFFFFFFu; break;
    case 8: c.a[0] = 0x7FFFFFFFu; c.a[1] = 0x80000000u; break;
    case 9: c.a[1] = c.a[0]; c.a[2] = g.R(2); break;
    case 10: c.a[0] = u32(int(g.R(5)) - 2); c.a[1] = u32(int(g.R(5)) - 2); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 13: IsPedPointerValid (0x5E8A30)
//   a[0] = capacidad del pool (0..5), a[1] = ped (0 = hueco i del almacenamiento con desplazamiento a[2], 1 = nulo, 2 = por debajo del almacenamiento, 3 = por encima), a[2] = hueco (0..4),
//   a[3] = desplazamiento en bytes respecto al inicio del hueco (con signo, -0x7C3..0x7C3), a[4] = bytes de estado de los huecos 0..3 (byte j) y a[10] = el del hueco 4, a[5] = dword +0x46C del ped (bit 8 = en vehiculo),
//   a[6] = vehiculo +0x58C (0 arena, 1 nulo, 2 crudo), a[7] = nodo de colision +0xB0 (0 nulo, 1 no nulo), a[8] = FindPlayerPed (0 el ped, 1 nulo, 2 otro ped, 3 crudo),
//   a[9] = lo que devuelve IsEntityPointerValid (dword)
inline void Make13(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = 1 + g.R(5);
    c.a[1] = g.P(80) ? 0u : g.R(4);
    c.a[2] = g.R(5);
    c.a[3] = g.P(60) ? 0u : u32(int(g.R(1999)) - 999);
    c.a[4] = 0;
    for (u32 j = 0; j < 4; ++j) c.a[4] |= (g.P(70) ? 0u : (0x80u | g.R(0x7F))) << (8 * j);
    c.a[10] = g.P(70) ? 0u : (0x80u | g.R(0x7F));
    c.a[5] = (g.W() & ~0x100u) | (g.P(50) ? 0x100u : 0u);
    c.a[6] = g.R(3); c.a[7] = g.P(50); c.a[8] = g.R(4); c.a[9] = g.P(50) ? g.R(2) : g.W();
    switch (f) {
    case 0: c.a[1] = 0; c.a[3] = 0; c.a[4] = 0; c.a[5] &= ~0x100u; c.a[7] = 1; break;           // valido, fuera de vehiculo, con nodo de colision
    case 1: c.a[1] = 0; c.a[3] = 0; c.a[4] = 0; c.a[5] &= ~0x100u; c.a[7] = 0; break;           // sin nodo: depende de FindPlayerPed
    case 2: c.a[1] = 0; c.a[3] = 0; c.a[4] = 0; c.a[5] |= 0x100u; c.a[6] = 0; break;            // en vehiculo con vehiculo
    case 3: c.a[1] = 0; c.a[3] = 0; c.a[4] = 0; c.a[5] |= 0x100u; c.a[6] = 1; break;            // en vehiculo sin vehiculo: sigue
    case 4: c.a[1] = 0; c.a[3] = 0; c.a[4] = 0xFFFFFFFFu; c.a[10] = 0xFFu; break;                                 // huecos libres
    case 5: c.a[1] = 1; break;                                                                    // ped nulo
    case 6: c.a[1] = 2; break;                                                                    // por debajo
    case 7: c.a[1] = 3; break;                                                                    // por encima
    case 8: c.a[1] = 0; c.a[3] = u32(int(g.R(0x7C3)) - 0x3E1); break;                                  // desplazamiento dentro del hueco (el indice no cambia)
    case 9: c.a[1] = 0; if (g.P(50)) c.a[2] = 0; c.a[3] = g.Pk({0x7C3u, u32(-0x7C3), 0x7C4u, u32(-0x7C4), 1u, u32(-1)}); break;   // frontera entre huecos
    case 10: c.a[1] = 0; c.a[2] = g.R(5); c.a[0] = g.R(6); break;                                // indice fuera de la capacidad
    case 11: c.a[1] = 0; c.a[3] = 0; c.a[0] = 1 + g.R(5); c.a[4] = 0; c.a[5] &= ~0x100u; c.a[7] = 0; c.a[8] = g.R(4); break;
    case 12: c.a[1] = 0; c.a[3] = 0; c.a[4] = 0; c.a[5] = g.W(); break;
    case 13: c.a[1] = 0; c.a[3] = 0; c.a[4] = g.W() & 0x80808080u; c.a[10] = g.W() & 0x80u; break;                          // estados con solo el bit alto
    default: break;
    }
    // El hueco j del ped vale 0x800 + j * 0x7C4 + desplazamiento: se mantiene dentro de la arena (hueco 4 con +0x7C3 llega a +0x2ECF; los campos del ped hasta +0x345F)
    if (c.a[1] == 0 && c.a[2] > 4) c.a[2] = 4;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    switch (fn) {
    case 0: Make0(c, n); break;
    case 1: Make1(c, n); break;
    case 2: Make2(c, n); break;
    case 3: Make3(c, n); break;
    case 4: Make4(c, n); break;
    case 5: Make5(c, n); break;
    case 6: Make6(c, n); break;
    case 7: Make7(c, n); break;
    case 8: Make8(c, n); break;
    case 9: Make9(c, n); break;
    case 10: Make10(c, n); break;
    case 11: Make11(c, n); break;
    case 12: Make12(c, n); break;
    default: Make13(c, n); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}
