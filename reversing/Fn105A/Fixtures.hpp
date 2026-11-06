#pragma once
// Fixtures del banco Fn105A (lote 105A): 16384 casos por funcion (163840 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//    0 TList_c::AddItem          0x4A8DF0  thiscall (ECX = lista {primero, ultimo, recuento}, nodo {anterior, siguiente}), `ret 4`; EAX = recuento anterior       (hook S, ListAddItemExe)
//    1 TList_c::RemoveAll        0x4A8EB0  thiscall (ECX = lista); tres palabras a cero; EAX = 0                                                                  (hook S, ListRemoveAllExe)
//    2 TList_c::GetNumItems      0x4A8EC0  thiscall (ECX = lista); EAX = recuento                                                                                 (hook S, ListGetNumItemsExe)
//    3 CMatrix::operator*=       0x411A80  thiscall (ECX = this, matriz*), `ret 4`; 0x59BE30, 0x59BBC0 y 0x59ACD0 sustituidas por falsos con hook (MakeCollab)   (hook S)
//    4 CVector / float           0x4119D0  cdecl (out oculto, vector*, divisor); EAX = out; multiplica por el inverso: z con el registro, y/x con el float guardado (hook S, VectorDividedByFloat)
//    5 CompressedVector a CVector 0x411590 cdecl (out, int16*); cada componente se escribe antes de leer la siguiente                                                (hook S, UncompressVectorExe)
//    6 CColPoint::operator=      0x40FC80  thiscall (ECX = this, origen*), `ret 4`; EAX = this; copia parcial (no field_C, field_1C ni +0x26/+0x27)                   (hook S, ColPointAssignExe)
//    7 vector constructor iterator 0x403D60 stdcall (base, tamano, recuento, constructor thiscall), `ret 10h`                                                       (hook S, VectorConstructorIteratorExe)
//    8 CVector::operator-=       0x406D70  thiscall (ECX = this, vector*), `ret 4`; fld + fsub + fstp por componente, leyendo la memoria viva                            (hook S)
//    9 CStreaming::SetModelTxdIsDeletable 0x409C70 cdecl (id de modelo); llama a 0x409C10 (SetModelIsDeletable, falso con hook) con TXD + 20000                        (hook S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Modo de solapamiento = (n / 16) % 8.
// Memoria: una ventana de 96 dwords (0x180 bytes) con valores explicitos en `mem`; los punteros de entrada y salida son indices de dword en esa ventana (pa, pb, po) y pueden solaparse. Las palabras
// que deben contener un puntero de la ventana (listas) se declaran en `fix` (dword, destino): en ejecucion valen direccion-de-la-ventana + 4 * destino.

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

namespace F105AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 10;
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
    u32 f0{}, f1{}, f2{};                 // escalares (divisor, desplazamiento en bytes, recuento, paso, id de modelo...)
    u32 nfix{};                           // cuantas palabras de `mem` son punteros de la ventana
    std::array<u32, 8> fixAt{}, fixTo{};  // mem[fixAt[k]] = direccion de la ventana + 4 * fixTo[k]
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

inline void Fix(C& c, u32 at, u32 to) { if (c.nfix < 8) { c.fixAt[c.nfix] = at; c.fixTo[c.nfix] = to; ++c.nfix; c.mem[at] = 0; } }
// Escribe un int16 en el byte `byteAt` de la ventana (little endian).
inline void Put16(C& c, u32 byteAt, u32 v) {
    for (u32 b = 0; b < 2; ++b) {
        const u32 w = (byteAt + b) / 4, sh = 8 * ((byteAt + b) % 4);
        c.mem[w] = (c.mem[w] & ~(0xFFu << sh)) | (((v >> (8 * b)) & 0xFFu) << sh);
    }
}
inline u32 GenInt16(Gen& g, u32 f) {
    switch (f) {
    case 0: return g.W() & 0xFFFFu;
    case 1: return u32(int(g.R(201)) - 100) & 0xFFFFu;
    case 2: return g.Pk({0u, 1u, 0xFFFFu, 2u, 0xFFFEu});
    case 3: return g.Pk({0x8000u, 0x7FFFu, 0xFFFFu, 0x8001u, 0x7FFEu, 0u});
    case 4: return (g.P(50) ? 1u << g.R(15) : (0x10000u - (1u << g.R(15)))) & 0xFFFFu;
    case 5: return u32(int(g.R(65536)) - 32768) & 0xFFFFu;
    case 6: return g.P(50) ? 0x7FFFu : 0x8000u;
    case 7: return (128u * g.R(256)) & 0xFFFFu;
    case 8: return u32(int(g.R(2001)) - 1000) & 0xFFFFu;
    case 9: return g.W() & 0xFFFFu;
    case 10: return g.Pk({0x0080u, 0xFF80u, 0x0100u, 0xFF00u, 0x007Fu, 0xFF81u});
    case 11: return g.P(70) ? 0u : (g.W() & 0xFFFFu);
    case 12: return g.P(50) ? 0xFFFFu : 1u;
    case 13: return u32(int(g.R(129)) - 64) & 0xFFFFu;
    case 14: return g.P(50) ? 0x7FFFu : (g.W() & 0xFFFFu);
    default: return g.Pk({0u, 0x8000u, 0x7FFFu, 0xFFFFu, 1u, g.W() & 0xFFFFu});
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
    const u32 sub = (c.n / 128) % 4;
    switch (fn) {
    case 0: {                   // AddItem: lista en po (tres palabras), nodo nuevo en pa, antiguo primero (si lo hay) en pb; recuento segun la familia
        LayoutM(c, g);
        static const u32 kCount[8] = {0u, 1u, 2u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 5u, 0u};
        c.mem[c.po + 2] = (f % 8 == 7) ? g.W() : kCount[f % 8];
        c.mem[c.po + 1] = 0;
        switch (c.mode) {
        case 0: c.mem[c.po] = 0; if (f & 1) c.mem[c.po + 1] = g.W(); break;                                    // lista vacia (con o sin ultimo suelto)
        case 1: Fix(c, c.po, c.pb); Fix(c, c.po + 1, c.pb); break;                                              // un elemento
        case 2: Fix(c, c.po, c.pb); if (f & 1) c.mem[c.po + 1] = g.W(); else Fix(c, c.po + 1, c.pa); break;    // primero conocido, ultimo suelto o el nuevo
        case 3: c.pa = c.po + 1 + sub; Fix(c, c.po, c.pb); break;                                               // el nodo nuevo solapa la lista
        case 4: c.pa = c.po - 1 - sub % 2; Fix(c, c.po, c.pb); break;                                           // el nodo nuevo termina dentro de la lista
        case 5: Fix(c, c.po, c.pa); break;                                                                      // primero == nodo nuevo (insercion doble)
        case 6: Fix(c, c.po, c.po); break;                                                                      // el primero es la propia lista
        default: Fix(c, c.po, c.po + 1 + sub % 2); break;                                                       // el primero apunta dentro de la lista
        }
        break;
    }
    case 1:                     // RemoveAll: la lista (po) con palabras de cualquier tipo
        LayoutM(c, g);
        c.po += c.mode;
        if (f & 1) for (u32 k = 0; k < 3; ++k) c.mem[c.po + k] = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 1u, g.W()});
        break;
    case 2:                     // GetNumItems: el recuento en po + 2
        LayoutM(c, g);
        c.po += c.mode;
        c.mem[c.po + 2] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 12345u, g.W(), g.W()});
        break;
    case 3: {                   // operator*=: this = po, matriz = pb (18 dwords cada una); modo 3: pb solapa this, 4: pb == po, 5: pb dentro de po, el resto distintas
        LayoutM(c, g);
        PutMatrix(g, c, c.po, f, nullptr);
        PutMatrix(g, c, c.pb, f, nullptr);
        switch (c.mode) {
        case 3: c.pb = c.po + 4; break;
        case 4: c.pb = c.po; break;
        case 5: c.pb = c.po >= 20 ? c.po - 16 : c.po + 16; break;   // (las dos matrices de 0x48 bytes caben siempre en la ventana)
        default: break;
        }
        break;
    }
    case 4:                     // vector / escalar: out = po, vector = pa; el divisor de la familia
        Layout(c, g, true);
        { u32 v[3]; GenVec(g, f, v, 0, nullptr); PutWords(c, c.pa, v, 3); }
        c.f0 = GenScalar(g, g.P(75) ? f : g.R(16));
        break;
    case 5: {                   // descomprimir: origen (3 int16) en el byte 4 * pa + f1 (0 o 2); salida (3 floats) en po; modos 3..7 solapan salida y origen
        LayoutM(c, g);
        c.f1 = (c.n / 64) % 2 * 2;
        switch (c.mode) {
        case 3: c.po = c.pa; break;
        case 4: c.po = c.pa + 1; break;
        case 5: c.po = c.pa - 1; break;
        case 6: c.po = c.pa - 2; break;
        case 7: c.po = c.pa + 2; break;
        default: break;
        }
        for (u32 k = 0; k < 3; ++k) Put16(c, 4 * c.pa + c.f1 + 2 * k, GenInt16(g, f));
        break;
    }
    case 6: {                   // CColPoint::operator=: this = po (con f0 bytes de desplazamiento en los modos 3..7), origen = pa (0x2C bytes cada uno)
        LayoutM(c, g);
        switch (c.mode) {
        case 3: c.po = c.pa; break;
        case 4: c.po = c.pa + 1; break;
        case 5: c.po = c.pa + 3; break;
        case 6: c.po = c.pa - 1; break;
        case 7: c.po = c.pa >= 5 ? c.pa - 5 : c.pa + 5; break;
        default: break;
        }
        c.f0 = c.mode >= 3 ? (c.n / 128) % 4 : 0;
        if (f == 1) for (u32 k = 0; k < 11; ++k) c.mem[c.pa + k] = 0xFFFFFFFFu;
        else if (f == 2) for (u32 k = 0; k < 11; ++k) c.mem[c.pa + k] = 0u;
        else if (f == 3) for (u32 k = 0; k < 11; ++k) c.mem[c.pa + k] = 0x01010101u * (1 + k);
        break;
    }
    case 7: {                   // iterador: base = pa, paso f0, recuento f1 (con signo; nunca mas de 64 vueltas ni INT_MIN)
        LayoutM(c, g);
        static const u32 kStride[16] = {4u, 8u, 0xCu, 0x2Cu, 0u, 0x10u, 0xFFFFFFFCu, 0xFFFFFFF0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0u, 0x180u, 0x100000u, 0u, 2u};
        static const i32 kCnt[22] = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 7, 8, 13, 16, 17, 31, 32, 33, 40, 48, 64, -1000, -0x7FFFFFF0};
        c.f0 = f == 11 ? g.R(65) : (f == 14 ? g.W() : kStride[f]);
        c.f1 = u32(kCnt[g.R(22)]);
        c.f2 = g.W();
        break;
    }
    case 8: {                   // operator-=: this = po (vector), otro = pa; solapamientos como Layout
        Layout(c, g, true);
        u32 v[3], w[3];
        GenVec(g, f, v, 0, nullptr);
        GenVec(g, f, w, 1, g.P(25) ? v : nullptr);
        PutWords(c, c.po, v, 3);
        if (c.pa != c.po && !(c.mode == 6 || c.mode == 7)) PutWords(c, c.pa, w, 3);
        break;
    }
    default: {                  // 9: SetModelTxdIsDeletable: id de modelo f0; la entrada de la tabla apunta al objeto en pa; el TXD (int16) esta en el byte +0xA del objeto
        LayoutM(c, g);
        switch (f % 4) {
        case 0: c.f0 = g.R(16); break;
        case 1: c.f0 = 16 + g.R(285); break;
        case 2: c.f0 = 19990 + g.R(10); break;
        default: c.f0 = g.R(20000); break;
        }
        static const u32 kTxd[16] = {0u, 0xFFFFu, 0x7FFFu, 0x8000u, 1u, 0x0100u, 0x4E20u, 0xB1E0u, 5000u, 0xEC78u, 0x00FFu, 0xFF00u, 0x1234u, 0xEDCCu, 0x7000u, 0x8001u};
        const u32 txd = (f / 4 == 0 || f % 3 == 0) ? kTxd[g.R(16)] : (g.W() & 0xFFFFu);
        c.mem[c.pa + 2] = (txd << 16) | (c.mem[c.pa + 2] & 0xFFFFu);
        break;
    }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("mode", c.mode).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("pa", c.pa).U("pb", c.pb).U("po", c.po).U("f0", c.f0).U("f1", c.f1).U("f2", c.f2);
    std::vector<u32> fix;
    for (u32 k = 0; k < c.nfix; ++k) { fix.push_back(c.fixAt[k]); fix.push_back(c.fixTo[k]); }
    j.A("fix", fix);
    j.A("mem", std::vector<u32>(c.mem.begin(), c.mem.end()));
    return j.End();
}
}
