#pragma once
// Fixtures del banco Fn113A (lote 113A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag 0x63AB70 thiscall (ECX = tarea), sin argumentos, `ret`; devuelve 10 o 0 en EAX                 (hook S)
//   1 CTaskComplexWander::RequestDir                         0x669D30 thiscall (ECX = tarea; dir como dword en pila), `ret 4`; void                    (hook S)
//   2 CTaskComplexWander::RequestNodesFrom                   0x669D50 thiscall (ECX = tarea; otra tarea en pila), `ret 4`; void                        (hook S)
//   3 CTaskComplexWanderStandard::SetMinNextScanTime         0x66B150 thiscall (ECX = tarea; tiempo en pila), `ret 4`; void                            (hook S)
//   4 CTaskComplexPartner::GetStateOfPartnerTask             0x6822B0 thiscall (ECX = tarea), `ret`; devuelve el estado (con signo) o 0 en EAX         (hook S; colaborador 0x681740)
//   5 GangDrinkAnimOfPed                                     0x65E6F0 cdecl (ped); devuelve 299 o 301 en EAX                                           (hook S, TaskComplexGangLeader.cpp)
//   6 GangSmokeAnimOfPed                                     0x65E710 cdecl (ped); devuelve 300 o 302 en EAX                                           (hook S)
//   7 CTaskAllocator::operator new                           0x69D980 cdecl (tamano, ignorado); devuelve el objeto del pool 0xB744BC en EAX            (hook S; colaborador real 0x69D8E0)
//   8 CTaskAllocator::operator delete                        0x69D990 cdecl (objeto); void                                                              (hook S)
//   9 CPedIntelligence::operator new                         0x6074A0 cdecl (tamano, ignorado); devuelve el objeto del pool 0xB744C0 en EAX            (hook S; colaborador real 0x605EC0)
//  10 CPedIntelligence::operator delete                      0x6074E0 cdecl (objeto); void                                                              (hook S)
//  11 CPlayerRelationshipRecorder::ClearRelationshipWithPlayer 0x61A150 thiscall (ECX = registro; ped en pila), `ret 4`; void                           (hook S)
//  12 CPlayerRelationshipRecorder::AddRelationship           0x61A180 thiscall (ECX = registro; ped y valor en pila), `ret 8`; void                    (hook S)
//  13 CPlayerRelationshipRecorder::GetRelationshipWithPlayer 0x61A1A0 thiscall (ECX = registro; ped en pila), `ret 4`; devuelve AL                      (hook S)
//  14 CPlayerRelationshipRecorder::Flush                     0x61A2A0 thiscall (ECX = registro), `ret`; void (el EXE deja this en EAX; ningun llamador lo lee) (hook S)
//  15 CEventScanner::ScanForEventsNow                        0x6053D0 thiscall (ECX = escaner; ped y dword en pila), `ret 8`; void                     (hook S; colaborador 0x603720)
//  16 CTaskTimer::GetTimeLeft                                0x62EB80 thiscall (ECX = temporizador), `ret`; devuelve int32 en EAX                       (hook S)
//
// Familia = n % 16 (1024 casos por familia).
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

namespace F113AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 17;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x100, kOtherWin = 0x100, kPedWin = 0x5A0, kIntelWin = 0x300, kPed2Win = 0x5A0, kIntel2Win = 0x300, kPTaskWin = 0x100, kPoolWin = 0x20, kStoWin = 0x1500, kStWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 113001u); }
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
    u32 W() { return Rand(n, nk()); }
};

// ------------------------------------------------------------------------------------------------ funcion 0: ComputeTargetDoorOppositeToFlag (0x63AB70)
//   a[0] = byte +0x10 (bit 0 = m_bAsDriver, el resto de bits son de otras banderas), a[1] = m_TargetDoor (dword +0x14)
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.W() & 0xFFu; c.a[1] = g.P(40) ? 8u : g.Pk({0u, 7u, 9u, 10u, 11u, 16u, 0xFFFFFFF8u, 0x80000008u, 0x108u, 0x10000008u, 1u, 0xFFFFFFFFu});
    switch (f) {
    case 0: c.a[0] |= 1; c.a[1] = 8; break;
    case 1: c.a[0] &= 0xFEu; c.a[1] = 8; break;
    case 2: c.a[0] |= 1; c.a[1] = g.Pk({7u, 9u, 10u, 11u, 0u, 0x108u, 0x10000008u, 0x80000008u}); break;
    case 3: c.a[0] = 1; c.a[1] = 8; break;
    case 4: c.a[0] = 0xFEu; c.a[1] = 8; break;
    case 5: c.a[0] = 0xFFu; c.a[1] = 8; break;
    case 6: c.a[0] = 0u; c.a[1] = 0u; break;
    case 7: c.a[0] = 1u << g.R(8); c.a[1] = 8; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 1: RequestDir (0x669D30)
//   a[0] = byte +0x10 (direccion actual), a[1] = argumento (dword), a[2] = byte de banderas +0x24
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.W() & 0xFFu; c.a[2] = g.W() & 0xFFu;
    c.a[1] = g.P(35) ? c.a[0] : (g.P(50) ? (g.W() & 0xFFu) : g.W());
    switch (f) {
    case 0: c.a[1] = c.a[0]; break;
    case 1: c.a[1] = (c.a[0] + 1 + g.R(255)) & 0xFFu; break;
    case 2: c.a[1] = c.a[0] | (0x100u << g.R(24)); break;
    case 3: c.a[1] = c.a[0] | 0x80000000u; break;
    case 4: c.a[0] = 0; c.a[1] = 0; c.a[2] = 0xFFu; break;
    case 5: c.a[1] = (c.a[0] + 1 + g.R(255)) & 0xFFu; c.a[2] |= 2u; break;
    case 6: c.a[1] = 0xFFu; c.a[2] = g.Pk({0u, 2u, 0xFDu, 0xFFu}); break;
    case 7: c.a[1] = c.a[0]; c.a[2] = g.Pk({0u, 2u, 0xFDu, 0xFFu}); break;
    case 8: c.a[1] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFF00u, 0x7FFFFFFFu}); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 2: RequestNodesFrom (0x669D50)
//   a[0..3] = palabras +0x18, +0x1A, +0x1C, +0x1E de la tarea (area y nodo anterior, area y nodo siguiente), a[4..7] = las de la otra tarea, a[8] = byte +0x10 de la tarea,
//   a[9] = byte +0x10 de la otra, a[10] = byte de banderas +0x24 de la tarea
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 4; ++k) c.a[k] = g.P(30) ? 0xFFFFu : (g.W() & 0xFFFFu);
    for (u32 k = 0; k < 4; ++k) c.a[4 + k] = g.P(60) ? c.a[k] : (g.P(30) ? 0xFFFFu : (g.W() & 0xFFFFu));
    c.a[8] = g.W() & 0xFFu; c.a[9] = g.W() & 0xFFu; c.a[10] = g.W() & 0xFFu;
    auto same = [&] { for (u32 k = 0; k < 4; ++k) c.a[4 + k] = c.a[k]; };
    auto set_diff = [&](u32 k) { c.a[4 + k] = (c.a[k] + 1 + g.R(0xFFFE)) & 0xFFFFu; };
    switch (f) {
    case 0: same(); break;
    case 1: same(); set_diff(0); break;
    case 2: same(); set_diff(1); break;
    case 3: same(); set_diff(2); break;
    case 4: same(); set_diff(3); break;
    case 5: for (u32 k = 0; k < 4; ++k) set_diff(k); break;
    case 6: same(); c.a[9] = c.a[8]; c.a[10] = 0xFFu; break;
    case 7: same(); set_diff(g.R(4)); c.a[10] = g.Pk({0u, 4u, 0xFBu, 0xFFu}); break;
    case 8: for (u32 k = 0; k < 4; ++k) c.a[4 + k] = c.a[k] ^ 1u; break;
    case 9: for (u32 k = 0; k < 4; ++k) { c.a[k] = 0xFFFFu; c.a[4 + k] = 0xFFFFu; } set_diff(g.R(4)); break;
    case 10: same(); c.a[8] = c.a[9] ^ 0xFFu; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 3: SetMinNextScanTime (0x66B150)
//   a[0] = dword +0x34 antes, a[1] = argumento
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.W(); c.a[1] = g.W();
    switch (f) {
    case 0: c.a[1] = c.a[0]; break;
    case 1: c.a[1] = 0; break;
    case 2: c.a[1] = 0xFFFFFFFFu; break;
    case 3: c.a[0] = 0; break;
    case 4: c.a[1] = c.a[0] ^ 0xFFu; break;
    case 5: c.a[1] = g.Pk({0x80000000u, 1u, 0x7FFFFFFFu, 100000u}); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 4: GetStateOfPartnerTask (0x6822B0)
//   a[0] = tipo de tarea (dword +0x34), a[1] = lo que devuelve FindActiveTaskByType (0 nulo, 1 la tarea de la arena), a[2] = byte +0x59 (estado) de esa tarea
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(50) ? g.Pk({0u, 1u, 0x3E8u, 0x4B0u, 0x4B1u, 0xFFFFFFFFu, 0x80000000u, 0x10000u}) : g.W(); c.a[1] = g.P(30) ? 0u : 1u; c.a[2] = g.W() & 0xFFu;
    switch (f) {
    case 0: c.a[1] = 0; break;
    case 1: c.a[1] = 1; c.a[2] = g.R(9); break;
    case 2: c.a[1] = 1; c.a[2] = 0x80u + g.R(128); break;
    case 3: c.a[1] = 1; c.a[2] = g.Pk({0u, 5u, 6u, 4u, 0x7Fu, 0x80u, 0xFFu}); break;
    case 4: c.a[1] = 1; c.a[0] = g.Pk({0x3E8u, 0x4B0u}); break;
    case 5: c.a[1] = 0; c.a[2] = g.W() & 0xFFu; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funciones 5 y 6: GangDrinkAnimOfPed / GangSmokeAnimOfPed
//   a[0] = palabra +0x22 del peaton (indice de modelo; solo cuentan los 16 bits)
inline void Make56(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(40) ? g.Pk({0x67u, 0x69u, 0x68u, 0x66u, 0u, 0xFFFFu, 0x8067u, 0x8069u, 0x167u, 0x1067u, 0x6Au, 0x65u}) : (g.W() & 0xFFFFu);
    switch (f) {
    case 0: c.a[0] = 0x67u; break;
    case 1: c.a[0] = 0x69u; break;
    case 2: c.a[0] = 0x68u; break;
    case 3: c.a[0] = 0x8067u + 2 * g.R(2); break;
    case 4: c.a[0] = 0x66u + g.R(5); break;
    case 5: c.a[0] = g.W() & 0xFFFFu; break;
    case 6: c.a[0] = 0xFFFFu; break;
    case 7: c.a[0] = 0u; break;
    default: break;
    }
    c.a[0] &= 0xFFFFu;
}

// ------------------------------------------------------------------------------------------------ funciones 7..10: operator new / delete de los pools
//   a[0] = capacidad (1..8), a[1] = m_LastFreeSlot (dword con signo, -1..cap-1), a[2..9] = bytes de estado de las 8 ranuras (bit 7 = libre, bits 0..6 = id),
//   a[10] = ranura a borrar (0..cap-1; su byte de estado se fuerza a "ocupada"), a[11] = objeto interior (0 no, 1 desplazamiento a[13], 2 ultimo byte de la ranura), a[12] = tamano que se pasa a `operator new` (se ignora)
//   a[13] = desplazamiento en bruto
inline void MakePool(C& c, unsigned n, bool del) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = 1 + g.R(8);
    c.a[1] = u32(i32(g.R(c.a[0] + 1)) - 1);
    for (u32 k = 0; k < 8; ++k) c.a[2 + k] = g.P(50) ? (0x80u | (g.W() & 0x7Fu)) : (g.W() & 0x7Fu);
    c.a[10] = g.R(c.a[0]); c.a[11] = 0; c.a[12] = g.Pk({0x14u, 0x20u, 0u, 0xFFFFFFFFu, 0x294u, 0x7FFFFFFFu}); c.a[13] = g.W();
    auto all_free = [&] { for (u32 k = 0; k < 8; ++k) c.a[2 + k] = 0x80u | (g.W() & 0x7Fu); };
    auto all_used = [&] { for (u32 k = 0; k < 8; ++k) c.a[2 + k] = g.W() & 0x7Fu; };
    switch (f) {
    case 0: all_free(); break;                                   // todas libres
    case 1: all_used(); break;                                   // ninguna libre: devuelve nulo (new) y deja el cursor en 0
    case 2: all_used(); c.a[2 + g.R(c.a[0])] |= 0x80u; break;    // una sola libre
    case 3: c.a[1] = c.a[0] - 1; break;                          // cursor en la ultima ranura: envuelve
    case 4: c.a[1] = u32(-1); break;                             // cursor inicial -1
    case 5: for (u32 k = 0; k < 8; ++k) c.a[2 + k] = 0x80u | 0x7Fu; break;   // ids al maximo (el contador de 7 bits da la vuelta)
    case 6: for (u32 k = 0; k < 8; ++k) c.a[2 + k] = 0x80u; break;           // ids a cero
    case 7: c.a[0] = 1; c.a[1] = g.Pk({u32(-1), 0u}); break;     // capacidad 1
    case 8: c.a[0] = 8; c.a[10] = g.R(8); break;                 // capacidad completa
    case 9: c.a[1] = 0; break;
    case 10: c.a[1] = c.a[10]; break;                            // (delete) el cursor igual a la ranura
    case 11: c.a[1] = c.a[10] ? c.a[10] - 1 : u32(-1); break;    // (delete) el cursor justo debajo
    case 12: c.a[0] = 8; c.a[10] = 7; c.a[1] = 7; break;         // (delete) ultima ranura
    case 13: c.a[10] = 0; break;                                 // (delete) primera ranura
    case 14: c.a[11] = 2; break;                                 // (delete) ultimo byte del objeto
    case 15: c.a[11] = 1; break;                                 // (delete) desplazamiento interior cualquiera
    default: break;
    }
    if (c.a[0] > 8 || c.a[10] >= c.a[0]) c.a[10] = c.a[0] - 1;
    if (del) {
        c.a[2 + c.a[10]] &= 0x7Fu;                               // la ranura a borrar esta ocupada
        if (f >= 3 && f <= 9 && g.P(30)) c.a[11] = 1;
        if (f == 0) c.a[11] = 0;
    } else {
        c.a[11] = 0;
    }
}

// ------------------------------------------------------------------------------------------------ funciones 11..14: CPlayerRelationshipRecorder
//   a[0] = argumento ped (0, valores crudos), a[1] = mascara de entradas (16 bits) cuyo ped es igual al argumento (las demas llevan valores distintos), a[2] = semilla de las demas
//   palabras (relaciones de 32 bits), a[3] = valor (AddRelationship), a[4] = 1: el ped de la entrada 0 es nulo
inline void MakeRec(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(25) ? 0u : g.Pk({0x1234u, 0xBADC00u, 0xFFFFFFFFu, 0x80000000u, 0x7F000000u, 1u});
    c.a[1] = g.P(30) ? 0u : (g.P(50) ? (1u << g.R(16)) : (g.W() & 0xFFFFu));
    c.a[2] = g.W(); c.a[3] = g.P(30) ? (g.W() & 0xFFu) : g.W(); c.a[4] = g.P(40) ? 1u : 0u;
    switch (f) {
    case 0: c.a[1] = 0; break;                                    // ninguna coincide
    case 1: c.a[1] = 1; break;                                    // solo la primera
    case 2: c.a[1] = 0x8000u; break;                              // solo la ultima
    case 3: c.a[1] = 0xFFFFu; break;                              // todas
    case 4: c.a[1] = 0xFFFEu; break;                              // todas menos la primera
    case 5: c.a[0] = 0; c.a[1] = 0x00F0u; break;                  // peaton nulo: coinciden entradas vacias
    case 6: c.a[4] = 1; c.a[1] = 0; break;                        // entrada 0 con ped nulo
    case 7: c.a[4] = 0; c.a[3] = 0x100u | (g.W() & 0xFFu); break; // valor con bits altos (solo el byte bajo)
    case 8: c.a[4] = 1; c.a[0] = g.Pk({0x1234u, 0xBADC00u}); break;
    case 9: c.a[1] = 1u << g.R(16); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 15: CEventScanner::ScanForEventsNow (0x6053D0)
//   a[0] = CTimer::m_snTimeInMilliseconds (0xB7CB84), a[1] = inicio del temporizador del escaner de vehiculos (+4), a[2] = intervalo (+8), a[3] = byte +0xC (en marcha), a[4] = dword "no escanear"
inline void Make15(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.W(); c.a[1] = g.W(); c.a[2] = g.W(); c.a[3] = g.P(50) ? g.R(2) : (g.W() & 0xFFu);
    c.a[4] = g.P(50) ? g.R(2) : g.Pk({0x100u, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFF00u, 0x7FFFFFFFu, 2u, g.W()});
    switch (f) {
    case 0: c.a[4] = 0; c.a[3] = 1; break;
    case 1: c.a[4] = 0; c.a[3] = 0; break;
    case 2: c.a[4] = 1; c.a[3] = 1; break;
    case 3: c.a[4] = 0; c.a[3] = g.Pk({2u, 0x80u, 0xFFu, 0x10u}); break;
    case 4: c.a[4] = g.Pk({0x100u, 0x80000000u, 0xFFFFFF00u, 0x1000000u}); c.a[3] = 1; break;
    case 5: c.a[4] = 0; c.a[3] = 1; c.a[0] = 0u; break;
    case 6: c.a[4] = 0; c.a[3] = 1; c.a[0] = 0xFFFFFFFFu; break;
    case 7: c.a[4] = 0; c.a[3] = 0; c.a[1] = 0u; c.a[2] = 0xFFFFFFFFu; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 16: CTaskTimer::GetTimeLeft (0x62EB80)
//   a[0] = inicio (+0), a[1] = intervalo (+4), a[2] = byte +8 (en marcha), a[3] = byte +9 (detenido), a[4] = CTimer::m_snTimeInMilliseconds
inline void Make16(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.W(); c.a[1] = g.W(); c.a[2] = g.P(60) ? 1u : (g.W() & 0xFFu); c.a[3] = g.W() & 0xFFu; c.a[4] = g.W();
    if (g.P(40)) { c.a[0] = c.a[4] - g.R(100000); c.a[1] = g.R(200000); }
    switch (f) {
    case 0: c.a[2] = 0; break;
    case 1: c.a[2] = 1; c.a[3] = 0; break;
    case 2: c.a[2] = 1; c.a[3] = 1; break;
    case 3: c.a[2] = 1; c.a[0] = c.a[4]; c.a[1] = 0; break;             // cero exacto
    case 4: c.a[2] = 1; c.a[0] = c.a[4] - 50u; c.a[1] = 100u; break;
    case 5: c.a[2] = g.Pk({2u, 0x80u, 0xFFu, 0x10u}); break;
    case 6: c.a[2] = 1; c.a[0] = 0xFFFFFFF0u; c.a[1] = 0x20u; c.a[4] = 5u; break;   // desbordamiento
    case 7: c.a[2] = 1; c.a[0] = 0u; c.a[1] = 0x80000000u; c.a[4] = 0x7FFFFFFFu; break;
    case 8: c.a[2] = 1; c.a[1] = 0xFFFFFFFFu; break;
    default: break;
    }
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
    case 5: case 6: Make56(c, n); break;
    case 7: case 9: MakePool(c, n, false); break;
    case 8: case 10: MakePool(c, n, true); break;
    case 11: case 12: case 13: case 14: MakeRec(c, n); break;
    case 15: Make15(c, n); break;
    default: Make16(c, n); break;
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
