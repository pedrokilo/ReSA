#pragma once
// Fixtures del banco Fn100A (lote 100A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTask::IsGoToTask                              0x61A360  cdecl (tarea) -> bool en AL: GetTaskType (ranura 4) == 900 o, en una segunda llamada, == 926      (hook S)
//   1 CMemoryMgr::Free                               0x72F430  cdecl (puntero); salta a la rutina free del CRT del EXE (0x82413F), sin comprobar nulo            (hook S)
//   2 CEvent::operator new                           0x4B5620  cdecl (tamano ignorado) -> CEventPool::New() sobre el pool de eventos (puntero global 0xB744AC)  (hook S)
//   3 CObject::operator new                          0x5A1EE0  cdecl (tamano ignorado) -> CObjectPool::New() sobre el pool de objetos (0xB7449C)             (hook S)
//   4 CVehicle::operator new(tamano, ref)            0x6E2D60  cdecl (tamano ignorado, ref) -> NewAt(ref) sobre el pool de vehiculos (0xB74494)              (hook S)
//   5 CWaterLevel: lerp de CRenPar (LerpRenPar)      0x6E5280  cdecl (destino oculto, rp1, rp2, t) -> destino en EAX; (1 - t) * a + t * b                      (hook S)
//   6 CEventGroupEvent::CEventGroupEvent             0x4ADFD0  thiscall (ped, evento), `ret 8`; devuelve this                                                   (hook S)
//   7 CEventGroupEvent::~CEventGroupEvent            0x4AE070  thiscall (sin argumentos)                                                                        (hook S)
//   8 CEventDamage::~CEventDamage                    0x4AD960  thiscall (sin argumentos)                                                                        (hook S)
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

namespace F100AFix {
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
constexpr std::size_t kObjWin = 0x40, kOutWin = 0x10, kPoolWin = 0x20, kStatesWin = 0x10, kStorageWin = 0x5140, kEvWin = 0x20, kPedWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
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

// ------------------------------------------------------------------------------------------------ utilidades de los generadores
inline constexpr std::array<u32, 10> kRawPtrs{1u, 0xFFFFFFFFu, 0x80000000u, 0xCDCDCDCDu, 0x00C0FFEEu, 0x7FFFFFFFu, 0x00010000u, 0xFDFDFDFDu, 0x00000004u, 0x0BADF00Du};
inline u32 RawPtr(Gen& g) { return g.P(60) ? kRawPtrs[g.R(10)] : g.W() | 1u; }
inline u32 NanBits(Gen& g) { return g.P(50) ? kNans[g.R(10)] : (0x7F800000u | (g.W() & 0x807FFFFFu) | 1u); }

// ------------------------------------------------------------------------------------------------ funcion 0: CTask::IsGoToTask (0x61A360)
//   a[0] = tipo devuelto por la primera llamada a GetTaskType, a[1] = tipo de la segunda
inline void Make0(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 t1, t2;
    auto any = [&] { return g.P(70) ? g.R(1500) : g.W(); };
    auto notGo = [&] { u32 v; do { v = g.P(70) ? g.R(1500) : g.W(); } while (v == 900u || v == 926u); return v; };
    switch (f) {
    case 0: t1 = 900; t2 = any(); break;                                         // primera coincide: una sola llamada
    case 1: t1 = notGo(); t2 = 926; break;                                       // segunda coincide
    case 2: t1 = notGo(); t2 = notGo(); break;                                   // ninguna
    case 3: t1 = 926; t2 = g.P(50) ? 926u : notGo(); break;                      // 926 en la primera no cuenta
    case 4: t1 = 900; t2 = 926; break;
    case 5: t1 = g.Pk({899u, 901u, 0x383u, 0x385u, 898u, 902u}); t2 = g.Pk({925u, 927u, 0x39Du, 0x39Fu, 924u, 928u}); break;
    case 6: t1 = 900u | (1u << g.R(32)); t2 = 926u | (1u << g.R(32)); if (g.P(30)) t2 = 926; break;   // bits altos distintos de cero
    case 7: t1 = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0u, 1u, 0xFFFFFFFEu}); t2 = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0u, 926u, 0x0000039Eu}); break;
    case 8: t1 = notGo(); t2 = 900; break;                                       // 900 en la segunda no cuenta
    case 9: t1 = 900; t2 = 900; break;
    case 10: t1 = 926; t2 = 900; break;
    case 11: t1 = g.R(2000); t2 = g.R(2000); break;                              // rango de tipos de tarea
    case 12: t1 = g.P(50) ? 900u : notGo(); t2 = g.P(50) ? 926u : notGo(); break;
    case 13: t1 = 0x384u + g.R(3) - 1; t2 = 0x39Eu + g.R(3) - 1; break;
    default: t1 = any(); t2 = any(); break;
    }
    c.a[0] = t1; c.a[1] = t2;
}

// ------------------------------------------------------------------------------------------------ funcion 1: CMemoryMgr::Free (0x72F430)
//   a[0] = clase del puntero (0 arena, 1 nulo, 2 crudo), a[1] = valor crudo, a[2] = desplazamiento dentro de la arena (0..0x20)
inline void Make1(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n);
    u32 kind;
    switch (f % 4) { case 0: kind = 0; break; case 1: kind = 1; break; case 2: kind = 2; break; default: kind = g.R(3); break; }
    c.a[0] = kind;
    c.a[1] = RawPtr(g);
    c.a[2] = (f < 8) ? g.R(0x21) : (g.P(50) ? 0u : g.R(0x21));
}

// ------------------------------------------------------------------------------------------------ funciones 2, 3 y 4: operator new de pool (0x4B5620, 0x5A1EE0, 0x6E2D60)
//   a[0] = tamano (ignorado), a[1] = capacidad, a[2] = ultima ranura libre (int32), a[3..10] = bytes de estado de las ranuras (bit 7 = vacia, bits 0..6 = contador), a[11] = ref (solo la funcion 4)
inline void MakePool(C& c, unsigned n, bool withRef) {
    const u32 f = c.family;
    Gen g(n, 200);
    const u32 cap = withRef ? 2 + g.R(7) : 1 + g.R(8);
    u32 st[8];
    for (u32 i = 0; i < 8; ++i) st[i] = g.R(256);
    auto setEmpty = [&](u32 i, bool e) { st[i] = e ? (st[i] | 0x80u) : (st[i] & 0x7Fu); };
    int last = int(g.R(cap + 1)) - 1;                      // -1 .. cap - 1
    switch (f) {
    case 0: for (u32 i = 0; i < cap; ++i) setEmpty(i, true); last = -1; break;
    case 1: for (u32 i = 0; i < cap; ++i) setEmpty(i, true); break;
    case 2: for (u32 i = 0; i < cap; ++i) setEmpty(i, false); break;                        // llena (New devuelve nulo; NewAt: se libera la ranura del ref mas abajo)
    case 3: { const u32 k = g.R(cap); for (u32 i = 0; i < cap; ++i) setEmpty(i, i == k); break; }
    case 4: { const u32 k = g.R(cap); for (u32 i = 0; i < cap; ++i) setEmpty(i, i == k); last = int(k); break; }
    case 5: { const u32 k = g.R(cap); for (u32 i = 0; i < cap; ++i) setEmpty(i, i == k); last = int(k) - 1; break; }
    case 6: for (u32 i = 0; i < cap; ++i) setEmpty(i, i == 0); last = int(cap) - 1; break;
    case 7: for (u32 i = 0; i < cap; ++i) { st[i] = (g.P(70) ? 0x7Fu : g.R(128)) | (g.P(60) ? 0x80u : 0u); } break;   // contadores en el tope
    case 8: for (u32 i = 0; i < cap; ++i) setEmpty(i, g.P(30)); break;
    case 9: for (u32 i = 0; i < cap; ++i) setEmpty(i, g.P(70)); break;
    case 10: for (u32 i = 0; i < cap; ++i) st[i] = g.P(50) ? 0x80u : 0u; break;              // contadores a cero
    case 11: for (u32 i = 0; i < cap; ++i) st[i] = g.P(50) ? 0xFFu : 0x7Fu; break;
    default: break;                                                                          // aleatorio
    }
    c.a[0] = g.P(50) ? g.Pk({0x44u, 0x19Cu, 0xA18u, 0x20u, 0u}) : g.W();
    c.a[1] = cap;
    c.a[2] = u32(last);
    for (u32 i = 0; i < 8; ++i) c.a[3 + i] = st[i];
    if (withRef) {
        const u32 idx = g.R(cap);
        // ranura de destino libre y, al menos, otra libre despues de ocupar la del ref (el EXE leeria mas alla de la capacidad con el pool lleno)
        c.a[3 + idx] |= 0x80u;
        bool other = false;
        for (u32 i = 0; i < cap; ++i) other |= (i != idx) && (c.a[3 + i] & 0x80u);
        if (!other) c.a[3 + (idx + 1 + g.R(cap - 1)) % cap] |= 0x80u;
        const u32 low = f == 13 ? g.Pk({0x7Fu, 0x80u, 0xFFu, 0u, 1u}) : g.R(256);
        c.a[11] = (idx << 8) | low;
    }
}
inline void Make2(C& c, unsigned n) { MakePool(c, n, false); }
inline void Make3(C& c, unsigned n) { MakePool(c, n, false); }
inline void Make4(C& c, unsigned n) { MakePool(c, n, true); }

// ------------------------------------------------------------------------------------------------ funcion 5: CWaterLevel LerpRenPar (0x6E5280)
//   a[0..3] = rp1 (z, bigWaves, smallWaves, palabra de flujo + relleno), a[4..7] = rp2, a[8] = t, a[9..12] = dwords iniciales del destino
inline u32 DistinctNan(Gen& g, u32 salt) { return (g.P(50) ? 0x80000000u : 0u) | 0x7F800000u | (1u + ((g.W() % 0x3FFF00u) & ~0xFFu) + salt) % 0x7FFFFFu; }
inline void Make5(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 300);
    u32 v[9];
    for (u32 i = 0; i < 9; ++i) v[i] = Coord(g, 1000.0f);
    v[8] = B(g.Fl(0.0f, 1.0f));
    auto big = [&] { return g.Pk({0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u, 0x7F400000u}); };
    auto dataIdx = [&](u32 k) { return (k % 4 == 3) ? k - 3 : k; };   // 3 y 7 son palabras de flujo: se desvian a z
    switch (f) {
    case 0: break;
    case 1: v[8] = g.Pk({0u, 0x3F800000u, 0x80000000u, 0xBF800000u}); break;                              // t = 0, 1, -0, -1
    case 2: v[8] = B(g.P(50) ? g.Fl(1.0f, 5.0f) : g.Fl(-4.0f, 0.0f)); break;                              // extrapolacion
    case 3: for (u32 i = 0; i < 8; ++i) if (i % 4 != 3) v[i] = Wide(g); v[8] = B(g.Fl(-2.0f, 3.0f)); break;
    case 4: for (u32 i = 0; i < 8; ++i) if (i % 4 != 3) v[i] = kTiny[g.R(12)]; if (g.P(50)) v[8] = kTiny[g.R(12)]; break;
    case 5: for (u32 i = 0; i < 8; ++i) if (i % 4 != 3) v[i] = g.P(70) ? big() : Coord(g, 100.0f); v[8] = g.P(50) ? B(g.Fl(-3.0f, 4.0f)) : big(); break;   // desbordes intermedios del x87
    case 6: for (u32 i = 0; i < 8; ++i) if (i % 4 != 3 && g.P(40)) v[i] = kInf[g.R(4)]; if (g.P(40)) v[8] = g.Pk({0x7F800000u, 0xFF800000u, 0u, 0x3F800000u}); break;
    case 7: { v[dataIdx(g.R(9))] = NanBits(g); if (g.P(30)) v[dataIdx(g.R(9))] = 0xFFC00000u; break; }       // un NaN (mismo patron si se repite)
    case 8: {                                                                                             // varios NaN con significandos distintos
        u32 salt = 0;
        const u32 count = 2 + g.R(3);
        for (u32 j = 0; j < count; ++j) v[dataIdx(g.R(9))] = DistinctNan(g, 0x100u * (++salt));
        break;
    }
    case 9: for (u32 i = 0; i < 9; ++i) if (i % 4 != 3 && g.P(60)) v[i] = g.Pk({0u, 0x80000000u}); break;       // ceros con signo
    case 10: { const float t = g.Fl(0.1f, 0.9f); v[8] = B(t); const u32 a = B(g.Fl(-50.0f, 50.0f)); v[0] = a; v[4] = B(-F(a) * (1.0f - t) / t); v[1] = v[5]; v[2] = v[6]; break; }   // casi cancelacion
    case 11: v[8] = NextUp(B(g.Fl(0.0f, 1.0f) * 1e-6f), int(g.R(5)) - 2); if (g.P(50)) v[8] = g.Pk({0x33800000u, 0x33000000u, 0x34000000u, 0x32800000u}); break;   // t diminuto (1 - t = 1)
    case 12: v[8] = NextUp(0x3F800000u, int(g.R(9)) - 4); break;                                          // t = 1 +- ulps
    case 13: v[8] = g.Pk({0x3F000000u, 0x3E800000u, 0x3F400000u, 0x3EAAAAABu, 0x3F2AAAABu}); break;       // fracciones tipicas
    case 14: for (u32 i = 0; i < 9; ++i) if (i % 4 != 3) v[i] = Special(g); break;
    default: for (u32 i = 0; i < 8; ++i) if (i % 4 != 3) v[i] = Coord(g, 3000.0f); v[8] = B(g.Fl(-2.0f, 3.0f)); break;
    }
    for (u32 i = 0; i < 9; ++i) c.a[i] = v[i];
    c.a[3] = g.W();                                                                                       // flujo y relleno: el EXE no los lee
    c.a[7] = g.W();
    for (u32 i = 0; i < 4; ++i) c.a[9 + i] = g.P(20) ? g.Pk({0u, 0xFFFFFFFFu, 0xCDCDCDCDu, 0x80000000u}) : g.W();   // dwords iniciales del destino (z, bigWaves, smallWaves, flujo + relleno)
}

// ------------------------------------------------------------------------------------------------ funcion 6: CEventGroupEvent::CEventGroupEvent (0x4ADFD0)
//   a[0] = clase del ped (0 arena, 1 nulo, 2 crudo), a[1] = ped crudo, a[2] = clase del evento, a[3] = evento crudo, a[4] = desplazamiento de this (multiplo de 4, 0..0x1C),
//   a[5] = relleno (0 ruido, 1 ceros, 2 0xFF, 3 0xCD), a[6..10] = cinco dwords iniciales del objeto (vtable, +4, +8, +0xC, +0x10)
inline void MakeObjWords(C& c, Gen& g, unsigned first) {
    for (unsigned i = 0; i < 5; ++i) c.a[first + i] = g.P(25) ? g.Pk({0u, 0xFFFFFFFFu, 0xCDCDCDCDu, 0xA5A5A5A5u}) : g.W();
}
inline void Make6(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 400);
    u32 pk = f % 3 == 0 ? 0 : (f % 3 == 1 ? g.Pk({0u, 1u}) : g.R(3));
    if (f == 15) pk = 2;
    const u32 ek = g.R(3);
    c.a[0] = pk; c.a[1] = RawPtr(g); c.a[2] = ek; c.a[3] = RawPtr(g);
    c.a[4] = 4 * g.R(8);
    c.a[5] = g.R(4);
    MakeObjWords(c, g, 6);
}

// ------------------------------------------------------------------------------------------------ funcion 7: CEventGroupEvent::~CEventGroupEvent (0x4AE070)
//   a[0] = clase del ped, a[1] = ped crudo, a[2] = clase del evento (0 arena con vtable falsa, 1 nulo), a[3] = desplazamiento de this, a[4] = relleno, a[5..9] = dwords iniciales (el +0xC y el +0x10 los fijan las clases)
inline void Make7(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 500);
    const u32 pk = f % 3 == 0 ? 0 : (f % 3 == 1 ? g.Pk({0u, 1u}) : g.R(3));
    c.a[0] = pk; c.a[1] = RawPtr(g);
    c.a[2] = (f & 4) ? 1u : 0u;
    if (f % 5 == 0) c.a[2] = g.R(2);
    c.a[3] = 4 * g.R(8);
    c.a[4] = g.R(4);
    MakeObjWords(c, g, 5);
}

// ------------------------------------------------------------------------------------------------ funcion 8: CEventDamage::~CEventDamage (0x4AD960)
//   a[0] = clase de la entidad de origen (0 arena, 1 nula, 2 cruda), a[1] = valor crudo, a[2] = desplazamiento de this, a[3] = relleno, a[4..8] = dwords iniciales (objeto +0 .. +0x10)
inline void Make8(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 600);
    const u32 ek = f % 3 == 0 ? 0 : (f % 3 == 1 ? g.Pk({0u, 1u}) : g.R(3));
    c.a[0] = ek; c.a[1] = RawPtr(g);
    c.a[2] = 4 * g.R(8);
    c.a[3] = g.R(4);
    MakeObjWords(c, g, 4);
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
