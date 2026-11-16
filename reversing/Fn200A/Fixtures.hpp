#pragma once
// Fixtures del banco Fn200A (lote 200A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en C++ con hook en el commit 8e4d6caa2e, sin banco; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 31 funciones.
//   idx  direccion  nombre real                                                                  forma
//    0   0x5B2AB0   CBoundingBox alias del constructor (el listado decia CCam)                  thiscall() `ret`: push esi; call 0x40FAF0; mov eax,esi
//    1   0x67BA70   CTaskComplexStuckInAir cuerpo del destructor                                thiscall() `ret`: [this]=870608h; jmp 0x61A3D0
//    2   0x67C0C0   CTaskSimpleGetUp::Clone (V, ranura 1)                                       thiscall() `ret`: new(10h) + base 0x61A390; +8, +9, +0Ch = 0
//    3   0x67C130   CTaskComplexGetUpAndStandStill::Clone (V)                                    thiscall() `ret`: new(0Ch) + base 0x61A3B0
//    4   0x67C180   CTaskSimpleFall::Clone (V)                                                   thiscall() `ret`: new(20h) + base 0x61A390; copia grupo, tiempo total, animacion
//    5   0x67C200   CTaskComplexFallAndGetUp::Clone (V)                                          thiscall() `ret`: new(18h) + base 0x61A3B0; copia +0Ch, +10h, +14h
//    6   0x67C2E0   CTaskComplexInAirAndLand::Clone (V)                                          thiscall() `ret`: new(10h) + base 0x61A3B0; normaliza los bytes +0Ch y +0Dh
//    7   0x67C360   CTaskSimpleInAir::Clone (V)                                                  thiscall() `ret`: new(3Ch) + constructor 0x678CD0 con los tres bits de +24h
//    8   0x67C3F0   CTaskSimpleLand::Clone (V)                                                   thiscall() `ret`: new(14h) + base 0x61A390 + constructor 0x678E90 en linea
//    9   0x67C480   CTaskComplexFallToDeath::Clone (V)                                           thiscall() `ret`: new(24h) + constructor 0x679040
//   10   0x67C750   CTaskSimpleGetUp destructor con borrado                                      thiscall(banderas) `ret 4`: cuerpo 0x677F80; con el bit 0, CTask::operator delete 0x61A5B0
//   11   0x6F6300   CTrain destructor con borrado                                                 thiscall(banderas) `ret 4`: cuerpo 0x6F54D0; con el bit 0, CVehicle::operator delete 0x6E2D90 (pool de vehiculos 0xB74494)
//   12   0x6F5930   CTrain::GetDooorAngleOpenRatioU32 (V, ranura 29)                             thiscall(codigo) `ret 4`: 8 -> 3, 9 -> 5, 10 -> 2, 11 -> 4: salta a la ranura 30; otro: 0.0f (ST0)
//   13   0x6F59C0   CTrain::GetDooorAngleOpenRatio (V, ranura 30)                                 thiscall(puerta) `ret 4`: CDoor::GetAngleOpenRatio 0x6F47E0 de m_aDoors[puerta] (ST0)
//   14   0x6F5EE0   CTrain::IsRoomForPedToLeaveCar (V, ranura 45)                                 thiscall(2 argumentos) `ret 8`: mov al,1
//   15   0x6F6370   CTrain::OpenDoor (V, ranura 27)                                               thiscall(ped, componente, puerta, razon, sonido) `ret 14h`: puertas 2 y 3; x87 en la rama del Streak
//   16   0x6F7190   CTrain::Teleport (V, ranura 14)                                              thiscall(x, y, z, orientacion) `ret 10h`
//   17   0x411A30   CVector::operator/=(float)                                                   thiscall(divisor) `ret 4`: r = 1.0 / d en el registro x87; x, y, z *= r
//   18   0x411BA0   CColPoint::GetNormalAddress                                                  thiscall() `ret`: lea eax,[ecx+10h]
//   19   0x411BF0   CColPoint::SetSurfaceTypeA                                                    thiscall(tipo) `ret 4`: byte [ecx+20h] = byte bajo del argumento
//   20   0x43E1A0   salto a Flush de las listas simples de punteros                              thiscall() : jmp 0x552400
//   21   0x54F440   CMatrixLinkList constructor                                                   thiscall() `ret`: 13 dwords a cero
//   22   0x54F490   CPlaceable cuerpo del destructor                                              thiscall() `ret`: vtable 863C40h; RemoveMatrix 0x54F3B0 y --numMatrices si habia matriz; m_matrix = 0xB74240
//   23   0x54F5D0   CPlaceable destructor con borrado                                             thiscall(banderas) `ret 4`: igual y operator delete 0x8214BD con el bit 0
//   24-30 0x54FA10, 0x54FA60, 0x54FAB0, 0x54FB00, 0x54FB50, 0x54FBA0, 0x54FBF0: vaciado de un bloque de dos punteros (siete copias identicas)   thiscall() `ret`
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

namespace F200AFix {
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
// Mitad baja (0..0x7FC) o alta (0x800..) de una arena auxiliar, para colocar dos objetos disjuntos en la misma arena
inline u32 AuxLow(Gen& g, u32 span) { return 4 * g.R((0x800u - span) / 4 + 1); }
inline u32 AuxHigh(Gen& g, u32 span) { return 0x800u + 4 * g.R((0x800u - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const auto avoidStack = [](u32 v) { return (v >= 0x00080000u && v < 0x00400000u) ? (v | 0x80000000u) : v; };   // la pila del hilo esta en esa zona: Tag() la anota aparte
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = avoidStack(g.W()), w6 = avoidStack(g.W()), w7 = avoidStack(g.W());
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword no nulo
inline u32 RawNZ(Gen& g) { u32 v = Raw(g); return v ? v : 0x1234567u; }
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float sin NaN
inline u32 FloatNoNan(Gen& g) { u32 v = FloatBits(g); while (IsNanBits(v)) v = FloatBits(g); return v; }

// Un float (como bits) finito (ni NaN ni infinito)
inline u32 FloatFinite(Gen& g) { u32 v = FloatBits(g); while ((v & 0x7F800000u) == 0x7F800000u) v = FloatBits(g); return v; }
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }

// Clases de funcion (forma)
enum Kind : u32 { kBBox, kComplexDtor, kCloneGetUp, kCloneStand, kCloneFall, kCloneFallGetUp, kCloneAirLand, kCloneInAir, kCloneLand, kCloneDeath, kTaskSdd, kTrainSdd, kDoorU32, kDoorRatio, kIsRoom, kOpenDoor,
                  kTeleport, kDivide, kNormalAddr, kSetSurface, kFlushJump, kMllCtor, kPlDtor, kPlSdd, kArrReset };
inline constexpr std::array<Kind, kFunctions> kKind{kBBox, kComplexDtor, kCloneGetUp, kCloneStand, kCloneFall, kCloneFallGetUp, kCloneAirLand, kCloneInAir, kCloneLand, kCloneDeath, kTaskSdd, kTrainSdd, kDoorU32,
                                                    kDoorRatio, kIsRoom, kOpenDoor, kTeleport, kDivide, kNormalAddr, kSetSurface, kFlushJump, kMllCtor, kPlDtor, kPlSdd,
                                                    kArrReset, kArrReset, kArrReset, kArrReset, kArrReset, kArrReset, kArrReset};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5B2AB0, 0x67BA70, 0x67C0C0, 0x67C130, 0x67C180, 0x67C200, 0x67C2E0, 0x67C360, 0x67C3F0, 0x67C480, 0x67C750, 0x6F6300, 0x6F5930, 0x6F59C0, 0x6F5EE0, 0x6F6370,
                                                    0x6F7190, 0x411A30, 0x411BA0, 0x411BF0, 0x43E1A0, 0x54F440, 0x54F490, 0x54F5D0, 0x54FA10, 0x54FA60, 0x54FAB0, 0x54FB00, 0x54FB50, 0x54FBA0, 0x54FBF0};
// Bytes que cada funcion toca a partir de su puntero `this` (o del primer objeto)
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x14, 4, 4, 0x1C, 0x18, 0x10, 0x28, 0x10, 0x24, 4, 4, 4, 4, 4, 0x680, 0x5C, 0xC, 4, 0x24, 4, 0x1FC, 0x18, 0x18, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14};
// Memoria global del juego tocada
inline constexpr u32 kClock = 0xB7CB84, kNumMatrices = 0xB74238, kVehiclePool = 0xB74494, kBenchMarker = 0xA9B0B0;
inline constexpr u32 kVehicleSize = 0xA18;       // tamano del hueco del pool de vehiculos (el EXE divide por la constante magica 0x6572EC3)
inline constexpr u32 kDoorsBase = 0x5D8, kDoorSize = 0x18, kNodesBase = 0x668;
// Vtable falsa de las ranuras de CTrain (arena C): 0x80 dwords; la ranura 30 (0x78) la ocupa el falso de GetDooorAngleOpenRatio
inline constexpr u32 kFakeVtableSpan = 0x200;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del primer objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kBBox: a[4] = EAX del falso del constructor, a[5] = dword que deja en [this]
//   kComplexDtor: a[4] = subtarea (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = desplazamiento de la vtable falsa en C
//   kCloneGetUp, kCloneStand: a[4] = memoria de CTask::operator new (0 nula, 1 en B), a[5] = desplazamiento en B
//   kCloneFall: a[4] = grupo (+10h), a[5] = tiempo total (+18h), a[6] = animacion (+0Ch), a[7] = memoria, a[8] = desplazamiento en B
//   kCloneFallGetUp: a[4] = +0Ch, a[5] = +10h, a[6] = +14h, a[7] = memoria, a[8] = desplazamiento en B
//   kCloneAirLand: a[4] = byte +0Ch, a[5] = byte +0Dh, a[6] = memoria, a[7] = desplazamiento en B
//   kCloneInAir: a[4] = byte +24h (banderas), a[5] = reloj (0xB7CB84), a[6] = memoria, a[7] = desplazamiento en B
//   kCloneLand: a[4] = animacion (+0Ch), a[5] = memoria, a[6] = desplazamiento en B
//   kCloneDeath: a[4..6] = posicion (+0Ch, +10h, +14h), a[7] = byte +20h, a[8] = byte +21h, a[9] = memoria, a[10] = desplazamiento en B
//   kTaskSdd: a[4] = banderas (pila), a[5] = EAX del falso del cuerpo, a[6] = dword que el falso deja en [this]
//   kTrainSdd: lo anterior mas a[7] = desplazamiento del pool de vehiculos en B, a[8] = desplazamiento de los estados en D, a[9] = indice (0..63), a[10] = resto de la division, a[11] = primer hueco libre (+0Ch del pool)
//   kDoorU32: a[4] = codigo de la puerta (pila), a[5] = float que devuelve la ranura 30 falsa, a[6] = desplazamiento de la vtable falsa en C
//   kDoorRatio: a[4] = puerta (0..255), a[5] = float que devuelve CDoor::GetAngleOpenRatio falso
//   kIsRoom: a[4] = puerta (pila), a[5] = puntero (pila, crudo)
//   kOpenDoor: a[4] = modo del ped (0 nulo, 1 en C sin matriz, 2 en C con matriz en D), a[5] = desplazamiento del ped en C, a[6] = componente (pila, crudo), a[7] = puerta (pila, 0..255), a[8] = razon (float, pila),
//         a[9] = sonido (pila, crudo), a[10] = indice del modelo (word en +22h), a[11] = marco izquierdo presente, a[12] = marco derecho presente, a[13] = desplazamiento del marco izquierdo en B (mitad baja),
//         a[14] = desplazamiento del marco derecho en B (mitad alta), a[15] = desplazamiento de la matriz del ped en D (mitad baja), a[16] = desplazamiento del grupo de eventos en D (mitad alta),
//         a[17] = angulo abierto, a[18] = angulo cerrado, a[19] = modo del angulo actual (0 igual al cerrado, 1 el de a[20]), a[20] = angulo actual, a[21] = eje (0..2), a[22] = modo del angulo tras CDoor::Open falso
//         (0 el cerrado, 1 el actual, 2 el de a[24]), a[23] = reloj, a[24] = angulo tras Open, a[25] = modo de la posicion de los marcos (0 relleno, 1 tres -0, 2 (-0, 0, 1), 3 NaN, 4 (inf, -inf, denormal)), a[26..34] = nueve dwords de la rotacion que deja CMatrix::SetRotate falso
//   kTeleport: a[4..6] = posicion (pila), a[7] = orientacion (dword de pila), a[8] = matriz (0 nula, 1 en B), a[9] = desplazamiento de la matriz en B
//   kDivide: a[4] = divisor (pila), a[5..7] = x, y, z (objeto)
//   kNormalAddr, kFlushJump, kMllCtor: nada mas
//   kSetSurface: a[4] = argumento (pila)
//   kPlDtor: a[4] = contador de matrices (0xB74238), a[5] = m_matrix (+14h, crudo); kPlSdd: ademas a[6] = banderas (pila)
//   kArrReset: a[4] = puntero A (+0), a[5] = puntero B (+4), a[6] = contador (+8), a[7] = entero (+0Ch), a[8] = byte +10h
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x870608u, 0x863C40u, 0x8703E0u});
    auto& a = c.a;
    const auto mem = [&] { return g.Pk({0u, 1u, 1u, 1u}); };
    const auto flags = [&] { return g.Pk({0u, 1u, 1u, 3u, 0xFFFFFFFEu, 0x100u, 0x101u, g.W(), (g.W() & 0xFFFFFF00u) | 1u, (g.W() & 0xFFFFFF00u)}); };
    switch (kKind[c.fn]) {
    case kBBox:
        a[4] = Raw(g); a[5] = g.Pk({0u, g.W(), 0xFFFFFFFFu, 0x40E8B8u});
        break;
    case kComplexDtor:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 4);
        a[6] = AuxOffset(g, kFakeVtableSpan);
        break;
    case kCloneGetUp:
    case kCloneStand:
        a[4] = mem(); a[5] = AuxOffset(g, 0x40);
        break;
    case kCloneFall:
        a[4] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu}); a[5] = g.Pk({0u, 0x5F5E0FFu, g.W(), FloatBits(g)}); a[6] = g.Pk({0u, 0x18u, 0xE6u, g.W(), 0xFFFFFFFFu});
        a[7] = mem(); a[8] = AuxOffset(g, 0x40);
        break;
    case kCloneFallGetUp:
        a[4] = g.Pk({0u, 0x18u, 0xE6u, g.W(), 0xFFFFFFFFu}); a[5] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu}); a[6] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu});
        a[7] = mem(); a[8] = AuxOffset(g, 0x40);
        break;
    case kCloneAirLand:
        a[4] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu}); a[5] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[6] = mem(); a[7] = AuxOffset(g, 0x40);
        break;
    case kCloneInAir:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0xFFu, 0xF8u, 0xA5u, g.W() & 0xFFu, g.W() & 0xFFu});
        a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[6] = mem(); a[7] = AuxOffset(g, 0x40);
        break;
    case kCloneLand:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x8000u, g.W(), g.W()});
        a[5] = mem(); a[6] = AuxOffset(g, 0x40);
        break;
    case kCloneDeath:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        a[7] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        a[8] = g.Pk({0u, 8u, 0x10u, 0x18u, 0xE0u, 0xFFu, 0xE7u, g.W() & 0xFFu, g.W() & 0xFFu});
        a[9] = mem(); a[10] = AuxOffset(g, 0x40);
        break;
    case kTaskSdd:
        a[4] = flags(); a[5] = g.Pk({0u, Raw(g), Raw(g)}); a[6] = g.Pk({0u, g.W(), 0x8703E0u, 0x86D4CCu});
        break;
    case kTrainSdd: {
        a[4] = flags(); a[5] = g.Pk({0u, Raw(g), Raw(g)}); a[6] = g.Pk({0u, g.W(), 0x872370u, 0x86D4CCu});
        a[7] = AuxOffset(g, 0x14);
        a[8] = 8u + AuxOffset(g, 0x100);
        a[9] = g.Pk({0u, 1u, 2u, 3u, 5u, 7u, 15u, 30u, 31u, 62u, 63u, g.R(40), g.R(64)});   // indice dentro del pool (0..63) y hueco usado: CPool::Delete de la DLL lo exige (assert)
        a[10] = g.Pk({0u, 1u, kVehicleSize - 1u, kVehicleSize / 2u, g.R(kVehicleSize), g.R(kVehicleSize), g.R(kVehicleSize), 1u});
        const u32 idx = a[9];
        a[11] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, 5u, 100u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, g.R(40)});
        break;
    }
    case kDoorU32:
        a[4] = g.Pk({8u, 8u, 9u, 9u, 10u, 10u, 11u, 11u, 7u, 12u, 13u, 16u, 17u, 18u, 19u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 0x80u, 0xFFu, 0x108u, 0xFFFFFFF8u, 0xFFFFFFFFu, 0x80000008u, 0x80000000u, g.W()});
        a[5] = FloatBits(g);
        a[6] = AuxOffset(g, kFakeVtableSpan);
        break;
    case kDoorRatio:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0x7Fu, 0x80u, 0xFFu, g.R(256)});
        a[5] = FloatBits(g);
        break;
    case kIsRoom:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kOpenDoor: {
        a[4] = g.Pk({0u, 1u, 2u, 2u});
        a[5] = AuxOffset(g, 0x60);
        a[6] = Raw(g);
        a[7] = g.Pk({2u, 2u, 2u, 2u, 3u, 3u, 3u, 3u, 0u, 1u, 4u, 5u, 6u, 7u, 8u, 0x80u, 0xFFu, g.R(256)});
        a[8] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0x3F000000u, FloatBits(g), FloatBits(g), FloatBits(g), 0xBF800000u, 0x7F800000u});
        a[9] = Raw(g);
        a[10] = g.Pk({0x23Au, 0x23Au, 0x23Au, 0x239u, 0x23Bu, 0u, 0xFFFFu, g.W() & 0xFFFFu});
        a[11] = g.Pk({1u, 1u, 1u, 1u, 0u}); a[12] = g.Pk({1u, 1u, 1u, 1u, 0u});
        a[13] = AuxLow(g, 0x60); a[14] = AuxHigh(g, 0x60);
        a[15] = AuxLow(g, 0x40); a[16] = AuxHigh(g, 0x40);
        a[17] = FloatBits(g); a[18] = FloatBits(g);
        a[19] = g.Pk({0u, 0u, 1u});
        a[20] = FloatBits(g);
        a[21] = g.R(3);
        a[22] = g.R(3);
        a[23] = g.Pk({0u, 1u, g.W(), g.W()});
        a[24] = FloatBits(g);
        a[25] = g.Pk({0u, 0u, 1u, 2u, 3u, 4u});
        for (u32 i = 26; i <= 34; ++i) a[i] = FloatBits(g);
        // el resultado del x87 de la rama del Streak combina tres floats (abierta, cerrada, razon): como mucho un NaN y, con NaN, los otros dos finitos (la x87 elige el NaN de mayor mantisa entre dos: no se modela)
        u32* const trio[3]{&a[17], &a[18], &a[8]};
        int nans = 0;
        for (u32* p : trio) nans += IsNanBits(*p) ? 1 : 0;
        if (nans) {
            bool kept = false;
            for (u32* p : trio) {
                if (IsNanBits(*p) && !kept) kept = true;
                else *p = FloatFinite(g);
            }
        }
        break;
    }
    case kTeleport:
        a[4] = FloatBits(g); a[5] = FloatBits(g); a[6] = FloatBits(g);
        a[7] = g.Pk({0u, 1u, 1u, 0x100u, 0x101u, (g.W() & 0xFFFFFF00u) | 1u, g.W() & 0xFFFFFF00u, g.W()});
        a[8] = g.Pk({0u, 1u, 1u});
        a[9] = AuxOffset(g, 0x40);
        break;
    case kDivide: {
        a[4] = g.Pk({FloatBits(g), FloatBits(g), FloatBits(g), 0x40400000u, 0x3DCCCCCDu, 0x3F800000u, 0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0x40000000u});
        for (u32 i = 5; i <= 7; ++i) a[i] = FloatBits(g);
        // un divisor NaN con un componente NaN: la x87 elige el de mayor mantisa (no se modela)
        if (IsNanBits(a[4])) for (u32 i = 5; i <= 7; ++i) if (IsNanBits(a[i])) a[i] = FloatFinite(g);
        break;
    }
    case kNormalAddr:
    case kFlushJump:
    case kMllCtor:
        break;
    case kSetSurface:
        a[4] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x1FFu, g.W(), g.W(), g.R(256)});
        break;
    case kPlDtor:
    case kPlSdd:
        a[4] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), 100u});
        a[5] = g.Pk({0u, 0u, RawNZ(g), RawNZ(g), 1u, 0xFFFFFFFFu});
        if (kKind[c.fn] == kPlSdd) a[6] = flags();
        break;
    case kArrReset:
        a[4] = g.Pk({0u, Raw(g), Raw(g), 1u, 0xFFFFFFFFu}); a[5] = g.Pk({0u, Raw(g), Raw(g), 1u, 0xFFFFFFFFu});
        a[6] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, 5u, 100u, g.W()});
        a[7] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu});
        a[8] = g.Pk({0u, 1u, 1u, 0x80u, 0xFFu, 2u, g.W() & 0xFFu});
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
