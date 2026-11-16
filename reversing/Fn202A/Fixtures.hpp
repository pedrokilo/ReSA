#pragma once
// Fixtures del banco Fn202A (lote 202A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas y enganchadas en el commit 7d697593e5, SIN validar; las clases del listado salen del desensamblado y de las vtables, ver README.md y fixture_oracle.py). 32 funciones.
//   idx  direccion  nombre real                                                                         forma
//    0   0x4AC900   CEventVehicleCollision::GetLifeTime (V, ranura 3)                                   thiscall() `ret`: xor eax,eax
//    1   0x4AC910   CEventVehicleCollision::GetEventPriority (V, ranura 2)                              thiscall() `ret`: mov eax,3Ah
//    2   0x4AC980   CEventVehicleCollision::TakesPriorityOver (V, ranura 11)                            thiscall(evento) `ret 4`: mov al,1
//    3   0x4B6C40   CEventPedCollisionWithPed::Clone (V, ranura 4)                                      thiscall() `ret`: pool 0xB744AC New 0x4B5570; ctor 0x4AC990 (7 argumentos)
//    4   0x4B6CC0   CEventObjectCollision::Clone (V, ranura 4)                                          idem; ctor 0x4ACCF0 (6 argumentos)
//    5   0x4B6D40   CEventBuildingCollision::Clone (V, ranura 4)                                        idem; ctor 0x4ACF00 (6 argumentos)
//    6   0x4B6DC0   CEventDraggedOutCar::CloneEditable (V, ranura 16)                                   idem; ctor 0x4AD250 (3 argumentos)
//    7   0x4B6E30   CEventDeath::Clone (V, ranura 4)                                                    New 0x4B5570 y construccion en linea (sin ctor)
//    8   0x4B6E70   CEventDeadPed::CloneEditable (V, ranura 16)                                         idem; ctor 0x4ADEA0 (3 argumentos)
//    9   0x4B6EE0   CEventGroupEvent::Clone (V, ranura 4)                                               New; Clone virtual (ranura 4) del evento interno (+0x10); ctor 0x4ADFD0 (2 argumentos)
//   10   0x4B6F40   CEventPotentialGetRunOver::CloneEditable (V, ranura 16)                             New; ctor 0x4AE1A0 (1 argumento)
//   11   0x4B6FB0   CEventPotentialWalkIntoVehicle::CloneEditable (V, ranura 16)                        New; ctor 0x4AE320 (2 argumentos)
//   12   0x4B7020   CEventPotentialWalkIntoObject::CloneEditable (V, ranura 16)                         New; ctor 0x4AE5D0 (2 argumentos)
//   13   0x4B7090   CEventPotentialWalkIntoPed::CloneEditable (V, ranura 16)                            New; ctor 0x4AE6E0 (3 argumentos)
//   14   0x61AC90   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: constructor por grupo e identificador              thiscall(7) `ret 1Ch`: base 0x61A390
//   15   0x61AD10   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: constructor por nombre                             thiscall(8) `ret 20h`: base 0x61A390, strcpy x2, 0x4D3940, 0x4D42F0
//   16   0x61ADF0   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: StartAnim                                           thiscall(ped) `ret 4`: 0x4D4410 o 0x4D4610; 0x4CEBE0
//   17   0x61B890   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: Clone (ranura 1)                                    thiscall() `ret`: CTask::operator new 0x61A5A0 (0x70); ctor 0x61AD10 o 0x61AC90
//   18   0x61BC80   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: destructor con borrado                              thiscall(flags) `ret 4`: cuerpo 0x61BCA0; CTask::operator delete 0x61A5B0
//   19   0x61BCA0   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: cuerpo del destructor                               thiscall() `jmp 0x61A6F0`
//   20   0x61BCB0   TASK_SIMPLE_ANIM_LOOPED_MIDDLE: ProcessPed (ranura 7)                                thiscall(ped) `ret 4`: x87, 0x4D68B0, 0x420E30, 0x61ADF0, 0x4CEA80
//   21   0x61AEA0   CTaskSimplePlayHandSignalAnim::GetTaskType (V, ranura 4)                             thiscall() `ret`: mov eax,1A9h
//   22   0x61AEB0   CTaskSimplePlayHandSignalAnim cuerpo del destructor                                  thiscall() `ret`: manos (+0x1C, +0x20), CWorld::Remove 0x563280, 0xBB4A70, 0x61A6F0
//   23   0x61AF50   CTaskSimplePlayHandSignalAnim::MakeAbortable (V, ranura 6)                           thiscall(3) `jmp 0x61A790`
//   24   0x61B980   CTaskSimplePlayHandSignalAnim::Clone (V, ranura 1)                                   thiscall() `ret`: CTask::operator new (0x24); ctor 0x61AE50 (4 argumentos)
//   25   0x61BD80   CTaskSimplePlayHandSignalAnim destructor con borrado                                 thiscall(flags) `ret 4`: cuerpo 0x61AEB0; CTask::operator delete 0x61A5B0
//   26   0x5921D0   Interior_c::Constructor                                                              thiscall() `ret`: 0x4A8DB0, 0x4A8DD0 (+0x5C), words +0x3EC y +0x3F0 = 0xFFFF
//   27   0x597FE0   InteriorGroup_c::Constructor                                                        thiscall() `ret`: 0x4A8DB0 (+0 y +4 a cero)
//   28   0x59A840   CMatrix::ConvertToEulerAngles                                                       thiscall(pX, pY, pZ, banderas) `ret 10h`: x87, fpatan
//   29   0x59AA40   CMatrix::ConvertFromEulerAngles                                                     thiscall(x, y, z, banderas) `ret 10h`: x87, fsin, fcos
//   30   0x5FFFD0   CAttractorScanner::ScanEffect                                                       thiscall(efecto, entidad, peaton) `ret 0Ch`: x87
//   31   0x600180   CAttractorScanner::GetBestEffect                                                    thiscall(pEfecto, pEntidad) `ret 8`: x87
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

namespace F202AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 32;
constexpr unsigned kArenas = 4;
constexpr unsigned kParams = 96;

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
// Desplazamiento de `base` mas hasta `jitter` palabras
inline u32 Slot(Gen& g, u32 base, u32 jitter) { return base + 4 * g.R(jitter); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNZ(Gen& g) { u32 v = Raw(g); return v ? v : 0x00ABCDEFu; }
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
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsFiniteBits(u32 b) { return (b & 0x7F800000u) != 0x7F800000u; }
// Un float finito (sin NaN ni infinitos)
inline u32 FiniteBits(Gen& g) { u32 v = FloatBits(g); for (int t = 0; t < 8 && !IsFiniteBits(v); ++t) v = FloatBits(g); return IsFiniteBits(v) ? v : 0x3F800000u; }
// Un float de magnitud moderada (coordenadas del mundo y desplazamientos): enteros, fracciones, valores alrededor de 3 y alguno diminuto
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
// Un entero de 16 bits interesante (se ponen en las mitades de un dword)
inline u32 Half(Gen& g) { return g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x8001u, 0xFFFEu, 0x1234u, g.R(0x10000), g.R(0x10000), g.R(0x10000), 0x100u, 0xFFu}); }
// Un dword con un byte interesante (valores distintos de 0 y 1) y basura en los demas bytes
inline u32 ByteDword(Gen& g) {
    const u32 b = g.Pk({0u, 1u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, g.R(256), g.R(256)});
    return g.P(50) ? (b | (g.W() & 0xFFFFFF00u)) : b;
}

// Byte j de una cadena sintetica (nunca cero): el oraculo Python repite la misma formula
inline u8 StrByte(u32 seed, u32 j) { return u8(1 + Rand(seed, 5000u + j) % 255u); }

// Clases de funcion (forma)
enum Kind : u32 { kConst, kClone, kDeath, kGroup, kLmCtorId, kLmCtorName, kLmStart, kLmClone, kSdd, kBodyOnly, kLmProcess, kHsDtor, kHsAbort, kHsClone, kInterior, kIgCtor, kEulerTo, kEulerFrom, kScan, kBest };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kConst, kConst, kClone, kClone, kClone, kClone, kDeath, kClone, kGroup, kClone, kClone, kClone, kClone, kLmCtorId, kLmCtorName, kLmStart, kLmClone,
                                                    kSdd, kBodyOnly, kLmProcess, kConst, kHsDtor, kHsAbort, kHsClone, kSdd, kInterior, kIgCtor, kEulerTo, kEulerFrom, kScan, kBest};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4AC900, 0x4AC910, 0x4AC980, 0x4B6C40, 0x4B6CC0, 0x4B6D40, 0x4B6DC0, 0x4B6E30, 0x4B6E70, 0x4B6EE0, 0x4B6F40, 0x4B6FB0, 0x4B7020, 0x4B7090,
                                                    0x61AC90, 0x61AD10, 0x61ADF0, 0x61B890, 0x61BC80, 0x61BCA0, 0x61BCB0, 0x61AEA0, 0x61AEB0, 0x61AF50, 0x61B980, 0x61BD80, 0x5921D0, 0x597FE0,
                                                    0x59A840, 0x59AA40, 0x5FFFD0, 0x600180};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 0x34, 0x30, 0x30, 0x20, 0x14, 0x20, 0x14, 0x18, 0x1C, 0x1C, 0x28, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 4, 0x24, 0x24, 0x24, 0x24, 0x3F4, 8,
                                                   0x2C, 0x2C, 0x90, 0x90};
// Memoria global del juego tocada
inline constexpr u32 kEventPool = 0xB744AC, kNoTempObjects = 0xBB4A70, kClockMs = 0xB7CB84, kRain = 0xC81324, kRadii = 0xC39EE0;
inline constexpr u32 kRadiiBytes = 0x100;   // las 64 primeras casillas de CScripted2dEffects::ms_radii
// Tablas de ejes del EXE (0x866D94, 0x866D9C; ver fixture_oracle.py)
inline constexpr std::array<u32, 8> kEulNext{1, 2, 0, 1, 0, 0, 0, 0};
inline constexpr std::array<u32, 4> kEulFirst{0, 1, 2, 0};
inline constexpr u32 kEulThreshold = 0x36000000u;   // 1.9073486e-6 (0x866D90)

// Clonado de eventos y tareas: campos del objeto origen que lee la funcion (desplazamiento, tipo de generador): W = dword formado por dos mitades de 16 bits, D = dword crudo, B = dword con un byte interesante, F = float
struct FieldSpec { u32 off; char kind; };
inline constexpr std::array<std::array<FieldSpec, 9>, kFunctions> kFields = [] {
    std::array<std::array<FieldSpec, 9>, kFunctions> t{};
    auto set = [&](unsigned fn, std::initializer_list<FieldSpec> l) { unsigned i = 0; for (auto f : l) t[fn][i++] = f; };
    set(3, {{0xC, 'W'}, {0x10, 'F'}, {0x14, 'D'}, {0x30, 'W'}});
    set(4, {{0xC, 'W'}, {0x10, 'F'}, {0x14, 'D'}});
    set(5, {{0xC, 'W'}, {0x10, 'F'}, {0x14, 'D'}});
    set(6, {{0x14, 'D'}, {0x18, 'D'}, {0x1C, 'B'}});
    set(7, {{0xC, 'B'}, {0x10, 'D'}});
    set(8, {{0x14, 'D'}, {0x18, 'B'}, {0x1C, 'D'}});
    set(9, {{0xC, 'D'}});
    set(10, {{0x14, 'D'}});
    set(11, {{0x14, 'D'}, {0x18, 'D'}});
    set(12, {{0x14, 'D'}, {0x18, 'D'}});
    set(13, {{0x20, 'D'}, {0x24, 'D'}});
    set(17, {{0xC, 'B'}, {0x10, 'D'}, {0x14, 'D'}, {0x18, 'D'}, {0x44, 'D'}, {0x48, 'F'}, {0x4C, 'F'}, {0x50, 'F'}, {0x5C, 'D'}});
    set(24, {{0xC, 'B'}, {0x10, 'D'}, {0x14, 'D'}, {0x18, 'B'}});
    return t;
}();
inline constexpr std::array<u32, kFunctions> kFieldCount = [] {
    std::array<u32, kFunctions> t{};
    t[3] = 4; t[4] = 3; t[5] = 3; t[6] = 3; t[7] = 2; t[8] = 3; t[9] = 1; t[10] = 1; t[11] = 2; t[12] = 2; t[13] = 2; t[17] = 9; t[24] = 4;
    return t;
}();
// Casillas de vtable falsas (CEventGroupEvent: evento interno; CTaskSimplePlayHandSignalAnim: manos)
constexpr u32 kVtableSlots = 32;

// ------------------------------------------------------------------------------------------------ generador
// a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[92..95] = semillas de las arenas A, B, C y D
//   kConst: a[4] = argumento de pila crudo (solo la funcion 2)
//   kClone / kDeath / kGroup / kHsClone / kLmClone: a[4] = memoria de New (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = dword que deja el falso del constructor en [ECX], a[7] = EAX crudo del falso,
//         a[8] = selector del EAX del falso (0 la memoria, 1 la memoria + 4, 2 cero, 3 crudo), a[9] = desplazamiento del pool 0xB744AC en D; a[10..18] = los campos de kFields[fn] (en orden);
//         kGroup: a[19] = desplazamiento del evento interno en C, a[20] = desplazamiento de la vtable en D, a[21] = EAX crudo del Clone del evento interno
//   kLmCtorId: a[4..10] = los 7 argumentos; a[11] = dword que escribe el falso de la base en +0, a[12] = en +4, a[13] = byte en +0xC
//   kLmCtorName: a[4] = desplazamiento del nombre en C, a[5] = del grupo en C (o igual), a[6..11] = banderas, mezcla, inicio, fin, intervalo, mantener; a[12], a[13] = longitudes; a[14] = semilla de las cadenas;
//         a[15] = 1 si el grupo es la misma cadena; a[16..18] = lo que escribe el falso de la base; a[19] = bloque que devuelve GetAnimationBlock; a[20] = jerarquia que devuelve GetAnimation
//   kLmStart: a[4] = jerarquia (+0x44), a[5] = banderas (+0x18), a[6] = grupo (+0x10), a[7] = animacion (+0x14), a[8] = mezcla (+0x48), a[9] = desplazamiento del peaton en C, a[10] = clump (+0x18 del peaton),
//         a[11] = asociacion que devuelven los falsos de BlendAnimation, a[12] = dword previo de +8
//   kSdd: a[4] = EAX crudo de los falsos, a[5] = dword que dejan, a[6] = banderas (dword de pila); kBodyOnly: a[4], a[5] igual
//   kLmProcess: a[4] = BYTE +0xC, a[5] = modo de la asociacion (+8: 0 nula, 1 objeto en D), a[6] = desplazamiento de la asociacion en D, a[7] = animacion (+0x14), a[8] = BYTE +0x68, a[9] = desplazamiento del peaton en C,
//         a[10] = clump, a[11] = lo que devuelve RpAnimBlendClumpGetAssociation, a[12] = EAX de IsOutOfTime, a[13] = asociacion nueva (D), a[14] = jerarquia nueva (D), a[15] = tiempo total (float),
//         a[16] = reloj 0xB7CB84, a[17] = tiempo actual (+0x20), a[18] = paso (+0x28), a[19] = fin del bucle (+0x58), a[20] = inicio del bucle (+0x54), a[21] = +0x4C, a[22] = +0x50, a[23] = +0x5C,
//         a[24] = dword +0x2C de la asociacion (BYTE +0x2E arriba), a[25] = estilo de los tiempos, a[26] = dword +0x60, a[27] = dword +0x64
//   kHsDtor: a[4] = mano izquierda (0 nula, 1 objeto), a[5] = derecha (0 nula, 1 objeto, 2 la misma que la izquierda), a[6] = desplazamiento de la mano izquierda en C, a[7] = de la derecha, a[8] = vtable en D,
//         a[9] = dword de 0xBB4A70, a[10] = llamada a CWorld::Remove tras la que cambia un campo (0 ninguna, 1, 2), a[11] = campo (0 = +0x1C, 1 = +0x20), a[12] = valor (0 nulo, 1 otra mano), a[13] = dword que deja el falso del cuerpo,
//         a[14] = EAX crudo de los falsos
//   kHsAbort: a[4..6] = los tres argumentos, a[7] = EAX crudo del falso
//   kInterior / kIgCtor: nada mas
//   kEulerTo: a[4] = modo de la matriz, a[5] = banderas, a[6..14] = elementos (bits) en los desplazamientos 0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28; a[15..20] = (arena, desplazamiento) de pX, pY y pZ (0 = A dentro de la matriz, 1 = B, 2 = C, 3 = D)
//   kEulerFrom: a[4] = modo, a[5] = banderas, a[6..8] = x, y, z (bits)
//   kScan: a[4] = Rain, a[5] = tipo, a[6] = modo de la entidad (0 nula, 1 objeto en B), a[7] = desplazamiento de la entidad en B, a[8] = desplazamiento del efecto en C, a[9..11] = posicion del efecto, a[12..14] = lo que
//         escribe el falso de TransformFromObjectSpace, a[15] = modo de la matriz del peaton (0 sin matriz, 1 con ella), a[16] = desplazamiento del peaton en D, a[17] = desplazamiento de su matriz en D, a[18..20] = posicion del
//         peaton (+4 o matriz +0x30), a[21] = field_68[tipo], a[22] = radio (bits), a[23] = indice de GetIndex, a[24] = gestor, a[25] = EAX de HasEmptySlot, a[26] = EAX de IsApproachable, a[27] = desplazamiento de la matriz de la
//         entidad en B, a[28] = field_18[tipo] previo, a[29] = field_40[tipo] previo, a[30] = dword +0x34 del efecto (byte bajo = tipo), a[31] = modo del escenario
//   kBest: a[4..13] = field_18, a[14..23] = field_40, a[24..33] = field_68 (bits), a[34..37] = (arena, desplazamiento) de *effect y *entity (0 = A dentro del escaner, 1 = B, 2 = C, 3 = D)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[92] = c.seed * 13u + 50u; c.a[93] = c.a[92] + 1u; c.a[94] = c.a[92] + 2u; c.a[95] = c.a[92] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85AC68u, 0x86D594u});
    auto& a = c.a;
    const auto field = [&](char k) -> u32 {
        switch (k) {
        case 'W': return Half(g) | (Half(g) << 16);
        case 'B': return ByteDword(g);
        case 'F': return FloatBits(g);
        default: return Raw(g);
        }
    };
    const auto cloneCommon = [&] {
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x40);
        a[6] = Raw(g);
        a[7] = Raw(g);
        a[8] = g.R(4);
        a[9] = AuxOffset(g, 0x20);
        for (u32 i = 0; i < kFieldCount[c.fn]; ++i) a[10 + i] = field(kFields[c.fn][i].kind);
    };
    switch (kKind[c.fn]) {
    case kConst:
        a[4] = Raw(g);
        break;
    case kClone:
    case kDeath:
    case kHsClone:
        cloneCommon();
        break;
    case kLmClone:
        cloneCommon();
        if (g.P(50)) a[14] = 0u;   // sin jerarquia (+0x44): constructor por identificador
        else if (a[14] == 0) a[14] = RawNZ(g);
        break;
    case kGroup:
        cloneCommon();
        a[19] = AuxOffset(g, 0x20);
        a[20] = AuxOffset(g, 4 * kVtableSlots);
        a[21] = Raw(g);
        break;
    case kLmCtorId:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FloatBits(g); a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = Raw(g);
        a[10] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFEu, 0xFFu, 0x100u, 0x101u, 0xFFFFFFFFu, g.W()});
        a[11] = Raw(g); a[12] = Raw(g); a[13] = g.Pk({0u, 1u, 4u, 5u, 7u, 0xF8u, 0xFFu, g.R(256), g.R(256), g.R(256)});
        break;
    case kLmCtorName: {
        a[6] = Raw(g); a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = FloatBits(g); a[10] = Raw(g);
        a[11] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFEu, 0xFFu, 0x100u, 0x101u, 0xFFFFFFFFu, g.W()});
        a[12] = g.Pk({0u, 1u, 5u, 10u, 20u, 22u, 23u, 24u, 25u, 30u, 40u, 0x50u, g.R(0x51), g.R(24)});
        a[13] = g.Pk({0u, 1u, 5u, 10u, 14u, 15u, 16u, 17u, 24u, 40u, 0x38u, g.R(0x39), g.R(16)});
        a[14] = g.W();
        a[15] = g.P(10) ? 1u : 0u;
        if (a[15]) a[13] = a[12];
        a[4] = AuxOffset(g, 0x60);
        a[5] = a[4];
        if (!a[15]) {
            for (int tries = 0; tries < 32; ++tries) {
                a[5] = AuxOffset(g, 0x60);
                if (a[5] + 0x60u <= a[4] || a[4] + 0x60u <= a[5]) break;
            }
            if (!(a[5] + 0x60u <= a[4] || a[4] + 0x60u <= a[5])) a[5] = a[4] >= 0x60u ? 0u : 0x200u;
        }
        a[16] = Raw(g); a[17] = Raw(g); a[18] = g.Pk({0u, 1u, 4u, 5u, 7u, 0xF8u, 0xFFu, g.R(256), g.R(256), g.R(256)});
        a[19] = Raw(g); a[20] = Raw(g);
        break;
    }
    case kLmStart:
        a[4] = g.Pk({0u, 0u, RawNZ(g), RawNZ(g), 0x00ABCDEFu});
        a[5] = Raw(g); a[6] = Raw(g); a[7] = Raw(g); a[8] = FloatBits(g);
        a[9] = AuxOffset(g, 0x20);
        a[10] = Raw(g);
        a[11] = g.Pk({0u, Raw(g), Raw(g), 1u, 0x00ABCDEFu});
        a[12] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kBodyOnly:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kLmProcess: {
        a[4] = g.Pk({0u, 0u, 4u, 4u, 1u, 5u, 0xFEu, 0xFFu, g.R(256), g.R(256)}) | (g.P(30) ? (g.W() & 0xFFFFFF00u) : 0u);
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = Slot(g, 0x100, 0x30);
        a[7] = g.Pk({0u, 0u, 1u, 2u, Raw(g)});
        a[8] = g.Pk({0u, 1u, 1u, 1u, 2u, 0xFFu}) | (g.P(30) ? (g.W() & 0xFFFFFF00u) : 0u);
        a[9] = AuxOffset(g, 0x20);
        a[10] = Raw(g);
        a[11] = g.Pk({0u, 0u, 0u, Raw(g), 1u});
        a[12] = g.Pk({0u, 1u, 1u, 0x100u, 0x101u, 0xFFFFFF00u, g.W(), g.W() | 1u, g.W() & ~0xFFu});
        a[13] = Slot(g, 0x400, 0x30);
        a[14] = Slot(g, 0x800, 0x30);
        a[15] = g.P(85) ? CoordBits(g) : FloatBits(g);
        a[16] = Raw(g);
        a[24] = g.W();
        a[26] = Raw(g); a[27] = Raw(g); a[23] = Raw(g);
        a[21] = g.P(80) ? CoordBits(g) : FloatBits(g);
        a[22] = g.P(80) ? CoordBits(g) : FloatBits(g);
        a[25] = g.R(8);
        {   // tiempo actual, paso y fin del bucle: aleatorios, a caballo del fin, iguales o con NaN
            const u32 mode = a[25];
            const float end = std::fabs(F(CoordBits(g))) + 0.5f, step = std::fabs(F(CoordBits(g))) * 0.01f + 0.01f;
            a[19] = B(end); a[20] = g.P(80) ? CoordBits(g) : FloatBits(g); a[18] = B(step);
            switch (mode) {
            case 0: a[17] = g.P(80) ? CoordBits(g) : FloatBits(g); a[19] = g.P(80) ? CoordBits(g) : FloatBits(g); a[18] = g.P(80) ? CoordBits(g) : FloatBits(g); break;
            case 1: a[17] = B(end + step * 0.5f); break;                       // a caballo del fin: actual > fin y actual - paso <= fin
            case 2: a[17] = B(end); break;                                     // actual igual al fin
            case 3: a[17] = B(end + step); break;                              // actual - paso == fin
            case 4: a[17] = B(std::nextafter(end + step, 1e30f)); break;       // actual - paso apenas por encima del fin
            case 5: a[17] = B(end - step); break;                              // actual < fin
            case 6: a[17] = B(std::nextafter(end, 1e30f)); break;              // actual apenas por encima
            default: a[17] = FloatBits(g); a[18] = FloatBits(g); break;
            }
        }
        a[3] = a[3];
        break;
    }
    case kHsDtor:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[6] = Slot(g, 0x100, 0x30);
        a[7] = Slot(g, 0x400, 0x30);
        a[8] = AuxOffset(g, 4 * kVtableSlots);
        a[9] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF0000u, g.W(), g.W(), 0x7FFFu, 0x8000u});
        a[10] = g.Pk({0u, 0u, 0u, 1u, 2u});
        a[11] = g.R(2);
        a[12] = g.R(2);
        a[13] = Raw(g); a[14] = Raw(g);
        break;
    case kHsAbort:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g); a[7] = Raw(g);
        break;
    case kInterior:
    case kIgCtor:
        break;
    case kEulerTo: {
        const u32 mode = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u});
        a[4] = mode;
        a[5] = g.P(15) ? g.W() : g.R(32);
        const u32 par = (a[5] >> 2) & 1, i = kEulFirst[(a[5] >> 3) & 3], j = kEulNext[i + par], k = kEulNext[i - par + 1], rep = (a[5] >> 1) & 1;
        std::array<float, 9> m{};
        const auto idx = [](u32 r, u32 cc) { return 3 * r + cc; };
        if (mode == 1) {   // rotacion a partir de angulos aleatorios
            const double x = (double(g.R(100000)) / 100000.0 - 0.5) * 6.4, y = (double(g.R(100000)) / 100000.0 - 0.5) * 6.4, z = (double(g.R(100000)) / 100000.0 - 0.5) * 6.4;
            const double cx = std::cos(x), sx = std::sin(x), cy = std::cos(y), sy = std::sin(y), cz = std::cos(z), sz = std::sin(z);
            m = {float(cy * cz), float(cy * sz), float(-sy), float(sx * sy * cz - cx * sz), float(sx * sy * sz + cx * cz), float(sx * cy), float(cx * sy * cz + sx * sz), float(cx * sy * sz - sx * cz), float(cx * cy)};
        } else {
            for (auto& v : m) v = F(g.P(70) ? CoordBits(g) : FiniteBits(g));
        }
        if (mode == 2 || mode == 3) {   // cerca del bloqueo: los dos elementos del radical pequenos, o justo en el umbral
            const u32 r1 = rep ? idx(i, j) : idx(i, i), r2 = rep ? idx(i, k) : idx(j, i);
            const double thr = double(F(kEulThreshold));
            if (mode == 2) {
                m[r1] = float((double(g.R(2001)) - 1000.0) / 1000.0 * thr);
                m[r2] = float((double(g.R(2001)) - 1000.0) / 1000.0 * thr);
            } else {
                const double ang = double(g.R(1000)) / 1000.0 * 6.2831853, scale = 1.0 + double(int(g.R(9)) - 4) * 1.1920929e-7;
                m[r1] = float(thr * scale * std::cos(ang));
                m[r2] = float(thr * scale * std::sin(ang));
                if (g.P(20)) { m[r1] = float(thr); m[r2] = 0.0f; }
            }
        }
        for (u32 q = 0; q < 9; ++q) a[6 + q] = B(m[q]);
        if (mode == 4) {   // excepcionales: ceros con signo, denormales, infinitos y NaN en algunos elementos
            for (u32 q = 0; q < 9; ++q) if (g.P(30)) a[6 + q] = FloatBits(g);
        }
        if (mode == 5) {   // magnitudes enormes y minusculas
            for (u32 q = 0; q < 9; ++q) if (g.P(50)) a[6 + q] = B(F(CoordBits(g)) * (g.P(50) ? 1e18f : 1e-18f));
        }
        // punteros de salida
        const u32 pm = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u});
        const auto outp = [&](u32 sel) { return std::array<u32, 2>{sel, sel == 0 ? 4 * g.R(11) : AuxOffset(g, 4) + (g.P(10) ? 1u + g.R(3) : 0u)}; };
        std::array<u32, 2> px = outp(1), py = outp(2), pz = outp(3);
        switch (pm) {
        case 1: pz = px; break;
        case 2: pz = py; break;
        case 3: py = px; break;
        case 4: py = px; pz = px; break;
        case 5: px = outp(0); break;
        case 6: pz = outp(0); break;
        case 7: std::swap(px, pz); break;
        case 8: px = outp(0); pz = outp(0); break;
        default: break;
        }
        a[15] = px[0]; a[16] = px[1]; a[17] = py[0]; a[18] = py[1]; a[19] = pz[0]; a[20] = pz[1];
        break;
    }
    case kEulerFrom: {
        const u32 mode = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        a[4] = mode;
        a[5] = g.P(15) ? g.W() : g.R(32);
        const auto ang = [&]() -> u32 {
            switch (mode) {
            case 0: return B(float((double(g.R(200001)) - 100000.0) / 10000.0));
            case 1: { const float h = float(int(g.R(33)) - 16) * 1.57079632679f; return g.P(60) ? B(h) : B(std::nextafter(h, g.P(50) ? 1e30f : -1e30f)); }
            case 2: return g.P(50) ? (g.W() & 0x807FFFFFu) : (g.W() & 0x80FFFFFFu & 0xBF800000u) | 0x00800000u;   // denormales y diminutos
            case 3: return B(float((double(g.R(200001)) - 100000.0)));
            case 4: return B(float((double(g.R(2001)) - 1000.0) * double(1u << 20) * (1.0 + double(g.R(4096)))));   // grandes (1e6 .. 1e13): sin oraculo exacto de fsin/fcos
            case 5: return B(F(0x5F000000u + g.R(5)) * (g.P(50) ? -1.0f : 1.0f));   // alrededor de 2^63 y mayores (fsin/fcos dejan el operando)
            case 6: return FloatBits(g);                                             // incluye NaN e infinitos
            default: return g.Pk({0u, 0x80000000u, 0u, 0x80000000u, 0x3F800000u, 0xBF800000u});
            }
        };
        a[6] = ang(); a[7] = ang(); a[8] = ang();
        if (mode == 0 && g.P(20)) a[7] = B(F(a[6]) * 0.5f);
        break;
    }
    case kScan: {
        a[31] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        a[4] = g.Pk({0u, 0x3E4CCCCDu, 0x3E4CCCCCu, 0x3E4CCCCEu, 0x3F800000u, 0x80000000u, 0xBF800000u, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x3DCCCCCDu, 0x3E000000u, 0x40000000u, FloatBits(g), 0x3E4CCCCDu, 0x3E4CCCCDu});
        const u32 rainy = F(a[4]) >= 0.2f ? 1u : 0u;   // 0.2f = 0x3E4CCCCD
        u32 type = g.Pk({0u, 1u, 2u, 3u, 4u, 4u, 4u, 5u, 6u, 7u, 7u, 7u, 8u, 9u});
        if (g.P(65)) type = rainy ? 4u : (type == 4 ? 7u : type);   // el tipo que pasa el filtro de la lluvia
        a[5] = type;
        a[6] = g.Pk({0u, 1u, 1u});
        a[7] = 4 * g.R(0x1C0);
        a[8] = AuxOffset(g, 0x40);
        a[27] = Slot(g, 0x800, 0x30);
        a[16] = Slot(g, 0x100, 0x40);
        a[17] = Slot(g, 0x400, 0x40);
        a[15] = g.R(2);
        for (u32 q = 0; q < 3; ++q) { a[9 + q] = CoordBits(g); a[12 + q] = CoordBits(g); a[18 + q] = CoordBits(g); }
        if (g.P(25)) for (u32 q = 0; q < 3; ++q) a[18 + q] = a[(a[6] ? 12 : 9) + q];   // peaton sobre el efecto (distancia cero)
        // distancia al cuadrado aproximada (float) para poner el umbral en el borde
        float d2 = 0.0f;
        {
            const float ex = F(a[6] ? a[12] : a[9]), ey = F(a[6] ? a[13] : a[10]), ez = F(a[6] ? a[14] : a[11]);
            const float dx = F(a[18]) - ex, dy = F(a[19]) - ey, dz = F(a[20]) - ez;
            d2 = (dz * dz + dy * dy) + dx * dx;
        }
        const u32 d2b = B(d2);
        const u32 tm = g.R(10);
        a[21] = tm == 0 ? d2b : (tm == 1 ? d2b + 1u : (tm == 2 ? d2b - 1u : (tm == 3 ? B(d2 * 2.0f + 1.0f) : (tm == 4 ? B(d2 * 0.5f) : (tm == 5 ? 0x7F7FFFFFu : (tm == 6 ? FloatBits(g) : (tm == 7 ? 0x7F800000u : B(d2 + 1.0f))))))));
        const float rd = std::sqrt(d2);
        const u32 rm = g.R(9);
        a[22] = rm == 0 ? 0xBF800000u : (rm == 1 ? 0u : (rm == 2 ? B(rd) : (rm == 3 ? B(std::nextafter(rd, 1e30f)) : (rm == 4 ? B(std::nextafter(rd, -1.0f)) : (rm == 5 ? B(rd * 2.0f + 1.0f) : (rm == 6 ? 0x7FC00000u : (rm == 7 ? 0x7F7FFFFFu : FloatBits(g))))))));
        a[23] = g.R(64);
        a[24] = Raw(g);
        a[25] = g.Pk({0u, 1u, 1u, 1u, 0x100u, 0xFFFFFF00u, g.W()});
        a[26] = g.Pk({0u, 1u, 1u, 1u, 0x100u, 0xFFFFFF00u, g.W()});
        a[28] = Raw(g); a[29] = Raw(g);
        a[30] = (g.W() & 0xFFFFFF00u) | type;
        break;
    }
    case kBest: {
        for (u32 q = 0; q < 10; ++q) {
            a[4 + q] = g.P(30) ? 0u : Raw(g);
            a[14 + q] = g.P(35) ? 0u : RawNZ(g);
        }
        const u32 mode = g.R(5);   // 0 aleatorios, 1 crecientes, 2 muchos iguales, 3 sin NaN, 4 con distancias iguales a FLT_MAX
        u32 prev = 0;
        for (u32 q = 0; q < 10; ++q) {
            u32 v = FloatBits(g);
            if (mode == 1) v = B(float(q * 10 + g.R(5)) * (g.P(10) ? -1.0f : 1.0f));
            if (mode == 2 && q > 0 && g.P(60)) v = prev;
            if (mode == 3) while (IsNanBits(v)) v = FloatBits(g);
            if (mode == 4 && g.P(40)) v = g.Pk({0x7F7FFFFFu, 0x7F800000u, 0xFF7FFFFFu});
            a[24 + q] = v; prev = v;
        }
        if (g.P(45)) a[8] = 0u;   // sin entidad de refugio (tipo 4): recorre los diez
        if (g.P(12)) for (u32 q = 0; q < 10; ++q) a[14 + q] = 0u;   // ningun candidato con efecto
        const u32 pm = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u});
        const auto outp = [&](u32 sel) { return std::array<u32, 2>{sel, sel == 0 ? 4 * g.R(36) : AuxOffset(g, 4)}; };
        std::array<u32, 2> pe = outp(1), pn = outp(2);
        if (pm == 1) pe = outp(0);                          // *effect dentro del escaner
        else if (pm == 2) pn = outp(0);                     // *entity dentro del escaner
        else if (pm == 3) { pe = outp(0); pn = outp(0); }   // los dos dentro
        else if (pm == 4) pn = pe;                          // el mismo puntero
        a[34] = pe[0]; a[35] = pe[1]; a[36] = pn[0]; a[37] = pn[1];
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
