#pragma once
// Fixtures del banco Fn183A (lote 183A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas y enganchadas en el commit 89eaf43558, SIN validar; las clases del listado salen del desensamblado y de las vtables, ver README.md y fixture_oracle.py). 19 funciones.
//   idx  direccion  nombre real                                                                      forma
//    0   0x4D40A0   CLinkList<CAnimBlendHierarchy*>::Shutdown (listado: CAnimManager)               thiscall() `ret`: operator delete (0x8214BD) de [this+30h]; despues [this+30h] = 0
//    1   0x4D40C0   CLinkList<CAnimBlendHierarchy*>::Insert(T const&)                               thiscall(const T*) `ret 4`: enlace libre -> usado; devuelve el enlace o 0
//    2   0x4D56F0   ConvertBoneTag2BoneName (RpAnimBlend)                                            cdecl(dword) `ret`: puntero a cadena del EXE o nulo (se compara el CONTENIDO)
//    3   0x4D5980   CPad::IsEnterJustPressed (static)                                                 cdecl() `ret`: AL; Enter y Enter del teclado numerico (palabras de NewKeyState / OldKeyState)
//    4   0x4D59B0   CPad::IsStandardKeyJustPressed                                                   thiscall(BYTE) `ret 4`: EAX completo; NO lee this; arrays estaticos de teclas estandar
//    5   0x4D59E0   CPad::IsCrossPressed                                                              thiscall() `ret`: EAX completo; ButtonCross de NewState (+20h) y OldState (+50h) (palabras)
//    6   0x637300   CTaskSimpleHitFromFront destructor con borrado (vtable 0x86E5EC)                  thiscall(flags) `ret 4`: cuerpo 0x637320 (jmp 0x61A6F0); delete 0x61A5B0 si bit 0
//    7   0x637330   CTaskSimpleHitFromBack destructor con borrado (0x86DB70)                         idem; cuerpo 0x637350
//    8   0x637360   CTaskSimpleHitFromLeft destructor con borrado (0x86DBA4)                         idem; cuerpo 0x637380
//    9   0x637390   CTaskSimpleHitFromRight destructor con borrado (0x86DBD8)                        idem; cuerpo 0x6373B0
//   10   0x6373C0   CTaskSimpleHitByGunFromFront destructor con borrado (0x86DC10)                   idem; cuerpo 0x6373E0
//   11   0x6373F0   CTaskSimpleHitByGunFromRear destructor con borrado (0x86DC4C)                    idem; cuerpo 0x637410
//   12   0x637420   CTaskSimpleHitByGunFromLeft destructor con borrado (0x86DC88)                    idem; cuerpo 0x637440
//   13   0x637450   CTaskSimpleHitByGunFromRight destructor con borrado (0x86DCC4)                   idem; cuerpo 0x637470
//   14   0x637480   CTaskSimpleStandStill destructor con borrado (0x86DD2C)                          thiscall(flags) `ret 4`: cuerpo 0x62F360 (con hook); delete 0x61A5B0 si bit 0
//   15   0x6374A0   CTaskSimpleSetStayInSamePlace destructor con borrado (0x86DD50)                  idem; cuerpo 0x62F5D0 (con hook)
//   16   0x6374C0   CTaskSimpleSetKindaStayInSamePlace destructor con borrado (0x86DD74)             idem; cuerpo 0x62F650 = `mov [ecx],86DD74h; jmp 61A3A0` (corre REAL; falso en 61A3A0)
//   17   0x6374E0   CTaskSimpleDuckToggle destructor con borrado (0x86DD98)                          idem; cuerpo 0x62F6D0 = `mov [ecx],86DD98h; jmp 61A3A0` (corre REAL; falso en 61A3A0)
//   18   0x637500   CTaskSimpleDie destructor con borrado (0x86DDBC)                                 idem; cuerpo 0x62FB40 (con hook)
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

namespace F183AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 19;
constexpr unsigned kArenas = 4;
constexpr unsigned kParams = 64;

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
    std::array<u32, kParams> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
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
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4, 1u, 1u, 0x101u});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline float F(u32 bits) { return std::bit_cast<float>(bits); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }
// Un float de magnitud moderada (coordenadas del mundo y desplazamientos): enteros, fracciones, valores alrededor de 3 (el cuadrado de la distancia se compara con 9) y alguno diminuto
inline u32 CoordBits(Gen& g) {
    const float sign = g.P(50) ? -1.0f : 1.0f;
    const u32 m = g.R(8);
    float v;
    if (m == 0) v = float(g.R(200000)) / 64.0f;
    else if (m == 1) v = float(g.R(6000));
    else if (m == 2) v = float(g.R(4000)) / 1000.0f;
    else if (m == 3) v = 3.0f + float(int(g.R(9)) - 4) / 1048576.0f;
    else if (m == 4) v = 1.7320508f + float(int(g.R(5)) - 2) / 8388608.0f;
    else if (m == 5) v = float(g.R(1000000)) / 3.0f;
    else if (m == 6) v = std::bit_cast<float>(0x00800000u + (g.W() & 0x0FFFFFFFu));
    else v = float(g.R(100)) / 7.0f;
    return B(sign * v);
}

// Bits de un float con exponente dado (sesgado 0..255) y mantisa
inline u32 ExpBits(u32 sign, u32 e, u32 mant) { return (sign ? 0x80000000u : 0u) | ((e & 0xFFu) << 23) | (mant & 0x7FFFFFu); }

// Clases de funcion (forma)
enum Kind : u32 { kShut, kLInsert, kBone, kEnter, kKey, kCross, kSdd };
inline constexpr std::array<Kind, kFunctions> kKind{kShut, kLInsert, kBone, kEnter, kKey, kCross, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4D40A0, 0x4D40C0, 0x4D56F0, 0x4D5980, 0x4D59B0, 0x4D59E0, 0x637300, 0x637330, 0x637360, 0x637390, 0x6373C0, 0x6373F0, 0x637420, 0x637450, 0x637480, 0x6374A0,
                                                    0x6374C0, 0x6374E0, 0x637500};
// Bytes que cada funcion toca a partir de su puntero `this` (o del objeto)
inline constexpr std::array<u32, kFunctions> kSpan{0x34, 0x34, 4, 4, 4, 0x54, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
// Memoria global del juego tocada (palabras de 16 bits de las teclas): Enter normal (nuevo, viejo) y Enter del teclado numerico (nuevo, viejo); cada region cubre la palabra y la siguiente (vecina)
inline constexpr u32 kNewEnter = 0xB733CC, kOldEnter = 0xB7315C, kNewExt = 0xB733EA, kOldExt = 0xB7317A;
// Teclas estandar: 256 palabras (NewKeyState 0xB73190 + 0x18 y OldKeyState 0xB72F20 + 0x18)
inline constexpr u32 kNewKeys = 0xB731A8, kOldKeys = 0xB72F38, kKeysBytes = 0x200;
// Funcion del EXE que hace de cuerpo / colaborador de cada destructor con borrado (0 = ninguno) y vtable que deja el cuerpo real (0 = la deja el falso)
inline constexpr std::array<u32, kFunctions> kSddBody{0, 0, 0, 0, 0, 0, 0x61A6F0, 0x61A6F0, 0x61A6F0, 0x61A6F0, 0x61A6F0, 0x61A6F0, 0x61A6F0, 0x61A6F0, 0x62F360, 0x62F5D0, 0x61A3A0, 0x61A3A0, 0x62FB40};
inline constexpr std::array<u32, kFunctions> kSddVtable{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x86DD74, 0x86DD98, 0};
// vtable propia de cada clase (dword plausible previo en [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0, 0x86E5EC, 0x86DB70, 0x86DBA4, 0x86DBD8, 0x86DC10, 0x86DC4C, 0x86DC88, 0x86DCC4, 0x86DD2C, 0x86DD50, 0x86DD74, 0x86DD98, 0x86DDBC};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[60..63] = semillas de las arenas A, B, C y D
//   a[56], a[57] = semillas de los arrays de teclas estandar (nuevo y viejo)
//   kShut: a[4] = puntero `links` (+0x30; crudo: nunca se desreferencia, el falso de operator delete no lo lee)
//   kLInsert: a[4] = enlaces libres (0..3), a[5] = enlaces usados (0..2), a[6] = desplazamiento (alineado) de los enlaces en la arena B, a[7] = desplazamiento (alineado) del dato en C, a[8] = el dato,
//         a[9..13] = el campo `data` de cada uno de los 5 enlaces, a[14] = torcedura (0 ninguna, 1 la cabeza usada apunta al enlace libre, 2 el enlace libre apunta a si mismo (next), 3 (prev), 4 la cabeza usada apunta a si misma)
//   kBone: a[4] = la etiqueta (dword de pila, con signo)
//   kEnter: a[4..7] = palabras Enter nuevo, Enter viejo, extenter nuevo, extenter viejo; a[8..11] = la palabra vecina (+2) de cada una; a[12] = ECX crudo (no se lee)
//   kKey: a[4] = dword de pila (BYTE bajo = tecla, el resto basura), a[5] = palabra de NewKeyState de la tecla, a[6] = palabra de OldKeyState, a[7] = ECX (0 = this de la arena, si no crudo), a[8] = ECX crudo
//   kCross: a[4] = palabra ButtonCross de NewState (+0x20), a[5] = la de OldState (+0x50)
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja el falso en [this], a[6] = banderas (pila)
// Palabra de 16 bits "interesante" de un boton / tecla (la funcion compara la PALABRA con cero: se incluyen valores con el byte bajo cero)
inline u32 KeyWord(Gen& g) {
    return g.Pk({0u, 0u, 0u, 1u, 1u, 0x100u, 0xFF00u, 0x8000u, 0xFFFFu, 0x0101u, 0xFFu, g.W() & 0xFFFFu, (g.W() & 0xFFFFu) | 1u, g.W() & 0xFF00u});
}
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[56] = c.seed * 17u + 5u; c.a[57] = c.a[56] + 1u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85AAB4u, 0x8710D0u, vt, vt, vt + 4, 0x86D48Cu, 0x86D504u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kShut:
        a[4] = g.Pk({0u, 0u, 0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0xDEADBEEFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), g.W(), g.W() | 1u, 0xFFFFFFFCu, 4u});
        break;
    case kLInsert:
        a[0] = Offset(g, kind, kSpan[c.fn]);   // la lista puede estar desalineada (el EXE y el oraculo la tratan byte a byte)
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 3u});
        a[5] = g.Pk({0u, 1u, 2u});
        a[6] = AuxOffset(g, 0x3C) & ~3u;
        a[7] = AuxOffset(g, 4) & ~3u;
        a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), 0xDEADBEEFu, 0x00400000u + g.R(0x800000), g.W()});
        for (u32 k = 0; k < 5; ++k) a[9 + k] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), 0xDEADBEEFu, g.W()});
        a[14] = g.P(70) ? 0u : 1u + g.R(4);
        if ((a[14] == 1 || a[14] == 2 || a[14] == 3) && a[4] == 0) a[14] = 4;   // sin enlaces libres no hay "el enlace libre"
        break;
    case kBone: {
        const u32 pick = g.R(12);
        const u32 valid[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 21, 22, 23, 24, 25, 26, 31, 32, 33, 34, 35, 36, 41, 42, 43, 44, 51, 52, 53, 54, 201, 301, 302};
        a[4] = pick <= 3 ? valid[g.R(32)]
             : pick == 4 ? g.R(0x40)                                                                  // 0..0x3F (los huecos de la tabla de saltos)
             : pick == 5 ? g.Pk({0xC8u, 0xC9u, 0xCAu, 0x12Cu, 0x12Du, 0x12Eu, 0x12Fu, 0x130u, 0x37u, 0x36u, 0x38u, 0x100u, 0x1FFu, 0x200u})
             : pick == 6 ? (valid[g.R(32)] | ((g.R(0xFFFF) + 1u) << 16))                              // bits altos puestos: el EXE compara el dword completo
             : pick == 7 ? g.Pk({0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFC9u, 0xFFFFFFFFu - g.R(0x200), 0x10009u, 0x100C9u, 0x1000012Du, 0x8000012Eu, 0xFFFF012Du, 0x10000u, 0x10001u})
             : pick == 8 ? (0x12Du + (g.R(3) << 16))
             : pick == 9 ? g.W()
             : pick == 10 ? 0x12Cu + g.R(6)
             : g.R(0x400);
        break;
    }
    case kEnter:
        for (u32 k = 0; k < 4; ++k) a[4 + k] = KeyWord(g);
        for (u32 k = 0; k < 4; ++k) a[8 + k] = g.Pk({0u, 0xFFFFu, 0x1234u, g.W() & 0xFFFFu, 0x8000u, 1u});
        a[12] = Raw(g);
        break;
    case kKey:
        a[4] = g.P(55) ? g.Pk({0u, 1u, 0xDu, 0xFFu, 0x80u, 0x7Fu, 0x20u, 0x41u, g.R(256), g.R(256), g.R(256), g.R(256)})
                       : (g.Pk({0u, 1u, 0xDu, 0xFFu, 0x80u, g.R(256), g.R(256), g.R(256)}) | (g.R(0x1000000) << 8));   // basura en los tres bytes altos
        a[5] = KeyWord(g); a[6] = KeyWord(g);
        a[7] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[8] = Raw(g);
        break;
    case kCross:
        a[4] = KeyWord(g); a[5] = KeyWord(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
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
