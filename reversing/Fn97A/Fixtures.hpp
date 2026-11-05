#pragma once
// Fixtures del banco Fn97A (lote 97A): 16384 casos por funcion (196608 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//    0 DotProduct          0x40FDB0  cdecl (a*, b*), `ret`; float en ST0 (sin redondear a float)                                     (hook S)
//    1 CrossProduct        0x59C730  cdecl (out*, a*, b*); devuelve out en EAX                                                      (hook S)
//    2 Multiply3x3Exe      0x59C790  cdecl (out*, matriz*, vector*); devuelve out en EAX (matriz (3x3) * vector)                     (hook S)
//    3 VectorAdd           0x40FE30  cdecl (out*, a*, b*); devuelve out en EAX                                                      (hook S)
//    4 VectorSubtract      0x40FE60  cdecl (out*, a*, b*); devuelve out en EAX                                                      (hook S)
//    5 FloatTimesVector    0x40FE90  cdecl (out*, float, v*); devuelve out en EAX                                                   (hook S)
//    6 VectorTimesFloat    0x40FEC0  cdecl (out*, v*, float); devuelve out en EAX                                                   (hook S)
//    7 CVector::operator*= 0x40FEF0  thiscall (ECX = vector, float), `ret 4`                                                       (hook S)
//    8 CVector::operator+= 0x411A00  thiscall (ECX = vector, v*), `ret 4`                                                          (hook S)
//    9 RectMin             0x404330  cdecl (float, float); float en ST0                                                             (hook S)
//   10 CColLine::Constructor 0x40EF50 thiscall (ECX = linea, inicio*, fin*), `ret 8`; devuelve this en EAX                          (hook S)
//   11 CMatrix::SetTranslate 0x59AF40 thiscall (ECX = matriz, 3 floats por valor), `ret 0Ch`                                       (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Modo de solapamiento = (n / 16) % 8.
// Memoria: una ventana de 48 dwords (0xC0 bytes) con valores explicitos en `mem`; los punteros de entrada y salida son indices de dword en esa ventana (pa, pb, po) y pueden solaparse.
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

namespace F97AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 12;
constexpr unsigned kWords = 48;         // ventana de memoria

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = kWords * 4;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 97001u); }
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
struct C {
    u32 fn{}, n{}, family{}, seed{}, mode{};
    u32 eax{}, edx{}, stack{};
    u32 pa{}, pb{}, po{};                 // indices de dword (entrada a, entrada b / matriz, salida / this)
    u32 f0{}, f1{};                       // escalares (factor, o los dos operandos de RectMin)
    std::array<u32, kWords> mem{};
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
// NaN de entrada. TODOS los NaN de los fixtures (kNanBits y kNanDistinct) tienen un significando distinto una vez silenciados (sin pares con el mismo significando y signo distinto: la x87 los
// resuelve por signo y el oraculo no lo modela); el indefinido 0xFFC00000 (el que dan inf*0 e inf-inf) esta en la lista y 0x7FC00000 no.
inline constexpr std::array<u32, 6> kNanBits{0xFFC00000u, 0x7FC12345u, 0x7F800001u, 0xFF812346u, 0xFFE00001u, 0x7FF55555u};
// Silenciosos (0xFFC00000, 0x7FC12345, 0x7FC00004, 0xFFC00002) y senalizantes (0x7F800001, 0xFF812346, 0x7F9FFFFF, 0xFF800003, 0x7FA00007, 0xFFAAAAAA) con cargas mayores y menores: sirven para combinar un
// senalizante de memoria con un silencioso de registro (gana el de mayor significando una vez silenciados).
inline constexpr std::array<u32, 10> kNanDistinct{0xFFC00000u, 0x7FC12345u, 0x7F800001u, 0xFF812346u, 0x7FC00004u, 0xFFC00002u, 0x7F9FFFFFu, 0xFF800003u, 0x7FA00007u, 0xFFAAAAAAu};

inline u32 SignedZero(Gen& g) { return g.P(50) ? 0x80000000u : 0u; }
inline u32 NormalBits(Gen& g, int e0, int e1) {
    const u32 e = u32(127 + e0 + int(g.R(u32(e1 - e0 + 1))));
    return (g.W() & 0x80000000u) | (e << 23) | (g.W() & 0x7FFFFFu);
}
inline u32 SpecialNoNan(Gen& g) {
    const u32 r = g.R(20);
    if (r < 8) return kTiny[g.R(12)];
    if (r < 12) return kInf[g.R(4)];
    if (r < 15) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 Special(Gen& g) { return g.P(25) ? kNanBits[g.R(6)] : SpecialNoNan(g); }
inline u32 NextUp(u32 bits, int steps) {   // desplaza un float finito `steps` unidades de ultimo lugar (signo respetado, sin salir de [0, FLT_MAX]: nunca produce un NaN ni un infinito)
    const bool neg = (bits >> 31) != 0;
    const i32 mag = i32(bits & 0x7FFFFFFFu);
    i32 moved = mag + steps;
    if (moved < 0) moved = 0;
    if (moved > 0x7F7FFFFF) moved = 0x7F7FFFFF;
    if (mag >= 0x7F800000) moved = mag;      // un NaN o un infinito de entrada se queda como esta
    return (neg ? 0x80000000u : 0u) | u32(moved);
}
inline u32 Wide(Gen& g) {
    const double e = double(int(g.R(61)) - 30);
    const double m = 1.0 + double(g.R(1000000)) / 1000000.0;
    return B(float((g.P(50) ? -1.0 : 1.0) * m * std::pow(10.0, e)));
}
inline u32 Coord(Gen& g, float range = 3000.0f) { return B(g.Fl(-range, range)); }

// ------------------------------------------------------------------------------------------------ generador de vectores
// role: 0 = vector `a` (o la primera fila de la matriz), 1 = vector `b` (la segunda); `other` = el ya generado (para la familia de cancelaciones)
inline void GenVec(Gen& g, u32 f, u32* v, unsigned role, const u32* other) {
    switch (f) {
    case 0:     // coordenadas de juego, en metros
        for (int k = 0; k < 3; ++k) v[k] = Coord(g);
        break;
    case 1:     // componentes en [-1, 1]
        for (int k = 0; k < 3; ++k) v[k] = B(g.Fl(-1.0f, 1.0f));
        break;
    case 2: {   // ceros y ejes
        const u32 m = g.R(4);
        for (int k = 0; k < 3; ++k) v[k] = SignedZero(g);
        if (m >= 1) v[g.R(3)] = g.P(50) ? Coord(g) : Wide(g);
        if (m >= 2) v[g.R(3)] = Coord(g);
        if (m == 3) v[g.R(3)] = kTiny[g.R(12)];
        break;
    }
    case 3:     // diminutos: los productos pierden precision o dan cero como float, pero no en la x87
        for (int k = 0; k < 3; ++k) v[k] = g.P(15) ? kTiny[g.R(12)] : NormalBits(g, -75, -10);
        break;
    case 4:     // enormes: los productos salen del rango de float (pero no del de la x87) y la resta puede volver a el
        for (int k = 0; k < 3; ++k) v[k] = g.P(12) ? NormalBits(g, -20, 20) : NormalBits(g, 40, 126);
        break;
    case 5:     // un NaN (de cualquier carga, senalizante o no) en una componente, el resto coordenadas o especiales
        for (int k = 0; k < 3; ++k) v[k] = g.P(30) ? SpecialNoNan(g) : Coord(g);
        v[g.R(3)] = kNanBits[g.R(6)];
        break;
    case 6:     // infinitos solos o con ceros y finitos
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? kInf[g.R(4)] : (g.P(40) ? SignedZero(g) : Coord(g));
        break;
    case 7:     // escalas mezcladas
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? NormalBits(g, -60, -30) : NormalBits(g, 40, 100);
        if (g.P(30)) v[g.R(3)] = SignedZero(g);
        break;
    case 8:     // mantisas completas aleatorias en un rango moderado: redondeo a 24 bits de producto y suma
        for (int k = 0; k < 3; ++k) v[k] = NormalBits(g, -6, 6);
        break;
    case 9:     // patrones de 32 bits cualesquiera
        for (int k = 0; k < 3; ++k) v[k] = g.W();
        break;
    case 10: {  // varios NaN con cargas distintas, senalizantes y silenciosos mezclados: cual elige la x87 segun el operando sea de memoria o de registro
        for (int k = 0; k < 3; ++k) v[k] = g.P(55) ? kNanDistinct[g.R(10)] : (g.P(50) ? Coord(g) : SignedZero(g));
        if (g.P(30)) { const u32 w = kNanDistinct[g.R(10)]; for (int k = 0; k < 3; ++k) if (g.P(60)) v[k] = w; }
        break;
    }
    case 11: {  // resultados exactos y empates: enteros pequenos, potencias de dos y unos pocos ulps
        static const float T[] = {0, 1, 2, 3, 4, 6, 8, 0.5f, 0.25f, -1, -2, -3, 12, 1024, 7, 9};
        for (int k = 0; k < 3; ++k) {
            v[k] = B(T[g.R(16)] * (g.P(50) ? 1.0f : -1.0f));
            if (g.P(35)) v[k] = NextUp(v[k], int(g.R(7)) - 3);
        }
        break;
    }
    case 12: {  // cancelaciones: el segundo vector es el primero por una potencia de dos (productos cruzados que se anulan) o su opuesto, con ulps de diferencia a veces
        for (int k = 0; k < 3; ++k) v[k] = Coord(g, 300.0f);
        if (role == 1 && other) {
            const int e = int(g.R(7)) - 3;
            const float s = std::ldexp(g.P(50) ? 1.0f : -1.0f, e);
            for (int k = 0; k < 3; ++k) v[k] = B(F(other[k]) * s);
            if (g.P(30)) for (int k = 0; k < 3; ++k) if (g.P(40)) v[k] = NextUp(v[k], int(g.R(5)) - 2);
        }
        break;
    }
    case 13:    // desnormales y casi desnormales en todas las componentes
        for (int k = 0; k < 3; ++k) v[k] = g.P(70) ? kTiny[g.R(12)] : NormalBits(g, -127, -100);
        break;
    case 14: {  // ceros con signo y diminutos con signo, a veces una coordenada
        for (int k = 0; k < 3; ++k) v[k] = g.P(50) ? SignedZero(g) : kTiny[g.R(12)];
        if (g.P(30)) v[g.R(3)] = Coord(g);
        break;
    }
    default:    // especiales de todo tipo (NaN, infinitos, ceros, diminutos, enormes, coordenadas)
        for (int k = 0; k < 3; ++k) v[k] = Special(g);
        break;
    }
}

inline u32 GenScalar(Gen& g, u32 f) {
    switch (f) {
    case 0: return Coord(g, 100.0f);
    case 1: return B(g.Fl(-1.0f, 1.0f));
    case 2: return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, B(2.0f), B(0.5f)});
    case 3: return g.P(40) ? kTiny[g.R(12)] : NormalBits(g, -75, -10);
    case 4: return NormalBits(g, 40, 126);
    case 5: return g.P(70) ? kNanBits[g.R(6)] : Coord(g);
    case 6: return g.P(60) ? kInf[g.R(4)] : Coord(g);
    case 7: return g.P(50) ? NormalBits(g, -60, -30) : NormalBits(g, 40, 100);
    case 8: return NormalBits(g, -6, 6);
    case 9: return g.W();
    case 10: return g.P(70) ? kNanDistinct[g.R(10)] : Coord(g, 10.0f);
    case 11: { static const float T[] = {0, 1, 2, 3, 0.5f, 4, 8, -1, -2, 1024}; return B(T[g.R(10)]); }
    case 12: return B(std::ldexp(g.P(50) ? 1.0f : -1.0f, int(g.R(7)) - 3));
    case 13: return kTiny[g.R(12)];
    case 14: return g.P(50) ? SignedZero(g) : kTiny[g.R(12)];
    default: return Special(g);
    }
}

// ------------------------------------------------------------------------------------------------ disposicion en la ventana
// Tres ranuras de 12 dwords (empiezan en 2, 14 y 26); a, b y out ocupan una cada una en orden aleatorio; los modos 3..7 hacen que se solapen.
inline void Layout(C& c, Gen& g, bool allowOverlap) {
    static const u32 slot[3] = {2, 14, 26};
    u32 perm[3] = {0, 1, 2};
    for (int k = 2; k > 0; --k) std::swap(perm[k], perm[g.R(u32(k + 1))]);
    c.pa = slot[perm[0]]; c.pb = slot[perm[1]]; c.po = slot[perm[2]];
    if (!allowOverlap) return;
    switch (c.mode) {
    case 3: c.po = c.pa; break;                                       // out == a
    case 4: c.po = c.pb; break;                                       // out == b
    case 5: c.pb = c.pa; break;                                       // a == b
    case 6: c.po = c.pa + 1 + (c.n / 128) % 2; break;                 // out solapa a desplazado 1 o 2 dwords
    case 7: c.po = c.pb - 1 - (c.n / 128) % 2; break;                 // out solapa b, 1 o 2 dwords antes
    default: break;
    }
}

inline void PutWords(C& c, u32 at, const u32* w, unsigned n) { for (unsigned k = 0; k < n; ++k) c.mem[at + k] = w[k]; }

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n; c.family = n % 16; c.mode = (n / 16) % 8;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    Gen g(n, 100 + 50 * fn);
    for (unsigned k = 0; k < kWords; ++k) c.mem[k] = g.W();
    const u32 f = c.family;
    u32 a[3], b[3];
    switch (fn) {
    case 7:     // vector *= escalar (solo `po`)
        Layout(c, g, false);
        GenVec(g, f, a, 0, nullptr);
        PutWords(c, c.po, a, 3);
        c.f0 = GenScalar(g, f);
        break;
    case 9:     // RectMin: dos escalares, sin memoria
        Layout(c, g, false);
        c.f0 = GenScalar(g, f); c.f1 = GenScalar(g, f);
        if (g.P(25)) c.f1 = c.f0;
        else if (g.P(20)) c.f1 = c.f0 ^ 0x80000000u;
        else if (g.P(15)) c.f1 = NextUp(c.f0, int(g.R(5)) - 2);
        break;
    case 10:    // CColLine::Constructor: el objeto (8 dwords) y los dos extremos, sin solapamientos
        Layout(c, g, false);
        GenVec(g, f, a, 0, nullptr); GenVec(g, f, b, 1, a);
        PutWords(c, c.pa, a, 3); PutWords(c, c.pb, b, 3);
        break;
    case 11: {  // CMatrix::SetTranslate: matriz de 18 dwords en `po` y la traslacion (3 floats) en `pa`
        static const u32 poS[2] = {0, 26};
        c.po = poS[g.R(2)];
        c.pa = c.po == 0 ? (g.P(50) ? 20u : 30u) : (g.P(50) ? 2u : 12u);
        c.pb = c.pa;
        GenVec(g, f, a, 0, nullptr);
        PutWords(c, c.pa, a, 3);
        break;
    }
    case 2: {   // Multiply3x3Exe: matriz (11 dwords utiles: derecha 0..2, adelante 4..6, arriba 8..10; 3 y 7 de relleno) en `pb`, vector en `pa`
        Layout(c, g, true);
        u32 m[11];
        GenVec(g, f, m, 0, nullptr); m[3] = g.W();
        GenVec(g, f, m + 4, g.P(50) ? 1u : 0u, m); m[7] = g.W();
        GenVec(g, f, m + 8, g.P(50) ? 1u : 0u, m + 4);
        GenVec(g, f, a, 1, m);
        PutWords(c, c.pb, m, 11);
        PutWords(c, c.pa, a, 3);
        break;
    }
    case 5: case 6:
        Layout(c, g, true);
        GenVec(g, f, a, 0, nullptr);
        PutWords(c, c.pa, a, 3);
        c.f0 = GenScalar(g, f);
        break;
    default:    // 0, 1, 3, 4, 8: dos vectores
        Layout(c, g, true);
        GenVec(g, f, a, 0, nullptr); GenVec(g, f, b, 1, a);
        if (c.mode == 5) PutWords(c, c.pa, a, 3);                      // a == b: el mismo vector
        else { PutWords(c, c.pa, a, 3); PutWords(c, c.pb, b, 3); }
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("mode", c.mode).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("pa", c.pa).U("pb", c.pb).U("po", c.po).U("f0", c.f0).U("f1", c.f1);
    j.A("mem", std::vector<u32>(c.mem.begin(), c.mem.end()));
    return j.End();
}
}
