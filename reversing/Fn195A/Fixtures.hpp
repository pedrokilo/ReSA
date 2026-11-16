#pragma once
// Fixtures del banco Fn195A (lote 195A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 31 reconstruidas (de 32 asignadas: 0x49B0E0 saltada).
//   idx  direccion  nombre real                                                                         forma
//    0   0x66D550   CPatrolRoute::Reverse (el listado decia CTaskComplexInvestigateDisturbance)         thiscall() `ret`: invierte el orden de los nodos (puntos y dos cadenas por nodo, con nodo temporal de pila y copia hasta el NUL)
//    1   0x66D6D0   CTaskSimpleGoTo destructor con borrado (ranura 0 de 0x86FD1C)                       thiscall(flags) `ret 4`: call 667A00; delete this (61A5B0) si bit 0 del BYTE; eax = this
//    2   0x66D6F0   CTaskSimpleGoToPoint destructor con borrado (ranura 0 de 0x86FD50)                  thiscall(flags) `ret 4`: call 667D50; delete this (61A5B0) si bit 0 del BYTE; eax = this
//    3   0x674BB0   CTaskComplexFollowPatrolRoute::Clone (ranura 1 de 0x870114)                         thiscall() `ret` con marco SEH: new(30h) (61A5A0); ctor 674930 con cinco argumentos de pila; eax = el del constructor
//    4   0x6D0BC0   CVehicle::SetRemapTexDictionary                                                      thiscall(txd) `ret 4`: compara con [+598h] (movsx); si -1: [+59Ch]=0, RemoveRef (731A30), [+598h]=-1; [+59Ah]=txd
//    5   0x6D3D50   CVehicle::FindWheelWidth (cuerpo por defecto, ranura 63)                             thiscall(bool) `ret 4`: fld [858C84h] (0.25f)
//    6   0x6D56B0   CVehicle::GetDooorAngleOpenRatio (eDoors, ranura 30)                                 thiscall(eDoors) `ret 4`: fld [858B50h] (0.0f)
//    7   0x6D5DD0   CPool<CVehicle,CHeli>::CreateAtRef (el listado decia CVehicle)                       thiscall(ref) `ret 4`: estado[ref >> 8] = ref & 7Fh; cursor +0Ch = primer hueco libre desde 0
//    8   0x6D5E20   CAutoPilot::Constructor (el listado decia CVehicle)                                 thiscall() `ret`: rellena campos (ancho exacto) y llama a rand (821B1E); eax = this
//    9   0x6D6270   CVehicle::ProcessControlCollisionCheck (por defecto, ranura 23)                      thiscall(bool) `ret 4`
//   10   0x6D6280   CVehicle::ProcessControlInputs (ranura 24)                                          thiscall(uint8) `ret 4`
//   11   0x6D6290   CVehicle::GetComponentWorldPosition (ranura 25)                                     thiscall(int, CVector&) `ret 8`
//   12   0x6D62A0   CVehicle::IsComponentPresent (ranura 26)                                             thiscall(int) `xor al,al; ret 4`
//   13   0x6D62B0   CVehicle::IsDoorReady (eDoors, ranura 32)                                            thiscall(eDoors) `xor al,al; ret 4`
//   14   0x6D62C0   CVehicle::IsDoorFullyOpen (eDoors, ranura 34)                                        idem
//   15   0x6D62D0   CVehicle::IsDoorClosed (eDoors, ranura 36)                                           idem
//   16   0x6D62E0   CVehicle::IsDoorMissing (eDoors, ranura 38)                                          idem
//   17   0x6D62F0   CVehicle::IsDoorReadyU32 (ranura 31)                                                 idem
//   18   0x6D6300   CVehicle::IsDoorFullyOpenU32 (ranura 33)                                             idem
//   19   0x6D6310   CVehicle::IsDoorClosedU32 (ranura 35)                                                idem
//   20   0x6D6320   CVehicle::IsDoorMissingU32 (ranura 37)                                               idem
//   21   0x6D6330   CVehicle::RemoveRefsToVehicle (ranura 40)                                            thiscall(CEntity*) `ret 4`
//   22   0x6D6340   CVehicle::BlowUpCar (ranura 41)                                                      thiscall(CEntity*, bool) `ret 8`
//   23   0x6D6350   CVehicle::SetUpWheelColModel (ranura 43)                                             thiscall(CColModel*) `xor al,al; ret 4`
//   24   0x6D6360   CVehicle::BurstTyre (ranura 44)                                                      thiscall(uint8, bool) `xor al,al; ret 8`
//   25   0x6D6370   CVehicle::IsRoomForPedToLeaveCar (ranura 45)                                         thiscall(uint32, CVector*) `xor al,al; ret 8`
//   26   0x6D6380   CVehicle::SetupSuspensionLines (ranura 48)                                           thiscall() `ret`
//   27   0x6D6390   CVehicle::Fix (ranura 50)                                                            thiscall() `ret`
//   28   0x6D63A0   CVehicle::SetupDamageAfterLoad (ranura 51)                                            thiscall() `ret`
//   29   0x6D63B0   CVehicle::DoBurstAndSoftGroundRatios (ranura 52)                                      thiscall() `ret`
//   30   0x6D63C0   CVehicle::PlayCarHorn (ranura 54)                                                    thiscall() `ret`
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F195AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 31;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
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
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}

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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 40> a{};
    std::vector<u8> s;          // imagen en bytes de CPatrolRoute desde +4 (solo Reverse)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Banderas de la pila (el BYTE bajo decide; el resto es ruido)
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un valor de puerta (eDoors) o de componente: pequenos, bordes y ruido
inline u32 SmallArg(Gen& g) {
    const u32 w = g.W();
    return g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0xFFFFFFFFu, 0x80000000u, 0x100u, 0xFFu, w, w, g.R(64), g.R(64)});
}

// Clases de funcion (forma)
enum Kind : u32 { kReverse, kSdd, kClone, kRemap, kFloatConst, kCreateAt, kApCtor, kStubVoid, kStubBool };
inline constexpr std::array<Kind, kFunctions> kKind{kReverse, kSdd, kSdd, kClone, kRemap, kFloatConst, kFloatConst, kCreateAt, kApCtor,
    kStubVoid, kStubVoid, kStubVoid, kStubBool, kStubBool, kStubBool, kStubBool, kStubBool, kStubBool, kStubBool, kStubBool, kStubBool,
    kStubVoid, kStubVoid, kStubBool, kStubBool, kStubBool, kStubVoid, kStubVoid, kStubVoid, kStubVoid, kStubVoid};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x66D550, 0x66D6D0, 0x66D6F0, 0x674BB0, 0x6D0BC0, 0x6D3D50, 0x6D56B0, 0x6D5DD0, 0x6D5E20,
    0x6D6270, 0x6D6280, 0x6D6290, 0x6D62A0, 0x6D62B0, 0x6D62C0, 0x6D62D0, 0x6D62E0, 0x6D62F0, 0x6D6300, 0x6D6310, 0x6D6320,
    0x6D6330, 0x6D6340, 0x6D6350, 0x6D6360, 0x6D6370, 0x6D6380, 0x6D6390, 0x6D63A0, 0x6D63B0, 0x6D63C0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x200, 4, 4, 0x20, 0x5A0, 4, 4, 0x10, 0x98,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
// Argumentos de pila de los cuerpos por defecto: [numero, bytes que limpia la funcion, 1 = el argumento i es un puntero a la arena B]
struct StubSig { u32 nargs; u32 ptrMask; };
inline constexpr std::array<StubSig, kFunctions> kStub{{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {1, 0}, {1, 0}, {2, 2}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0},
    {1, 0}, {2, 0}, {1, 0}, {2, 0}, {2, 2}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}};
// Memoria global del juego tocada
inline constexpr u32 kFlag = 0xA9B0B0, kClock = 0xB7CB84, kPoolMaxCap = 0x1F0;
inline constexpr u32 kNodeBase = 4, kNodeStride = 0x28, kPoints = 0x144, kReverseSpan = 0x200, kImage = 0x1F0;   // la imagen `s` cubre [4, kImage)

// Estado inicial (byte) del hueco i de un pool falso: 7 bits bajos aleatorios; bit 7 (libre) con un 30%
inline u32 SlotByte(u32 seed, u32 i) {
    const u32 s = Rand(seed, i);
    return (s & 0x7Fu) | ((((s >> 8) % 100u) < 30u) ? 0x80u : 0u);
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this] (en Reverse: el contador), a[36..39] = semillas de las arenas A, B, C y D
//   kReverse: a[3] = contador (con signo), s = imagen de los bytes [4, 0x1F0) de la ruta (nodos, puntos y, si el contador pasa de 8, los nodos 8..11 sobre los puntos)
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kClone: a[4] = 0 sin memoria / 1 con memoria, a[5] = desplazamiento de la memoria en B, a[6] = EAX del falso del constructor, a[7] = dword que deja en [mem], a[8] = dword en +0Ch, a[9] = dword en +10h,
//           a[10] = dword en +14h, a[11] = dword en +18h, a[12] = puntero de ruta crudo (+1Ch), a[13] = 1 si la ruta es un puntero a la arena C, a[14] = desplazamiento en C
//   kRemap: a[4] = dword en +598h (palabra anterior y palabra +59Ah), a[5] = dword en +59Ch, a[6] = argumento (txd)
//   kFloatConst: a[4] = argumento de pila
//   kCreateAt: A: objeto del pool en a[0] (+0 almacenamiento crudo a[9], +4 estados, +8 capacidad, +0Ch cursor a[6]); B: estados en a[4]; a[5] = capacidad; a[7] = indice; a[8] = byte bajo de la referencia; a[10] = semilla de los estados
//   kApCtor: a[4] = reloj (0xB7CB84), a[5] = lo que devuelve rand (0x821B1E), a[6] = dword en +48h, a[7] = dword en +4Ch
//   kStubVoid / kStubBool: a[4], a[5] = argumentos crudos de pila, a[6] = desplazamiento en B de los argumentos puntero
inline bool IsNameChar(u8 c) { return c >= 0x21 && c <= 0x7E; }
inline void MakeReverse(C& c, Gen& g) {
    auto& a = c.a;
    const u32 cnt = g.Pk({0u, 1u, 2u, 2u, 3u, 3u, 4u, 5u, 6u, 7u, 7u, 8u, 8u, 8u, 8u, 8u, 9u, 10u, 12u, 0xFFFFFFFFu, 0x80000001u, 0xFFFFFFF0u});
    a[3] = cnt;
    const bool over = cnt >= 9 && cnt <= 12;     // los nodos 8.. caen sobre los puntos: solo cadenas cortas (el EXE no desborda el nodo temporal de pila)
    std::vector<u8> img(kImage - 4);
    for (std::size_t i = 0; i < img.size(); ++i) img[i] = u8(g.W());
    auto str = [&](u32 off, u32 field, u32 type) {   // type 0 corta, 1 vacia, 2 larga, 3 maxima con NUL, 4 sin NUL (solo name1), 5 cualquier longitud
        const u32 len = type == 0 ? 1 + g.R(7) : (type == 1 ? 0 : (type == 2 ? 8 + g.R(field - 8) : (type == 3 ? field - 1 : g.R(field))));
        if (type == 4) { for (u32 i = 0; i < field; ++i) img[off + i - 4] = u8(0x21 + g.R(0x5E)); return; }
        for (u32 i = 0; i < len; ++i) img[off + i - 4] = u8(0x21 + g.R(0x5E));
        img[off + len - 4] = 0;                 // despues del NUL queda basura aleatoria (no se intercambia)
    };
    for (u32 k = 0; k < 8; ++k) {
        const u32 n1 = kNodeBase + kNodeStride * k, n2 = n1 + 0x18;
        const u32 t1 = over ? g.Pk({0u, 0u, 0u, 1u}) : g.Pk({0u, 0u, 1u, 2u, 2u, 3u, 4u, 5u, 5u});
        const u32 t2 = over ? g.Pk({0u, 0u, 1u}) : g.Pk({0u, 0u, 1u, 2u, 3u, 5u});
        str(n1, 0x18, t1);
        str(n2, 0x10, t2 == 5 ? 5 : t2);
    }
    if (over) {
        for (u32 off = kPoints; off + 4 <= kImage; off += 4) {
            const u32 w = g.W() & 0x00FFFFFFu;
            std::memcpy(&img[off - 4], &w, 4);
        }
    }
    c.s = img;
}
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x86FD1Cu, 0x86FD50u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kReverse:
        MakeReverse(c, g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x30);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), Raw(g), 1u});
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 0xFFFFu, 0x8000u, 0x7FFFu, 0xFFFF8000u, g.W(), g.W(), g.R(0x10000)});
        a[9] = g.Pk({0u, 0xFFFFu, 0x8000u, 0x7FFFu, 0xFFFF8000u, g.W(), g.W(), g.R(0x10000)});
        a[10] = Raw(g); a[11] = Raw(g);
        a[12] = Raw(g);
        a[13] = g.Pk({0u, 1u});
        a[14] = AuxOffset(g, 0x10);
        break;
    case kRemap: {
        const u32 w = g.Pk({0xFFFFu, 0xFFFFu, 0u, 1u, 0x8000u, 0xFFFEu, g.R(0x2000), g.R(0x10000), g.R(0x10000)});   // palabra anterior (+598h)
        const u32 hi = g.W() & 0xFFFF0000u;
        a[4] = hi | w;
        a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), Raw(g)});
        const u32 sw = (w & 0x8000u) ? (w | 0xFFFF0000u) : w;   // la palabra con signo
        const u32 mode = g.R(10);
        a[6] = mode < 3 ? sw : (mode < 5 ? 0xFFFFFFFFu : (mode == 5 ? (w | 0x10000u) : (mode == 6 ? g.Pk({0x1FFFFu, 0xFFFFu, 0xFFFF0000u}) : (mode == 7 ? g.R(0x2000) : (mode == 8 ? g.R(0x10000) : g.W())))));
        break;
    }
    case kFloatConst:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), Raw(g), 2u, 3u, 4u, 5u});
        break;
    case kCreateAt: {
        const u32 cap = 0x100u + g.R(kPoolMaxCap - 0x100u + 1);
        a[4] = 4u * g.R((u32(kWin) - kPoolMaxCap - 4u) / 4u);
        a[5] = cap;
        a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(cap), g.R(cap), cap, cap - 1u});   // cursor inicial
        a[7] = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), g.R(cap), 1u, cap / 2u});                           // indice
        a[8] = g.Pk({0u, 0x7Fu, 0x80u, 0xFFu, g.R(256), g.R(256), g.R(256), 1u, 0x40u});                             // byte bajo de la referencia
        a[9] = Raw(g);
        a[10] = g.W();
        break;
    }
    case kApCtor:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.W(), 1000u});
        a[5] = g.Pk({0u, 1u, 7u, 8u, 9u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.W(), 0x7FFFFFFFu, 0xF8u, 0x1007u});
        a[6] = g.Pk({0u, 0xFFFFFFFFu, 0xFF000000u, 0x04000000u, 0x03000000u, 0x07000000u, 0xFC000000u, g.W(), g.W(), g.W()});
        a[7] = g.Pk({0u, 0xFFFFFFFFu, 0xFFu, 0x03u, 0xFCu, 0x01u, 0x02u, g.W(), g.W(), g.W()});
        break;
    case kStubVoid:
    case kStubBool:
        a[4] = kStub[c.fn].nargs ? SmallArg(g) : 0u; a[5] = kStub[c.fn].nargs > 1 ? Raw(g) : 0u;
        a[6] = AuxOffset(g, 0x10);
        if (c.fn == 13 || c.fn == 14 || c.fn == 15 || c.fn == 16) a[4] = SmallArg(g);
        break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.H("s", c.s);
    return j.End();
}
}
