#pragma once
// Fixtures del banco Fn116A (lote 116A): 18 funciones x 16384 casos (294912 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CBoundingBox::IsPointWithin             0x49C300  thiscall (ECX = caja; &punto en pila), `ret 4`; bool en AL                                          (hook S)
//   1 CColBox::Set                            0x40EE70  thiscall (ECX = caja; &sup, &inf, material, pieza, iluminacion en pila), `ret 14h`; void            (hook S)
//   2 CColBox::operator=                      0x40EEC0  thiscall (ECX = destino; &origen en pila), `ret 4`; devuelve this en EAX                            (hook S)
//   3 tColLighting::GetCurrentLighting        0x59F0C0  cdecl (byte de iluminacion como dword, factor float), float en ST0 sin guardar                      (hook S)
//   4 CPad::sub_540530                        0x540530  thiscall (ECX = pad), bool en AL                                                                   (hook S)
//   5 FightHitLevel                           0x5BD360  cdecl (char*), nivel en AL                                                                         (hook S)
//   6 FightHitSound                           0x5BD3B0  cdecl (int), sonido en EAX                                                                         (hook S)
//   7 approxEqual                             0x4EEA80  cdecl (f1, f2, epsilon), bool en AL                                                                (hook S)
//   8 ConvertPedNode2BoneTag                  0x4D58A0  cdecl (int), tag en EAX                                                                            (hook S)
//   9 CStuckCarCheck::ResetArrayElement       0x463970  thiscall (ECX = lista; indice en pila), `ret 4`; void                                              (hook S)
//  10 CEventDamage::ComputeHeadShot           0x4ADD60  thiscall (ECX = evento; &bool en pila), `ret 4`; void                                              (hook S)
//  11 FindCutsceneAudioTrackId                0x5AFA50  cdecl (char*), id en AX                                                                            (hook S)
//  12 CSimpleTransform::Invert                0x54EF90  thiscall (ECX = destino; &base en pila), `ret 4`; void                                             (hook S)
//  13 TransformVectors (CSimpleTransform)     0x54EE30  cdecl (salida, cuenta, &transformacion, entrada); void                                             (hook S)
//  14 CGarages::IsPointWithinAnyGarage        0x448990  cdecl (&punto), bool en AL                                                                         (hook S)
//  15 CGarages::IsPointInAGarageCameraZone    0x449BA0  cdecl (x, y, z por valor), bool en AL                                                              (hook S)
//  16 IsTeleportPointValid                    0x43EA90  cdecl (&origen, &destino), bool en AL                                                              (hook S)
//  17 COnscreenTimer::Init                    0x44CBC0  thiscall (ECX = temporizador), void                                                                (hook S)
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

namespace F116AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 18;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kObjWin = 0x260, kA1Win = 0x40, kA2Win = 0x40, kInWin = 0x80, kOutWin = 0x80, kStrWin = 0x60;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 116001u); }
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
// NaN silenciosos (carga util distinta) y senalizantes
inline constexpr std::array<u32, 8> kNans{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7FC12345u, 0xFFFFFFFFu};
inline u32 Nan(Gen& g) { return kNans[g.R(8)]; }
// Un float "especial" sin NaN: cero, desnormal, infinito, enorme o finito normal
inline u32 SpecialNoNan(Gen& g) {
    const u32 r = g.R(28);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 22) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 Special(Gen& g) { return g.P(25) ? Nan(g) : SpecialNoNan(g); }
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInf(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }
inline u32 Upper(u32 b) {   // siguiente float hacia +inf
    if (b == 0x80000000u) return 1u;
    if ((b >> 31) == 0) return b + 1;
    return b - 1;
}
inline u32 Lower(u32 b) {   // siguiente float hacia -inf
    if (b == 0u) return 0x80000001u;
    if ((b >> 31) == 0) return b - 1;
    return b + 1;
}
inline u32 Raw(Gen& g) { return g.P(60) ? g.W() : Special(g); }   // palabra cruda: cualquier patron

// ------------------------------------------------------------------------------------------------ funcion 0: CBoundingBox::IsPointWithin (0x49C300)
//   a[0..2] = minimo (x, y, z), a[3..5] = maximo, a[6..8] = punto
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 3; ++k) {
        const float lo = g.Fl(-50.0f, 0.0f), hi = g.Fl(0.0f, 50.0f);
        c.a[k] = B(lo); c.a[3 + k] = B(hi); c.a[6 + k] = B(g.Fl(lo, hi));
    }
    const u32 k = g.R(3);
    switch (f) {
    case 0: break;
    case 1: for (u32 j = 0; j < 3; ++j) c.a[6 + j] = c.a[j]; break;                                  // sobre el minimo
    case 2: for (u32 j = 0; j < 3; ++j) c.a[6 + j] = c.a[3 + j]; break;                              // sobre el maximo
    case 3: c.a[6 + k] = Lower(c.a[k]); break;                                                      // un ulp por debajo del minimo
    case 4: c.a[6 + k] = Upper(c.a[3 + k]); break;                                                  // un ulp por encima del maximo
    case 5: c.a[6 + k] = Nan(g); break;                                                              // NaN en el punto
    case 6: c.a[g.P(50) ? k : 3 + k] = Nan(g); break;                                                // NaN en un extremo
    case 7: for (u32 j = 0; j < 9; ++j) if (g.P(35)) c.a[j] = kInf[g.R(4)]; break;                  // infinitos
    case 8: for (u32 j = 0; j < 9; ++j) c.a[j] = g.P(50) ? 0u : 0x80000000u; break;                 // ceros con signo
    case 9: for (u32 j = 0; j < 3; ++j) std::swap(c.a[j], c.a[3 + j]); break;                       // caja invertida
    case 10: for (u32 j = 0; j < 9; ++j) c.a[j] = kTiny[g.R(12)]; break;                            // desnormales y ceros
    case 11: for (u32 j = 0; j < 9; ++j) c.a[j] = g.W(); break;                                     // patrones libres
    case 12: for (u32 j = 0; j < 9; ++j) c.a[j] = j < 3 ? Lower(kBig[g.R(6)]) : (j < 6 ? Upper(kBig[g.R(6)]) : kBig[g.R(6)]); break;
    case 13: for (u32 j = 0; j < 3; ++j) c.a[6 + j] = g.P(50) ? c.a[j] : c.a[3 + j]; break;          // en la frontera, cada eje en un extremo
    case 14: for (u32 j = 0; j < 3; ++j) { c.a[3 + j] = c.a[j]; c.a[6 + j] = g.P(40) ? c.a[j] : (g.P(50) ? Upper(c.a[j]) : Lower(c.a[j])); } break;   // caja degenerada
    default: for (u32 j = 0; j < 9; ++j) if (g.P(25)) c.a[j] = Special(g); break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 1: CColBox::Set (0x40EE70)
//   a[0..2] = sup, a[3..5] = inf (dwords crudos), a[6] = material, a[7] = pieza, a[8] = iluminacion (dwords; el EXE guarda solo el byte bajo), a[9] = modo de alias (bit 0: sup apunta dentro del objeto; bit 1: inf),
//   a[10] = desplazamiento (con signo, multiplo de 4) de sup respecto al objeto, a[11] = el de inf
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 6; ++k) c.a[k] = g.P(50) ? g.W() : Raw(g);
    c.a[6] = g.R(256); c.a[7] = g.R(256); c.a[8] = g.R(256);
    c.a[9] = 0; c.a[10] = u32(4 * (int(g.R(13)) - 4)); c.a[11] = u32(4 * (int(g.R(13)) - 4));
    switch (f) {
    case 0: break;
    case 1: c.a[6] = g.W(); c.a[7] = g.W(); c.a[8] = g.W(); break;                                   // basura en los bytes altos
    case 2: for (u32 k = 0; k < 6; ++k) c.a[k] = Special(g); break;
    case 3: c.a[9] = 1; break;
    case 4: c.a[9] = 2; break;
    case 5: c.a[9] = 3; break;
    case 6: c.a[9] = 1; c.a[10] = 0; break;                                                          // sup == this
    case 7: c.a[9] = 2; c.a[11] = 0; break;                                                          // inf == this
    case 8: c.a[9] = 1; c.a[10] = 0xC; break;                                                        // sup = this + 0xC (area de inf)
    case 9: c.a[9] = 2; c.a[11] = 4; break;
    case 10: c.a[9] = 3; c.a[10] = 0xC; c.a[11] = 0; break;                                          // sup e inf intercambiados
    case 11: c.a[6] = g.Pk({0u, 0xFFu, 0x80u, 0x7Fu}); c.a[7] = g.Pk({0u, 0xFFu, 0x80u, 0x7Fu}); c.a[8] = g.Pk({0u, 0xFFu, 0x80u, 0x7Fu}); break;
    case 12: c.a[9] = 3; c.a[10] = u32(-4 * int(1 + g.R(2))); c.a[11] = 4 * g.R(8); break;
    case 13: for (u32 k = 0; k < 6; ++k) c.a[k] = kNans[g.R(8)]; break;
    case 14: c.a[9] = g.R(4); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 2: CColBox::operator= (0x40EEC0)
//   a[0] = el origen esta dentro del objeto (0 = arena aparte), a[1] = desplazamiento (con signo) del origen respecto al objeto
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(30); c.a[1] = u32(int(g.R(73)) - 36);
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 1; c.a[1] = 0; break;
    case 2: c.a[0] = 1; c.a[1] = 4; break;
    case 3: c.a[0] = 1; c.a[1] = u32(-4); break;
    case 4: c.a[0] = 1; c.a[1] = 0xC; break;
    case 5: c.a[0] = 1; c.a[1] = u32(-0xC); break;
    case 6: c.a[0] = 1; c.a[1] = 0x18; break;
    case 7: c.a[0] = 1; c.a[1] = u32(-0x18); break;
    case 8: c.a[0] = 1; c.a[1] = 2; break;
    case 9: c.a[0] = 1; c.a[1] = 1; break;
    case 10: c.a[0] = 1; c.a[1] = u32(-1); break;
    case 11: c.a[0] = 1; c.a[1] = 0x1A; break;
    case 12: c.a[0] = 1; c.a[1] = 0x10; break;
    case 13: c.a[0] = 1; c.a[1] = u32(-0x10); break;
    case 14: c.a[0] = 1; break;
    default: c.a[0] = 0; break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 3: tColLighting::GetCurrentLighting (0x59F0C0)
//   a[0] = dword de iluminacion (el EXE usa el byte bajo: dia = nibble bajo, noche = nibble alto), a[1] = factor, a[2] = balance dia/noche (dword en 0x8D12C0)
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.P(70) ? g.R(256) : g.W();
    c.a[1] = g.P(60) ? 0x3F000000u : B(g.Fl(0.0f, 4.0f));
    c.a[2] = B(g.Fl(0.0f, 1.0f));
    switch (f) {
    case 0: break;
    case 1: c.a[2] = 0u; break;
    case 2: c.a[2] = 0x3F800000u; break;
    case 3: c.a[1] = B(g.Fl(-1000.0f, 1000.0f)); c.a[2] = B(g.Fl(-2.0f, 3.0f)); break;
    case 4: c.a[1] = Special(g); break;
    case 5: c.a[2] = Special(g); if (g.P(50)) c.a[1] = B(g.Fl(0.1f, 10.0f)); break;
    case 6: c.a[0] = g.P(50) ? 0u : (g.W() & 0xFFFFFF00u); break;
    case 7: c.a[0] = g.P(50) ? 0xFFu : (g.W() | 0xFFu); break;
    case 8: c.a[0] = g.W(); break;
    case 9: c.a[1] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0x7E800000u, 0xFE800000u}); break;
    case 10: c.a[1] = g.Pk({1u, 0x007FFFFFu, 0x00000100u, 0x00800000u, 0x80000001u, 0x807FFFFFu}); break;
    case 11: c.a[2] = B(g.Fl(-10.0f, 10.0f)); break;
    case 12: c.a[1] = B(-g.Fl(0.0f, 8.0f)); break;
    case 13: c.a[2] = Nan(g); c.a[1] = B(g.Fl(0.1f, 4.0f)); break;
    case 14: c.a[1] = g.W(); c.a[2] = g.W(); break;
    default: c.a[1] = Special(g); c.a[2] = Special(g); break;
    }
    // El modelo no resuelve la eleccion entre dos NaN distintos: un NaN en el balance exige factor finito (0 * inf del EXE mezclaria dos NaN) y los dos no pueden ser NaN a la vez
    if (IsNan(c.a[1]) && IsNan(c.a[2])) c.a[2] = B(g.Fl(0.0f, 1.0f));
    if (IsNan(c.a[2]) && !std::isfinite(F(c.a[1]))) c.a[1] = B(g.Fl(0.0f, 4.0f));
}

// ------------------------------------------------------------------------------------------------ funcion 4: CPad::sub_540530 (0x540530)
//   a[0] = modo (palabra en +0x10A), a[1] = palabra +0x10, a[2] = +0x12, a[3] = +0x1A, a[4] = +0x40, a[5] = +0x42, a[6] = +0x4A
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    const auto w = [&] { return g.P(50) ? 0u : (g.P(50) ? g.R(2) : g.R(65536)); };
    c.a[0] = g.R(4);
    for (u32 k = 1; k < 7; ++k) c.a[k] = w();
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 1; break;
    case 2: c.a[0] = 2; break;
    case 3: c.a[0] = 3; break;
    case 4: c.a[0] = 4 + g.R(100); break;
    case 5: c.a[0] = g.Pk({0x7FFFu, 0x8000u, 0xFFFFu, 0x100u, 0x101u, 0x4000u}); break;
    case 6: c.a[0] = g.R(4); c.a[3] = 1 + g.R(65535); c.a[6] = 0; c.a[1] = 1 + g.R(65535); c.a[4] = 0; break;   // seleccion nueva / flecha arriba nueva
    case 7: c.a[0] = g.R(4); c.a[2] = 1 + g.R(65535); c.a[5] = 0; break;                                        // flecha abajo nueva
    case 8: c.a[0] = 1; c.a[1] = 0; c.a[2] = 1; c.a[5] = 0; break;
    case 9: c.a[0] = 1; c.a[1] = 1; c.a[4] = 1; c.a[2] = 1; c.a[5] = 0; break;
    case 10: c.a[0] = 0; c.a[3] = 1; c.a[6] = 1; break;
    case 11: for (u32 k = 1; k < 7; ++k) c.a[k] = 0; break;
    case 12: for (u32 k = 1; k < 7; ++k) c.a[k] = 0x8000u; break;
    case 13: for (u32 k = 1; k < 7; ++k) c.a[k] = g.Pk({0x100u, 0x1u, 0xFF00u, 0x0u}); break;
    case 14: c.a[0] = g.R(4); for (u32 k = 1; k < 7; ++k) c.a[k] = g.R(2); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 5: FightHitLevel (0x5BD360)
//   a[0..2] = los tres primeros caracteres del texto (bytes), a[3] = 0 (terminador), a[4] = desplazamiento de la cadena dentro de la arena
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    static constexpr char kLetters[]{'H', 'L', 'G', 'B'};
    c.a[0] = g.W() & 0xFFu; c.a[1] = g.W() & 0xFFu; c.a[2] = g.W() & 0xFFu; c.a[3] = 0; c.a[4] = g.R(8);
    switch (f) {
    case 0: case 1: case 2: case 3: c.a[0] = u32(kLetters[f]); c.a[1] = g.P(50) ? 0u : g.R(256); break;
    case 4: c.a[0] = u32(kLetters[g.R(4)]); c.a[1] = u32(kLetters[g.R(4)]); break;                        // pares como "HL", "LL", "GL" del EXE
    case 5: c.a[0] = u32("hlgb"[g.R(4)]); break;                                                          // minusculas
    case 6: c.a[0] = u32('A' + g.R(26)); break;
    case 7: c.a[0] = 0; break;
    case 8: c.a[0] = g.Pk({0x80u, 0xFFu, 0x48u | 0x80u, 0xCCu, 0x4Cu | 0x80u}); break;
    case 9: c.a[0] = 0x48u; c.a[1] = 0x4Cu; break;                                                         // "HL"
    case 10: c.a[0] = 0x4Cu; c.a[1] = 0x4Cu; break;                                                        // "LL"
    case 11: c.a[0] = 0x47u; c.a[1] = 0x4Cu; break;                                                        // "GL"
    case 12: c.a[0] = u32(kLetters[g.R(4)]); c.a[1] = g.W() & 0xFFu; c.a[2] = g.W() & 0xFFu; break;
    case 13: c.a[0] = 0x42u + g.R(8); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 6: FightHitSound (0x5BD3B0)
//   a[0] = n (int32)
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(12);
    switch (f) {
    case 0: c.a[0] = 1 + g.R(8); break;
    case 1: c.a[0] = g.R(3); break;
    case 2: c.a[0] = 9 + g.R(4); break;
    case 3: c.a[0] = u32(-int(g.R(10))); break;
    case 4: c.a[0] = g.Pk({0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000001u, 0x7FFFFFFEu}); break;
    case 5: c.a[0] = g.W(); break;
    case 6: c.a[0] = 0x100u + g.R(10); break;
    case 7: c.a[0] = g.Pk({0u, 1u, 8u, 9u, 0xFFFFFFFFu}); break;
    case 8: c.a[0] = 2; break;
    case 9: c.a[0] = u32(-1 - int(g.R(100))); break;
    case 10: c.a[0] = 1u << g.R(32); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 7: approxEqual (0x4EEA80)
//   a[0] = f1, a[1] = f2, a[2] = epsilon
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    const float x = g.Fl(-100.0f, 100.0f);
    c.a[0] = B(x); c.a[1] = B(x + g.Fl(-0.1f, 0.1f)); c.a[2] = B(g.Fl(0.0f, 0.1f));
    switch (f) {
    case 0: break;
    case 1: c.a[1] = c.a[0]; break;                                                                    // iguales
    case 2: c.a[1] = c.a[0]; c.a[2] = 0u; break;                                                       // iguales con epsilon cero
    case 3: c.a[1] = Upper(c.a[0]); c.a[2] = B(F(c.a[1]) - F(c.a[0])); break;                           // diferencia igual al epsilon
    case 4: c.a[1] = Upper(c.a[0]); c.a[2] = Upper(B(F(c.a[1]) - F(c.a[0]))); break;                    // diferencia apenas menor
    case 5: c.a[0] = Special(g); break;
    case 6: c.a[1] = Special(g); break;
    case 7: c.a[2] = Special(g); break;
    case 8: c.a[0] = 0x7F7FFFFFu; c.a[1] = 0xFF7FFFFFu; c.a[2] = g.Pk({0x7F800000u, 0x7F7FFFFFu, 0x7F000000u}); break;       // desborda el float, no la x87
    case 9: c.a[0] = kInf[g.R(4)]; c.a[1] = g.P(50) ? c.a[0] : B(g.Fl(-10.0f, 10.0f)); c.a[2] = g.P(50) ? 0x7F800000u : B(g.Fl(0.0f, 1.0f)); break;
    case 10: c.a[0] = kTiny[g.R(12)]; c.a[1] = kTiny[g.R(12)]; c.a[2] = kTiny[g.R(12)]; break;
    case 11: c.a[0] = g.W(); c.a[1] = g.W(); c.a[2] = g.W(); break;
    case 12: c.a[0] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u}); c.a[1] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0u}); c.a[2] = g.Pk({0x7F800000u, 0x7F7FFFFFu, 0x7F000000u, 0x3F800000u}); break;
    case 13: c.a[2] = B(-g.Fl(0.0f, 1.0f)); break;                                                    // epsilon negativo
    case 14: c.a[0] = 0u; c.a[1] = 0x80000000u; c.a[2] = g.Pk({0u, 0x80000000u, 1u, 0x3F800000u}); break;   // ceros con signo
    default: for (u32 k = 0; k < 3; ++k) if (g.P(30)) c.a[k] = Special(g); break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 8: ConvertPedNode2BoneTag (0x4D58A0)
//   a[0] = nodo (int32)
inline void Make8(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(24);
    switch (f) {
    case 0: c.a[0] = 1 + g.R(18); break;
    case 1: c.a[0] = g.R(2) ? 1u : 18u; break;
    case 2: c.a[0] = 0; break;
    case 3: c.a[0] = 19 + g.R(5); break;
    case 4: c.a[0] = u32(-int(g.R(20))); break;
    case 5: c.a[0] = g.Pk({0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000001u, 0x00000001u}); break;
    case 6: c.a[0] = g.W(); break;
    case 7: c.a[0] = 1 + g.R(18); break;
    case 8: c.a[0] = 0x100u + 1 + g.R(18); break;
    case 9: c.a[0] = 0x10000u + 1 + g.R(18); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 9: CStuckCarCheck::ResetArrayElement (0x463970)
//   a[0] = indice (dword; el EXE usa la palabra baja; valido 0..15)
inline void Make9(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(16);
    switch (f) {
    case 0: c.a[0] = 0; break;
    case 1: c.a[0] = 15; break;
    case 2: c.a[0] = g.R(16) | (g.W() & 0xFFFF0000u); break;                                           // basura en la palabra alta
    case 3: c.a[0] = g.R(16) | 0xFFFF0000u; break;
    case 4: c.a[0] = 7 + g.R(2); break;
    case 5: c.a[0] = g.R(4); break;
    case 6: c.a[0] = 12 + g.R(4); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 10: CEventDamage::ComputeHeadShot (0x4ADD60)
//   a[0] = arma (+0x1C, dword), a[1] = pieza (+0x20, dword), a[2] = byte +0x41 (muerte forzada), a[3] = valor inicial del byte de salida
inline void Make10(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(64); c.a[1] = g.P(30) ? 9u : g.R(20); c.a[2] = g.P(50) ? g.R(2) : g.R(256); c.a[3] = g.R(256);
    switch (f) {
    case 0: c.a[0] = g.R(16); break;                                                                   // cuerpo a cuerpo
    case 1: c.a[0] = 22 + g.R(13); break;                                                              // armas de fuego
    case 2: c.a[0] = g.Pk({38u, 46u, 52u, 0x34u, 0x35u, 0x33u}); break;                                // fronteras de la tabla
    case 3: c.a[0] = 0x35u + g.R(100); break;                                                          // fuera de la tabla
    case 4: c.a[0] = g.W(); break;
    case 5: c.a[0] = g.Pk({0x80000000u, 0xFFFFFFFFu, 0x100u, 0x134u, 0x7FFFFFFFu}); break;
    case 6: c.a[1] = 9; c.a[2] = 1 + g.R(255); break;
    case 7: c.a[1] = 9; c.a[2] = 0; break;
    case 8: c.a[1] = g.Pk({0u, 8u, 10u, 0x109u, 0x80000009u, 0xFFFFFFFFu}); c.a[2] = 1; break;
    case 9: c.a[0] = 16 + g.R(6); c.a[2] = 1; break;                                                   // clase sin efecto
    case 10: c.a[0] = g.R(0x35); c.a[1] = 9; c.a[2] = 1; break;
    case 11: c.a[0] = g.R(0x35); c.a[1] = 9; c.a[2] = g.Pk({2u, 0x80u, 0xFFu, 0x100u & 0xFFu}); break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 11: FindCutsceneAudioTrackId (0x5AFA50)
//   a[0] = longitud de la cadena (0..23), a[1] = tipo de caso, a[2] = indice de la tabla del nombre base, a[4..9] = bytes de la cadena empaquetados (cuatro por dword; el byte siguiente a la longitud es 0)
inline constexpr const char* kCutNames[151]{"BCESAR2", "BCESAR4", "BCESA4W", "BCESAR5", "BCESA5W", "BCRAS1", "BCRAS2", "BHILL1", "BHILL2", "BHILL3a", "BHILL3b", "BHILL3c", "BHILL5a", "BHILL5b", "CAS_1a", "CAS_1b", "CAS_2", "CAS_3", "CAS_4a", "CAS_4b", "CAS_4c", "CAS_5a", "CAS_6a", "CAS6b_1", "CAS6b_2", "CAS_7b", "CAS_9a1", "CAS_9a2", "CAS_11a", "CAT_1", "CAT_2", "CAT_3", "CAT_4", "CESAR1A", "CESAR2A", "CRASH1A", "CRASH2A", "CRASH3A", "CRASHV1", "CRASv2a", "CRASv2b", "D10_ALT", "D8_ALT", "DESERT1", "DESERT2", "DESERT3", "DESERT4", "DESERT6", "DESERT8", "DESERT9", "DES_10A", "DES_10B", "DOC_2", "EPILOG", "FARL_2A", "FARL_3A", "FARL_3B", "FARL_4A", "FARL_5A", "FINAL1A", "FINAL2A", "FINAL2B", "GARAG1B", "GARAG1C", "GARAG3A", "GROVE1a", "GROVE1b", "GROVE1c", "GROVE2", "HEIST1a", "HEIST2a", "HEIST4a", "HEIST5a", "HEIST6a", "HEIST8a", "INTRO1A", "INTRO1B", "INTRO2A", "PROLOG1", "PROLOG2", "PROLOG3", "RIOT_1a", "RIOT_1b", "RIOT_2", "RIOT_4a", "RIOT_4b", "RIOT_4c", "RIOT_4d", "RIOT4e1", "RIOT4e2", "RYDER1A", "RYDER2A", "RYDER3A", "SCRASH1", "SCRASH2", "SMOKE1A", "SMOKE1B", "SMOKE2A", "SMOKE2B", "SMOKE3A", "SMOKE4A", "STEAL_1", "STEAL_2", "STEAL_4", "STEAL_5", "STRAP1A", "STRAP2A", "STRAP3A", "STRAP4A", "STRP4B1", "STRP4B2", "SWEET1A", "SWEET1B", "SWEET1C", "SWEET2A", "SWEET2B", "SWEET3A", "SWEET3B", "SWEET4A", "SWEET5A", "SWEET6A", "SWEET6B", "SWEET7A", "SYND_2A", "SYND_2B", "SYND_3A", "SYND_4A", "SYND_4B", "SYND_7", "TRUTH_1", "TRUTH_2", "W2_ALT", "WOOZI1A", "WOOZI1B", "WOOZIE2", "WOOZIE4", "ZERO_1", "ZERO_2", "ZERO_4", "DATE1a", "DATE1b", "DATE2a", "DATE2b", "DATE3a", "DATE3b", "DATE4a", "DATE4b", "DATE5a", "DATE5b", "DATE6a", "DATE6b"};
inline void Make11(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    std::string s;
    const u32 idx = g.R(151);
    c.a[2] = idx;
    const std::string base = kCutNames[idx];
    u32 kind = f % 8;
    if (f >= 8) kind = g.R(8);
    c.a[1] = kind;
    switch (kind) {
    case 0: s = base; break;                                                                           // nombre exacto
    case 1: s = base; for (auto& ch : s) if (g.P(50)) ch = char((ch >= 'a' && ch <= 'z') ? ch - 32 : (ch >= 'A' && ch <= 'Z' ? ch + 32 : ch)); break;   // otro uso de mayusculas
    case 2: s = base.substr(0, g.R(unsigned(base.size()) + 1)); break;                                 // prefijo
    case 3: s = base + char('A' + g.R(26)); break;                                                     // nombre mas un caracter
    case 4: { const u32 len = g.R(12); for (u32 k = 0; k < len; ++k) s += char(1 + g.R(255)); break; } // bytes cualesquiera (sin 0)
    case 5: break;                                                                                     // vacia
    case 6: s = base; s[g.R(unsigned(s.size()))] = char(1 + g.R(255)); break;                          // un caracter cambiado (a veces el mismo)
    default: { s = base; const u32 p = g.R(unsigned(s.size())); s[p] = char(s[p] ^ 0x20); if (g.P(50)) s[p] = char(s[p] ^ 0x20); break; } // caja de una letra
    }
    if (s.size() > 23) s.resize(23);
    c.a[0] = u32(s.size());
    for (u32 k = 0; k < 6; ++k) {
        u32 w = 0;
        for (u32 j = 0; j < 4; ++j) {
            const size_t at = k * 4 + j;
            const u32 byte = at < s.size() ? u32(u8(s[at])) : 0u;
            w |= byte << (8 * j);
        }
        c.a[4 + k] = w;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 12: CSimpleTransform::Invert (0x54EF90)
//   a[0..2] = base x, y, z (dwords crudos), a[3] = base rumbo, a[4] = la base es el propio objeto (alias; entonces a[0..3] son los valores iniciales del objeto, el resto del objeto es relleno)
inline u32 HeadingFinite(Gen& g) { return g.P(80) ? B(g.Fl(-8.0f, 8.0f)) : g.Pk({0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x3F490FDBu, 0xBFC90FDBu, 0x40490FDBu, 0xC0490FDBu}); }
inline void Make12(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 3; ++k) c.a[k] = g.P(70) ? B(g.Fl(-1000.0f, 1000.0f)) : g.W();
    c.a[3] = HeadingFinite(g); c.a[4] = g.P(10);
    switch (f) {
    case 0: for (u32 k = 0; k < 3; ++k) c.a[k] = B(g.Fl(-100.0f, 100.0f)); break;
    case 1: c.a[4] = 1; break;
    case 2: c.a[g.R(3)] = SpecialNoNan(g); break;
    case 3: c.a[0] = Nan(g); break;
    case 4: c.a[1] = Nan(g); break;
    case 5: c.a[2] = Nan(g); break;                                                                    // z: la negacion silencia los NaN senalizantes
    case 6: c.a[3] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFFC12345u}); for (u32 k = 0; k < 3; ++k) c.a[k] = B(g.Fl(-10.0f, 10.0f)); break;   // rumbo no finito
    case 7: c.a[3] = Nan(g); for (u32 k = 0; k < 3; ++k) c.a[k] = g.P(80) ? B(g.Fl(-10.0f, 10.0f)) : kInf[g.R(4)]; break;
    case 8: c.a[3] = kTiny[g.R(12)]; break;
    case 9: c.a[0] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u}); c.a[1] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u}); break;   // el exponente ampliado evita el desbordamiento intermedio
    case 10: c.a[0] = g.Pk({1u, 0x007FFFFFu, 0x00800000u}); c.a[1] = g.Pk({1u, 0x80000001u, 0x00800000u}); break;
    case 11: c.a[3] = B(g.Fl(-3.2f, 3.2f)); c.a[4] = g.P(30); break;
    case 12: c.a[0] = kInf[g.R(4)]; c.a[1] = g.P(50) ? kInf[g.R(4)] : B(g.Fl(-10.0f, 10.0f)); break;
    case 13: c.a[3] = 0; c.a[0] = g.W(); c.a[1] = g.W(); break;
    case 14: c.a[4] = 1; c.a[g.R(3)] = SpecialNoNan(g); break;
    default: break;
    }
    // x e y no pueden ser NaN a la vez (el modelo no elige entre dos NaN); tampoco NaN y rumbo no finito
    if (IsNan(c.a[0]) && IsNan(c.a[1])) c.a[1] = B(g.Fl(-10.0f, 10.0f));
    if (!std::isfinite(F(c.a[3])) && (IsNan(c.a[0]) || IsNan(c.a[1]))) { c.a[0] = B(g.Fl(-10.0f, 10.0f)); c.a[1] = B(g.Fl(-10.0f, 10.0f)); }
}

// ------------------------------------------------------------------------------------------------ funcion 13: TransformVectors (CSimpleTransform) (0x54EE30)
//   a[0] = cuenta (0..6), a[1] = rumbo, a[2] = la salida es la entrada (alias), a[4 .. 4 + 3 * cuenta) = vectores de entrada (dwords crudos)
inline void Make13(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = g.R(7); c.a[1] = HeadingFinite(g); c.a[2] = g.P(15);
    for (u32 k = 0; k < 18; ++k) c.a[4 + k] = g.P(75) ? B(g.Fl(-1000.0f, 1000.0f)) : g.W();
    switch (f) {
    case 0: for (u32 k = 0; k < 18; ++k) c.a[4 + k] = B(g.Fl(-100.0f, 100.0f)); break;
    case 1: c.a[0] = 0; break;
    case 2: c.a[0] = 1; break;
    case 3: c.a[2] = 1; c.a[0] = 1 + g.R(6); break;
    case 4: for (u32 k = 0; k < 18; ++k) if (k % 3 != 2 && g.P(25)) c.a[4 + k] = SpecialNoNan(g); break;
    case 5: c.a[0] = 1 + g.R(6); c.a[4 + 3 * g.R(c.a[0]) + g.R(3)] = Nan(g); break;                      // un NaN (en x, y o z de un vector)
    case 6: c.a[1] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7FC00000u, 0x7F800001u, 0xFFC12345u}); c.a[0] = 1 + g.R(6); for (u32 k = 0; k < 18; ++k) c.a[4 + k] = g.P(80) ? B(g.Fl(-10.0f, 10.0f)) : kInf[g.R(4)]; break;
    case 7: c.a[1] = kTiny[g.R(12)]; break;
    case 8: c.a[0] = 1 + g.R(6); for (u32 k = 0; k < 18; ++k) c.a[4 + k] = g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0x3F800000u}); break;
    case 9: c.a[0] = 1 + g.R(6); for (u32 k = 0; k < 18; ++k) c.a[4 + k] = g.Pk({1u, 0x007FFFFFu, 0x00800000u, 0x80000001u, 0x3F800000u}); break;
    case 10: c.a[1] = B(g.Fl(-3.2f, 3.2f)); c.a[2] = g.P(30); break;
    case 11: c.a[0] = 6; c.a[2] = g.P(50); break;
    case 12: for (u32 k = 0; k < 18; ++k) c.a[4 + k] = g.W(); break;
    case 13: c.a[1] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0x3FC90FDBu}); c.a[0] = 1 + g.R(6); break;
    case 14: c.a[2] = 1; c.a[1] = B(g.Fl(-8.0f, 8.0f)); c.a[0] = 2 + g.R(5); break;
    default: break;
    }
    // un NaN en una componente que se combina con otro NaN no se modela: como mucho un NaN por vector en x o y, y rumbo finito si hay algun NaN
    for (u32 v = 0; v < 6; ++v) if (IsNan(c.a[4 + 3 * v]) && IsNan(c.a[5 + 3 * v])) c.a[5 + 3 * v] = B(g.Fl(-10.0f, 10.0f));
    if (!std::isfinite(F(c.a[1]))) for (u32 v = 0; v < 6; ++v) { if (IsNan(c.a[4 + 3 * v])) c.a[4 + 3 * v] = B(1.5f); if (IsNan(c.a[5 + 3 * v])) c.a[5 + 3 * v] = B(2.5f); }
}

// ------------------------------------------------------------------------------------------------ funciones 14 y 15: garajes
//   a[0..12] = bytes de tipo de los 50 garajes (cuatro por dword, el garaje k es el byte k; 0 = INVALID), a[13], a[14] = mascara de aciertos del colaborador (bit k de a[13] + a[14] << 32; k < 50),
//   a[15..17] = punto (dwords crudos), a[18] = basura de los bytes altos de EAX del colaborador
inline void MakeGarage(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 types[50]; u64 mask = 0;
    const u32 density = 5 + g.R(80);
    for (u32 k = 0; k < 50; ++k) types[k] = g.P(density) ? 1 + g.R(255) : 0u;
    const u32 pHit = g.Pk({0u, 2u, 10u, 50u, 100u});
    for (u32 k = 0; k < 50; ++k) if (g.P(pHit)) mask |= (1ull << k);
    switch (f) {
    case 0: for (u32 k = 0; k < 50; ++k) types[k] = 0; break;                                                  // ninguno activo: no llama
    case 1: for (u32 k = 0; k < 50; ++k) types[k] = 1 + g.R(255); mask = 0; break;                             // todos activos, ningun acierto
    case 2: for (u32 k = 0; k < 50; ++k) types[k] = 1 + g.R(255); mask = 1ull << g.R(50); break;              // todos activos, un acierto
    case 3: for (u32 k = 0; k < 50; ++k) types[k] = 0; types[g.R(50)] = 1 + g.R(255); mask = ~0ull; break;     // un garaje activo, acierta
    case 4: for (u32 k = 0; k < 50; ++k) types[k] = 0; types[49] = 1; mask = 1ull << 49; break;                // el ultimo
    case 5: for (u32 k = 0; k < 50; ++k) types[k] = 0; types[0] = 1; mask = 1; break;                          // el primero
    case 6: for (u32 k = 0; k < 50; ++k) types[k] = g.P(50) ? 0u : 1u; mask = ~0ull; break;                    // todo acierta: gana el primero activo
    case 7: for (u32 k = 0; k < 50; ++k) types[k] = g.P(50) ? 0u : (0x80u | g.R(128)); mask = g.W() | (u64(g.W()) << 32); break;   // tipos con el bit alto
    case 8: for (u32 k = 0; k < 50; ++k) types[k] = 0; for (u32 j = 0; j < 3; ++j) types[g.R(50)] = 1 + g.R(255); mask = (1ull << g.R(50)) | (1ull << g.R(50)); break;   // aciertos en garajes inactivos
    case 9: for (u32 k = 0; k < 50; ++k) types[k] = 0x16 + g.R(3); mask = g.W() | (u64(g.W()) << 32); break;      // tipos alrededor de 0x16
    case 10: for (u32 k = 0; k < 50; ++k) types[k] = k < 25 ? 0u : 1u; mask = 1ull << 25; break;
    case 11: for (u32 k = 0; k < 50; ++k) types[k] = g.R(3); mask = g.W() | (u64(g.W()) << 32); break;
    default: break;
    }
    mask &= (1ull << 50) - 1;
    for (u32 w = 0; w < 13; ++w) {
        u32 d = 0;
        for (u32 j = 0; j < 4; ++j) { const u32 k = w * 4 + j; if (k < 50) d |= (types[k] & 0xFFu) << (8 * j); }
        c.a[w] = d;
    }
    c.a[13] = u32(mask); c.a[14] = u32(mask >> 32);
    for (u32 k = 0; k < 3; ++k) c.a[15 + k] = g.P(80) ? B(g.Fl(-3000.0f, 3000.0f)) : Raw(g);
    c.a[18] = g.W() & 0xFFFFFF00u;
}

// ------------------------------------------------------------------------------------------------ funcion 16: IsTeleportPointValid (0x43EA90)
//   a[0..2] = origen, a[3..5] = destino (dwords crudos), a[6] = lo que devuelve TestSphereAgainstWorld (dword completo), a[7] = lo que devuelve GetIsLineOfSightClear (dword; solo cuenta AL)
inline void Make16(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 6; ++k) c.a[k] = g.P(80) ? B(g.Fl(-3000.0f, 3000.0f)) : Raw(g);
    c.a[6] = g.P(50) ? 0u : g.W(); c.a[7] = g.P(50) ? g.R(2) : g.W();
    switch (f) {
    case 0: c.a[6] = 0; c.a[7] = 1; break;
    case 1: c.a[6] = 0; c.a[7] = 0; break;
    case 2: c.a[6] = 0x00C0FFEEu; break;
    case 3: c.a[6] = g.Pk({0x80000000u, 0x00000100u, 1u, 0xFFFFFFFFu}); break;
    case 4: c.a[6] = 0; c.a[7] = g.Pk({0x100u, 0x80000000u, 0xFFFFFF00u, 2u, 0xFFu}); break;                // solo cuenta AL
    case 5: c.a[6] = 0; c.a[7] = g.W() | 1u; break;
    case 6: c.a[6] = 0; c.a[7] = g.W() & 0xFFFFFF00u; break;
    case 7: for (u32 k = 0; k < 6; ++k) c.a[k] = Special(g); break;
    case 8: for (u32 k = 0; k < 3; ++k) c.a[3 + k] = c.a[k]; c.a[6] = 0; break;                              // origen == destino (valores)
    case 9: c.a[6] = 0; for (u32 k = 0; k < 6; ++k) c.a[k] = kNans[g.R(8)]; break;
    default: break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 17: COnscreenTimer::Init (0x44CBC0)
//   sin parametros: el objeto lleva relleno pseudoaleatorio (arena de 0x260 bytes)
inline void Make17(C& c, unsigned n) {
    (void)c; (void)n;
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
    case 13: Make13(c, n); break;
    case 14: case 15: MakeGarage(c, n); break;
    case 16: Make16(c, n); break;
    default: Make17(c, n); break;
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
