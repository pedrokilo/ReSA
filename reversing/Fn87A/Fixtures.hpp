#pragma once
// Fixtures del banco Fn87A (lote 87A): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::ProcessWeaponSwitch                          0x60D850  thiscall (CPad* pad), ret 4; void   (hook CPlayerPed / ProcessWeaponSwitch, tipo S)
//
// Familia = n % 16 (1024 casos por familia). Todo lo que la funcion lee o llama es un dato del caso: los campos del jugador (arena), las 20 primeras ranuras de armas (tipo y municion), los datos del
// jugador (arena), el modo de la camara (word 0xB6F858, memoria real) y los resultados de los 11 colaboradores falsos.
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

namespace F87AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;
constexpr unsigned kSlots = 20;         // ranuras 0..19 con tipo y municion definidos por el caso (la arma 13 comparte su municion con la ranura activa, +0x718)

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPedWin = 0x800, kPdWin = 0xC0, kIntelWin = 0x20, kPadWin = 0x20, kTaskWin = 0x40, kInfoWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPedArena = Arena(kPedWin), kPdArena = Arena(kPdWin), kIntelArena = Arena(kIntelWin), kPadArena = Arena(kPadWin), kTaskArena = Arena(kTaskWin), kInfoArena = Arena(kInfoWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 87001u); }
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

template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, edx{}, ecx{}, stack{};
    u32 attached{};                     // CPhysical::m_pAttachedTo (+0xFC), cualquier valor (nulo o no)
    u32 targeted{};                     // m_pTargetedObject (+0x71C)
    u32 slot{};                         // m_nActiveWeaponSlot (byte +0x718)
    u32 slothi{};                       // 24 bits altos del dword +0x718 (la municion de la ranura 13)
    u32 chosen0{};                      // m_nChosenWeapon inicial (byte +0x20 de los datos del jugador)
    u32 pflags{};                       // dword +0x34 de los datos del jugador (bit 3: apuntado libre; bit 11: bloqueo de cambio)
    u32 dontallow{};                    // byte +0x85 de los datos del jugador
    u32 cam{};                          // word 0xB6F858 (modo de la camara activa)
    u32 wtype[kSlots]{};                // tipos de las ranuras 0..19
    u32 wammo[kSlots]{};                // municion total (+0xC) de las ranuras 0..19 (la 13 no se escribe: es el dword de la ranura activa)
    u32 frenzy{}, jet{}, right{}, left{}, coop{};
    u32 hasmask{}, can2mask{};          // bit k: HasWeaponAmmoToBeUsed / CanBeUsedFor2Player de la arma en la ranura k
    u32 infomask{}, infoval{};          // bit (tipo & 31): el primer dword de la informacion del arma es infoval (si no, 0)
    u32 padw{};                         // resultado (eax) de CPad::GetWeapon
    u32 ug{}, ugcmd{}, uganim{};        // CTaskSimpleUseGun: existe, ultimo comando (byte +0x10), animacion (+0x2C)
    u32 gate{};
};

inline constexpr std::array<u32, 16> kTypes{0u, 0u, 1u, 5u, 0x26u, 0x27u, 0x28u, 9u, 22u, 24u, 25u, 30u, 31u, 34u, 41u, 46u};
inline constexpr std::array<u32, 12> kAmmo{0u, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 2u, 100u, 25000u, 0xFFFFu, 0xFFFFFFF0u, 7u};
inline constexpr std::array<u32, 5> kInfoVals{1u, 2u, 3u, 0x80000000u, 0xFFFFFFFFu};

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
    // base aleatoria
    c.attached = P(6) ? (Rand(n, nk()) | 1u) : 0u;
    c.targeted = P(10) ? (Rand(n, nk()) | 1u) : 0u;
    c.slot = P(12) ? R(kSlots) : R(13);
    c.slothi = Rand(n, nk()) & 0xFFFFFFu;
    c.chosen0 = P(6) ? 0xFFu : (P(15) ? R(kSlots) : R(13));
    c.pflags = (Rand(n, nk()) & ~0x808u) | (P(10) ? 0x8u : 0u) | (P(10) ? 0x800u : 0u);
    if (P(40)) c.pflags &= ~0x808u;
    c.dontallow = P(8) ? (P(50) ? 1u : (Rand(n, nk()) & 0xFFu)) : 0u;
    c.cam = P(12) ? Pk({0x22u, 0x2Au, 0x07u, 0x27u, 0x08u, 0x28u, 0x33u, 0x34u, 0x2Eu}) : (R(0x60) | (P(5) ? 0x100u : 0u));
    for (unsigned k = 0; k < kSlots; ++k) {
        c.wtype[k] = P(75) ? Pick(kTypes, n, nk()) : (P(50) ? R(48) : Rand(n, nk()));
        c.wammo[k] = P(55) ? (1u + R(40)) : Pick(kAmmo, n, nk());
    }
    c.frenzy = P(5) ? 1u : 0u;
    c.jet = P(5) ? 1u : 0u;
    c.right = P(30) ? 1u : 0u;
    c.left = P(30) ? 1u : 0u;
    c.coop = P(25) ? 1u : 0u;
    c.hasmask = Rand(n, nk()) | Rand(n, nk());
    c.can2mask = Rand(n, nk()) | Rand(n, nk());
    c.infomask = Rand(n, nk()) | Rand(n, nk()) | Rand(n, nk());
    c.infoval = Pick(kInfoVals, n, nk());
    c.padw = P(25) ? (P(50) ? 1u : (Rand(n, nk()) | 1u)) : 0u;
    c.ug = P(40) ? 1u : 0u;
    c.ugcmd = Pk({0xFFu, 0u, 1u, 2u, 3u, 4u, 4u, 5u, 6u, 7u, 0x82u, 0x04u});
    c.uganim = P(50) ? (Rand(n, nk()) | 1u) : 0u;
    // a cada familia le toca una parte de la funcion
    auto open = [&] { c.attached = 0; c.targeted = 0; c.pflags &= ~0x808u; c.dontallow = 0; c.frenzy = 0; c.jet = 0; c.cam = R(0x20) + 0x10; };
    auto nocycle = [&] { c.right = 0; c.left = 0; };
    auto fireOn = [&](const u32 slot) { c.infomask |= (1u << (c.wtype[slot] & 31)); };
    auto fireOff = [&](const u32 slot) { c.infomask &= ~(1u << (c.wtype[slot] & 31)); };
    switch (f) {
    case 0: break;
    case 1: c.frenzy = 1; break;
    case 2:
        c.frenzy = 0;
        if (P(50)) { c.attached = Rand(n, nk()) | 1u; } else { c.attached = 0; c.jet = 1; }
        break;
    case 3: open(); c.right = 1; c.slot = P(20) ? R(kSlots) : R(13); break;                                   // ciclo derecho
    case 4: open(); c.right = 0; c.left = 1; c.slot = P(20) ? R(kSlots) : R(13); break;                       // ciclo izquierdo
    case 5: open(); c.right = 1; c.coop = 1; c.slot = R(12); break;                                           // derecho con cooperativo
    case 6: open(); c.right = 0; c.left = 1; c.coop = 1; c.slot = 1 + R(12); break;                           // izquierdo con cooperativo
    case 7:                                                                                                    // ciclo cerrado por alguna condicion o por el modo de la camara
        open();
        c.right = P(50) ? 1u : 0u; c.left = c.right ? 0u : 1u;
        switch (R(8)) {
        case 0: c.targeted = Rand(n, nk()) | 1u; break;
        case 1: c.pflags |= 0x8u; break;
        case 2: c.dontallow = 1u + R(255); break;
        case 3: c.pflags |= 0x800u; break;
        case 4: case 5: c.cam = Pk({0x22u, 0x2Au, 0x07u, 0x27u, 0x08u, 0x28u, 0x33u, 0x34u, 0x2Eu}); break;
        case 6: c.cam = 0x2Au + R(3); break;
        default: c.cam = Pk({0x2Eu, 0x34u, 0x2Au}); break;
        }
        break;
    case 8:                                                                                                    // sin ciclo, arma activa sin municion
        open(); nocycle(); fireOn(c.slot); if (P(50)) c.padw = 0;
        c.wammo[c.slot] = P(70) ? 0u : 0xFFFFFFFFu;
        if (P(30)) c.cam = Pk({0x22u, 0x07u, 0x08u, 0x33u});
        if (P(25)) { c.wtype[c.slot] = 0x28u; c.wtype[8] = P(60) ? 0x27u : Pick(kTypes, n, nk()); fireOn(c.slot); }
        break;
    case 9:                                                                                                    // retroceso por municion con distintas ranuras
        open(); nocycle(); c.padw = 0;
        c.slot = P(40) ? Pk({5u, 6u, 17u, 18u, 19u, 16u, 0u, 1u}) : R(kSlots);
        fireOn(c.slot);
        c.wammo[c.slot] = 0;
        for (unsigned k = 0; k < kSlots; ++k) if (P(50)) c.wammo[k] = P(50) ? 0u : (1u + R(5));
        c.wammo[c.slot] = 0;
        if (P(55)) c.wtype[5] = 5u;
        if (P(30)) for (unsigned k = 0; k < kSlots; ++k) c.wammo[k] = 0u;
        break;
    case 10: open(); nocycle(); fireOff(c.slot); break;                                                       // informacion de arma de cuerpo a cuerpo
    case 11:                                                                                                   // GetWeapon distinto de cero con la minigun
        open(); nocycle(); c.wtype[c.slot] = 0x26u; fireOn(c.slot); c.padw = P(80) ? 1u : 0u; c.wammo[c.slot] = P(50) ? 0u : 5u;
        break;
    case 12:                                                                                                   // etapa final
        open(); nocycle(); fireOff(c.slot); c.chosen0 = P(15) ? c.slot : (P(10) ? 0xFFu : R(13)); c.ug = P(70) ? 1u : 0u;
        break;
    case 13:
        c.slot = Pk({0u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 11u, 1u}); c.attached = 0; c.targeted = 0; c.frenzy = 0; c.jet = 0; c.pflags &= ~0x808u; c.dontallow = 0; c.cam = R(0x20) + 0x10;
        break;
    case 14: c.ecx = Pk({0u, 0xFFFFFFFFu, c.ecx}); c.edx = Pk({0u, 0xFFFFFFFFu, c.edx}); c.eax = Pk({0u, 0xFFFFFFFFu, c.eax}); break;
    default: break;
    }
    if (c.chosen0 != 0xFFu && c.chosen0 >= kSlots) c.chosen0 = c.chosen0 % kSlots;
    c.slot %= kSlots;
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("attached", c.attached).U("targeted", c.targeted).U("slot", c.slot).U("slothi", c.slothi).U("chosen0", c.chosen0).U("pflags", c.pflags).U("dontallow", c.dontallow).U("cam", c.cam);
    j.A("wtype", std::vector<u32>(c.wtype, c.wtype + kSlots)).A("wammo", std::vector<u32>(c.wammo, c.wammo + kSlots));
    j.U("frenzy", c.frenzy).U("jet", c.jet).U("right", c.right).U("left", c.left).U("coop", c.coop).U("hasmask", c.hasmask).U("can2mask", c.can2mask);
    j.U("infomask", c.infomask).U("infoval", c.infoval).U("padw", c.padw).U("ug", c.ug).U("ugcmd", c.ugcmd).U("uganim", c.uganim).U("gate", c.gate);
    return j.End();
}
}
