#pragma once
// Fixtures del banco Fn98A (lote 98A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 FloatMax (CCarAI)                              0x420800  cdecl (a, b) -> float en ST0: a si a > b (ordenado), si no b                                    (hook S)
//   1 CCarPathLink::GetNodeCoors (a CVector)         0x420A60  thiscall (ECX = enlace; destino en pila), `ret 4`; escribe (x/8, y/8, 0) y devuelve el destino   (hook S)
//   2 CVehicleModelInfo::IncrementTimesUsed          0x421120  thiscall (ECX = info de modelo), byte +0x50 = min(int8 + 1, 0x78)                              (hook S)
//   3 CPlaceable::GetForwardVector                   0x41CCB0  thiscall (ECX = placeable; destino en pila), `ret 4`; devuelve el destino                       (hook S)
//   4 CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName 0x46CED0 thiscall (nombre, entidad, tipo), `ret 0Ch`; void                         (hook S)
//   5 CTheScripts::IsPointWithinSearchLight          0x493280  cdecl (posicion*, indice) -> bool en AL                                                         (hook S)
//   6 WinInput::diPadEnumerate                       0x7477C0  cdecl () -> int en EAX: EnumDevices (vtabla +0x10) y rangos de los dos mandos                  (hook S)
//   7 CCarCtrl::WeaveForPed                          0x426970  cdecl (entidad, vehiculo, &limite1, &limite2); void                                          (hook S)
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

namespace F98AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 8;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kSmallWin = 0x20, kLinkWin = 0x20, kOutWin = 0x10, kMiWin = 0x60, kPlcWin = 0x20, kMatWin = 0x40, kBrainsWin = 0x20, kStrWin = 0x20, kEntWin = 0x20, kPosWin = 0x10, kVehWin = 0x430, kSmallWin2 = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 98001u); }
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
    std::array<u32, 48> a{};
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
// NaN con cargas distintas (silenciosos y senalizantes)
inline constexpr std::array<u32, 10> kNans{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FA00000u, 0x7FC12345u, 0xFFA5A5A5u, 0x7F812345u};
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(Gen& g) {
    const u32 r = g.R(32);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float normal `steps` unidades de ultimo lugar (signo respetado)
    const bool neg = (bits >> 31) != 0;
    u32 mag = bits & 0x7FFFFFFFu;
    mag = u32(i32(mag) + (neg ? -steps : steps));
    return (bits & 0x80000000u) | (mag & 0x7FFFFFFFu);
}
// Un float de magnitud aleatoria (de 1e-30 a 1e30) con signo
inline u32 Wide(Gen& g) {
    const double e = double(int(g.R(61)) - 30);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, e)));
}
// Una coordenada de juego en metros, con signo
inline u32 Coord(Gen& g, float range = 3000.0f) { return B(g.Fl(-range, range)); }

// ------------------------------------------------------------------------------------------------ funcion 0: FloatMax (0x420800)
//   a[0] = a, a[1] = b (patrones de float)
inline constexpr std::array<u32, 16> kF0Special{0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x80800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u, 0xFF800000u, 0x3F800000u, 0xBF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFBFFFFFu};
inline u32 NanBits(Gen& g) { return g.P(50) ? kNans[g.R(10)] : (0x7F800000u | (g.W() & 0x807FFFFFu) | 1u); }
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 a, b;
    switch (f) {
    case 0: a = Coord(g, 100.0f); b = Coord(g, 100.0f); break;
    case 1: a = g.P(50) ? kF0Special[g.R(16)] : Coord(g, 1000.0f); b = a; break;                      // iguales (mismo patron)
    case 2: a = g.Pk({0u, 0x80000000u}); b = g.Pk({0u, 0x80000000u}); break;                          // ceros con signo
    case 3: {                                                                                          // NaN en uno o en ambos
        const u32 r = g.R(3);
        a = r != 1 ? NanBits(g) : Coord(g, 100.0f);
        b = r != 0 ? NanBits(g) : Coord(g, 100.0f);
        if (r == 2 && g.P(50)) b = a;
        break;
    }
    case 4: a = kInf[g.R(4)]; b = g.P(50) ? kInf[g.R(4)] : Coord(g, 1000.0f); if (g.P(50)) std::swap(a, b); break;
    case 5: a = Coord(g, 1000.0f); b = NextUp(a, int(g.R(7)) - 3); break;                              // a +- ulps
    case 6: a = kTiny[g.R(12)]; b = g.P(30) ? 0u : kTiny[g.R(12)]; if (g.P(50)) std::swap(a, b); break;
    case 7: a = g.P(70) ? kBig[g.R(6)] : Wide(g); b = g.P(70) ? kBig[g.R(6)] : Wide(g); break;
    case 8: { const float y = g.Fl(-1000.0f, 1000.0f); a = B(y + g.Fl(0.001f, 500.0f)); b = B(y); break; }     // a > b
    case 9: { const float y = g.Fl(-1000.0f, 1000.0f); b = B(y + g.Fl(0.001f, 500.0f)); a = B(y); break; }     // a < b
    case 10: a = B(-std::fabs(g.Fl(0.0f, 100.0f))); b = B(-std::fabs(g.Fl(0.0f, 100.0f))); break;
    case 11: a = kF0Special[g.R(16)]; b = kF0Special[g.R(16)]; break;
    case 12: a = g.W(); b = g.W(); break;
    case 13: a = B(float(int(g.R(41)) - 20)); b = B(float(int(g.R(41)) - 20)); break;
    case 14: a = NanBits(g); b = g.P(50) ? Wide(g) : Coord(g, 100.0f); if (g.P(50)) std::swap(a, b); break;
    default: a = Special(g); b = Special(g); break;
    }
    c.a[0] = a; c.a[1] = b;
}

// ------------------------------------------------------------------------------------------------ funcion 1: CCarPathLink::GetNodeCoors (0x420A60)
//   a[0] = x (int16, en los 16 bits bajos), a[1] = y (int16), a[2..4] = valor inicial del destino (x, y, z como patrones)
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 x, y;
    constexpr std::array<u32, 8> ext{0x7FFFu, 0x8000u, 0x8001u, 0xFFFFu, 1u, 0u, 0x4000u, 0xC000u};
    switch (f) {
    case 0: x = g.R(65536); y = g.R(65536); break;
    case 1: x = g.P(50) ? 0u : g.R(65536); y = g.P(50) ? 0u : g.R(65536); break;
    case 2: x = ext[g.R(8)]; y = ext[g.R(8)]; break;
    case 3: x = u32(int(g.R(33)) - 16) & 0xFFFFu; y = u32(int(g.R(33)) - 16) & 0xFFFFu; break;
    case 4: x = (1u << g.R(16)) ^ (g.P(50) ? 0xFFFFu : 0u); y = (1u << g.R(16)) ^ (g.P(50) ? 0xFFFFu : 0u); break;
    case 5: x = ext[g.R(8)]; y = g.R(65536); break;
    case 6: x = g.R(65536); y = ext[g.R(8)]; break;
    case 7: x = (u32(int(g.R(8192)) - 4096) * 8u) & 0xFFFFu; y = (u32(int(g.R(8192)) - 4096) * 8u) & 0xFFFFu; break;
    case 8: x = g.R(65536) | 1u; y = g.R(65536) | 1u; break;
    default: x = g.R(65536); y = g.R(65536); break;
    }
    c.a[0] = x; c.a[1] = y;
    for (u32 k = 0; k < 3; ++k) c.a[2 + k] = g.P(55) ? g.W() : g.Pk({0u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0x3F800000u});
}

// ------------------------------------------------------------------------------------------------ funcion 2: CVehicleModelInfo::IncrementTimesUsed (0x421120)
//   a[0] = byte +0x50 (m_nTimesUsed)
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 b;
    switch (f) {
    case 8: b = g.Pk({0x76u, 0x77u, 0x78u, 0x79u, 0x7Fu, 0x80u, 0xFFu, 0u, 1u, 0x7Eu, 0x81u, 0xF0u}); break;
    case 9: b = 0x78u + g.R(8); break;
    case 10: b = 0x80u + g.R(128); break;
    case 11: b = g.R(0x78); break;
    case 12: b = g.R(256); break;
    case 13: b = g.Pk({0x77u, 0x78u, 0xFFu, 0x7Fu}); break;
    case 14: b = g.Pk({0u, 1u, 2u, 0x7Fu, 0xFEu, 0xFFu, 0x80u}); break;
    case 15: b = g.R(256); break;
    default: b = (n / 16 * 8 + f) % 256; break;   // recorre todos los valores
    }
    c.a[0] = b;
}

// ------------------------------------------------------------------------------------------------ funcion 3: CPlaceable::GetForwardVector (0x41CCB0)
//   a[0] = el placeable tiene matriz (+0x14), a[1] = rumbo (+0x10), a[2..4] = fila adelante de la matriz (+0x10..+0x18 de la matriz), a[5..7] = valor inicial del destino
constexpr float kPi = 3.14159265358979f;
inline constexpr std::array<u32, 12> kHeadSpecial{0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x80800000u, 0x3F800000u, 0xBF800000u, 0x007FFFFFu, 0x2EDBE6FFu, 0xAEDBE6FFu, 0x3F490FDBu};
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 mat = 0, head;
    const auto fwd = [&](bool raw) {
        for (u32 k = 0; k < 3; ++k) c.a[2 + k] = raw ? (g.P(70) ? g.W() : Special(g)) : B(g.Fl(-1.0f, 1.0f));
    };
    head = B(g.Fl(-4.0f, 4.0f));
    fwd(false);
    switch (f) {
    case 0: break;
    case 1: mat = 1; break;
    case 2: head = kHeadSpecial[g.R(12)]; break;
    case 3: mat = 1; fwd(true); break;
    case 4: head = B(g.Fl(-8.0f, 8.0f)); break;
    case 5: head = g.Pk({0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x7F800001u, 0xFFA00000u, 0x7FFFFFFFu}); break;
    case 6: mat = 1; head = Special(g); fwd(g.P(50)); break;
    case 7: head = B(g.Fl(-1.0e-3f, 1.0e-3f)); break;
    case 8: head = NextUp(B(float(int(g.R(9)) - 4) * (kPi / 2.0f)), int(g.R(7)) - 3); break;      // cerca de multiplos de pi/2
    case 9: head = B(g.Fl(-0.1f, 0.1f)); break;
    case 10: mat = 1; fwd(false); break;
    case 11: head = B(g.Fl(-3.2f, 3.2f)); break;
    case 12: head = B(g.Fl(-1.0e-6f, 1.0e-6f)); break;
    case 13: head = g.Pk({0x80000000u, 0x80000001u, 0x80800000u, 0xB0000000u, 0xA0000000u}); break;
    case 14: mat = 1; head = g.W(); fwd(true); break;
    default: if (g.P(30)) mat = 1; head = g.P(60) ? B(g.Fl(-6.0f, 6.0f)) : Special(g); break;
    }
    if (!mat) {   // sin matriz solo cabe un rumbo finito pequeno (fsin de valores grandes depende de la reduccion interna de la x87, fuera del modelo) o no finito
        const float hv = F(head);
        if (std::isfinite(hv) && std::fabs(hv) > 8.0f) head = B(g.Fl(-8.0f, 8.0f));
    }
    c.a[0] = mat; c.a[1] = head;
    for (u32 k = 0; k < 3; ++k) c.a[5 + k] = g.P(55) ? g.W() : g.Pk({0u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0x3F800000u});
}

// ------------------------------------------------------------------------------------------------ funcion 4: CScriptsForBrains::StartOrRequestNewStreamedScriptBrainWithThisName (0x46CED0)
//   a[0] = tipo (un byte, extendido con ceros a 32 bits como lo pasa la DLL), a[1] = indice que devuelve 0x46AA30 (int16 en los 16 bits bajos), a[2] = nombre (0 arena, 1 nulo, 2 valor crudo 0x00C0FFEE), a[3] = entidad (igual),
//   a[4] = parte alta de EAX que deja el falso de 0x46AA30 cuando el indice es negativo
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 type = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0xFFu, 0x80u, 0x7Fu, 0x64u, 0xFEu});   // el tipo viaja como byte extendido con ceros (asi lo pasa la DLL: movzx)
    u32 idx = g.R(70);
    u32 name = 0, ent = 0;
    switch (f) {
    case 0: idx = g.R(70); break;
    case 1: idx = 70u + g.R(186); break;                                                   // fuera del rango real pero dentro de un byte
    case 2: idx = u32(-int(1 + g.R(8))) & 0xFFFFu; break;                                  // negativo: no llama
    case 3: idx = g.Pk({0x8000u, 0xFFFFu, 0xFFFEu, 0xC000u, 0x8001u}); break;
    case 4: idx = g.Pk({0u, 1u, 69u, 255u, 254u, 128u, 127u}); break;
    case 5: name = 1; break;
    case 6: ent = 1; break;
    case 7: name = 2; ent = 2; break;
    case 8: type = g.R(256); break;
    case 9: type = g.R(9); break;
    case 10: idx = 0; break;
    case 11: idx = 69; break;
    case 12: idx = g.P(50) ? (u32(-int(1 + g.R(32768))) & 0xFFFFu) : g.R(256); break;
    case 13: name = g.R(3); ent = g.R(3); idx = g.R(256); break;
    case 14: type = 3; idx = g.R(70); break;
    default: idx = g.P(30) ? (u32(-int(1 + g.R(20))) & 0xFFFFu) : g.R(256); name = g.P(10) ? g.R(3) : 0; ent = g.P(10) ? g.R(3) : 0; break;
    }
    c.a[0] = type; c.a[1] = idx; c.a[2] = name; c.a[3] = ent; c.a[4] = g.W();
}

// ------------------------------------------------------------------------------------------------ funcion 5: CTheScripts::IsPointWithinSearchLight (0x493280)
//   a[0..2] = origen del foco (+0x58 del registro de 0x7C bytes; el punto de impacto), a[3..5] = el punto (posicion), a[6..8] = el vector que deja el falso de 0x59C970 en el primer eje (+0x64 del registro),
//   a[9] = la magnitud que devuelve, a[10..12] y a[13] = lo mismo del segundo eje (+0x70), a[14..16] = el primer eje antes de normalizar (+0x64), a[17..19] = el segundo (+0x70), a[20] = indice (0..7)
inline void NormOf(u32* out, u32* mag, double x, double y, double z) {
    const double len = std::sqrt(x * x + y * y + z * z);
    if (!(len > 1e-30)) { out[0] = B(1.0f); out[1] = 0; out[2] = 0; *mag = B(1.0f); return; }
    out[0] = B(float(x / len)); out[1] = B(float(y / len)); out[2] = B(float(z / len)); *mag = B(float(len));
}
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    // geometria consistente: dos ejes (A, B) perpendiculares de longitudes la, lb; el punto = origen + u*A + v*B + w*C con (u, v) de radio r
    const double la = g.Fl(0.5f, 60.0f), lb = g.Fl(0.5f, 60.0f);
    const double th = g.Fl(0.0f, 6.2831853f), ph = g.Fl(0.0f, 3.14159265f);
    const double dx = std::sin(ph) * std::cos(th), dy = std::sin(ph) * std::sin(th), dz = std::cos(ph);
    double bx = -dy, by = dx, bz = 0.0;
    double bl = std::sqrt(bx * bx + by * by);
    if (bl < 1e-6) { bx = 1.0; by = 0.0; bl = 1.0; }
    bx /= bl; by /= bl;
    const double ax = dx * la, ay = dy * la, az = dz * la;
    const double bxx = bx * lb, byy = by * lb, bzz = bz * lb;
    const double sx = g.Fl(-2000.0f, 2000.0f), sy = g.Fl(-2000.0f, 2000.0f), sz = g.Fl(-50.0f, 150.0f);
    const double r = g.Fl(0.0f, 2.0f);
    const double ang = g.Fl(0.0f, 6.2831853f);
    const double u = r * std::cos(ang), v = r * std::sin(ang), w = g.Fl(-1.0f, 1.0f);
    const double cx = dy * bzz - dz * byy, cy = dz * bxx - dx * bzz, cz = dx * byy - dy * bxx;   // tercer eje (sin normalizar)
    auto setPoint = [&](double uu, double vv, double ww) {
        c.a[3] = B(float(sx + uu * ax + vv * bxx + ww * cx));
        c.a[4] = B(float(sy + uu * ay + vv * byy + ww * cy));
        c.a[5] = B(float(sz + uu * az + vv * bzz + ww * cz));
    };
    c.a[0] = B(float(sx)); c.a[1] = B(float(sy)); c.a[2] = B(float(sz));
    c.a[14] = B(float(ax)); c.a[15] = B(float(ay)); c.a[16] = B(float(az));
    c.a[17] = B(float(bxx)); c.a[18] = B(float(byy)); c.a[19] = B(float(bzz));
    NormOf(&c.a[6], &c.a[9], F(c.a[14]), F(c.a[15]), F(c.a[16]));
    NormOf(&c.a[10], &c.a[13], F(c.a[17]), F(c.a[18]), F(c.a[19]));
    c.a[20] = g.R(8);
    setPoint(u, v, w);
    switch (f) {
    case 0: break;
    case 1: {   // frontera del radio 1: (u, v) sobre el circulo +- ulps
        const double k = 1.0 + double(int(g.R(9)) - 4) * 1.1920929e-7;
        setPoint(std::cos(ang) * k, std::sin(ang) * k, 0.0);
        break;
    }
    case 2: c.a[3] = c.a[0]; c.a[4] = c.a[1]; c.a[5] = c.a[2]; break;   // punto = origen
    case 3: {   // cancelacion solo con redondeo a 24 bits: az*dz + ay*dy = 0 con (1 + u * 2^-23) * (1 + v * 2^-23) redondeado
        const float aa = 1.0f + float(g.R(1u << 22)) * 1.1920929e-7f, dd = 1.0f + float(g.R(1u << 22)) * 1.1920929e-7f;
        const float p = float(double(aa) * double(dd));
        const float scale = std::ldexp(1.0f, int(g.R(11)) - 5);
        // normA = (0, -p, aa) * scale; punto - origen = (0, 1, dd) * scale (el producto de z y el de y se anulan al redondear a 24 bits)
        c.a[6] = 0u; c.a[7] = B(-p * scale); c.a[8] = B(aa * scale);
        c.a[0] = 0u; c.a[1] = 0u; c.a[2] = 0u;   // origen en cero: la diferencia (0, scale, dd * scale) es exacta
        c.a[3] = 0u; c.a[4] = B(scale); c.a[5] = B(dd * scale);
        c.a[9] = B(g.Fl(0.5f, 20.0f));
        break;
    }
    case 4:   // magnitudes degeneradas
        c.a[9] = g.Pk({0u, 0x80000000u, 1u, 0x00800000u, 0x7F7FFFFFu, B(1.0f), 0x7F800000u});
        if (g.P(50)) c.a[13] = g.Pk({0u, 0x80000000u, 1u, 0x00800000u, 0x7F7FFFFFu, B(1.0f), 0x7F800000u});
        if (g.P(30)) { c.a[6] = 0u; c.a[7] = 0u; c.a[8] = 0u; }
        break;
    case 5:   // NaN e infinitos
        for (u32 k = 0; k < 14; ++k) if (g.P(12)) c.a[k] = g.Pk({0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7FC00000u, 0u, 0x80000000u});
        break;
    case 6:   // magnitudes enormes: los productos salen del rango de float en los registros de la x87
        for (u32 k = 3; k < 6; ++k) if (g.P(60)) c.a[k] = g.Pk({0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x70000000u, 0xF0000000u});
        for (u32 k = 6; k < 9; ++k) if (g.P(40)) c.a[k] = g.Pk({0x7F000000u, 0xFF000000u, 0x7E800000u, 0x70000000u, 0xF0000000u, 0x60000000u, 0xE0000000u});
        for (u32 k = 10; k < 13; ++k) if (g.P(40)) c.a[k] = g.Pk({0x7F000000u, 0xFF000000u, 0x7E800000u, 0x70000000u, 0xF0000000u, 0x60000000u, 0xE0000000u});
        if (g.P(60)) c.a[9] = g.Pk({0x7F000000u, 0x7E800000u, 0x70000000u, 0x60000000u, 0x7F7FFFFFu});
        if (g.P(60)) c.a[13] = g.Pk({0x7F000000u, 0x7E800000u, 0x70000000u, 0x60000000u, 0x7F7FFFFFu});
        break;
    case 7:   // magnitudes diminutas
        for (u32 k = 3; k < 6; ++k) if (g.P(60)) c.a[k] = g.Pk({0x00000100u, 0x80000100u, 0x007FFFFFu, 0x00800000u, 0x80800000u, 0x1E000000u, 0x9E000000u, 0u});
        for (u32 k = 6; k < 9; ++k) if (g.P(40)) c.a[k] = g.Pk({0x00000100u, 0x80000100u, 0x00800000u, 0x80800000u, 0x1E000000u, 0x9E000000u});
        if (g.P(50)) c.a[9] = g.Pk({0x00000100u, 0x00800000u, 0x1E000000u, 0x20000000u});
        if (g.P(50)) c.a[13] = g.Pk({0x00000100u, 0x00800000u, 0x1E000000u, 0x20000000u});
        break;
    case 8:   // dentro por mucho
        setPoint(g.Fl(-0.4f, 0.4f), g.Fl(-0.4f, 0.4f), g.Fl(-1.0f, 1.0f));
        break;
    case 9:   // fuera por mucho
        setPoint(g.Fl(1.5f, 6.0f) * (g.P(50) ? 1.0f : -1.0f), g.Fl(1.5f, 6.0f) * (g.P(50) ? 1.0f : -1.0f), g.Fl(-1.0f, 1.0f));
        break;
    case 10:  // ceros con signo y valores enteros pequenos
        for (u32 k = 3; k < 14; ++k) if (g.P(35)) c.a[k] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u});
        break;
    case 11:  // los vectores normalizados que devuelve el falso son arbitrarios (no unitarios)
        for (u32 k = 6; k < 9; ++k) c.a[k] = g.P(50) ? B(g.Fl(-3.0f, 3.0f)) : Special(g);
        for (u32 k = 10; k < 13; ++k) c.a[k] = g.P(50) ? B(g.Fl(-3.0f, 3.0f)) : Special(g);
        c.a[9] = g.P(60) ? B(g.Fl(0.1f, 30.0f)) : Special(g);
        c.a[13] = g.P(60) ? B(g.Fl(0.1f, 30.0f)) : Special(g);
        break;
    case 12:  // frontera con un solo eje activo: u = +-1 + ulps, v = 0
        setPoint((g.P(50) ? 1.0 : -1.0) * (1.0 + double(int(g.R(9)) - 4) * 1.1920929e-7), 0.0, 0.0);
        break;
    case 13:  // rango amplio
        for (u32 k = 3; k < 6; ++k) c.a[k] = Wide(g);
        for (u32 k = 0; k < 3; ++k) if (g.P(50)) c.a[k] = Wide(g);
        break;
    case 14:  // el punto en el plano de un eje (la otra proyeccion es cero)
        setPoint(g.Fl(-1.2f, 1.2f), 0.0, g.Fl(-1.0f, 1.0f));
        break;
    default:  // mezcla
        if (g.P(30)) for (u32 k = 3; k < 6; ++k) c.a[k] = Special(g);
        if (g.P(30)) c.a[9] = Special(g);
        break;
    }
}

// ------------------------------------------------------------------------------------------------ funcion 6: WinInput::diPadEnumerate (0x7477C0)
//   a[0] = HRESULT que devuelve el falso de EnumDevices, a[1] = primer mando (+0x20 de psGlobal; 0 nulo, 1 arena, 2 valor crudo 0x00C0FFEE), a[2] = segundo mando (+0x24; igual, crudo 0x00C0FFEF)
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 hr = 0, d1 = 1, d2 = 1;
    constexpr std::array<u32, 8> hrs{1u, 0x80004005u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80070057u, 2u, 0x8000FFFFu, 0x80000000u};
    switch (f) {
    case 0: break;                                   // todo presente
    case 1: d2 = 0; break;                           // sin segundo mando
    case 2: d1 = 0; d2 = g.R(3); break;              // sin primer mando: devuelve 1 sin mirar el segundo
    case 3: hr = hrs[g.R(8)]; d1 = g.R(3); d2 = g.R(3); break;   // EnumDevices falla
    case 4: d1 = 2; d2 = 2; break;
    case 5: d1 = 1; d2 = 2; break;
    case 6: d1 = 2; d2 = 0; break;
    case 7: d1 = g.R(3); d2 = g.R(3); break;
    case 8: hr = g.P(50) ? hrs[g.R(8)] : 0u; d1 = 1; d2 = 1; break;
    default: hr = g.P(25) ? hrs[g.R(8)] : 0u; d1 = g.R(3); d2 = g.R(3); break;
    }
    c.a[0] = hr; c.a[1] = d1; c.a[2] = d2;
}

// ------------------------------------------------------------------------------------------------ funcion 7: CCarCtrl::WeaveForPed (0x426970)
//   a[0] = mision (byte +0x3BA del vehiculo), a[1] = el peaton tiene matriz, a[2..4] = su posicion en la activa (matriz + 0x30 o placement +4), a[5..7] = en la otra (basura),
//   a[8] = el vehiculo tiene matriz, a[9..11] = su posicion en la activa, a[12..14] = en la otra, a[15] = lo que devuelve FindPlayerPed(-1) (0 el peaton, 1 nulo, 2 valor crudo 0x00ABCD00),
//   a[16] = objetivo del vehiculo (+0x41C; 0 el peaton, 1 nulo, 2 valor crudo 0x00ABCD01), a[17] = lo que devuelve 0x53CC70 (angulo), a[18] = maxima x de la caja del modelo del vehiculo,
//   a[19] = *limite1 inicial, a[20] = *limite2 inicial, a[21] = indice de modelo del vehiculo (1500..1507)
constexpr float kTwoPi = 6.2831853071795f;
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    const u32 missions[]{0u, 1u, 2u, 4u, 10u, 0x36u, 0x38u, 0xFFu, 0x80u, 0x33u};
    u32 mission = missions[g.R(10)];
    // posicion del vehiculo y del peaton a distancia d en una direccion th
    float vx = g.Fl(-3000.0f, 3000.0f), vy = g.Fl(-3000.0f, 3000.0f), vz = g.Fl(-50.0f, 100.0f);
    float d = g.Fl(1.0f, 40.0f);
    const float th = g.Fl(-3.1415927f, 3.1415927f);
    float px = vx + d * std::cos(th), py = vy + d * std::sin(th);
    float pz = vz + g.Fl(-2.0f, 2.0f);
    const float angle = g.Fl(-3.1415927f, 3.1415927f);
    u32 abits = B(angle);
    u32 maxx = B(g.Fl(0.5f, 4.0f));
    float la = angle + g.Fl(-0.5f, 0.5f), lb = angle + g.Fl(-0.5f, 0.5f);
    u32 lim1 = B(la), lim2 = B(lb);
    u32 pedMat = g.P(50), vehMat = g.P(50), player = 1, target = 1;
    switch (f) {
    case 0: break;
    case 1: mission = 3; player = 0; break;                                                  // mision 3 y el peaton es el jugador
    case 2: mission = 3; player = g.Pk({1u, 2u}); break;                                      // mision 3, otro jugador
    case 3: mission = 0x37; target = 0; break;                                                // mision 0x37 y el peaton es el objetivo
    case 4: mission = 0x37; target = g.Pk({1u, 2u}); player = 0; break;                       // mision 0x37, otro objetivo (el jugador no cuenta)
    case 5: d = g.Fl(0.0f, 0.999f); px = vx + d * std::cos(th); py = vy + d * std::sin(th); break;   // demasiado cerca: sale
    case 6: {   // frontera de la distancia 1.0: vehiculo en coordenadas enteras pequenas, peaton a 1.0 +- ulps en x o en una diagonal de sqrt(2)/2
        vx = float(int(g.R(17))); vy = float(int(g.R(33)) - 16); vz = 0.0f;   // vx >= 0: vx + 1.0 nunca es cero (NextUp de cero daria un NaN con carga)
        if (g.P(60)) { px = F(NextUp(B(vx + 1.0f), int(g.R(7)) - 3)); py = vy; }
        else { const float h = 0.70710678f; px = F(NextUp(B(vx + h), int(g.R(7)) - 3)); py = F(NextUp(B(vy + h), int(g.R(7)) - 3)); }
        pz = vz;
        break;
    }
    case 7: {   // el primer limite se sale por debajo de -pi
        abits = B(g.Fl(-3.14159f, -2.5f)); la = F(abits) + g.Fl(-0.02f, 0.05f); lim1 = B(la);
        break;
    }
    case 8: {   // el segundo limite se sale por encima de pi
        abits = B(g.Fl(2.5f, 3.14159f)); lb = F(abits) + g.Fl(-0.05f, 0.02f); lim2 = B(lb);
        break;
    }
    case 9: lim1 = B(angle + g.Fl(1.0f, 3.0f)); lim2 = B(angle - g.Fl(1.0f, 3.0f)); break;   // arcos que no se solapan
    case 10: lim1 = B(angle + kTwoPi * float(int(g.R(41)) - 20) + g.Fl(-0.3f, 0.3f)); lim2 = B(angle + kTwoPi * float(int(g.R(41)) - 20) + g.Fl(-0.3f, 0.3f)); break;   // vueltas de 2pi
    case 11: {   // NaN y ceros
        if (g.P(40)) abits = 0xFFC00000u;
        if (g.P(30)) lim1 = 0xFFC00000u;
        if (g.P(30)) lim2 = 0xFFC00000u;
        if (g.P(25)) maxx = 0xFFC00000u;
        if (g.P(25)) px = F(0xFFC00000u);
        if (g.P(15)) vy = 0.0f;
        break;
    }
    case 12: maxx = g.Pk({0u, 0x80000000u, 1u, 0x00800000u, 0x007FFFFFu, 0x1E000000u, 0xFF7FFFFFu, 0xFF000000u, B(1.0f), B(10.0f), B(50.0f), B(0.001f)}); break;   // anchos extremos (sin positivos enormes: el limite se saldria de [-pi, pi] por millones de vueltas, el EXE no termina)
    case 13: {   // distancias enormes (la diferencia sale del rango de float)
        px = F(g.Pk({0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F7FFFFFu})); py = 0.0f; break;
    }
    case 14: pedMat = n / 16 % 2; vehMat = n / 32 % 2; break;
    default: if (g.P(30)) { abits = g.Pk({0u, 0x80000000u, 1u, B(3.1415927f), B(-3.1415927f), 0xFFC00000u, B(1.5707964f)}); } break;
    }
    c.a[0] = mission; c.a[1] = pedMat;
    c.a[2] = B(px); c.a[3] = B(py); c.a[4] = B(pz);
    for (u32 k = 0; k < 3; ++k) c.a[5 + k] = g.P(60) ? Coord(g) : Special(g);
    c.a[8] = vehMat; c.a[9] = B(vx); c.a[10] = B(vy); c.a[11] = B(vz);
    for (u32 k = 0; k < 3; ++k) c.a[12 + k] = g.P(60) ? Coord(g) : Special(g);
    c.a[15] = player; c.a[16] = target;
    c.a[17] = abits; c.a[18] = maxx; c.a[19] = lim1; c.a[20] = lim2; c.a[21] = 1500u + g.R(8);
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
    default: Make7(c, n); break;
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
