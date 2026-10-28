#pragma once
// Fixtures del banco HeliUpdate78 (lote 78C): CHeli::UpdateHelis (EXE 0x6C79A0), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CHeli::UpdateHelis  0x6C79A0  cdecl sin argumentos, sin resultado, sin pila que limpiar   (hook CHeli / UpdateHelis, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) para n < 16384 y 1024 casos por cada familia 16..20 despues.
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
#include <utility>
#include <vector>

namespace Hu78Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 5;    // familias 16..20
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). Los helicopteros son CHeli completos (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kHeliWin = 0xA18, kMatWin = 0x48;
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 78175u); }
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
    void Key(const std::string& k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const std::string& k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]
inline u32 Nudge(u32 bits, int ulps) { return u32(std::int32_t(bits) + ulps); }
// Un float: modo 0 moderado, 3 especial (cero, desnormal, inf, NaN), 5 mezcla
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 11);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    default: {
        const u32 m = Rand(c, k + 3) % 3;
        return m == 0 ? FloatOf(c, k + 5, 0u) : m == 1 ? FloatOf(c, k + 5, 3u) : B(Unit(c, k + 6) * 500.0f);
    }
    }
}

// ------------------------------------------------------------------------------------------------ el caso
// Ranuras: 0 y 1 = pHelis[0] y pHelis[1]; 2 = el helicoptero nuevo que devuelve GenerateHeli
struct Slot {
    u32 present{};     // solo ranuras 0 y 1: pHelis[i] != 0
    u32 model{};       // word en +0x22 (se compara con 0x1F1 y 0x1E8 como 16 bits)
    u32 flags{};       // dword en +0x40 (bit 29 = pendiente de retirar)
    u32 b42b{};        // byte en +0x42B (bit 6 = pendiente de retirar)
    u32 mission{};     // byte en +0x3BA (0x2A = huye)
    u32 hasMat{};      // tiene matriz (posicion en +0x30) o solo posicion compacta (+4)
    u32 pos[3]{};      // posicion del helicoptero
};
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 now{}, test{};        // CTimer::m_snTimeInMilliseconds (0xB7CB84) y TestForNewRandomHelisTimer (0xC1C960)
    u32 maxHelis{};           // retorno de CWanted::NumOfHelisRequired (con signo)
    u32 noRain{};             // retorno de CCullZones::PlayerNoRain: solo cuenta AL
    u32 area{};               // CGame::currArea (0xB72914)
    u32 allowed{};            // CHeli::bPoliceHelisAllowed (byte 0x8D338C)
    u32 wOld{}, wNew{};       // CWeather::OldWeatherType (0xC81320) y NewWeatherType (0xC8131C), dwords: el EXE compara solo 16 bits
    u32 useNews{};            // CWanted::UseNewsHeliInAdditionToPolice (byte 0xB7CB8C)
    u32 b73a4{}, b72f0{};     // bytes 0x8E73A4 y 0x8E72F0
    u32 lights{};             // CHeli::NumberOfSearchLights inicial (0xC1C96C)
    u32 wanted{}, ped{};      // retornos de FindPlayerWanted y FindPlayerPed
    u32 player[3]{};          // retorno de FindPlayerCoors
    Slot s[3]{};
    u32 gate{};               // informativo
};

inline u32 Byte8(u32 n, u32 k) { return Rand(n, k) & 0xFFu; }
inline u32 Hi(u32 n, u32 k, u32 low8) { return (Rand(n, k) & 0xFFFFFF00u) | (low8 & 0xFFu); }   // dword con basura arriba y el byte bajo elegido

inline u32 PickModel(u32 n, u32 k) {
    static constexpr u32 m[]{0x1F1u, 0x1F1u, 0x1F1u, 0x1E8u, 0x1E8u, 0x1F0u, 0x1F2u, 0x1E7u, 0x1E9u, 0u, 0xFFFFu, 0x11F1u, 0x2E8u};
    return Rand(n, k) % 5 == 0 ? Rand(n, k + 1) & 0xFFFFu : m[Rand(n, k + 2) % std::size(m)];
}
inline u32 PickMission(u32 n, u32 k) {
    static constexpr u32 m[]{0x2Au, 0x2Au, 0x2Au, 0x2Au, 0x29u, 0x28u, 0x2Bu, 0x3Au, 0u, 0xFFu, 0xAAu, 0x2Au};
    return Rand(n, k) % 5 == 0 ? Byte8(n, k + 1) : m[Rand(n, k + 2) % std::size(m)];
}
// bit 29 (flags) y bit 6 (b42b) a 1 con probabilidad 1/4; el resto aleatorio
inline u32 PickFlags(u32 n, u32 k) { u32 f = Rand(n, k); f = (Rand(n, k + 1) % 4 == 0) ? (f | 0x20000000u) : (f & ~0x20000000u); return f; }
inline u32 PickB42b(u32 n, u32 k) { u32 f = Byte8(n, k); f = (Rand(n, k + 1) % 4 == 0) ? (f | 0x40u) : (f & ~0x40u); return f; }

inline void RandSlot(C& c, int i, u32 n, u32 k) {
    Slot& s = c.s[i];
    s.present = i < 2 ? (Rand(n, k) % 4 != 0) : 1u;
    s.model = PickModel(n, k + 1);
    s.flags = PickFlags(n, k + 4);
    s.b42b = PickB42b(n, k + 6);
    s.mission = PickMission(n, k + 8);
    s.hasMat = Rand(n, k + 11) % 2;
    // posicion a [0, 400] del jugador por componentes
    for (int a = 0; a < 3; ++a) s.pos[a] = B(F(c.player[a]) + Unit(n, k + 12 + a) * 250.0f);
}

// Pone en el helicoptero i un desplazamiento (dx, dy, dz) respecto al jugador: pos = jugador - d, de modo que jugador - pos = d (con el redondeo de la resta)
inline void SetOffset(C& c, int i, float dx, float dy, float dz) {
    c.s[i].pos[0] = B(F(c.player[0]) - dx); c.s[i].pos[1] = B(F(c.player[1]) - dy); c.s[i].pos[2] = B(F(c.player[2]) - dz);
}
// Ternas exactas de 170: a2 + b2 + c2 = 28900
inline constexpr int kTriples[14][3]{{0, 0, 170}, {0, 26, 168}, {0, 72, 154}, {0, 80, 150}, {0, 102, 136}, {8, 90, 144}, {10, 24, 168}, {10, 120, 120}, {24, 118, 120}, {42, 80, 144}, {48, 64, 150}, {48, 90, 136}, {64, 102, 120}, {80, 90, 120}};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.ecx = SeedReg(n, 3); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.now = 100000u + Rand(n, 10) % 1000000u;
    c.test = Rand(n, 11) % 3 == 0 ? c.now - 1u - Rand(n, 12) % 20000u : c.now + 1u + Rand(n, 12) % 20000u;   // activo (el temporizador ya vencio) o no
    static constexpr i32 mh[]{0, 1, 2, 3, 1, 2, -1, -5, 6, 4, 2, 1};
    c.maxHelis = Rand(n, 13) % 20 == 0 ? Rand(n, 14) : u32(mh[Rand(n, 15) % std::size(mh)]);
    c.noRain = Rand(n, 16) % 5 == 0 ? Hi(n, 17, 1) : Hi(n, 17, 0);
    c.area = Rand(n, 18) % 8 == 0 ? Rand(n, 19) : 0u;
    c.allowed = Rand(n, 20) % 6 == 0 ? Byte8(n, 21) : 1u;
    c.wOld = Rand(n, 22) % 8 == 0 ? (Rand(n, 23) & 0xFFFF0000u) | 0x13u : Rand(n, 24) % 24;
    c.wNew = Rand(n, 25) % 8 == 0 ? (Rand(n, 26) & 0xFFFF0000u) | 0x13u : Rand(n, 27) % 24;
    c.useNews = Rand(n, 28) % 2;
    c.b73a4 = Rand(n, 29) % 8 == 0 ? Byte8(n, 30) : 1u;
    c.b72f0 = Rand(n, 31) % 8 == 0 ? Byte8(n, 32) : 1u;
    c.lights = Rand(n, 33) % 2 ? Rand(n, 34) : Rand(n, 34) % 5;
    c.wanted = 0xC0000000u | (Rand(n, 35) & 0x3FFFFFF0u);
    c.ped = 0xC0000000u | (Rand(n, 36) & 0x3FFFFFF0u);
    for (int i = 0; i < 3; ++i) c.player[i] = B(Unit(n, 40 + i) * 1500.0f);
    for (int i = 0; i < 3; ++i) RandSlot(c, i, n, 100 + 20 * i);
    c.gate = 1;
    // Casi todos los casos de la mayoria de las familias pasan por la ruta de generacion y la de retirada con probabilidad razonable: los bytes de "permitido" y el temporizador estan
    // sesgados a favor (los valores raros se prueban en las familias dedicadas).
    const bool biasSpawn = c.family != 3 && c.family != 4 && c.family != 2 && c.family != 19;
    if (biasSpawn) { c.allowed = 1; c.noRain = Hi(n, 17, 0); c.area = 0; c.wOld = Rand(n, 24) % 19; c.wNew = Rand(n, 27) % 19; c.b73a4 = 1; c.b72f0 = 1; }
    switch (c.family) {
    case 0:   // el caso base
        break;
    case 1: {  // generacion: temporizador vencido, cupo libre; ranuras vacias / una / dos; bandera de noticias
        c.test = c.now - 1u - Rand(n, 200) % 100u;
        c.maxHelis = u32(1 + Rand(n, 201) % 3);
        const u32 occ = Rand(n, 202) % 4;
        c.s[0].present = occ & 1; c.s[1].present = occ >> 1;
        if (Rand(n, 203) % 2) { c.s[0].model = 0x1F1; c.s[1].model = 0x1F1; }
        c.s[2].mission = Rand(n, 204) % 3 ? 0x28u : 0x2Au;   // el nuevo (con 0x2A pasa por la retirada)
        break;
    }
    case 2: {  // temporizador: now frente a test (sin signo): igual, +-1, vuelta de 32 bits
        c.maxHelis = u32(1 + Rand(n, 200) % 3);
        const u32 t = Rand(n, 201) % 10;
        c.now = t == 0 ? 0u : t == 1 ? 0xFFFFFFFFu : t == 2 ? 0x80000000u : t == 3 ? 0x7FFFFFFFu : c.now;
        c.test = t >= 4 ? c.now + u32(int(Rand(n, 202) % 5u) - 2) : t == 0 ? Pick(kNan, n, 203) & 0x3u : t == 1 ? 0xFFFFFFFEu : t == 2 ? 0x7FFFFFFFu : 0x80000000u;
        if (Rand(n, 204) % 3 == 0) c.test = Rand(n, 205);
        break;
    }
    case 3: {  // bytes de "permitido" y bandera de noticias: valores 0, 1, 2, 0x81, 0xFF
        static constexpr u32 v[]{0u, 1u, 2u, 0x81u, 0xFFu, 0x80u, 0x100u, 0x101u};
        c.test = c.now - 1u - Rand(n, 200) % 100u;
        c.maxHelis = u32(1 + Rand(n, 201) % 3);
        c.b73a4 = v[Rand(n, 202) % std::size(v)] & 0xFFu; c.b72f0 = v[Rand(n, 203) % std::size(v)] & 0xFFu;
        c.useNews = Rand(n, 204) % 5 == 0 ? Byte8(n, 205) : Rand(n, 206) % 2;
        c.allowed = 1;
        c.noRain = Hi(n, 207, 0); c.area = 0; c.wOld = 0; c.wNew = 0;
        c.s[0].present = 1; c.s[1].present = Rand(n, 208) % 2;
        c.s[0].model = 0x1F1; c.s[0].flags &= ~0x20000000u; c.s[0].b42b &= ~0x40u;
        if (c.s[1].present) c.s[1].model = Rand(n, 209) % 2 ? 0x1F1u : 0x1E8u;
        break;
    }
    case 4: {  // condiciones que anulan maxHelis: lluvia, area, permitido, clima 0x13 (16 bits), con la ranura 0 ocupada
        c.test = c.now - 1u - Rand(n, 200) % 100u;
        c.maxHelis = u32(1 + Rand(n, 201) % 3);
        c.allowed = 1; c.noRain = Hi(n, 202, 0); c.area = 0; c.wOld = 0; c.wNew = 0;
        switch (Rand(n, 203) % 10) {
        case 0: c.noRain = Hi(n, 204, 1); break;
        case 1: c.area = 1u + Rand(n, 205) % 20u; break;
        case 2: c.area = Rand(n, 206) | 0x80000000u; break;
        case 3: c.allowed = 0; break;
        case 4: c.allowed = 2u + Byte8(n, 207) % 200u; break;
        case 5: c.wOld = 0x13u; break;
        case 6: c.wNew = 0x13u; break;
        case 7: c.wOld = (Rand(n, 208) | 0x10000u) & 0xFFFF0013u; c.wNew = 0x13u + (Rand(n, 209) % 3) * 0x10000u; break;   // 0x13 en 16 bits con basura arriba
        case 8: c.wOld = 0x113u; c.wNew = 0x12u + (Rand(n, 210) % 4); break;
        default: c.wOld = 0x13u; c.wNew = 0x13u; c.noRain = Hi(n, 211, 1); break;
        }
        c.s[0].present = Rand(n, 212) % 3 != 0; c.s[1].present = Rand(n, 213) % 2;
        break;
    }
    case 5: {  // frontera exacta de la distancia 170 (ternas pitagoricas) con la mision 0x2A
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int i = 0; i < 2; ++i) {
            const int* t = kTriples[Rand(n, 200 + i) % 14];
            int a[3]{t[0], t[1], t[2]};
            for (int k = 2; k > 0; --k) std::swap(a[k], a[Rand(n, 210 + i * 4 + k) % (k + 1)]);
            SetOffset(c, i, float(Rand(n, 220 + i) % 2 ? a[0] : -a[0]), float(Rand(n, 222 + i) % 2 ? a[1] : -a[1]), float(Rand(n, 224 + i) % 2 ? a[2] : -a[2]));
            if (Rand(n, 226 + i) % 3 == 0) c.s[i].pos[Rand(n, 228 + i) % 3] = Nudge(c.s[i].pos[Rand(n, 228 + i) % 3], int(Rand(n, 230 + i) % 3u) - 1);
        }
        for (int i = 0; i < 2; ++i) c.player[i] = B(Rand(n, 240) % 2 ? Unit(n, 241 + i) * 100.0f : Unit(n, 241 + i) * 2000.0f);
        break;
    }
    case 6: {  // cerca de 170 con desplazamientos reales que no son ternas: redondeo de la suma (z2 + y2) + x2 a 24 bits y de la raiz
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int i = 0; i < 2; ++i) {
            float a = Unit(n, 200 + i * 3), b = Unit(n, 201 + i * 3), d = Unit(n, 202 + i * 3);
            const float len = std::sqrt(a * a + b * b + d * d);
            const float k = len == 0.0f ? 0.0f : 170.0f / len;
            const float scale = 1.0f + float(int(Rand(n, 210 + i) % 5u) - 2) * 1.0e-6f;
            SetOffset(c, i, a * k * scale, b * k * scale, d * k * scale);
            if (Rand(n, 214 + i) % 2) c.s[i].pos[Rand(n, 216 + i) % 3] = Nudge(c.s[i].pos[Rand(n, 216 + i) % 3], int(Rand(n, 218 + i) % 5u) - 2);
        }
        if (Rand(n, 230) % 2) for (int i = 0; i < 3; ++i) c.player[i] = B(Unit(n, 231 + i) * 5000.0f);
        break;
    }
    case 7:   // especiales en la posicion del jugador y de los helicopteros (NaN no canonicos, infinitos, ceros con signo, desnormales); mision 0x2A
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int a = 0; a < 3; ++a) if (Rand(n, 200 + a) % 3 == 0) c.player[a] = FloatOf(n, 203 + a, 3u);
        for (int i = 0; i < 2; ++i) for (int a = 0; a < 3; ++a) if (Rand(n, 210 + i * 3 + a) % 3 == 0) c.s[i].pos[a] = FloatOf(n, 220 + i * 3 + a, 3u);
        break;
    case 8: {  // marcados para retirar (bit 29 de +0x40, bit 6 de +0x42B) con cualquier mision: solo la mision y la ranura
        c.test = c.now + 1u;
        for (int i = 0; i < 3; ++i) {
            c.s[i].present = 1;
            const u32 t = Rand(n, 200 + i) % 4;
            c.s[i].flags = (c.s[i].flags & ~0x20000000u) | ((t & 1) ? 0x20000000u : 0u);
            c.s[i].b42b = (c.s[i].b42b & ~0x40u) | ((t & 2) ? 0x40u : 0u);
            c.s[i].mission = Rand(n, 210 + i) % 2 ? 0x2Au : PickMission(n, 220 + i);
        }
        c.maxHelis = Rand(n, 230) % 2 ? u32(Rand(n, 231) % 4) : c.maxHelis;
        break;
    }
    case 9: {  // retirada: mision 0x2A, lejos (de 180 a 5000) o cerca (de 0 a 160): CWorld::Remove + destructor
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int i = 0; i < 2; ++i) {
            const float r = Rand(n, 200 + i) % 2 ? 180.0f + float(Rand(n, 202 + i) % 4800u) : float(Rand(n, 202 + i) % 160u);
            const float a = Unit(n, 204 + i * 3), b = Unit(n, 205 + i * 3), d = Unit(n, 206 + i * 3);
            const float len = std::sqrt(a * a + b * b + d * d);
            const float k = len == 0.0f ? 0.0f : r / len;
            SetOffset(c, i, a * k, b * k, d * k);
        }
        break;
    }
    case 10: {  // el resto: mision distinta de 0x2A con maxHelis variado (el primero y el segundo se mandan a huir con alturas 100)
        c.test = c.now + 1u;
        for (int i = 0; i < 2; ++i) {
            c.s[i].present = Rand(n, 200 + i) % 4 != 0;
            c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u;
            c.s[i].mission = Rand(n, 202 + i) % 4 == 0 ? 0x2Au : PickMission(n, 204 + i * 2) == 0x2Au ? 0x29u : PickMission(n, 204 + i * 2);
        }
        static constexpr i32 v[]{0, 1, 2, 3, -1, -2, 0x7FFFFFFF, i32(0x80000000), 1, 0, 2};
        c.maxHelis = u32(v[Rand(n, 210) % std::size(v)]);
        break;
    }
    case 11:   // mezcla de todo (sin ajustes)
        for (int i = 0; i < 3; ++i) RandSlot(c, i, n, 300 + 20 * i);
        c.maxHelis = Rand(n, 320);
        if (Rand(n, 321) % 2) c.player[0] = FloatOf(n, 322, 5u);
        break;
    case 12: {  // modelos y bandera de noticias: 0x1F1 / 0x1E8 / otros (word), con los byte 0x8E72F0 y 0xB7CB8C
        c.test = c.now - 1u - Rand(n, 200) % 100u;
        c.maxHelis = u32(1 + Rand(n, 201) % 4);
        c.useNews = 1; c.b72f0 = 1; c.b73a4 = Rand(n, 202) % 2;
        c.s[0].present = 1; c.s[1].present = Rand(n, 203) % 2;
        for (int i = 0; i < 2; ++i) { c.s[i].model = PickModel(n, 204 + i * 2); if (Rand(n, 208 + i) % 2) c.s[i].model = 0x1F1; }
        c.s[0].flags = Rand(n, 210) % 3 ? c.s[0].flags & ~0x20000000u : c.s[0].flags | 0x20000000u;
        c.s[0].b42b = Rand(n, 211) % 3 ? c.s[0].b42b & ~0x40u : c.s[0].b42b | 0x40u;
        if (Rand(n, 212) % 4 == 0) { c.s[1].flags |= 0x20000000u; }
        break;
    }
    case 13:   // matriz o posicion compacta para cada helicoptero, mision 0x2A, la posicion no usada con basura
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; c.s[i].hasMat = Rand(n, 200 + i) % 2; }
        c.test = c.now + 1u;
        for (int i = 0; i < 2; ++i) { if (Rand(n, 210 + i) % 2) SetOffset(c, i, float(Rand(n, 212 + i) % 400u), float(Rand(n, 214 + i) % 400u), 0.0f); }
        break;
    case 14:   // registros y pila sembrados con generacion y retirada (ABI)
        c.test = c.now - 1u - Rand(n, 200) % 100u;
        c.maxHelis = u32(1 + Rand(n, 201) % 3);
        c.s[0].present = Rand(n, 202) % 2; c.s[1].present = Rand(n, 203) % 2;
        c.s[2].mission = 0x2Au;
        for (int i = 0; i < 3; ++i) { c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; }
        break;
    case 15: {  // 15: generacion con las dos ranuras llenas (el nuevo no se registra), una sola ocupada (0 o 1) y el nuevo marcado para retirar
        c.test = c.now - 1u - Rand(n, 200) % 100u;
        c.maxHelis = u32(3 + Rand(n, 201) % 3);
        const u32 occ = 1 + Rand(n, 202) % 3;   // 1: solo 0, 2: solo 1, 3: ambas
        c.s[0].present = occ & 1; c.s[1].present = (occ >> 1) & 1;
        if (occ == 3) c.maxHelis = u32(3 + Rand(n, 203) % 3);
        c.s[2].flags = (c.s[2].flags & ~0x20000000u) | (Rand(n, 204) % 3 == 0 ? 0x20000000u : 0u);
        c.s[2].mission = Rand(n, 205) % 2 ? 0x2Au : 0x28u;
        // un numero de helicopteros vivos menor que maxHelis con maxHelis hasta 5: ambas ranuras pueden no estar llenas
        break;
    }
    case 16: {  // exponentes extremos del jugador y de los helicopteros: la resta, los cuadrados y la raiz cambian de rango (mision 0x2A)
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int a = 0; a < 3; ++a) c.player[a] = ExpFloat(n, 200 + a, int(Rand(n, 203 + a) % 100u) - 60);
        for (int i = 0; i < 2; ++i) for (int a = 0; a < 3; ++a) c.s[i].pos[a] = Rand(n, 210 + i * 3 + a) % 4 == 0 ? Denorm(n, 220 + i * 3 + a) : ExpFloat(n, 230 + i * 3 + a, int(Rand(n, 240 + i * 3 + a) % 100u) - 60);
        break;
    }
    case 17:   // NaN en cualquier componente: nunca cuentan como lejos
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int a = 0; a < 3; ++a) if (Rand(n, 200 + a) % 3 == 0) c.player[a] = Pick(kNan, n, 203 + a);
        for (int i = 0; i < 2; ++i) for (int a = 0; a < 3; ++a) if (Rand(n, 210 + i * 3 + a) % 3 == 0) c.s[i].pos[a] = Pick(kNan, n, 220 + i * 3 + a);
        if (Rand(n, 230) % 2) for (int i = 0; i < 2; ++i) SetOffset(c, i, 300.0f, 0.0f, 0.0f);
        break;
    case 18: {  // desbordamientos y diferencias enormes (cuadrados de 1e20 o mas) con infinitos
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int a = 0; a < 3; ++a) c.player[a] = Rand(n, 200 + a) % 5 == 0 ? Pick(kInf, n, 203 + a) : Rand(n, 206 + a) % 2 ? Pick(kBig, n, 209 + a) : ExpFloat(n, 212 + a, 5 + int(Rand(n, 215 + a) % 60u));
        for (int i = 0; i < 2; ++i) for (int a = 0; a < 3; ++a) c.s[i].pos[a] = Rand(n, 220 + i * 3 + a) % 5 == 0 ? Pick(kInf, n, 226 + i * 3 + a) : Rand(n, 232 + i * 3 + a) % 2 ? Pick(kBig, n, 238 + i * 3 + a) : ExpFloat(n, 244 + i * 3 + a, 5 + int(Rand(n, 250 + i * 3 + a) % 60u));
        break;
    }
    case 19: {  // temporizador y salvaguardas con todo mezclado (estado inicial sucio: NumberOfSearchLights, referencias)
        c.test = Rand(n, 200) % 2 ? c.now - 1u : Rand(n, 201);
        c.maxHelis = Rand(n, 202) % 2 ? u32(1 + Rand(n, 203) % 3) : Rand(n, 204);
        c.useNews = Byte8(n, 205); c.b73a4 = Byte8(n, 206) % 3 ? 1u : Byte8(n, 207); c.b72f0 = Byte8(n, 208) % 3 ? 1u : Byte8(n, 209);
        c.lights = Rand(n, 210);
        break;
    }
    default: {  // 20: dos helicopteros de la misma terna exacta de 170 con la segunda en el limite +-1 ulp en una componente y el jugador con exponentes variados
        for (int i = 0; i < 3; ++i) { c.s[i].mission = 0x2Au; c.s[i].flags &= ~0x20000000u; c.s[i].b42b &= ~0x40u; c.s[i].present = 1; }
        c.test = c.now + 1u;
        for (int a = 0; a < 3; ++a) c.player[a] = B(Unit(n, 200 + a) * (Rand(n, 203 + a) % 2 ? 4000.0f : 40.0f));
        for (int i = 0; i < 2; ++i) {
            const int* t = kTriples[Rand(n, 210 + i) % 14];
            SetOffset(c, i, float(Rand(n, 212 + i) % 2 ? t[0] : -t[0]), float(Rand(n, 214 + i) % 2 ? t[1] : -t[1]), float(Rand(n, 216 + i) % 2 ? t[2] : -t[2]));
            const u32 comp = Rand(n, 218 + i) % 3;
            c.s[i].pos[comp] = Nudge(c.s[i].pos[comp], int(Rand(n, 220 + i) % 7u) - 3);
        }
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("now", c.now).U("test", c.test).U("max_helis", c.maxHelis).U("no_rain", c.noRain).U("area", c.area).U("allowed", c.allowed).U("w_old", c.wOld).U("w_new", c.wNew);
    j.U("use_news", c.useNews).U("b73a4", c.b73a4).U("b72f0", c.b72f0).U("lights", c.lights).U("wanted", c.wanted).U("ped", c.ped);
    j.U("player_x", c.player[0]).U("player_y", c.player[1]).U("player_z", c.player[2]).U("gate", c.gate);
    for (int i = 0; i < 3; ++i) {
        const std::string p = "s" + std::to_string(i) + "_";
        j.U(p + "present", c.s[i].present).U(p + "model", c.s[i].model).U(p + "flags", c.s[i].flags).U(p + "b42b", c.s[i].b42b).U(p + "mission", c.s[i].mission).U(p + "has_mat", c.s[i].hasMat);
        j.U(p + "x", c.s[i].pos[0]).U(p + "y", c.s[i].pos[1]).U(p + "z", c.s[i].pos[2]);
    }
    return j.End();
}
}
