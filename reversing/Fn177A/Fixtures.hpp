#pragma once
// Fixtures del banco Fn177A (lote 177A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 26 asignadas: 26 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x558F90   CStats::GetFullFavoriteRadioStationList                                          cdecl() `ret`: mov eax,0B78E58h (ECX/EDX intactos)
//    1   0x55D0E0   SurfaceInfos_c::LoadAdhesiveLimits (el listado decia CStats)                     thiscall() `ret`: SetDir 5387D0, OpenFile 538900, LoadLine 536F80 en bucle, sscanf real, CloseFile 5389D0
//    2   0x5F6190   CEventCreatePartnerTask::Constructor                                              thiscall(tipo, companero, byte, float) `ret 10h`: base 4ABFC0; campos; RegisterReference 571B70
//    3   0x5F6200   CEventCreatePartnerTask cuerpo del destructor                                     thiscall() `ret`: [this]=86C6D0h; CleanUpOldReference 571A00; 4AC010
//    4   0x5F6280   CEventCreatePartnerTask::Clone (vtable 0x86C6D0, ranura 4)                        thiscall() `ret`: new(1Ch) 4B5620; ctor 5F6190; eax = el del constructor
//    5   0x5F6300   CEventCreatePartnerTask destructor con borrado (ranura 0)                         thiscall(flags) `ret 4`: call 5F6200; delete this (4B5630) si bit 0
//    6   0x62FF00   CTaskSimpleDrown::GetTaskType (vtable 0x86DE30)                                   thiscall() `ret`: mov eax,0D5h
//    7   0x62FF10   CTaskSimpleDrown cuerpo del destructor                                            thiscall() `ret`: [ecx]=86DE30h; jmp 62FB40
//    8   0x62FF20   CTaskSimpleDrownInCar::Constructor (vtable 0x86DE54)                              thiscall() `ret`: base 61A390; [this]=86DE54h; eax=this
//    9   0x62FF40   CTaskSimpleDrownInCar::GetTaskType                                                thiscall() `ret`: mov eax,0D8h
//   10   0x62FF50   CTaskSimpleDrownInCar::MakeAbortable                                              thiscall(3 dwords) `ret 0Ch`: xor al,al
//   11   0x62FF60   CTaskSimpleDrownInCar cuerpo del destructor                                       thiscall() `ret`: [ecx]=86DE54h; jmp 61A3A0
//   12   0x6378D0   CTaskSimpleDrown destructor con borrado                                           thiscall(flags) `ret 4`: call 62FF10; delete this (61A5B0) si bit 0
//   13   0x6378F0   CTaskSimpleDrownInCar destructor con borrado                                      thiscall(flags) `ret 4`: call 62FF60; delete
//   14   0x637910   CTaskComplexDie destructor con borrado                                            thiscall(flags) `ret 4`: call 6300C0; delete
//   15   0x637930   CTaskSimpleDead destructor con borrado                                            thiscall(flags) `ret 4`: call 6305F0; delete
//   16   0x637950   CTaskSimpleTired destructor con borrado                                           thiscall(flags) `ret 4`: call 630F60; delete
//   17   0x637970   CTaskSimpleTired::ProcessPed (vtable 0x86DED0, ranura 7)                           thiscall(ped) `ret 4`: StartAnim 630F70 si no hay animacion; temporizador sin signo con el reloj 0xB7CB84; llamada virtual a la ranura 6
//   18   0x6379D0   CTaskSimpleSitDown destructor con borrado                                         thiscall(flags) `ret 4`: call 631070; delete
//   19   0x640430   CTaskComplexDragPedFromCar::Constructor (vtable 0x86EB6C)                         thiscall(ped, tiempo) `ret 8`: base 63A220(0,0,0,1,0); +50h = ped; RegisterReference; +20h = tiempo
//   20   0x6404C0   CTaskComplexDragPedFromCar::GetTaskType                                           thiscall() `ret`: mov eax,2BFh
//   21   0x6404D0   CTaskComplexDragPedFromCar cuerpo del destructor                                  thiscall() `ret`: [this]=86EB6Ch; CleanUpOldReference 571A00 sobre +50h; 63DFA0
//   22   0x643950   CTaskComplexDragPedFromCar::Clone                                                 thiscall() `ret`: new(54h) 61A5A0; ctor 640430(ped +50h, tiempo +20h)
//   23   0x660ED0   CTaskGoToVehicleAndLean::GetTaskType (vtable 0x86FAC8)                            thiscall() `ret`: mov eax,4C0h
//   24   0x664770   CTaskGoToVehicleAndLean::CalcTargetPos                                            thiscall(retorno oculto, ped) `ret 8`: x87; CMatrix * CVector 59C890
//   25   0x6648A0   CTaskLeanOnVehicle destructor con borrado (vtable 0x86FAF4)                       thiscall(flags) `ret 4`: call 661030; delete
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F177AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 26;
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
constexpr unsigned kSeed = 60;          // a[60..63] = semillas de las arenas A, B, C y D
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 64> a{};
    std::vector<u8> s;          // datos en bytes: las lineas de surface.dat (cada una terminada en 0) en LoadAdhesiveLimits
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
// Desplazamiento alineado de un objeto de `span` bytes en una zona [lo, lo + size) de una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxIn(Gen& g, u32 lo, u32 size, u32 span) { return lo + 4 * g.R((size - span) / 4 + 1); }
inline u32 AuxOffset(Gen& g, u32 span) { return AuxIn(g, 0, u32(kWin), span); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float de magnitud moderada (sin NaN ni infinitos, exponente entre 2^-60 y 2^60): para posiciones y vectores de las matrices
inline u32 FloatMid(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    return g.Pk({sign | ((67u + g.R(120)) << 23) | (g.W() & 0x7FFFFFu), sign | ((120u + g.R(14)) << 23) | (g.W() & 0x7FFFFFu), sign | 0x3F800000u, sign | 0x3F000000u, 0u, 0x80000000u});
}
// Rumbo (heading) para el seno y el coseno de la x87: la parte accesible al oraculo (valores finitos con |h| < 1000, que la x87 reduce con precision de sobra) y los casos que el oraculo conoce
// exactos (cero con signo, denormales, infinitos y NaN; magnitud >= 2^63, donde fcos / fsin dejan el operando intacto)
inline u32 HeadingBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nearZero = sign | ((0x70u + g.R(0x1Au)) << 23) | (g.W() & 0x7FFFFFu);          // 2^-15 .. 2^10 (|h| < 1024; se limita a < 1000 mas abajo)
    const u32 tinyN = sign | ((1u + g.R(0x70u)) << 23) | (g.W() & 0x7FFFFFu);              // normales muy pequenos
    const u32 den = sign | (g.W() & 0x7FFFFFu);
    const u32 huge = sign | ((191u + g.R(64u)) << 23) | (g.W() & 0x7FFFFFu);                // >= 2^64 hasta el infinito exclusive (exponente 254 maximo)
    const u32 inf = sign | 0x7F800000u, nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    u32 pick = g.Pk({0u, 0x80000000u, 0x40490FDBu, 0xC0490FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x40C90FDBu, 0x3F800000u, nearZero, nearZero, nearZero, nearZero, nearZero, nearZero, nearZero, tinyN, den, huge, huge, huge, inf, nanq, nans});
    if (((pick >> 23) & 0xFF) == 0x88 || ((pick >> 23) & 0xFF) == 0x89) pick = (pick & 0x80000000u) | 0x43000000u | (pick & 0x7FFFu);   // |h| >= 1024 -> en [128, 256)
    return pick;
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kBaseCtor, kBody, kMakeAb, kSdd, kLoadAdh, kEvCtor, kEvBody, kEvClone, kTiredProc, kDragCtor, kDragBody, kDragClone, kCalc };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kLoadAdh, kEvCtor, kEvBody, kEvClone, kSdd, kConst, kBody, kBaseCtor, kConst, kMakeAb, kBody, kSdd, kSdd, kSdd, kSdd, kSdd, kTiredProc,
                                                    kSdd, kDragCtor, kConst, kDragBody, kDragClone, kConst, kCalc, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x558F90, 0x55D0E0, 0x5F6190, 0x5F6200, 0x5F6280, 0x5F6300, 0x62FF00, 0x62FF10, 0x62FF20, 0x62FF40, 0x62FF50, 0x62FF60, 0x6378D0, 0x6378F0, 0x637910, 0x637930,
                                                    0x637950, 0x637970, 0x6379D0, 0x640430, 0x6404C0, 0x6404D0, 0x643950, 0x660ED0, 0x664770, 0x6648A0};
// Valor que devuelven las funciones constantes (por indice; 0 si no es constante)
inline constexpr std::array<u32, kFunctions> kConstValue{0xB78E58, 0, 0, 0, 0, 0, 0xD5, 0, 0, 0xD8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2BF, 0, 0, 0x4C0, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0x86C6D0, 0x86C6D0, 0x86C6D0, 0x86C6D0, 0x86DE30, 0x86DE30, 0x86DE54, 0x86DE54, 0x86DE54, 0x86DE54, 0x86DE30, 0x86DE54, 0x86DE78, 0x86DEA4,
                                                     0x86DED0, 0x86DED0, 0x86DEF4, 0x86EB6C, 0x86EB6C, 0x86EB6C, 0x86EB6C, 0x86FAC8, 0x86FAC8, 0x86FAF4};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x100, 0x1C, 0x1C, 0x1C, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x1C, 4, 0x54, 4, 0x54, 0x54, 4, 0x10, 4};
// Entrada de la tabla de model infos
inline constexpr u32 kModelTable = 0xA9B0C8, kModelCount = 20000, kClockAddr = 0xB7CB84;

// Zonas de las arenas de CalcTargetPos (cada objeto en la suya, sin solaparse): B = vehiculo [0, 0x300), ped [0x300, 0x600), matriz del vehiculo [0x600, 0x900), matriz del ped [0x900, 0xC00);
// C = model info [0, 0x200), modelo de colision [0x200, 0x400), buffer del resultado [0x400, 0x800)
inline constexpr u32 kVehSpan = 0x60, kPedSpan = 0x60, kMatSpan = 0x40, kMiSpan = 0x20, kColSpan = 0x18, kRetSpan = 0x10;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[60..63] = semillas de las arenas A, B, C y D
//   kConst: nada mas
//   kBaseCtor, kBody: a[4] = EAX del falso (constructor base / cuerpo base), a[5] = dword que el falso deja en [this]
//   kMakeAb: a[4..6] = los tres dwords de pila
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kLoadAdh: a[4] = valor que devuelve el falso de OpenFile (el "FILE*"), a[5] = numero de lineas; s = las lineas (cada una terminada en 0)
//   kEvCtor: a[4] = tipo, a[5] = companero (crudo), a[6] = dword del byte (pila), a[7] = float (bits), a[8] = EAX del falso del constructor base, a[9] = dword que deja en [this]
//   kEvBody: a[4] = companero (+10h, crudo), a[5] = dword que deja el falso del cuerpo en [this], a[6] = EAX de los falsos
//   kEvClone: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor, a[7] = dword que deja en [mem], a[8] = tipo (+0Ch), a[9] = companero (+10h), a[10] = dword +14h, a[11] = float (+18h)
//   kTiredProc: a[4] = animacion (+18h, crudo), a[5] = byte +14h, a[6] = byte +15h, a[7] = inicio (+0Ch), a[8] = duracion (+10h), a[9] = reloj (0xB7CB84), a[10] = ped (pila, crudo), a[11] = bytes +16h y +17h
//   kDragCtor: a[4] = ped (crudo), a[5] = tiempo, a[6] = dword que deja el falso del constructor base en [this], a[7] = EAX del falso
//   kDragBody: a[4] = ped (+50h, crudo), a[5] = dword que deja el falso del cuerpo en [this], a[6] = EAX de los falsos
//   kDragClone: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor, a[7] = dword que deja en [mem], a[8] = tiempo (+20h), a[9] = ped (+50h, crudo)
//   kCalc: a[4] = desplazamiento del vehiculo en B, a[5] = desplazamiento del ped en B, a[6] = matriz del vehiculo (0 nula, 1 en B), a[7] = desplazamiento de esa matriz en B, a[8] = matriz del ped (0 nula, 1 en B),
//         a[9] = desplazamiento de esa matriz en B, a[10..12] = posicion del placement del vehiculo, a[13] = rumbo del vehiculo, a[14..16] = derecha de la matriz del vehiculo (+0, +4, +8),
//         a[17..19] = posicion de la matriz del vehiculo (+30h..), a[20..22] = posicion del placement del ped, a[23..25] = posicion de la matriz del ped, a[26] = indice del modelo (< 20000),
//         a[27] = dword en vehiculo+20h (los 16 bits altos se sustituyen por el indice), a[28] = desplazamiento del model info en C, a[29] = desplazamiento del modelo de colision en C,
//         a[30..32] = minimo del modelo de colision, a[33..35] = maximo, a[36..38] = lo que escribe el falso de CMatrix * CVector, a[39] = desplazamiento del buffer del resultado en C
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeed] = c.seed * 13u + 50u; c.a[kSeed + 1] = c.a[kSeed] + 1u; c.a[kSeed + 2] = c.a[kSeed] + 2u; c.a[kSeed + 3] = c.a[kSeed] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    const auto mem = [&](u32 span) { c.a[4] = g.Pk({0u, 1u, 1u, 1u, 1u}); c.a[5] = AuxOffset(g, span); };
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kBaseCtor:
    case kBody:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kMakeAb:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kLoadAdh: {
        // surface.dat: lineas de matriz triangular (la fila N trae N + 1 valores), con comentarios (;) y lineas vacias intercalados, filas de mas o de menos, valores '-', 'x', con sufijo, tabuladores
        a[4] = Raw(g);
        std::vector<u8>& s = c.s;
        const auto add = [&](const std::string& line) { s.insert(s.end(), line.begin(), line.end()); s.push_back(0); ++a[5]; };
        const auto noise = [&](char ch) { while (g.R(4) == 0) add(g.P(50) ? std::string(1, ch) + " comentario de prueba" : std::string()); };
        const auto sep = [&] { static const char* const t[]{" ", " ", " ", "  ", "\t", " \t "}; return std::string(t[g.R(6)]); };
        // numeros con fraccion binaria exacta (los CRT del EXE y de la DLL los leen igual) y los valores del surface.dat real
        const auto num = [&](bool negative, u32 maxInt) {
            static const char* const fr[]{"0", "25", "5", "75", "125", "375", "0625"};
            return std::string(negative ? "-" : "") + std::to_string(g.R(maxInt + 1)) + "." + fr[g.R(7)];
        };
        static const char* const real[]{"6.0", "3.6", "2.0", "4.5", "3.0", "3.2", "3.5", "1.0", "2.8", "0.5"};
        static const char* const names[]{"PED", "CAR", "CAR_PANEL", "TARMAC", "DEFAULT", "GRASS_SHORT_LUSH"};
        const u32 rows = g.R(4) == 0 ? g.R(9) : 6;
        a[5] = 0;
        for (u32 r = 0; r < rows; ++r) {
            noise(';');
            std::string l = g.R(3) ? std::string(names[g.R(6)]) : "FILA" + std::to_string(r);
            l += g.R(4) == 0 ? "\t" : " ";
            const u32 vals = g.R(8) == 0 ? g.R(r + 3) : r + 1;
            for (u32 col = 0; col < vals; ++col) {
                if (col) l += sep();
                const u32 k = g.R(12);
                l += k == 0 ? std::string("-") : k == 1 ? "-" + num(false, 2) : k == 2 ? std::string("x") : k == 3 ? num(false, 2) + "z" : k <= 6 ? std::string(real[g.R(10)]) : num(false, 3);
            }
            if (g.R(5) == 0) l += sep();
            add(l);
        }
        noise(';');
        break;
    }
    case kEvCtor:
        a[4] = g.Pk({0u, 1u, 2u, 6u, 0xFFFFFFFFu, g.W(), g.R(0x40)});
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80u, g.W(), g.W()});
        a[7] = FloatBits(g);
        a[8] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[9] = Raw(g);
        break;
    case kEvBody:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        break;
    case kEvClone:
        mem(0x1C);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 1u, 2u, 6u, 0xFFFFFFFFu, g.W(), g.R(0x40)});
        a[9] = Raw(g);
        a[10] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80u, g.W(), g.W()});
        a[11] = FloatBits(g);
        break;
    case kTiredProc: {
        a[4] = g.Pk({0u, 0u, 1u, 0x12345678u, 0xFFFFFFFFu, g.W()});
        a[5] = g.Pk({0u, 1u, 1u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        a[6] = g.Pk({0u, 0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        const u32 start = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W(), 1000u, 0xFFFFF830u});
        const u32 interval = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 1999u, 2000u, 2001u, g.W(), g.R(5000), g.R(100)});
        a[7] = start; a[8] = interval;
        const u32 end = start + interval;
        const u32 d = g.R(8);
        a[9] = d == 0 ? end : (d == 1 ? end - 1 : (d == 2 ? end + 1 : (d == 3 ? g.W() : (d == 4 ? start : (d == 5 ? start + 2000u : (d == 6 ? 0xFFFFFFFFu : end + g.R(20)))))));
        a[10] = Raw(g);
        a[11] = g.W() & 0xFFFFu;
        break;
    }
    case kDragCtor:
        a[4] = Raw(g);
        a[5] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(5000)});
        a[6] = Raw(g);
        a[7] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        break;
    case kDragBody:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        break;
    case kDragClone:
        mem(0x54);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(5000)});
        a[9] = Raw(g);
        break;
    case kCalc: {
        a[4] = AuxIn(g, 0x000, 0x300, kVehSpan);
        a[5] = AuxIn(g, 0x300, 0x300, kPedSpan);
        a[6] = g.Pk({0u, 1u, 1u, 1u});
        a[7] = AuxIn(g, 0x600, 0x300, kMatSpan);
        a[8] = g.Pk({0u, 1u, 1u, 1u});
        a[9] = AuxIn(g, 0x900, 0x300, kMatSpan);
        // posiciones y vectores de la matriz: ordinarios la mayoria de las veces (el signo del producto escalar sale de ellos), con casos extremos (infinitos, NaN, extremos del float) en el resto
        const auto comp = [&]() -> u32 { return g.P(88) ? FloatMid(g) : FloatBits(g); };
        for (u32 i = 10; i <= 12; ++i) a[i] = comp();
        a[13] = HeadingBits(g);
        for (u32 i = 14; i <= 25; ++i) a[i] = comp();
        a[26] = g.Pk({0u, 1u, 19999u, 400u, 500u, 595u, g.R(kModelCount), g.R(kModelCount), g.R(kModelCount), 0x4000u});
        a[27] = g.W();
        a[28] = AuxIn(g, 0x000, 0x200, kMiSpan);
        a[29] = AuxIn(g, 0x200, 0x200, kColSpan);
        for (u32 i = 30; i <= 35; ++i) a[i] = g.P(85) ? FloatMid(g) : FloatBits(g);
        for (u32 i = 36; i <= 38; ++i) a[i] = FloatBits(g);
        a[39] = AuxIn(g, 0x400, 0x400, kRetSpan);
        break;
    }
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
