#pragma once
// Fixtures del banco Fn103A (lote 103A): 16384 casos por funcion (114688 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//    0 CMatrix::SetRotateXOnly  0x59AFA0  thiscall (ECX = matriz, angulo), `ret 4`; fcos/fsin de la x87; no toca la posicion                             (hook S)
//    1 CMatrix::SetRotateYOnly  0x59AFE0  thiscall (ECX = matriz, angulo), `ret 4`                                                                   (hook S)
//    2 CMatrix::Rotate          0x59B460  thiscall (ECX = matriz, x, y, z), `ret 0Ch`; SetRotate(x, y, z) y cuatro TransformVector en linea            (hook S)
//    3 CMatrix::ForceUpVector   0x59B7E0  thiscall (ECX = matriz, up.x, up.y, up.z por valor), `ret 0Ch`; right = forward x up, forward = up x right  (hook S)
//    4 CMatrix::SetRotate(quat) 0x59BBF0  thiscall (ECX = matriz, CQuaternion*), `ret 4`                                                             (hook S)
//    5 operator+(CMatrix, CMatrix) 0x59BFA0 cdecl (out oculto, a*, b*); devuelve out en EAX; relleno = basura de la pila (se enmascara)                (hook S)
//    6 CVector::FromMultiply3x3 0x59C6D0  thiscall (ECX = destino, matriz*, vector*), `ret 8`; matriz * vector sin posicion, componente a componente (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Modo de solapamiento = (n / 16) % 8.
// Memoria: una ventana de 96 dwords (0x180 bytes) con valores explicitos en `mem`; los punteros de entrada y salida son indices de dword en esa ventana (pa, pb, po) y pueden solaparse.
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

namespace F103AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 7;
constexpr unsigned kWords = 96;         // ventana de memoria

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = kWords * 4;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 99001u); }
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
    u32 f0{}, f1{}, f2{};                 // escalares (angulos, escala o traslacion)
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

// ------------------------------------------------------------------------------------------------ angulos (CMatrix::SetRotate)
// Sin angulos con modulo entre 2^19 y 2^62: la reduccion del x87 con el pi de 66 bits no se modela. Los de 2^63 o mas los deja intactos la x87 (C2 = 1).
inline u32 GenAngle(Gen& g, u32 f) {
    static const float kPi = 3.14159265358979323846f;
    switch (f) {
    case 0: return B(g.Fl(-kPi, kPi));
    case 1: return B(g.Fl(-1.0f, 1.0f));
    case 2: { if (g.P(40)) return SignedZero(g); return B(float(int(g.R(17)) - 8) * (kPi * 0.5f)); }
    case 3: return g.P(40) ? kTiny[g.R(12)] : NormalBits(g, -75, -10);
    case 4: return NormalBits(g, -3, 10);
    case 5: return NormalBits(g, 10, 18);
    case 6: return g.P(45) ? kNanBits[g.R(6)] : B(g.Fl(-kPi, kPi));
    case 7: return g.P(50) ? kInf[g.R(4)] : B(g.Fl(-kPi, kPi));
    case 8: return g.P(25) ? Special(g) : B(g.Fl(-kPi, kPi));
    case 9: return B(g.Fl(-2.0f * kPi, 2.0f * kPi));
    case 10: return g.P(55) ? kNanDistinct[g.R(10)] : B(g.Fl(-kPi, kPi));
    case 11: { static const float T[] = {0.785398185f, 1.57079637f, 3.14159274f, 4.71238899f, 6.28318548f, 1.0f, 0.5f, 2.0f, 0.0f, 0.3926990926f}; return B(T[g.R(10)] * (g.P(50) ? 1.0f : -1.0f)); }
    case 12: { const float k = float(int(g.R(9)) - 4) * (kPi * 0.5f); return NextUp(B(k), int(g.R(9)) - 4); }
    case 13: return g.P(70) ? kTiny[g.R(12)] : NormalBits(g, -127, -100);
    case 14: return g.P(50) ? SignedZero(g) : kTiny[g.R(12)];
    default: { const u32 r = g.R(20); if (r < 6) return kTiny[g.R(12)]; if (r < 9) return kInf[g.R(4)]; if (r < 12) return kNanBits[g.R(6)]; if (r < 14) return kBig[g.R(6)]; return B(g.Fl(-kPi, kPi)); }
    }
}

// ------------------------------------------------------------------------------------------------ disposicion en la ventana
// Tres ranuras de vectores (2, 14, 26); a, b y out ocupan una cada una en orden aleatorio; los modos 3..7 hacen que se solapen.
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
// Matrices de 18 dwords (16 de la RwMatrix + puntero enlazado +0x40 + bandera +0x44) en ranuras {4, 24, 44, 64}, en orden aleatorio.
inline void LayoutM(C& c, Gen& g) {
    static const u32 slot[4] = {4, 24, 44, 64};
    u32 perm[4] = {0, 1, 2, 3};
    for (int k = 3; k > 0; --k) std::swap(perm[k], perm[g.R(u32(k + 1))]);
    c.pa = slot[perm[0]]; c.pb = slot[perm[1]]; c.po = slot[perm[2]];
}

inline void PutWords(C& c, u32 at, const u32* w, unsigned n) { for (unsigned k = 0; k < n; ++k) c.mem[at + k] = w[k]; }
// Cuatro filas de una matriz (derecha 0..2, adelante 4..6, arriba 8..10, posicion 12..14; los dwords 3, 7, 11 y 15 son relleno con valores aleatorios). `other` = 12 dwords (3 filas de 3) para la
// familia de cancelaciones (la fila generada es la de `other` por una potencia de dos).
inline void PutMatrix(Gen& g, C& c, u32 at, u32 f, const u32* other) {
    u32 row[3];
    for (unsigned r = 0; r < 4; ++r) {
        const bool useOther = other && r < 3;
        GenVec(g, f, row, useOther ? 1u : (r == 0 ? 0u : (g.P(50) ? 1u : 0u)), useOther ? other + 3 * r : (r ? &c.mem[at + 4 * (r - 1)] : nullptr));
        PutWords(c, at + 4 * r, row, 3);
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n; c.family = n % 16; c.mode = (n / 16) % 8;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    Gen g(n, 100 + 50 * fn);
    for (unsigned k = 0; k < kWords; ++k) c.mem[k] = g.W();
    const u32 f = c.family;
    switch (fn) {
    case 0: case 1:             // SetRotateXOnly/YOnly: una matriz (po) con palabras aleatorias de cualquier tipo y un angulo
        LayoutM(c, g);
        c.f0 = GenAngle(g, f);
        break;
    case 2:                     // Rotate: la matriz (po) con la familia f y tres angulos (de la misma familia o de otra)
        LayoutM(c, g);
        PutMatrix(g, c, c.po, f, nullptr);
        c.f0 = GenAngle(g, g.P(50) ? f : g.R(16));
        c.f1 = GenAngle(g, g.P(50) ? f : g.R(16));
        c.f2 = GenAngle(g, g.P(50) ? f : g.R(16));
        break;
    case 3: {                   // ForceUpVector: la matriz (po) con la familia f y el vector `up` (f0, f1, f2) de la misma familia (a veces cerca de las filas de la matriz)
        LayoutM(c, g);
        PutMatrix(g, c, c.po, f, nullptr);
        u32 v[3];
        GenVec(g, f, v, 1, g.P(30) ? &c.mem[c.po + 4] : nullptr);
        c.f0 = v[0]; c.f1 = v[1]; c.f2 = v[2];
        break;
    }
    case 4: {                   // SetRotate(quat): la matriz (po, palabras aleatorias) y el cuaternion (pb: x, y, z, real); modos 3..7: el cuaternion solapa la matriz
        LayoutM(c, g);
        const u32 shift = 1 + (c.n / 128) % 14;
        switch (c.mode) {
        case 3: c.pb = c.po; break;
        case 4: c.pb = c.po + 4; break;
        case 5: c.pb = c.po + 12; break;
        case 6: c.pb = c.po + shift; break;
        case 7: c.pb = c.po - 1 - (c.n / 128) % 3; break;
        default: break;
        }
        u32 v[3];
        GenVec(g, f, v, 0, nullptr);
        PutWords(c, c.pb, v, 3);
        c.mem[c.pb + 3] = GenScalar(g, g.P(50) ? f : g.R(16));
        break;
    }
    case 5: {                   // operator+: a = pa, b = pb, resultado oculto = po; modo 3 out == a, 4 out == b, 5 a == b, 6/7 out desplazado en filas de a / b (el relleno cae siempre en dword = 3 mod 4)
        LayoutM(c, g);
        const u32 rows = 4 * (1 + (c.n / 128) % 3);
        switch (c.mode) {
        case 3: c.po = c.pa; break;
        case 4: c.po = c.pb; break;
        case 5: c.pb = c.pa; break;
        case 6: c.po = c.pa + rows; break;
        case 7: c.po = c.pb + rows; break;
        default: break;
        }
        PutMatrix(g, c, c.pa, f, nullptr);
        if (c.mode != 5) {
            u32 other[9];
            for (unsigned r = 0; r < 3; ++r) for (unsigned k = 0; k < 3; ++k) other[3 * r + k] = c.mem[c.pa + 4 * r + k];
            PutMatrix(g, c, c.pb, f, f == 12 ? other : nullptr);
        }
        break;
    }
    default: {  // 6: FromMultiply3x3: destino = po (this), matriz en pa (4 filas de 3 componentes con relleno), vector en pb; modos 3..7 con solapamientos
        LayoutM(c, g);                                  // pa, pb, po: ranuras de matriz distintas
        const u32 shift = 1 + (c.n / 128) % 14;
        switch (c.mode) {
        case 3: c.po = c.pb; break;                     // destino == vector
        case 4: c.po = c.pa + 12; break;                // destino == posicion de la matriz
        case 5: c.po = c.pa + shift; break;             // destino en cualquier dword de la matriz
        case 6: c.pb = c.pa + 12; break;                // vector == posicion de la matriz
        case 7: c.pb = c.pa + shift; break;             // vector dentro de la matriz
        default: break;
        }
        PutMatrix(g, c, c.pa, f, nullptr);
        if (c.mode < 6) {                               // en 6 y 7 el vector es parte de la matriz ya generada
            u32 v[3];
            GenVec(g, f, v, 1, nullptr);
            PutWords(c, c.pb, v, 3);
        }
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("mode", c.mode).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("pa", c.pa).U("pb", c.pb).U("po", c.po).U("f0", c.f0).U("f1", c.f1).U("f2", c.f2);
    j.A("mem", std::vector<u32>(c.mem.begin(), c.mem.end()));
    return j.End();
}
}
