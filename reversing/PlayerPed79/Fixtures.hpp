#pragma once
// Fixtures del banco PlayerPed79 (lote 79B). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::SetInitialState  0x60CD20  (bool bGroupCreated)   thiscall, ECX = jugador, 1 argumento en pila, `ret 4`, void   (hook CPlayerPed / SetInitialState, tipo S)
//   1 CPlayerPed::FindWeaponLockOnTarget  0x60DC50  ()   thiscall, ECX = jugador, sin argumentos, `ret`, bool en AL   (hook CPlayerPed / FindWeaponLockOnTarget, tipo S; ver Fixtures1.hpp)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n < 16384: familia = n % 16 (1024 casos por familia); despues las familias 16..19 (ver MakeExtra).
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

namespace Pp79Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamSweep = 1024;     // familia 16: barrido de mutaciones: sitio (18) x tipo (31)
constexpr unsigned kFamGroups = 1024;    // familia 17: indices de grupo (8 x 8) x intercambio de datos x respuesta
constexpr unsigned kFamFlags = 1024;     // familia 18: banderas del peaton / de la entidad en los bordes
constexpr unsigned kFamTask = 1024;      // familia 19: identificador de tarea del evento (16 bits con signo) x respuesta x fallos de reserva
constexpr unsigned kCases0 = kBaseCases + kFamSweep + kFamGroups + kFamFlags + kFamTask;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 2;
constexpr unsigned kSites = 18;          // sitios de mutacion (llamadas a colaboradores)

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPlayerWin = 0x800, kDataWin = 0x100, kIntelWin = 0x100, kStatWin = 0x40, kFireWin = 0x40, kTaskWin = 0x40, kEvWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPlayerArena = Arena(kPlayerWin), kDataArena = Arena(kDataWin), kIntelArena = Arena(kIntelWin), kStatArena = Arena(kStatWin), kFireArena = Arena(kFireWin),
                      kTaskArena = Arena(kTaskWin), kEvArena = Arena(kEvWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 79176u); }
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
    std::string End() const { return s + "}"; }
};

inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// ================================================================================================ SetInitialState
struct C0 {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 arg{};                                     // dword empujado (solo el byte bajo es bGroupCreated)
    u32 f1C{}, f40{}, f46C{}, f474{}, fFC{}, f784{}, f4D4{};   // campos del jugador que la funcion toca
    u32 fire{};                                    // [jugador+0x730] != 0
    u32 clump{};                                   // [jugador+0x18] (valor opaco que se pasa a RemoveAllAssociations)
    u32 hasData{};                                 // [jugador+0x480] != 0 (solo con arg != 0)
    u32 dflags{}, odflags{}, group{}, ogroup{};    // datos del jugador (principal y "otros"): dword +0x34 e indice de grupo +0x38
    u32 timeScale{}, drunk{};                      // 0xB7CB64 y 0xC73C58 iniciales
    u32 taskFail1{}, taskFail2{};                  // CTask::operator new devuelve nulo en la 1a / 2a llamada
    u32 will{}, taskId{}, randRet{};               // retorno de WillRespond (AL + basura), word que deja ComputeResponseTaskType en +0xE, retorno de rand
    u32 grpFlags{};                                // byte +4 de cada grupo (8 bytes empaquetados por bit: bit i = grupo i)
    u32 mutSite{}, mutKind{}, mutF46C{}, mutF474{}, mutF1C{}, mutFC{}, mutDFlags{};
};

inline u32 Flags32(u32 n, u32 k) {   // dword de banderas con patrones notables
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x2000u, 0x4000000u, 0x100000u, 0x42u, 0xFFFFFFBDu, 0xFBFFFFFFu, 0xFFEFFFFFu, 0x80000000u, 1u, 2u};
    return Rand(n, k) % 3 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
inline u32 PtrLike(u32 n, u32 k) { return Rand(n, k) % 3 == 0 ? 0x1000u + (Rand(n, k + 1) & 0xFFFF0u) : Rand(n, k + 2); }

inline C0 BaseDefaults0(unsigned n) {
    C0 c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.arg = Rand(n, 3) % 3 == 0 ? Rand(n, 4) % 2 : (Rand(n, 5) % 2 ? Rand(n, 6) & ~0xFFu : Rand(n, 8) | 1u);   // 0/1, o con basura en los bits altos
    c.f1C = Flags32(n, 10); c.f40 = Flags32(n, 14); c.f46C = Flags32(n, 18); c.f474 = Flags32(n, 22);
    c.fFC = Rand(n, 26) % 2 ? 0u : PtrLike(n, 27); c.f784 = Rand(n, 30); c.f4D4 = Rand(n, 31);
    c.fire = Rand(n, 32) % 2; c.clump = Rand(n, 33);
    c.hasData = 1;
    c.dflags = Flags32(n, 34); c.odflags = Flags32(n, 38);
    if (Rand(n, 42) % 3) c.dflags &= ~0x400u;
    if (Rand(n, 43) % 3) c.odflags &= ~0x400u;
    c.group = Rand(n, 44) % 8; c.ogroup = Rand(n, 45) % 8;
    c.timeScale = Rand(n, 46) % 4 ? Rand(n, 47) : 0x3F800000u; c.drunk = Rand(n, 48);
    c.taskFail1 = Rand(n, 49) % 8 == 0; c.taskFail2 = Rand(n, 50) % 8 == 0;
    c.will = Rand(n, 51) % 2 ? 1u : 0u; if (Rand(n, 52) % 3 == 0) c.will |= Rand(n, 53) << 8;
    c.taskId = Rand(n, 54) % 4 ? Rand(n, 55) & 0xFFFFu : Rand(n, 56);
    c.randRet = Rand(n, 57);
    c.grpFlags = Rand(n, 58);
    return c;
}

inline C0 MakeExtra0(unsigned n) {
    C0 c = BaseDefaults0(n);
    const unsigned j = n - kBaseCases;
    c.arg &= ~0xFFu; if (Rand(n, 60) % 8) c.arg = 0;   // casi siempre sin grupo creado: es la rama larga
    c.dflags &= ~0x400u;
    if (j < kFamSweep) {   // familia 16: barrido de sitios x tipos de mutacion
        c.family = 16;
        c.mutSite = 1 + j % kSites; c.mutKind = 1 + (j / kSites) % 31;
        c.will = 1 | (Rand(n, 61) % 2 ? Rand(n, 62) << 8 : 0u);
        c.fFC = 1 + Rand(n, 63) % 3 ? PtrLike(n, 64) | 1u : 0u;
        c.mutF46C = Flags32(n, 65); c.mutF474 = Flags32(n, 66); c.mutF1C = Flags32(n, 67); c.mutFC = Rand(n, 68) % 2 ? 0u : PtrLike(n, 69); c.mutDFlags = Flags32(n, 70);
        c.taskFail1 = c.taskFail2 = 0; c.hasData = 1; c.fire = 1;
        if (Rand(n, 71) % 4 == 0) c.mutDFlags |= 0x400u; else c.mutDFlags &= ~0x400u;
        return c;
    }
    if (j < kFamSweep + kFamGroups) {   // familia 17: grupos
        c.family = 17;
        const unsigned m = j - kFamSweep;
        c.group = m % 8; c.ogroup = (m / 8) % 8;
        c.will = (m / 64) % 2 ? 1u : 0u;
        if ((m / 128) % 2) { c.mutSite = 1 + Rand(n, 72) % kSites; c.mutKind = 1; }   // intercambio de punteros de datos
        else if ((m / 256) % 2) { c.mutSite = 1 + Rand(n, 73) % kSites; c.mutKind = 16; c.mutDFlags = Rand(n, 74) % 2 ? 0x400u : 0u; }
        c.taskFail1 = c.taskFail2 = 0;
        return c;
    }
    if (j < kFamSweep + kFamGroups + kFamFlags) {   // familia 18: banderas en los bordes
        c.family = 18;
        c.f1C = Flags32(n, 75); c.f40 = Flags32(n, 76); c.f46C = Flags32(n, 77); c.f474 = Flags32(n, 78);
        const unsigned m = j - kFamSweep - kFamGroups;
        c.fFC = m % 3 == 0 ? 0u : PtrLike(n, 79) | 1u;
        if (m % 5 == 0) c.f1C |= 1u; else if (m % 5 == 1) c.f1C &= ~1u;
        if (m % 7 < 3) { c.mutSite = 1 + Rand(n, 80) % kSites; c.mutKind = 4 | (Rand(n, 81) % 2 ? 8u : 0u); c.mutF46C = Flags32(n, 82); c.mutF474 = Flags32(n, 83); c.mutF1C = Flags32(n, 84); c.mutFC = Rand(n, 85) % 2 ? 0u : 7u; }
        return c;
    }
    c.family = 19;   // familia 19: identificador de tarea del evento
    const unsigned m = j - kFamSweep - kFamGroups - kFamFlags;
    static constexpr u32 ids[]{0u, 1u, 0xC8u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0xFFFF8000u, 0xFFFFFFFFu, 0x12345678u, 0x80u, 0xFFu};
    c.taskId = m % 3 ? ids[Rand(n, 86) % std::size(ids)] : Rand(n, 87);
    c.will = (m / 3) % 2 ? 1u : 0u; if ((m / 6) % 3 == 0) c.will |= Rand(n, 88) << 8;
    c.taskFail1 = (m / 12) % 4 == 0; c.taskFail2 = (m / 24) % 4 == 0;
    return c;
}

inline C0 MakeC0(unsigned n) {
    if (n >= kBaseCases) return MakeExtra0(n);
    C0 c = BaseDefaults0(n);
    const u32 k = c.family;
    auto longPath = [&] { c.arg &= ~0xFFu; c.dflags &= ~0x400u; c.hasData = 1; };
    switch (k) {
    case 0: break;
    case 1: c.arg = (c.arg & ~0xFFu) | (1 + Rand(n, 90) % 255); break;                                   // grupo creado: salta la rama larga
    case 2: longPath(); c.dflags |= 0x400u; break;                                                       // bit 0x400 de las banderas: salta
    case 3: longPath(); c.will = 0; break;
    case 4: longPath(); c.will = 1 | (Rand(n, 91) % 2 ? Rand(n, 92) << 8 : 0u); break;
    case 5: c.fFC = PtrLike(n, 93) | 1u; c.f1C &= ~1u; break;
    case 6: c.fFC = PtrLike(n, 94) | 1u; c.f1C |= 1u; break;
    case 7: c.fire = 1; longPath(); break;
    case 8: longPath(); c.taskFail1 = 1; c.taskFail2 = 0; c.will = 1; break;
    case 9: longPath(); c.taskFail1 = 0; c.taskFail2 = 1; c.will = 1; break;
    case 10: longPath(); c.taskFail1 = c.taskFail2 = 1; break;
    case 11: c.arg &= ~0xFFu; if (Rand(n, 95) % 2) c.arg = 1 + (Rand(n, 96) << 8) ; c.hasData = 0; c.arg = (c.arg & ~0xFFu) | 1u; break;   // sin datos del jugador: solo con grupo creado
    case 12: longPath(); c.arg |= Rand(n, 97) << 8; c.will |= Rand(n, 98) << 8; c.will |= 1u; break;      // basura en los bits altos
    case 13: longPath(); c.mutSite = 1 + Rand(n, 99) % kSites; c.mutKind = 1 + Rand(n, 100) % 31; c.mutF46C = Flags32(n, 101); c.mutF474 = Flags32(n, 102); c.mutF1C = Flags32(n, 103); c.mutFC = Rand(n, 104) % 2 ? 0u : 9u;
             c.mutDFlags = Flags32(n, 105); c.will = Rand(n, 106) % 2; break;
    case 14: c.arg = (c.arg & ~0xFFu) | 1u; c.mutSite = 1 + Rand(n, 107) % kSites; c.mutKind = 1 + Rand(n, 108) % 31; c.mutF46C = Flags32(n, 109); c.mutF474 = Flags32(n, 110); c.mutF1C = Flags32(n, 111);
             c.mutFC = Rand(n, 112) % 2 ? 0u : 9u; c.mutDFlags = Flags32(n, 113); break;
    default: if (Rand(n, 114) % 2) longPath(); if (Rand(n, 115) % 2) { c.mutSite = 1 + Rand(n, 116) % kSites; c.mutKind = 1 + Rand(n, 117) % 31; c.mutF46C = Flags32(n, 118); c.mutF474 = Flags32(n, 119);
             c.mutF1C = Flags32(n, 120); c.mutFC = Rand(n, 121) % 2 ? 0u : 9u; c.mutDFlags = Flags32(n, 122); } break;
    }
    // Sin datos del jugador solo si el grupo esta creado (si no, el EXE lee [datos+0x34] de la direccion 0x34 y se cae); la mutacion de punteros no los anula
    if (!c.hasData) c.arg = (c.arg & ~0xFFu) | 1u;
    return c;
}

inline std::string JsonC0(const C0& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("arg", c.arg);
    j.U("f1C", c.f1C).U("f40", c.f40).U("f46C", c.f46C).U("f474", c.f474).U("fFC", c.fFC).U("f784", c.f784).U("f4D4", c.f4D4).U("fire", c.fire).U("clump", c.clump).U("has_data", c.hasData);
    j.U("dflags", c.dflags).U("odflags", c.odflags).U("group", c.group).U("ogroup", c.ogroup).U("time_scale", c.timeScale).U("drunk", c.drunk);
    j.U("task_fail1", c.taskFail1).U("task_fail2", c.taskFail2).U("will", c.will).U("task_id", c.taskId).U("rand_ret", c.randRet).U("grp_flags", c.grpFlags);
    j.U("mut_site", c.mutSite).U("mut_kind", c.mutKind).U("mut_f46C", c.mutF46C).U("mut_f474", c.mutF474).U("mut_f1C", c.mutF1C).U("mut_fFC", c.mutFC).U("mut_dflags", c.mutDFlags);
    return j.End();
}
}
