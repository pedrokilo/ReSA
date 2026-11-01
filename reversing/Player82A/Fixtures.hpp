#pragma once
// Fixtures del banco Player82A (lote 82A): CPlayerPed::DrawTriangleForMouseRecruitPed (EXE 0x60BA80, thiscall, ECX = jugador, sin argumentos, `ret`, void).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::DrawTriangleForMouseRecruitPed  0x60BA80  ()   (hook CPlayerPed / DrawTriangleForMouseRecruitPed, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n % 16 = familia (1024 casos por familia, 16384 casos).
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

namespace Pm82Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPedWin = 0x800, kMatWin = 0x80;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPedArena = Arena(kPedWin), kMatArena = Arena(kMatWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 82101u); }
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

// Valores especiales de float (0, -0, desnormales, uno, maximos, infinitos, NaN con carga util)
inline u32 SpecialF(u32 n, u32 k) {
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x7F7FFFFFu, 0xFF7FFFFFu,
                              0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x3F000000u, 0x42C80000u, 0x41200000u, 0x42700000u};
    return sp[Rand(n, k) % std::size(sp)];
}
inline bool NonFinite(u32 b) { return (b & 0x7F800000u) == 0x7F800000u; }
// Coordenada de mundo: casi siempre -3000..3000 (con dos decimales)
inline u32 Coord(u32 n, u32 k) { return B(float(i32(Rand(n, k) % 600001) - 300000) / 100.0f); }
inline u32 CoordOrSpecial(u32 n, u32 k, unsigned sp) { return sp && Rand(n, k) % sp == 0 ? SpecialF(n, k + 1) : Coord(n, k + 2); }
inline u32 Vida(u32 n, u32 k) {   // vida de un peaton
    switch (Rand(n, k) % 5) {
    case 0: return B(float(Rand(n, k + 1) % 20001) / 100.0f);
    case 1: return B(float(Rand(n, k + 1) % 101));
    case 2: return B(float(Rand(n, k + 1) % 2001) / 10.0f);
    case 3: return SpecialF(n, k + 2);
    default: return B(float(Rand(n, k + 1) % 20001) / 20.0f);
    }
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 hasTarget{};                       // m_p3rdPersonMouseTarget distinto de nulo
    u32 health{}, maxHealth{}, pedType{};  // vida, vida maxima y tipo (+0x598) del objetivo
    u32 pHasM{}, tHasM{}, cHasM{};         // jugador / objetivo / camara con matriz
    u32 pPl[3]{}, pM[3]{};                 // jugador: posicion de m_placement (+4) y de la matriz (+0x30)
    u32 tPl[3]{}, tM[3]{};                 // objetivo, igual
    u32 cPl[3]{}, cM[3]{}, cRight[3]{};    // camara: posicion de m_placement (0xB6F02C), posicion de la matriz y eje derecho de la matriz (+0, +4, +8)
    u32 cHeading{};                        // rumbo de la camara (0xB6F038)
    u32 xform{};                           // retorno completo (EAX) de RwIm3DTransform
    u32 rsRet{};                           // semilla de los retornos de RwRenderStateSet
};

inline void Pos(u32* dst, u32 n, u32 k) { for (unsigned i = 0; i < 3; ++i) dst[i] = Coord(n, k + 3 * i); }
inline void PosFixed(u32* dst, const float v[3]) { for (unsigned i = 0; i < 3; ++i) dst[i] = B(v[i]); }
inline std::array<u32*, 7> Vecs(C& c) { return {c.pPl, c.pM, c.tPl, c.tM, c.cPl, c.cM, c.cRight}; }

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.hasTarget = Rand(n, 3) % 16 != 0;
    c.health = Vida(n, 10);
    c.maxHealth = Rand(n, 12) % 3 ? B(float(1 + Rand(n, 13) % 100) * (Rand(n, 14) % 2 ? 1.0f : 2.5f)) : Vida(n, 15);
    if (Rand(n, 16) % 2) c.health = B(float(Rand(n, 17) % 10001) / 100.0f);
    c.pedType = Rand(n, 20) % 2 ? 7u : (Rand(n, 21) % 3 ? 4u + Rand(n, 22) % 4 : Rand(n, 23) % 50);
    c.pHasM = Rand(n, 24) % 4 != 0; c.tHasM = Rand(n, 25) % 4 != 0; c.cHasM = Rand(n, 26) % 4 != 0;
    Pos(c.pPl, n, 30); Pos(c.pM, n, 40);
    {   // objetivo a 0..90 m del jugador en el plano (la distancia relevante es 10..60)
        const float d = float(Rand(n, 90) % 9000) / 100.0f;
        const float a = float(Rand(n, 91) % 6283) / 1000.0f;
        for (int k = 0; k < 2; ++k) {
            u32* tp = k ? c.tM : c.tPl;
            const u32* pp = k ? c.pM : c.pPl;
            tp[0] = B(F(pp[0]) + d * std::cos(a)); tp[1] = B(F(pp[1]) + d * std::sin(a)); tp[2] = B(F(pp[2]) + float(i32(Rand(n, 92 + k) % 201) - 100) / 100.0f);
        }
    }
    {   // camara a 5..40 m del objetivo con el eje derecho unitario
        const float a = float(Rand(n, 100) % 6283) / 1000.0f;
        const float d = 5.0f + float(Rand(n, 101) % 3500) / 100.0f;
        for (int k = 0; k < 2; ++k) {
            u32* cp = k ? c.cM : c.cPl;
            const u32* tp = k ? c.tM : c.tPl;
            cp[0] = B(F(tp[0]) + d * std::cos(a)); cp[1] = B(F(tp[1]) + d * std::sin(a)); cp[2] = B(F(tp[2]) + float(i32(Rand(n, 102 + k) % 1001) - 500) / 100.0f);
        }
        const float h = float(Rand(n, 104) % 6283) / 1000.0f;
        c.cRight[0] = B(std::cos(h)); c.cRight[1] = B(std::sin(h)); c.cRight[2] = 0u;
        c.cHeading = B(h);
    }
    c.xform = Rand(n, 110) % 3 ? (Rand(n, 111) % 2 ? 1u : Rand(n, 112) | 1u) : 0u;
    if (Rand(n, 114) % 8 == 0) c.xform = Rand(n, 115) & 0xFFFFFF00u;   // AL = 0 pero no nulo
    c.rsRet = Rand(n, 116);
    return c;
}

inline C MakeC(unsigned n) {
    C c = BaseDefaults(n);
    switch (c.family) {
    case 0: c.pedType = 7; c.pHasM = c.tHasM = c.cHasM = 1; break;
    case 1: c.pedType = 4u + Rand(n, 120) % 3; c.pHasM = c.tHasM = c.cHasM = 1; break;
    case 2: c.pedType = Rand(n, 121) % 2 ? 7u : 5u; c.pHasM = 0; break;
    case 3: c.pedType = Rand(n, 122) % 2 ? 7u : 5u; c.tHasM = 0; break;
    case 4:   // camara sin matriz: eje derecho = (cos(rumbo), sin(rumbo), 0)
        c.cHasM = 0; c.pedType = Rand(n, 123) % 2 ? 7u : 5u;
        c.cHeading = B(float(i32(Rand(n, 126) % 2000001) - 1000000) / 1000.0f);
        break;
    case 5: {   // barrido de la distancia jugador-objetivo alrededor de 10 y 60 (desplazamiento exacto en x)
        static constexpr float ds[]{0.0f, 5.0f, 9.99f, 10.0f, 10.01f, 10.5f, 20.0f, 35.0f, 59.5f, 59.99f, 60.0f, 60.01f, 60.5f, 100.0f, 1000.0f};
        const float d = ds[Rand(n, 128) % std::size(ds)] + (Rand(n, 129) % 3 == 0 ? float(Rand(n, 130) % 100) / 1000.0f : 0.0f);
        const float px = float(i32(Rand(n, 131) % 2001) - 1000);
        const float py = float(i32(Rand(n, 132) % 2001) - 1000);
        const float pz = float(i32(Rand(n, 133) % 201) - 100);
        const float p[3]{px, py, pz}, t[3]{px + d, py, pz};
        PosFixed(c.pPl, p); PosFixed(c.pM, p); PosFixed(c.tPl, t); PosFixed(c.tM, t);
        c.pedType = Rand(n, 134) % 2 ? 7u : 5u; break; }
    case 6: {   // cocientes de vida
        static constexpr float hs[]{0.0f, -1.0f, 1.0f, 100.0f, 50.0f, 25.0f, 99.0f, 100.5f, 200.0f, 0.001f, 1e-30f, 1e30f};
        c.health = Rand(n, 135) % 10 == 0 ? SpecialF(n, 136) : B(hs[Rand(n, 137) % std::size(hs)]);
        c.maxHealth = Rand(n, 138) % 10 == 0 ? SpecialF(n, 139) : B(Rand(n, 140) % 4 ? 100.0f : hs[Rand(n, 141) % std::size(hs)]);
        if (Rand(n, 142) % 3 == 0) c.maxHealth = c.health;
        if (Rand(n, 143) % 4 == 0) c.health = B(F(c.maxHealth) * float(Rand(n, 144) % 1100) / 1000.0f);
        break; }
    case 7: {   // coincidentes: el vertice cae sobre la camara (normalizacion de longitud cero: solo x = 1)
        const float p[3]{float(i32(Rand(n, 145) % 401) - 200), float(i32(Rand(n, 146) % 401) - 200), float(i32(Rand(n, 147) % 41) - 20)};
        PosFixed(c.pPl, p); PosFixed(c.pM, p); PosFixed(c.tPl, p); PosFixed(c.tM, p);
        const float cam[3]{p[0], p[1], p[2] + 1.0f};   // Q = z + 1: v0 (pandillero), v1 y v2 (con eje derecho nulo)
        PosFixed(c.cPl, cam); PosFixed(c.cM, cam);
        if (Rand(n, 148) % 3) { c.cRight[0] = c.cRight[1] = c.cRight[2] = Rand(n, 149) % 2 ? 0u : 0x80000000u; c.cHeading = 0; }
        c.cHasM = Rand(n, 150) % 2; c.pHasM = Rand(n, 151) % 2; c.tHasM = Rand(n, 152) % 2;
        c.pedType = Rand(n, 153) % 2 ? 7u : 4u; break; }
    case 8: {   // camara muy lejos (la longitud al cuadrado desborda) o muy cerca
        const float farD = Rand(n, 154) % 2 ? 1e20f : 1e19f;
        const bool nearD = Rand(n, 160) % 3 == 0;
        for (int k = 0; k < 2; ++k) {
            u32* cp = k ? c.cM : c.cPl;
            const u32* tp = k ? c.tM : c.tPl;
            for (int i = 0; i < 3; ++i) {
                const float sgn = Rand(n, 164 + i + 3 * k) % 2 ? 1.0f : -1.0f;
                cp[i] = B(F(tp[i]) + (nearD ? sgn * float(Rand(n, 170 + i + 3 * k) % 100) / 1000.0f : sgn * farD));
            }
        }
        break; }
    case 9:   // no finitos en posiciones y camara
        for (unsigned v = 0; v < 7; ++v) for (int i = 0; i < 3; ++i) if (Rand(n, 180 + i + 3 * v) % 5 == 0) Vecs(c)[v][i] = SpecialF(n, 210 + i + 3 * v);
        if (Rand(n, 171) % 3 == 0) c.health = SpecialF(n, 172);
        break;
    case 10:   // valores muy pequenos y muy grandes (desnormales incluidos)
        for (unsigned v = 0; v < 7; ++v) for (int i = 0; i < 3; ++i) {
            static constexpr u32 sp[]{0x00000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0x7F000000u, 0x40000000u, 0x33800000u, 0x00000000u};
            if (Rand(n, 240 + i + 3 * v) % 3 == 0) Vecs(c)[v][i] = sp[Rand(n, 270 + i + 3 * v) % std::size(sp)] | (Rand(n, 300 + i + 3 * v) % 2 ? 0x80000000u : 0u);
        }
        break;
    case 11: c.hasTarget = Rand(n, 230) % 4 == 0; break;   // casi siempre sin objetivo
    case 12: c.xform = Rand(n, 231) % 2 ? 0u : (Rand(n, 232) % 2 ? 1u : Rand(n, 233)); c.rsRet = Rand(n, 234); c.pedType = Rand(n, 235) % 2 ? 7u : 6u; break;
    case 13: {  // tipos de peaton alrededor de 7 (la comparacion es de 32 bits)
        static constexpr u32 ts[]{7u, 6u, 8u, 0u, 1u, 0x107u, 0x80000007u, 0xFFFFFFFFu, 0x700u, 0x17u, 0x7u, 0x10000007u, 0xFFFFFF07u, 3u, 12u, 0x7FFFFFFFu};
        c.pedType = ts[Rand(n, 236) % std::size(ts)]; break; }
    case 14: {  // camara sin matriz y rumbos especiales
        c.cHasM = 0;
        static constexpr float hs[]{0.0f, -0.0f, 1.5707964f, 3.1415927f, -3.1415927f, 6.2831855f, 1e5f, 1e10f, -1e10f, 1e8f, 0.5f, 100.0f, 1.0f};
        c.cHeading = B(hs[Rand(n, 237) % std::size(hs)]);
        c.pedType = Rand(n, 238) % 2 ? 7u : 5u; break; }
    default:   // 15: mezcla con especiales poco frecuentes
        for (unsigned v = 0; v < 7; ++v) for (int i = 0; i < 3; ++i) if (Rand(n, 330 + i + 3 * v) % 40 == 0) Vecs(c)[v][i] = SpecialF(n, 360 + i + 3 * v);
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("has_target", c.hasTarget).U("health", c.health).U("max_health", c.maxHealth).U("ped_type", c.pedType);
    j.U("p_has_m", c.pHasM).U("t_has_m", c.tHasM).U("c_has_m", c.cHasM);
    for (unsigned i = 0; i < 3; ++i) {
        const std::string s = std::to_string(i);
        j.U(("p_pl" + s).c_str(), c.pPl[i]).U(("p_m" + s).c_str(), c.pM[i]).U(("t_pl" + s).c_str(), c.tPl[i]).U(("t_m" + s).c_str(), c.tM[i]);
        j.U(("c_pl" + s).c_str(), c.cPl[i]).U(("c_m" + s).c_str(), c.cM[i]).U(("c_right" + s).c_str(), c.cRight[i]);
    }
    j.U("c_heading", c.cHeading).U("xform", c.xform).U("rs_ret", c.rsRet);
    return j.End();
}
}
