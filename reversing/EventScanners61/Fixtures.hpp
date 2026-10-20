#pragma once
// Fixtures del banco EventScanners61. Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp.
// Cinco funciones, cada una con su generador y sus arenas:
//   CMentalState::Process                                          (0x6008A0, thiscall, 1 argumento de pila (ped), `ret 4`, void)
//   CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents (0x603720, thiscall, 3 argumentos, `ret 0Ch`, void)
//   CEventScanner::ScanForEvents                                   (0x607E30, thiscall, 1 argumento (ped&), `ret 4`, void)
//   CPedStuckChecker::TestPedStuck                                 (0x602C00, thiscall, 2 argumentos, `ret 8`, bool)
//   CCollisionEventScanner::ScanForCollisionEvents                 (0x604500, thiscall, 2 argumentos, `ret 8`, void)
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace EventScanners61Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
constexpr u32 F(float f) { return std::bit_cast<u32>(f); }
inline float FromBits(u32 v) { return std::bit_cast<float>(v); }

constexpr u32 kMaxEvents = 64;
// Funciones activas en esta compilacion (iterar de una en una; el banco final las activa todas)
constexpr u32 kFnMental = 1, kFnVpc = 2, kFnScan = 4, kFnStuck = 8, kFnColl = 16;
constexpr u32 kEnabled = kFnMental | kFnVpc | kFnScan | kFnStuck;

// ------------------------------------------------------------------------------------------------ CMentalState::Process
constexpr unsigned kCasesMental = 4096;   // 16 familias x 256
// Arenas (guarda | objeto | guarda; suma a mano):
constexpr u32 kMsOff = 0x20, kMsBytes = 0x14, kMsArena = 0x54;           // 0x20 + 0x14 + 0x20
constexpr u32 kMpedOff = 0x40, kMpedBytes = 0x5A0, kMpedArena = 0x620;   // 0x40 + 0x5A0 + 0x40
constexpr u32 kMvehOff = 0x40, kMvehBytes = 0x4D0, kMvehArena = 0x550;   // 0x40 + 0x4D0 + 0x40
constexpr u32 kMintOff = 0x40, kMintBytes = 0x100, kMintArena = 0x180;   // 0x40 + 0x100 + 0x40
// Desplazamientos (EXE): estado: +0 enfado, +1 enfado anterior, +10h salud peaton, +11h anterior, +12h salud vehiculo, +13h anterior
constexpr u32 kMsAnger = 0, kMsLastAnger = 1, kMsPedHp = 0x10, kMsOldPedHp = 0x11, kMsVehHp = 0x12, kMsOldVehHp = 0x13;
constexpr u32 kPedFlagsAt = 0x46C, kPedIntelAt = 0x47C, kPedHealthAt = 0x540, kPedVehicleAt = 0x58C, kVehHealthAt = 0x4C0, kIntelGroupAt = 0x68;

struct M {
    u32 family;
    u32 anger, lastAnger, pedHp, oldPedHp, vehHp, oldVehHp;     // bytes del estado (valores 0..255)
    u32 pedHealthBits, vehHealthBits;                          // floats
    u32 inVehicle, vehNull, flagsNoise;
    u32 timerFill;                                             // semilla de los bytes del temporizador (+2..+0xF)
    u32 eax, edx, stack;
};

inline constexpr float kHealthFloats[]{
    0.0f, -0.0f, 0.5f, 1.0f, 9.0f, 9.5f, 9.9999f, 10.0f, 10.0001f, 10.5f, 11.0f, 49.0f, 49.5f, 49.9999f, 50.0f, 50.0001f, 50.5f, 51.0f, 99.9f, 100.0f, 254.0f, 254.9f, 255.0f, 255.5f,
    256.0f, 256.5f, 257.0f, 300.0f, 511.9f, 512.0f, 1000.0f, 65535.5f, 65536.0f, 65546.0f, 16777216.0f, 2147483648.0f, 4294967296.0f, 4294967306.0f, 9.223372e18f, 9.2233725e18f, 1.0e19f, 1.0e30f, 3.4028235e38f,
    -0.5f, -1.0f, -9.0f, -10.0f, -49.0f, -50.0f, -255.0f, -256.0f, -257.0f, -65536.0f, -2147483648.0f, -4294967296.0f, -9.223372e18f, -1.0e19f, -3.4028235e38f};
constexpr u32 kHealthSpecial[]{0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x7FC12345u, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x00800000u};
inline constexpr u8 kBoundary[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 48, 49, 50, 51, 52, 99, 100, 127, 128, 129, 200, 254, 255, 255, 3, 4, 6, 7};

inline u32 HealthBits(u32 n, u32 k) {
    const u32 r = Rand(n, k);
    switch (r % 8) {
    case 0: return kHealthSpecial[Rand(n, k + 1) % std::size(kHealthSpecial)];
    case 1: return F(float(Rand(n, k + 1) % 400) * 0.99f);
    case 2: return F(float(Rand(n, k + 1) % 20000) - 10000.0f);
    default: return F(kHealthFloats[Rand(n, k + 1) % std::size(kHealthFloats)]);
    }
}

// Familias (n % 16, 256 casos cada una):
//  0..3  fuera de vehiculo, salud anterior alta (>= 50) con la nueva en <10, 10..49, >= 50 y en los bordes (eventos Really_Low/Low o ninguno)
//  4..5  fuera de vehiculo, salud anterior 10..49 y nueva < 10 / otras; salud anterior < 10
//  6..7  en vehiculo (con y sin puntero a vehiculo): las ramas de vehiculo nunca disparan eventos (bytes <= 255 contra 300/600)
//  8..11 enfado: anterior <= 3 con nuevo >6, 4..6, <=3 y 7+; anterior 4..6 y 7+ con nuevo > 6
//  12    fuera de vehiculo con la salud del peaton y el enfado a la vez (varios eventos en una llamada)
//  13    sin vehiculo/ruido en los banderas (bits del peaton alrededor de 0x100 sin cambiar el bit)
//  14..15 al azar
inline M MakeM(u32 n) {
    M m{};
    const u32 f = n % 16;
    m.family = f;
    m.anger = kBoundary[Rand(n, 1) % std::size(kBoundary)];
    m.lastAnger = kBoundary[Rand(n, 2) % std::size(kBoundary)];
    m.pedHp = kBoundary[Rand(n, 3) % std::size(kBoundary)];
    m.oldPedHp = kBoundary[Rand(n, 4) % std::size(kBoundary)];
    m.vehHp = kBoundary[Rand(n, 5) % std::size(kBoundary)];
    m.oldVehHp = kBoundary[Rand(n, 6) % std::size(kBoundary)];
    if (Rand(n, 7) % 4 == 0) { m.anger = Rand(n, 8) & 0xFF; m.lastAnger = Rand(n, 9) & 0xFF; m.pedHp = Rand(n, 10) & 0xFF; m.oldPedHp = Rand(n, 11) & 0xFF; m.vehHp = Rand(n, 12) & 0xFF; m.oldVehHp = Rand(n, 13) & 0xFF; }
    m.pedHealthBits = HealthBits(n, 20);
    m.vehHealthBits = HealthBits(n, 30);
    m.inVehicle = 0;
    m.vehNull = Rand(n, 40) % 4 == 0;
    m.flagsNoise = Rand(n, 41);
    m.timerFill = Rand(n, 42);
    m.eax = Rand(n, 70); m.edx = Rand(n, 71);
    m.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    const auto pick = [&](u32 lo, u32 hi, u32 k) { return lo + Rand(n, k) % (hi - lo + 1); };
    switch (f) {
    case 0: m.oldPedHp = pick(50, 255, 50); m.pedHealthBits = F(float(pick(0, 9, 51)) + (Rand(n, 52) % 2 ? 0.75f : 0.0f)); break;                // -> ReallyLow
    case 1: m.oldPedHp = pick(50, 255, 50); m.pedHealthBits = F(float(pick(10, 49, 51)) + (Rand(n, 52) % 2 ? 0.75f : 0.0f)); break;               // -> Low
    case 2: m.oldPedHp = pick(50, 255, 50); m.pedHealthBits = F(float(pick(50, 255, 51)) + (Rand(n, 52) % 2 ? 0.75f : 0.0f)); break;              // nada
    case 3: m.oldPedHp = kBoundary[Rand(n, 53) % std::size(kBoundary)]; m.pedHealthBits = F(kHealthFloats[Rand(n, 54) % std::size(kHealthFloats)]); m.inVehicle = Rand(n, 59) % 6 == 0; break;
    case 4: m.oldPedHp = pick(10, 49, 50); m.pedHealthBits = F(float(pick(0, 9, 51))); break;                                                    // -> ReallyLow
    case 5: m.oldPedHp = pick(0, 9, 50); m.pedHealthBits = F(float(pick(0, 60, 51))); break;                                                     // nada (anterior < 10)
    case 6: m.inVehicle = 1; m.vehNull = 0; break;
    case 7: m.inVehicle = 1; m.vehNull = Rand(n, 55) % 2; break;
    case 8: m.lastAnger = pick(0, 3, 56); m.anger = pick(7, 255, 57); break;
    case 9: m.lastAnger = pick(0, 3, 56); m.anger = pick(4, 6, 57); break;
    case 10: m.lastAnger = pick(0, 3, 56); m.anger = pick(0, 3, 57); break;
    case 11: m.lastAnger = pick(4, 6, 56); m.anger = pick(0, 255, 57); break;
    case 12: m.oldPedHp = pick(50, 255, 50); m.pedHealthBits = F(float(pick(0, 49, 51))); m.lastAnger = pick(0, 3, 56); m.anger = pick(4, 255, 57); break;
    case 13: m.lastAnger = pick(7, 255, 56); m.anger = pick(0, 255, 57); break;
    default: m.inVehicle = Rand(n, 58) % 3 == 0; break;
    }
    return m;
}

#include "FixVpc.hpp"
#include "FixScan.hpp"
#include "FixStuck.hpp"

// Constantes de las arenas que usa el banco: se vuelcan en la fila `begin` y run_local.py las compara con las del oraculo (misma suma a mano en ambos lados).
inline const auto& ArenaTable() {
    static const std::vector<std::pair<const char*, u32>> t{
        {"kMsOff", kMsOff}, {"kMsBytes", kMsBytes}, {"kMsArena", kMsArena}, {"kMpedOff", kMpedOff}, {"kMpedBytes", kMpedBytes}, {"kMpedArena", kMpedArena},
        {"kMvehOff", kMvehOff}, {"kMvehBytes", kMvehBytes}, {"kMvehArena", kMvehArena}, {"kMintOff", kMintOff}, {"kMintBytes", kMintBytes}, {"kMintArena", kMintArena},
        {"kVsOff", kVsOff}, {"kVsBytes", kVsBytes}, {"kVsArena", kVsArena}, {"kVpedOff", kVpedOff}, {"kVpedBytes", kVpedBytes}, {"kVpedArena", kVpedArena},
        {"kVmatOff", kVmatOff}, {"kVmatBytes", kVmatBytes}, {"kVmatArena", kVmatArena}, {"kVvehOff", kVvehOff}, {"kVvehBytes", kVvehBytes}, {"kVvehArena", kVvehArena},
        {"kVintOff", kVintOff}, {"kVintBytes", kVintBytes}, {"kVintArena", kVintArena}, {"kVtaskOff", kVtaskOff}, {"kVtaskBytes", kVtaskBytes}, {"kVtaskArena", kVtaskArena},
        {"kVcolOff", kVcolOff}, {"kVcolBytes", kVcolBytes}, {"kVcolArena", kVcolArena},
        {"kEsOff", kEsOff}, {"kEsBytes", kEsBytes}, {"kEsArena", kEsArena}, {"kSpedOff", kSpedOff}, {"kSpedBytes", kSpedBytes}, {"kSpedArena", kSpedArena},
        {"kSintOff", kSintOff}, {"kSintBytes", kSintBytes}, {"kSintArena", kSintArena}, {"kSentOff", kSentOff}, {"kSentBytes", kSentBytes}, {"kSentArena", kSentArena},
        {"kStgtOff", kStgtOff}, {"kStgtBytes", kStgtBytes}, {"kStgtArena", kStgtArena}, {"kSktOff", kSktOff}, {"kSktBytes", kSktBytes}, {"kSktArena", kSktArena},
        {"kSvehOff", kSvehOff}, {"kSvehBytes", kSvehBytes}, {"kSvehArena", kSvehArena}, {"kSplOff", kSplOff}, {"kSplBytes", kSplBytes}, {"kSplArena", kSplArena},
        {"kSfireOff", kSfireOff}, {"kSfireBytes", kSfireBytes}, {"kSfireArena", kSfireArena}, {"kStaskOff", kStaskOff}, {"kStaskBytes", kStaskBytes}, {"kStaskArena", kStaskArena},
        {"kSevOff", kSevOff}, {"kSevBytes", kSevBytes}, {"kSevArena", kSevArena},
        {"kKcOff", kKcOff}, {"kKcBytes", kKcBytes}, {"kKcArena", kKcArena}, {"kKpedOff", kKpedOff}, {"kKpedBytes", kKpedBytes}, {"kKpedArena", kKpedArena},
        {"kKmatOff", kKmatOff}, {"kKmatBytes", kKmatBytes}, {"kKmatArena", kKmatArena}, {"kKentOff", kKentOff}, {"kKentBytes", kKentBytes}, {"kKentArena", kKentArena},
        {"kKwatOff", kKwatOff}, {"kKwatBytes", kKwatBytes}, {"kKwatArena", kKwatArena}, {"kKgrpOff", kKgrpOff}, {"kKgrpBytes", kKgrpBytes}, {"kKgrpArena", kKgrpArena},
        {"kKintOff", kKintOff}, {"kKintBytes", kKintBytes}, {"kKintArena", kKintArena},
    };
    return t;
}
}  // namespace EventScanners61Fix
