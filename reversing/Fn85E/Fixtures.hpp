#pragma once
// Fixtures del banco Fn85E (lote 85E): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::GetAvailableComboSet                 0x61C7F0  thiscall (tarea en ECX, ped, comando), `ret 8`; devuelve AL            (hook CTaskSimpleFight / GetAvailableComboSet, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo (aqui no hay floats). Familia = n % 16 (1024 casos por familia).
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

namespace F85EFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kGroups = 48, kGroupsWin = kGroups * 0x14, kInfoWin = 0x70, kNameWin = 0x100, kTaskWin = 0x28, kPedWin = 0x740;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kInfoArena = Arena(kInfoWin), kNameArena = Arena(kNameWin), kGroupsArena = Arena(kGroupsWin);

constexpr u32 kGroupsAddr = 0xB4EA34;       // CAnimManager::ms_aAnimAssocGroups (puntero al arreglo de grupos, 0x14 bytes por grupo; +0 = CAnimBlock*)
constexpr u32 kBlocksAddr = 0xB5D4A0;       // CAnimManager::ms_aAnimBlocks (0x20 bytes por bloque; +0x10 = IsLoaded)
constexpr std::size_t kBlockRegion = 16 * 0x20;
constexpr u32 kComboAddr = 0xC170D0;        // CTaskSimpleFight::m_aComboData (0x88 bytes por fila; +0 = grupo de animacion, +0x85 = byte alto de las banderas)
constexpr std::size_t kComboRegion = 12 * 0x88;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85003u); }
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
    u32 cmd{};                      // dword de pila: el byte bajo (con signo) es el comando; el resto es basura
    u32 slot{};                     // byte m_nActiveWeaponSlot (con signo)
    u32 wtype{};                    // tipo del arma de la ranura activa (dword en 0x5A0 + slot * 0x1C del ped)
    u32 binfo{};                    // byte +0x6E de la CWeaponInfo (combo base del arma)
    u32 fstyle{};                   // byte +0x72D del ped (estilo de pelea)
    u32 rowGroup[12]{};             // grupo de animacion (+0) de las filas 0..11 de la tabla de combos (fila = estilo - 4)
    u32 rowFlags[12]{};             // byte +0x85 de cada fila
    u32 ref{}, cur{}, nextCmd{};    // tarea: byte +0xA (referenciado), dword +0xC (grupo requerido), byte +0x26 (comando siguiente)
    u32 gblk[kGroups]{};            // bloque de cada grupo: 0 = nulo; k + 1 = bloque k de ms_aAnimBlocks
    u32 lookup{};                   // bloque (0..15) que devuelve GetAnimationBlock
    u32 loaded[16]{};               // byte IsLoaded de los bloques 0..15
    u32 mutSite{}, mutVal{};        // mutacion de una lectura: 1 tras RemoveAnimBlockRef (grupo de todas las filas), 2 tras RequestModel (comando siguiente), 3 en GetWeaponInfo (estilo de pelea)
    u32 gate{};
};

inline u32 StyleOf(const C& c) {   // estilo efectivo sin mutacion (solo con comandos validos)
    const u32 cmdb = c.cmd & 0xFFu;
    u32 st = c.binfo & 0xFFu;
    if (cmdb == 0xC) st = c.fstyle & 0xFFu;
    else if (st == 4 && (cmdb == 2 || cmdb == 0)) st = c.fstyle & 0xFFu;
    return st;
}

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
    // --- datos aleatorios base
    const u32 hi = P(50) ? 0u : R(0x1000000u);
    u32 low = 0;
    c.slot = u32(i32(R(16)) - 3) & 0xFFu;                                     // -3..12
    c.wtype = P(70) ? R(46) : Rand(n, nk());
    c.binfo = P(40) ? 4u : 5u + R(11);                                         // 4..15
    c.fstyle = P(25) ? 4u : 4u + R(12);
    for (u32 i = 0; i < 12; ++i) {
        c.rowGroup[i] = P(15) ? 0x21u : R(40);
        c.rowFlags[i] = R(256);
        if (P(60)) c.rowFlags[i] |= 4u; else c.rowFlags[i] &= ~4u;
    }
    c.ref = P(50) ? 0u : (P(70) ? 1u : Pk({2u, 0x80u, 0xFFu}));
    c.cur = P(15) ? 0x21u : R(40);
    c.nextCmd = P(50) ? 0xBu + R(4) : R(256);
    for (u32 g = 0; g < kGroups; ++g) c.gblk[g] = P(35) ? 0u : 1u + R(16);
    c.lookup = R(16);
    for (u32 k = 0; k < 16; ++k) c.loaded[k] = P(45) ? 0u : Pk({1u, 1u, 1u, 2u, 0x80u, 0xFFu});
    auto pickValid = [&] { return Pk({0u, 2u, 0xBu, 0xCu, 0xDu, 0xEu, 0u, 2u}); };
    auto pickNeg = [&] { return 0x80u + R(0x80); };
    auto pickInvalid = [&] { u32 v = Pk({1u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 0xAu, 0xFu, 0x10u, 0x20u, 0x7Fu}); return v; };
    auto rowOf = [&] { c.cmd = low; return StyleOf(c) - 4u; };
    auto groupNormal = [&](u32 g) { return g == 0x21u ? 1u + R(39) : g; };
    // --- por familia
    switch (f) {
    case 0: low = pickNeg(); c.cur = 0x21u; break;                                                                                                   // comando negativo, sin animaciones
    case 1: low = pickNeg(); c.cur = groupNormal(c.cur); c.ref = c.ref ? c.ref : 1u; break;                                                        // negativo con referencia
    case 2: low = pickNeg(); c.cur = groupNormal(c.cur); c.ref = 0u; c.gblk[c.cur] = 1u + R(16); c.loaded[c.gblk[c.cur] - 1] = Pk({1u, 2u, 0x80u, 0xFFu}); break;   // negativo: bloque en la tabla y cargado
    case 3: low = pickNeg(); c.cur = groupNormal(c.cur); c.ref = 0u; c.gblk[c.cur] = 0u; c.loaded[c.lookup] = Pk({1u, 2u, 0x80u, 0xFFu}); break;       // negativo: bloque por nombre y cargado
    case 4: low = pickNeg(); c.cur = groupNormal(c.cur); c.ref = 0u; if (c.gblk[c.cur]) c.loaded[c.gblk[c.cur] - 1] = 0u; else c.loaded[c.lookup] = 0u; break;   // negativo: no cargado
    case 5: low = pickInvalid(); break;                                                                                                              // comando que no hace nada
    case 6: low = 0; c.rowFlags[rowOf()] &= ~4u; break;                                                                                               // comando 0 sin bandera: devuelve 4
    case 7: low = P(50) ? 0u : pickValid(); c.rowGroup[rowOf()] = 0x21u; if (low == 0) c.rowFlags[rowOf()] |= 4u; break;                             // grupo 0x21: devuelve el estilo
    case 8: low = 2; c.binfo = 4; break;                                                                                                             // comando 2 con estilo base 4: usa el estilo de pelea
    case 9: low = 0xC; break;                                                                                                                         // comando 0xC
    case 10: low = Pk({0xBu, 0xDu, 0xEu}); if (P(40)) c.binfo = 4; break;                                                                            // 0xB, 0xD, 0xE
    case 11: low = pickValid(); { const u32 r = rowOf(); c.rowGroup[r] = groupNormal(c.rowGroup[r]); if (low == 0) c.rowFlags[r] |= 4u; c.cur = c.rowGroup[r]; c.ref = c.ref ? c.ref : 1u; } break;   // mismo grupo y referenciado
    case 12: low = pickValid(); { const u32 r = rowOf(); c.rowGroup[r] = groupNormal(c.rowGroup[r]); if (low == 0) c.rowFlags[r] |= 4u; c.cur = c.rowGroup[r]; c.ref = 0u; } break;   // mismo grupo sin referencia
    case 13: low = pickValid(); { const u32 r = rowOf(); c.rowGroup[r] = groupNormal(c.rowGroup[r]); if (low == 0) c.rowFlags[r] |= 4u; c.cur = (c.rowGroup[r] + 1 + R(30)) % 40u; c.ref = c.ref ? c.ref : 1u;
                c.loaded[c.gblk[c.rowGroup[r]] ? c.gblk[c.rowGroup[r]] - 1 : c.lookup] = Pk({1u, 2u, 0x80u, 0xFFu}); } break;                                                  // grupo distinto con referencia
    case 14: low = pickValid(); { const u32 r = rowOf(); c.rowGroup[r] = groupNormal(c.rowGroup[r]); if (low == 0) c.rowFlags[r] |= 4u; c.cur = (c.rowGroup[r] + 1 + R(30)) % 40u; c.ref = P(30) ? c.ref : 0u;
                c.loaded[c.gblk[c.rowGroup[r]] ? c.gblk[c.rowGroup[r]] - 1 : c.lookup] = 0u; if (P(70)) c.nextCmd = 0xBu + R(4); } break;                                              // grupo distinto, bloque sin cargar
    default: low = P(15) ? pickNeg() : (P(10) ? pickInvalid() : pickValid()); break;
    }
    if (f == 15 || P(12)) {   // mutacion de una lectura
        c.mutSite = 1u + R(3);
        c.mutVal = c.mutSite == 1 ? (P(15) ? 0x21u : R(40)) : (c.mutSite == 2 ? (P(60) ? 0xBu + R(4) : R(256)) : 4u + R(12));
    }
    if ((f >= 6 && f <= 14) && c.mutSite == 3) c.mutSite = 0;   // los estilos forzados de estas familias no son los mutados
    c.cmd = low | (hi << 8);
    // el peso del arma de la ranura activa va a su posicion
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("cmd", c.cmd).U("slot", c.slot).U("wtype", c.wtype).U("binfo", c.binfo).U("fstyle", c.fstyle)
     .A("rowgroup", std::vector<u32>(c.rowGroup, c.rowGroup + 12)).A("rowflags", std::vector<u32>(c.rowFlags, c.rowFlags + 12))
     .U("ref", c.ref).U("cur", c.cur).U("nextcmd", c.nextCmd).A("gblk", std::vector<u32>(c.gblk, c.gblk + kGroups)).U("lookup", c.lookup).A("loaded", std::vector<u32>(c.loaded, c.loaded + 16))
     .U("mutsite", c.mutSite).U("mutval", c.mutVal);
    j.U("gate", c.gate);
    return j.End();
}
}
