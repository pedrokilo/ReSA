#pragma once
// Fixtures del banco PlayerGroup78 (lote 78B): CPlayerPed::ProcessGroupBehaviour (EXE 0x60D350, thiscall, ECX = jugador, 1 argumento en pila (CPad*), `ret 4`, void).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::ProcessGroupBehaviour  0x60D350  (CPad* pad)   (hook CPlayerPed / ProcessGroupBehaviour, tipo S)
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

namespace Pg78Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamExact = 1024;     // familia 16: pulsacion que llega exactamente a 0x4B0 (arriba) o a >= 0x4B0 (abajo), +-2 y envolturas
constexpr unsigned kFamStep = 1024;      // familia 17: paso de tiempo especial (NaN, infinitos, desnormales, enormes, negativos) x contador previo
constexpr unsigned kFamMut = 1024;       // familia 18: mutaciones entre llamadas: sitio (9) x tipo (15) x modo de pulsacion
constexpr unsigned kFamPool = 1024;      // familia 19: pool de peatones: tamano x estados de ranura x nivel de busqueda x mutacion del nivel
constexpr unsigned kCases = kBaseCases + kFamExact + kFamStep + kFamMut + kFamPool;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;
constexpr unsigned kPoolPeds = 6;        // peatones del pool falso

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPlayerWin = 0x800, kDataWin = 0x100, kPedWin = 0x800, kPadWin = 0x100, kWantedWin = 0x100, kPoolWin = 0x20, kSlotsWin = 0x10, kPedsWin = kPoolPeds * 0x7C4;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPlayerArena = Arena(kPlayerWin), kDataArena = Arena(kDataWin), kPedArena = Arena(kPedWin), kPadArena = Arena(kPadWin), kWantedArena = Arena(kWantedWin),
                      kPoolArena = Arena(kPoolWin), kSlotsArena = Arena(kSlotsWin), kPedsArena = Arena(kPedsWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 78176u); }
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

inline constexpr std::array<u32, 2> kNan{0x7FC00000u, 0xFFC00000u};
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// NaN con signo, carga util y tipo (QNaN/SNaN) arbitrarios
inline u32 NanBits(u32 c, u32 k) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1);
    u32 payload;
    switch (s % 4) {
    case 0: payload = 0; break;
    case 1: payload = 1 + (r % 0xFF); break;
    case 2: payload = 0x3FFFFFu; break;
    default: payload = (r >> 3) & 0x3FFFFFu; break;
    }
    const bool quiet = ((r >> 30) & 1) != 0;
    if (!quiet && payload == 0) payload = 1;
    return ((r >> 31) << 31) | 0x7F800000u | (quiet ? 0x400000u : 0u) | payload;
}

// Pasos de tiempo: normales y los que rompen la conversion a entero
inline u32 Step(u32 c, u32 k) {
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x3F800000u, 0x41A00000u, 0x42480000u, 0x41F00000u, 0x40000000u, 0x3F000000u, 0x3DCCCCCDu, 0x41200000u, 0x42C80000u, 0x447A0000u,   // 0,-0,1,20,50,30,2,.5,.1,10,100,1000
                              0xBF800000u, 0xC1A00000u, 0xC2480000u, 0xC47A0000u,                                                                                       // negativos
                              0x7F800000u, 0xFF800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x00800000u,                                    // inf, max, desnormales
                              0x5F000000u, 0x5F7FFFFFu, 0x5E800000u, 0x5D800000u, 0x5C800000u, 0x5B800000u, 0xDF000000u, 0xDE800000u,                                   // alrededor de 2^63, 2^31, 2^32
                              0x4F000000u, 0x4F800000u, 0x4EFFFFFFu, 0xCF000000u, 0xCF800000u, 0x4B800000u, 0x4B7FFFFFu, 0x4A800000u};
    const u32 r = Rand(c, k) % 10;
    if (r < 3) return sp[Rand(c, k + 1) % std::size(sp)];
    if (r == 3) return kNan[Rand(c, k + 2) % 2];
    if (r == 4) return NanBits(c, k + 3);
    if (r == 5) return B(float(Rand(c, k + 4) % 2000) / 10.0f);
    if (r == 6) return B(float(i32(Rand(c, k + 5) % 4000) - 2000) * 1000.0f);
    return B(float(Rand(c, k + 6) % 400) / 20.0f);   // el caso normal: 0..20 ms/fotograma
}
// Lo que suma una pulsacion con el paso `ts` (low16 de la conversion a entero de 64 bits truncada de (ts * 0.02f) * 1000.0f). Solo para ELEGIR fixtures; el oraculo lo recalcula aparte.
inline u32 AddOf(u32 ts) {
    volatile float a = F(ts) * 0.02f;
    volatile float b = a * 1000.0f;
    const float v = b;
    if (!(v > -9.0e18f && v < 9.0e18f)) return 0;
    return u32(std::int64_t(v)) & 0xFFFFu;
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 mouse{}, tgt{}, mtgt{};                    // byte 0xB6EC2E; objetivo (+0x71C) presente; objetivo del raton (+0x79C) presente
    u32 tType{}, tPed{}, mType{}, mPed{};          // byte +0x36 y dword +0x598 del objetivo y del objetivo del raton
    u32 veh1{}, veh2{};                            // retorno de FindPlayerVehicle en la 1a y la 2a llamada
    u32 up{}, upHi{}, down{}, downHi{};            // retorno de GetGroupControlForward / Back (AL y basura en el resto de EAX)
    u32 flags{}, oflags{}, upT{}, downT{}, oupT{}, odownT{};   // datos del jugador (principal y "otro"): dword +0x34, word +0x8A (arriba) y +0x88 (abajo)
    u32 ts{}, frame{};                             // paso de tiempo sin recortar (0xB7CB58) y contador de fotogramas (0xB7CB4C)
    u32 jf{}, jfHi{}, pedFlag{}, wanted{};         // retorno de 0x609380 (AL + basura), de GetPedFlag, y nivel de busqueda del jugador (+0x2C del CWanted)
    u32 poolSize{};
    u32 slot[kPoolPeds]{}, spt[kPoolPeds]{};       // byte de estado y tipo (dword +0x598) de cada peaton del pool falso
    u32 mutSite{}, mutKind{}, mutFlags{}, mutUp{}, mutDown{}, mutWanted{};
};

inline u32 PedLike(u32 n, u32 k) { return (Rand(n, k) & 0xF8u) | 3u; }
inline u32 PedTypeVal(u32 n, u32 k) {
    const u32 r = Rand(n, k) % 8;
    if (r < 4) return 8;
    if (r == 4) return 0x108;
    if (r == 5) return 0x80000008u;
    return Rand(n, k + 1) % 20;
}
inline u32 Held(u32 n, u32 k) {   // contador de pulsacion previo
    const u32 r = Rand(n, k) % 8;
    if (r == 0) return 0;
    if (r == 1) return 0x4B0;
    if (r == 2) return 0x4AF;
    if (r == 3) return 0x4B1;
    if (r == 4) return Rand(n, k + 1) & 0xFFFFu;
    return 1 + Rand(n, k + 2) % 0x4AF;
}
inline u32 Level(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x80000001u, 0x100u};
    return Rand(n, k) % 3 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.mouse = Rand(n, 10) % 2 ? 1 + Rand(n, 11) % 255 : 0;
    c.tgt = Rand(n, 12) % 4 != 0; c.mtgt = Rand(n, 13) % 2;
    c.tType = Rand(n, 14) % 4 ? PedLike(n, 15) : Rand(n, 16) & 0xFF; c.tPed = PedTypeVal(n, 17);
    c.mType = Rand(n, 18) % 4 ? PedLike(n, 19) : Rand(n, 20) & 0xFF; c.mPed = PedTypeVal(n, 21);
    c.veh1 = Rand(n, 22) % 4 == 0 ? 1 + Rand(n, 23) % 0xFFFFFFF0u : 0; c.veh2 = Rand(n, 24) % 4 == 0 ? 1 + Rand(n, 25) % 0xFFFFFFF0u : 0;
    c.up = Rand(n, 26) % 2; c.upHi = Rand(n, 27) % 2 ? 0u : Rand(n, 28) << 8; c.down = Rand(n, 29) % 2; c.downHi = Rand(n, 30) % 2 ? 0u : Rand(n, 31) << 8;
    c.flags = Rand(n, 32); if (Rand(n, 33) % 5) c.flags &= ~0x100u;
    c.oflags = Rand(n, 34); if (Rand(n, 35) % 2) c.oflags &= ~0x100u;
    c.upT = Held(n, 36); c.downT = Held(n, 40); c.oupT = Held(n, 44); c.odownT = Held(n, 48);
    c.ts = Step(n, 52);
    c.frame = Rand(n, 56) % 3 == 0 ? (Rand(n, 57) & ~0x1Fu) | 6u : Rand(n, 58);
    c.jf = Rand(n, 59) % 2; c.jfHi = Rand(n, 60) % 2 ? 0u : Rand(n, 61) << 8; c.pedFlag = Rand(n, 62); c.wanted = Level(n, 63);
    c.poolSize = Rand(n, 66) % (kPoolPeds + 1);
    for (unsigned i = 0; i < kPoolPeds; ++i) {
        c.slot[i] = (Rand(n, 70 + i) % 4 == 0 ? 0x80u : 0u) | (Rand(n, 80 + i) & 0x7Fu);
        c.spt[i] = PedTypeVal(n, 90 + i);
    }
    return c;
}

inline void Pressed(C& c, bool upSide, u32 n, u32 k) {   // fija el contador previo para que la pulsacion llegue a 0x4B0 +- 2
    const u32 add = AddOf(c.ts);
    const i32 delta = i32(Rand(n, k) % 5) - 2;
    const u32 v = (0x4B0u - add + u32(delta)) & 0xFFFFu;
    if (upSide) c.upT = v; else c.downT = v;
}

inline C MakeExtra(unsigned n) {
    C c = BaseDefaults(n);
    const unsigned j = n - kBaseCases;
    if (j < kFamExact) {   // familia 16
        c.family = 16;
        static constexpr u32 steps[]{0x41A00000u, 0x41F00000u, 0x42480000u, 0x3F800000u, 0x40000000u, 0x41200000u, 0x3DCCCCCDu, 0x42C80000u};
        c.ts = steps[Rand(n, 100) % std::size(steps)];
        c.veh1 = c.veh2 = 0; c.flags &= ~0x100u; if (Rand(n, 101) % 4 == 0) c.flags |= 0x100u;
        c.up = j % 2; c.down = 1 - c.up;
        if (j % 2) Pressed(c, true, n, 102); else Pressed(c, false, n, 103);
        if (Rand(n, 104) % 4 == 0) { c.up = c.down = 1; Pressed(c, true, n, 105); Pressed(c, false, n, 106); }
        return c;
    }
    if (j < kFamExact + kFamStep) {   // familia 17
        c.family = 17;
        c.ts = Step(n, 110); if (Rand(n, 111) % 2) c.ts = Step(n, 112);
        c.veh1 = c.veh2 = 0; c.flags &= ~0x100u;
        c.up = Rand(n, 113) % 2; c.down = Rand(n, 114) % 2;
        c.upT = Rand(n, 115) % 3 == 0 ? Held(n, 116) : Rand(n, 117) & 0xFFFFu; c.downT = Rand(n, 118) % 3 == 0 ? Held(n, 119) : Rand(n, 120) & 0xFFFFu;
        return c;
    }
    if (j < kFamExact + kFamStep + kFamMut) {   // familia 18
        c.family = 18;
        const unsigned m = j - kFamExact - kFamStep;
        c.mutSite = 1 + m % 9; c.mutKind = 1 + (m / 9) % 15;
        c.mutFlags = Rand(n, 130); if (Rand(n, 131) % 2) c.mutFlags ^= 0x100u;
        c.mutUp = Held(n, 132); c.mutDown = Held(n, 136); c.mutWanted = Level(n, 140);
        c.veh1 = Rand(n, 141) % 3 == 0 ? 7u : 0; c.veh2 = Rand(n, 142) % 3 == 0 ? 9u : 0;
        c.flags &= ~0x100u; if (Rand(n, 143) % 4 == 0) c.flags |= 0x100u;
        c.tgt = 1; c.tType = PedLike(n, 144); c.mtgt = 1; c.mType = PedLike(n, 145);
        c.up = (m / 135) % 2; c.down = (m / 270) % 2;
        if (c.up) { c.upT = (0x4B0u - AddOf(c.ts) + u32(i32(Rand(n, 146) % 3) - 1)) & 0xFFFFu; }
        if (Rand(n, 147) % 2) { c.up = 0; c.upT = 1 + Rand(n, 148) % 0x4AF; }
        c.frame = (c.frame & ~0x1Fu) | 6u; c.poolSize = 1 + Rand(n, 149) % kPoolPeds;
        for (unsigned i = 0; i < kPoolPeds; ++i) { c.slot[i] &= 0x7F; c.spt[i] = 8; }
        return c;
    }
    c.family = 19;   // familia 19
    const unsigned m = j - kFamExact - kFamStep - kFamMut;
    c.veh1 = Rand(n, 150) % 2 ? 5u : 0; c.veh2 = Rand(n, 151) % 2 ? 6u : 0;
    c.frame = (Rand(n, 152) & ~0x1Fu) | 6u;
    c.poolSize = m % (kPoolPeds + 1);
    c.wanted = Level(n, 153 + m / 7 % 3);
    for (unsigned i = 0; i < kPoolPeds; ++i) {
        const u32 r = Rand(n, 160 + i) % 6;
        c.slot[i] = (r == 0 ? 0x80u : r == 1 ? 0xFFu : 0u) | (Rand(n, 170 + i) & 0x7Fu);
        c.spt[i] = r == 2 ? 7u : r == 3 ? 9u : 8u;
    }
    if (Rand(n, 180) % 2) { c.mutSite = 9; c.mutKind = 1; c.mutWanted = Level(n, 181); }
    c.pedFlag = Rand(n, 182) % 4 == 0 ? 0u : Rand(n, 183);
    return c;
}

inline C MakeC(unsigned n) {
    if (n >= kBaseCases) return MakeExtra(n);
    C c = BaseDefaults(n);
    auto clear = [&] { c.veh1 = 0; c.flags &= ~0x100u; };
    auto asPed8 = [&] { c.tgt = 1; c.tType = PedLike(n, 190); c.tPed = 8; };
    auto valid = [&](bool upSide) { const u32 v = 1 + Rand(n, 191) % 0x4AF; if (upSide) c.upT = v; else c.downT = v; };
    switch (c.family) {
    case 0: break;
    case 1: c.veh1 = 1 + Rand(n, 192) % 0xFFFFFF; break;
    case 2: clear(); c.up = 1; c.ts = Step(n, 193); if (Rand(n, 194) % 2) c.ts = 0x41F00000u; Pressed(c, true, n, 195); if (Rand(n, 196) % 3 == 0) c.flags |= 0x100u; break;
    case 3: clear(); c.up = 1; c.ts = Rand(n, 197) % 2 ? Step(n, 198) : 0x41A00000u; Pressed(c, true, n, 199); c.upT = (c.upT + u32(i32(Rand(n, 200) % 7) - 3)) & 0xFFFFu; break;
    case 4: clear(); c.up = 0; asPed8(); valid(true); c.mouse = Rand(n, 201) % 2; break;
    case 5: clear(); c.up = 0; c.tgt = 1; c.tType = PedLike(n, 202); c.tPed = PedTypeVal(n, 203); if (c.tPed == 8) c.tPed = 9; valid(true); break;
    case 6: clear(); c.up = 0; valid(true); c.tgt = Rand(n, 204) % 2; c.mtgt = Rand(n, 205) % 2; c.mouse = Rand(n, 206) % 3 ? 1 + Rand(n, 207) % 255 : 0;
            c.tType = Rand(n, 208) % 2 ? PedLike(n, 209) : Rand(n, 210) & 0xFF; c.mType = Rand(n, 211) % 2 ? PedLike(n, 212) : Rand(n, 213) & 0xFF; break;
    case 7: clear(); c.up = 0; c.upT = Held(n, 214); if (Rand(n, 215) % 2) asPed8(); break;
    case 8: c.flags |= 0x100u; c.veh1 = Rand(n, 216) % 2 ? 0u : 3u; if (Rand(n, 217) % 4 == 0) { c.mutSite = 1 + Rand(n, 218) % 9; c.mutKind = 2; c.mutFlags = Rand(n, 219) & ~0x100u; } break;
    case 9: c.veh1 = 0; c.flags &= ~0x100u; c.veh2 = 0; c.down = 1; c.ts = Rand(n, 220) % 2 ? Step(n, 221) : 0x41A00000u; Pressed(c, false, n, 222); break;
    case 10: c.veh1 = Rand(n, 223) % 2 ? 0u : 3u; c.flags &= ~0x100u; c.veh2 = 0; c.down = 0; valid(false); if (Rand(n, 224) % 2) asPed8(); break;
    case 11: c.veh2 = 1 + Rand(n, 225) % 0xFFFFFF; c.flags &= ~0x100u; c.frame = (c.frame & ~0x1Fu) | 6u; break;
    case 12: c.flags &= ~0x100u; c.frame = (c.frame & ~0x1Fu) | 6u; c.poolSize = 1 + Rand(n, 226) % kPoolPeds; for (unsigned i = 0; i < kPoolPeds; ++i) if (Rand(n, 227 + i) % 3) c.spt[i] = 8; break;
    case 13: c.flags &= ~0x100u; c.frame = (Rand(n, 233) & ~0x1Fu) | (Rand(n, 234) % 3 ? 5u + Rand(n, 235) % 3 : 6u); c.poolSize = 1 + Rand(n, 236) % kPoolPeds; break;
    case 14: c.flags &= ~0x100u; c.veh1 = c.veh2 = 0; c.ts = Step(n, 237); c.up = Rand(n, 238) % 2; c.down = Rand(n, 239) % 2; break;
    default:  // 15: mutaciones
        c.mutSite = 1 + Rand(n, 240) % 9; c.mutKind = 1 + Rand(n, 241) % 15; c.mutFlags = Rand(n, 242); c.mutUp = Held(n, 243); c.mutDown = Held(n, 247); c.mutWanted = Level(n, 251);
        c.flags &= ~0x100u; if (Rand(n, 252) % 2) { c.frame = (c.frame & ~0x1Fu) | 6u; } break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("mouse", c.mouse).U("tgt", c.tgt).U("mtgt", c.mtgt).U("t_type", c.tType).U("t_ped", c.tPed).U("m_type", c.mType).U("m_ped", c.mPed);
    j.U("veh1", c.veh1).U("veh2", c.veh2).U("up", c.up).U("up_hi", c.upHi).U("down", c.down).U("down_hi", c.downHi);
    j.U("flags", c.flags).U("oflags", c.oflags).U("up_t", c.upT).U("down_t", c.downT).U("oup_t", c.oupT).U("odown_t", c.odownT);
    j.U("ts", c.ts).U("frame", c.frame).U("jf", c.jf).U("jf_hi", c.jfHi).U("ped_flag", c.pedFlag).U("wanted", c.wanted).U("pool_size", c.poolSize);
    for (unsigned i = 0; i < kPoolPeds; ++i) { const std::string a = "slot" + std::to_string(i), b = "spt" + std::to_string(i); j.U(a.c_str(), c.slot[i]).U(b.c_str(), c.spt[i]); }
    j.U("mut_site", c.mutSite).U("mut_kind", c.mutKind).U("mut_flags", c.mutFlags).U("mut_up", c.mutUp).U("mut_down", c.mutDown).U("mut_wanted", c.mutWanted);
    return j.End();
}
}
