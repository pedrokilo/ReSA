#pragma once
// Fixtures del banco Fn102A (lote 102A): 16384 casos por funcion (9 funciones). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 FxPrtMult_c::FxPrtMult_c(7 floats)           0x4AB290  thiscall (ECX = objeto de 0x1C bytes; 7 floats en pila), `ret 1Ch`; devuelve this (hook S)
//   1 CSimpleTransform::UpdateMatrix                0x54F1B0  thiscall (ECX = transformacion de 0x10 bytes; matriz en pila), `ret 4`; void (hook S)
//   2 CSimpleTransform::UpdateRwMatrix              0x54EF40  thiscall (ECX = transformacion; RwMatrix en pila), `ret 4`; void (hook S)
//   3 CVector::SquaredMagnitudeExe                  0x406DA0  thiscall (ECX = vector), float en ST0; no toca ECX ni EDX (hook S)
//   4 CAESound::Destructor (~CAESound)              0x4EF660  thiscall (ECX = sonido); void (hook S)
//   5 RpClumpForAllAtomics                          0x749B70  cdecl (clump, callback, data); devuelve el clump (hook S)
//   6 RpGeometryForAllMaterials                     0x74C790  cdecl (geometria, callback, data); devuelve la geometria (hook S)
//   7 RpAtomicSetFrame                              0x74BF20  cdecl (atomico, marco); devuelve el atomico (hook S)
//   8 AtomicDefaultRenderCallBack                   0x7491C0  cdecl (atomico); devuelve el atomico o 0 (hook S)
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

namespace F102AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 9;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x80, kOutWin = 0x80, kListWin = 0x400;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 102001u); }
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
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }


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



// ------------------------------------------------------------------------------------------------ valores comunes
// Un patron de 32 bits para guardar tal cual (cualquier patron: infinitos, NaN con carga, senalizantes)
inline u32 AnyBits(Gen& g, u32 cls) {
    switch (cls & 7) {
    case 0: return Coord(g);
    case 1: return Wide(g);
    case 2: return kTiny[g.R(12)];
    case 3: return kInf[g.R(4)];
    case 4: return g.W();
    case 5: return (g.W() & 0x807FFFFFu) | 0x7F800000u;
    case 6: return g.Pk({0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7FC00001u, 0xFFC00000u, 0x7FFFFFFFu, 0xFFFFFFFFu});
    default: return Special(g);
    }
}
inline u32 Mixed(Gen& g, u32 f) { return g.P(65) ? AnyBits(g, f) : AnyBits(g, g.R(8)); }
inline u32 NonZeroWord(Gen& g) { const u32 w = g.W(); return w ? w : 0x00C0FFEEu; }

// ------------------------------------------------------------------------------------------------ funcion 0: FxPrtMult_c::FxPrtMult_c(7 floats) (0x4AB290)
//   a[0..6] = rojo, verde, azul, alfa, tamano, rotacion, vida (argumentos en pila); a[7..13] = contenido previo de los siete campos (+0x00..+0x18)
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    const u32 eq = f < 8 ? 20u : 70u;
    for (u32 k = 0; k < 7; ++k) c.a[k] = Mixed(g, f);
    for (u32 k = 0; k < 7; ++k) c.a[7 + k] = g.P(eq) ? c.a[k] : Mixed(g, g.R(8));
}

// ------------------------------------------------------------------------------------------------ funcion 1: CSimpleTransform::UpdateMatrix (0x54F1B0)
//   a[0..2] = posicion (x, y, z), a[3] = rumbo; la matriz de salida es la arena 1 (SetTranslate y SetRotateZOnly se sustituyen por falsos que registran)
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 4; ++k) c.a[k] = Mixed(g, f);
}

// ------------------------------------------------------------------------------------------------ funcion 2: CSimpleTransform::UpdateRwMatrix (0x54EF40)
//   a[0..2] = posicion, a[3] = rumbo (cualquier clase salvo 1e6 .. 2^63, donde la reduccion de la x87 usa pi de 66 bits y el oraculo no la reproduce), a[4..19] = contenido previo de la RwMatrix (16 palabras)
inline u32 Heading(Gen& g, u32 f) {
    constexpr double kPi = 3.14159265358979323846;
    switch (f) {
    case 0: return B(g.Fl(-10.0f, 10.0f));
    case 1: return B(g.Fl(-6.4f, 6.4f));
    case 2: { const int k = int(g.R(2001)) - 1000; return NextUp(B(float(double(k) * kPi)), int(g.R(7)) - 3); }
    case 3: return kTiny[g.R(12)];
    case 4: return g.Pk({0x5F000000u, 0x5F000001u, 0x60000000u, 0x7F7FFFFFu, 0xDF000000u, 0xDF000001u, 0xE0000000u, 0xFF7FFFFFu, 0x5F800000u, 0x7E000000u});
    case 5: return kInf[g.R(4)];
    case 6: return g.Pk({0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA55555u, 0xFFC12345u, 0x7FFFFFFFu, 0xFFFFFFFFu});
    case 7: return B(g.Fl(-100000.0f, 100000.0f));
    case 8: return B(g.Fl(-1.0f, 1.0f) * 1.0e-4f);
    case 9: { const int k = int(g.R(801)) - 400; return NextUp(B(float(double(k) * kPi / 2.0)), int(g.R(5)) - 2); }
    case 10: return B(g.Fl(-100.0f, 100.0f));
    case 11: return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, 0xC0000000u, 0x40490FDBu, 0xC0490FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x40C90FDBu, 0x3F000000u});
    case 12: return B(g.Fl(-7.0f, 7.0f));
    case 13: return B(g.Fl(-1000.0f, 1000.0f));
    case 14: return g.P(50) ? kTiny[g.R(12)] : B(g.Fl(-1.0f, 1.0f) * 1.0e-20f);
    default: return B(g.Fl(-3.2f, 3.2f));
    }
}
inline void Make2(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 3; ++k) c.a[k] = Mixed(g, f);
    c.a[3] = Heading(g, f);
    for (u32 k = 0; k < 16; ++k) c.a[4 + k] = g.P(15) ? 0xFFFFFFFFu : g.W();
}

// ------------------------------------------------------------------------------------------------ funcion 3: CVector::SquaredMagnitudeExe (0x406DA0)
//   a[0..2] = x, y, z (el unico NaN es 0xFFC00000: el modelo del oraculo no reproduce cargas distintas)
inline u32 Comp3(Gen& g, u32 f) {
    switch (f) {
    case 0: return Coord(g);
    case 1: return Wide(g);
    case 2: return kTiny[g.R(12)];
    case 3: return Special(g);
    case 4: return g.P(50) ? Coord(g, 50.0f) : g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u});
    case 5: return kBig[g.R(6)];
    case 6: return B(g.Fl(1.0e19f, 3.0e19f) * (g.P(50) ? -1.0f : 1.0f));
    case 7: return g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x00800000u, 0x80800000u, 0x007FFFFFu, 0x3F800001u, 0x3F7FFFFFu});
    case 8: return B(g.Fl(-1.0f, 1.0f));
    case 9: return g.P(60) ? kInf[g.R(4)] : 0u;
    case 10: return g.P(50) ? 0xFFC00000u : Coord(g, 10.0f);
    case 11: return B(g.Fl(-1.0e-15f, 1.0e-15f));
    case 12: return NextUp(B(g.Fl(-100.0f, 100.0f)), int(g.R(9)) - 4);
    case 13: return B(g.Fl(-1.0e10f, 1.0e10f));
    default: return Special(g);
    }
}
inline void Make3(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    for (u32 k = 0; k < 3; ++k) c.a[k] = g.P(70) ? Comp3(g, f) : Comp3(g, g.R(16));
    if (f == 15) {           // cancelacion: x = a, y = -a, z pequeno (o iguales)
        c.a[1] = c.a[0] ^ 0x80000000u;
        if (g.P(50)) c.a[2] = c.a[0];
    }
    for (u32 k = 0; k < 3; ++k)   // el unico NaN del banco es 0xFFC00000 (el signo invertido de la cancelacion no debe crear 0x7FC00000)
        if ((c.a[k] & 0x7F800000u) == 0x7F800000u && (c.a[k] & 0x007FFFFFu)) c.a[k] = 0xFFC00000u;
    if (f == 14) c.a[2] = c.a[0];
}

// ------------------------------------------------------------------------------------------------ funcion 4: CAESound::Destructor (0x4EF660)
//   a[0] = tipo de la referencia (+8): 0 nula, 1 entidad de la arena 3, 2 valor crudo, 3 puntero al propio objeto; a[1] = valor crudo no nulo
inline void Make4(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = f < 4 ? 0u : (f < 8 ? 1u : (f < 12 ? 2u : (g.P(50) ? 3u : g.R(3))));
    if (f % 4 == 3) c.a[0] = g.R(4);
    c.a[1] = NonZeroWord(g);
}

// ------------------------------------------------------------------------------------------------ funcion 5: RpClumpForAllAtomics (0x749B70)
//   a[0] = numero de atomicos (0..5), a[1] = indice de la llamada que devuelve 0 (99: ninguna), a[2] = que hace el callback con el enlace actual (0 nada, 1 sobrescribe next, 2 sobrescribe prev),
//   a[3] = dato (crudo), a[4] = valor no nulo que devuelve el callback, a[5] = 1: el ultimo next vuelve al primero (anillo sin terminador), a[6] = basura que escribe el callback
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    const u32 count = f == 0 ? 0u : (f == 1 ? 1u : (f < 8 ? 1u + g.R(5) : g.R(6)));
    c.a[0] = count;
    u32 stop = 99;
    if (f == 3) stop = 0;
    else if (f == 4) stop = g.R(5);
    else if (f == 7) stop = 3 + g.R(5);
    else if (f >= 8 && g.P(40)) stop = g.R(6);
    c.a[1] = stop;
    c.a[2] = f == 5 ? 1u : (f == 6 ? 2u : (f >= 9 && g.P(40) ? 1u + g.R(2) : 0u));
    c.a[3] = g.W();
    c.a[4] = g.P(50) ? 1u : NonZeroWord(g);
    c.a[5] = (f == 7 || (f >= 12 && g.P(30))) ? 1u : 0u;
    c.a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x00BADC00u, g.W()});
    if (c.a[5] && c.a[2] == 1) c.a[2] = 0;   // en un anillo el callback volveria a leer un siguiente sobrescrito (salto a basura): solo se sobrescribe prev
}

// ------------------------------------------------------------------------------------------------ funcion 6: RpGeometryForAllMaterials (0x74C790)
//   a[0] = numero de materiales (int32), a[1] = indice de la llamada que devuelve 0 (99: ninguna), a[2] = que hace el callback (0 nada, 1 cambia el arreglo de materiales al segundo tras la primera llamada,
//   2 pone el numero de materiales a 0, 3 lo pone a 100), a[3] = dato (crudo), a[4..11] = arreglo 1 de materiales, a[12..19] = arreglo 2, a[20] = valor no nulo que devuelve el callback
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 cnt;
    if (f == 0) cnt = 0;
    else if (f == 1) cnt = 1;
    else if (f < 8) cnt = 1 + g.R(8);
    else cnt = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u, 100u, 0x7FFFFFFFu});
    c.a[0] = cnt;
    u32 stop = 99;
    if (f == 3) stop = 0;
    else if (f == 4) stop = g.R(8);
    else if (f >= 8 && g.P(40)) stop = g.R(8);
    if (i32(cnt) > 8 && stop == 99) stop = g.R(8);
    c.a[1] = stop;
    c.a[2] = f == 5 ? 1u : (f == 6 ? 2u : (f == 7 ? 3u : (f >= 9 && g.P(40) ? 1u + g.R(3) : 0u)));
    c.a[3] = g.W();
    for (u32 k = 0; k < 16; ++k) c.a[4 + k] = g.P(50) ? NonZeroWord(g) : g.W();
    c.a[20] = g.P(50) ? 1u : NonZeroWord(g);
}

// ------------------------------------------------------------------------------------------------ funcion 7: RpAtomicSetFrame (0x74BF20)
//   a[0] = marco (0 arena 1, 1 nulo, 2 crudo), a[1] = valor crudo, a[2] = byte +3 del atomico antes, a[3] = byte que deja _rwObjectHasFrameSetFrame (el falso) en +3
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = f < 6 ? 0u : (f < 10 ? 1u : 2u);
    if (f >= 12) c.a[0] = g.R(3);
    c.a[1] = NonZeroWord(g);
    c.a[2] = g.P(25) ? 0u : (g.P(25) ? 0xFFu : g.R(256));
    c.a[3] = g.P(25) ? 0u : (g.P(25) ? 0xFEu : g.R(256));
}

// ------------------------------------------------------------------------------------------------ funcion 8: AtomicDefaultRenderCallBack (0x7491C0)
//   a[0] = tuberia propia (0 nula: usa la del motor, 1 valor crudo), a[1] = valor crudo no nulo, a[2] = lo que devuelve RxPipelineExecute (el falso)
inline void Make8(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    c.a[0] = f < 8 ? (f % 2) : g.R(2);
    c.a[1] = NonZeroWord(g);
    const u32 r = g.R(8);
    c.a[2] = r < 3 ? 0u : (r < 5 ? 1u : (r < 6 ? 0x80000000u : NonZeroWord(g)));
    if (f % 4 == 3) c.a[2] = 0;
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
    default: Make8(c, n); break;
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
