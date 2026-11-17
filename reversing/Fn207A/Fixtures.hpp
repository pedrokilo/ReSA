#pragma once
// Fixtures del banco Fn207A (lote 207A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 207A SIN validar: ver reversing/PendienteValidar/207A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores).
//   idx  direccion  nombre real                                                                         forma
//    0   0x634C10   CTaskComplexUseMobilePhone::CreateFirstSubTask (V)                                  thiscall(ped) `ret 4`: banderas +474h/+478h; IsPlayer 5DF8F0; ClearWeaponTarget 609C80; arma guardada +70Ch == 37h: SetCurrentWeapon 5E6280(0); +480h: byte +85h; ClearPlayerWeaponMode 50AB10; StandStill local + ProcessPed 62F370; new(20h) 61A5A0 + CTaskSimpleRunAnim 61A900 (91h, 4.0f, 642h, "PhoneIn")
//    1   0x634D60   CTaskComplexUseMobilePhone::ControlSubTask (V)                                      thiscall(ped) `ret 4`: asociaciones 91h/92h (4D68B0), x87 (0.85f, 1.566), AddWeaponModel 5E5ED0 (14Ah), 6348F0 por direccion, IsOutOfTime 420E30, new + 61A900 (92h, 643h, "PhoneOut"), FindTaskByType 600EE0 + PlayerControlZelda 6883D0
//    2   0x636FD0   CTaskComplexUseMobilePhone::Clone (V)                                               thiscall() `ret`: new(20h) 61A5A0; 61A3B0; +0Ch copiado; vtable 86E454; temporizador y bytes +1Ch/+1Dh a cero
//    3   0x639660   CTaskComplexUseMobilePhone destructor con borrado                                   thiscall(flags) `ret 4`: cuerpo 6348E0 (solo por direccion del EXE); delete 61A5B0 si bit 0 del BYTE; devuelve this
//    4   0x639680   CTaskComplexUseGoggles destructor con borrado                                       idem con el cuerpo 634F20
//    5   0x6396A0   CTaskComplexInWater destructor con borrado                                           idem con el cuerpo 635100
//    6   0x6396C0   CTaskComplexUseSequence cuerpo del destructor                                        thiscall() `ret`: [this] = 86E518; indice +0Ch != -1: tabla 0C178F0h + indice * 40h, dec dword [+3Ch]; a cero con bandera +38h: bandera a 0 y Flush 632C10; 61A3D0
//    7   0x421A50   CCarCtrl barrido de rectangulos (WeaveForOtherCarSweepOverlaps)                      cdecl(13 args) AL: velocidad x87 con fcos/fsin, bandera +70h intercambia, 16 llamadas a Test2DLineAgainst2DLine 4138D0 en orden
//    8   0x422E10   CCarCtrl sonda de altitud (FlyAIPlaneProbeGround)                                    cdecl(avion, elevacion, rumbo, salida) AL: inicio = velocidad * 50 + posicion; fcos/fsin; Normalise 59C910; extremo x87; ProcessLineOfSight 56BA00; z del punto
//    9   0x422F80   CCarCtrl margen por rumbo (FlyAIPlaneClearanceAhead)                                 cdecl(avion, rumbo) ST0: tabla 8A5B2C de 6 elevaciones con la sonda 422E10; primera ranura que acierta -> 100000.0f; maximo
//   10   0x424160   CVehiclePool::GetRef                                                                  thiscall(vehiculo) `ret 4`: indice = (vehiculo - [this]) / 0A18h con signo; EAX = (indice << 8) + byte [[this+4] + indice]; ECX = tabla; EDX = indice << 8
//   11   0x44D0E0   CPathFind::PreparePathData                                                           thiscall() `ret`
//   12   0x44D2B0   CPathFind::AllocatePathFindInfoMem                                                   `ret`
//   13   0x44D2C0   CPathFind::StoreNodeInfoCar                                                          `ret 44h`
//   14   0x44D2D0   CPathFind::StoreNodeInfoPed                                                          `ret 30h`
//   15   0x44D2E0   CPathFind::StoreDetachedNodeInfoPed                                                  `ret 38h`
//   16   0x44D2F0   CPathFind::StoreDetachedNodeInfoCar                                                  `ret 44h`
//   17   0x44E480   CVector2D::Normalise (EXE)                                                           thiscall() `ret`: x87 suma = x*x + y*y; <= 0 -> x = 1.0f; si no 1/sqrt y x, y
//   18   0x44FA30   CPathFind::RecordNodesClosestToCoors                                                 thiscall(11 args) `ret 2Ch`: limpia la marca (bit 1 de +19h) y guarda con FindNodeClosestToCoors 44F460
//   19   0x44FB60   CPathFind::RecordNodesInCircle                                                       thiscall(10 args) `ret 28h`: EAX = contador; circulo x87
//   20   0x44FEE0   CPathFind::FindNodePairClosestToCoors                                                thiscall(12 args) `ret 30h`: puntuacion x87, enlaces, Normalise 59C910, fpatan
//   21   0x452090   CPathFind::Find2NodesForCarCreation                                                  thiscall(6 args) `ret 18h`: Record 44FA30 (4 nodos) y These2NodesAreAdjacent 44D230 sobre ThePaths 96F050
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A de 0x2000 por el objeto CPathFind; B y D de 0x1000; C de 0x4000 por el peaton; guardas de 0x40; las vtables falsas viven al final de D) y los colaboradores falseados
// (el mismo falso en la direccion del EXE y en el cuerpo de la DLL). Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4.
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

namespace F207AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
constexpr unsigned kArenas = 4;
constexpr unsigned kPayload = 96;       // a[0..95]; las semillas de las arenas van en a[92..95]
constexpr unsigned kSeedAt = 92;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWinA = 0x2000;    // ventana de la arena A (el objeto CPathFind ocupa hasta +11E4h)
constexpr std::size_t kWin = 0x1000;     // ventana de las arenas B y D
constexpr std::size_t kWinC = 0x4000;    // ventana de la arena C (peaton, avion y nodos)
constexpr std::size_t kVtbl = 0xF00;     // las vtables falsas viven al final de la arena D
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
    std::array<u32, kPayload> a{};
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

// Desplazamiento de un objeto de `span` bytes en la ventana `win` de la arena A segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span, u32 win = u32(kWinA)) {
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en el tramo [lo, hi) de una arena auxiliar, sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 Aux(Gen& g, u32 lo, u32 hi, u32 span) { return lo + 4 * g.R((hi - lo - span) / 4 + 1); }
inline u32 AuxB(Gen& g, u32 span) { return Aux(g, 0, u32(kWin), span); }
inline u32 AuxD(Gen& g, u32 span) { return Aux(g, 0, u32(kVtbl), span); }     // la arena D reserva su final para las vtables falsas
inline u32 AuxC(Gen& g, u32 span) { return Aux(g, 0, u32(kWinC), span); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila)
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    const u32 huge = sign | (0x7E800000u + (g.W() & 0x007FFFFFu));
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, huge, g.W() & 0xFF7FFFFFu});
}
// Como FloatBits pero sin NaN senalizantes (la x87 silencia un NaN senalizante al cargarlo con fld y uno en memoria no): para los valores que el EXE carga en la x87 o recibe por ST0
inline u32 FloatQ(Gen& g) {
    u32 v = FloatBits(g);
    if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu) != 0) v |= 0x00400000u;
    return v;
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float finito normal positivo con exponente sesgado en [lo, hi)
inline u32 PosNormal(Gen& g, u32 lo, u32 hi) { return ((lo + g.R(hi - lo)) << 23) | (g.W() & 0x7FFFFFu); }
// Una posicion (bits): normalmente de magnitud moderada (para que las sumas sean comparables), a veces cualquiera (con una probabilidad baja de valores especiales)
inline u32 Coord(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    if (g.P(94)) return sign | PosNormal(g, 120, 136);
    return FloatQ(g);
}
// Un angulo (bits): ceros, pequenos, de moderados a miles de radianes, multiplos de pi con un ulp de margen, algun NaN o infinito. Nunca entre 2^20 y 2^63 (la reduccion de la x87 usa pi con 66 bits)
inline u32 Angle(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 pis[4]{0x40490FDBu, 0x3FC90FDBu, 0x40C90FDBu, 0x3F490FDBu};
    const u32 k = g.R(30);
    if (k < 3) return k == 0 ? 0u : (k == 1 ? 0x80000000u : sign | (g.W() & 0x7FFFFFu));      // cero, cero negativo, denormal
    if (k < 6) return sign | PosNormal(g, 100, 120);                                          // muy pequeno
    if (k < 16) return sign | PosNormal(g, 120, 130);                                         // hasta 2^3
    if (k < 20) return sign | PosNormal(g, 126, 140);                                         // hasta 2^13
    if (k < 24) return sign | (pis[g.R(4)] + g.R(5) - 2u);                                    // multiplos de pi
    if (k < 26) return sign | PosNormal(g, 140, 147);                                         // hasta 2^20
    if (k == 26) return sign | 0x7F800000u;                                                   // infinito
    if (k == 27) return sign | 0x7FC00000u | (g.W() & 0x3FFFFFu);                             // NaN silencioso
    if (k == 28) return sign | PosNormal(g, 190, 254);                                        // 2^63 o mas: la x87 deja el operando
    return sign | PosNormal(g, 127, 133);
}

// Clases de funcion (forma)
enum Kind : u32 { kPhoneFirst, kPhoneControl, kPhoneClone, kSdd, kSeqDtor, kSweep, kProbe, kClear, kPoolRef, kNop, kNorm, kRecord, kCircle, kPair, kFind2 };
inline constexpr std::array<Kind, kFunctions> kKind{kPhoneFirst, kPhoneControl, kPhoneClone, kSdd, kSdd, kSdd, kSeqDtor, kSweep, kProbe, kClear, kPoolRef, kNop, kNop, kNop, kNop, kNop, kNop, kNorm,
                                                    kRecord, kCircle, kPair, kFind2};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x634C10, 0x634D60, 0x636FD0, 0x639660, 0x639680, 0x6396A0, 0x6396C0, 0x421A50, 0x422E10, 0x422F80, 0x424160, 0x44D0E0, 0x44D2B0, 0x44D2C0,
                                                    0x44D2D0, 0x44D2E0, 0x44D2F0, 0x44E480, 0x44FA30, 0x44FB60, 0x44FEE0, 0x452090};
// Palabras de pila que consume la funcion (cdecl: las limpia el llamador; thiscall: `ret N`)
inline constexpr std::array<u32, kFunctions> kArgs{1, 1, 0, 1, 1, 1, 0, 13, 4, 2, 1, 0, 0, 17, 12, 14, 17, 0, 11, 10, 12, 6};
inline constexpr std::array<u32, kFunctions> kCdecl{0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x20, 0x20, 0x20, 4, 4, 4, 0x20, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 8, 0x11E4, 0x11E4, 0x11E4, 0x11E4};

// Memoria global del juego tocada
inline constexpr u32 kSeqTable = 0xC178F0;                    // CTaskSequences::ms_taskSequence (64 entradas de 0x40 bytes)
inline constexpr u32 kSeqBytes = 64 * 0x40;
inline constexpr u32 kMarker = 0xA9B0B0;                      // byte que la prueba negativa (mutate.py) lee para que las funciones vacias tengan algo observable; el juego real nunca lo pone a 0x5A
inline constexpr u32 kPathObjSize = 0x11E4;

// ------------------------------------------------------------------------------------------------ el mundo de nodos de CPathFind (RecordNodes*, FindNodePair): todo sale de una semilla
// Hasta 3 areas pobladas con 8 nodos de 0x1C bytes en la arena C (kNodeBase) y sus tablas de enlaces en la arena D (kLinkBase). Campos usados por el EXE: x, y, z (int16 en +8, +0Ah, +0Ch), base de enlaces (int16 en +10h),
// byte +18h (nibble bajo = numero de enlaces, bit 5 = poco trafico, bit 7 = agua) y byte +19h (bit 1 = marca).
constexpr u32 kNodeBase[3] = {0x100, 0x500, 0x900};
constexpr u32 kLinkBase[3] = {0x40, 0x340, 0x640};
struct World { u32 na; u32 id[3]; u32 n[3]; u32 nv[3]; };
inline World MakeWorld(u32 seed) {
    World w{};
    w.na = 1 + Rand(seed, 1) % 3;
    for (u32 k = 0; k < 3; ++k) {
        w.id[k] = (Rand(seed, 10 + k) % 24) * 3 + k;
        const u32 r = Rand(seed, 20 + k);
        w.n[k] = r % 10 < 2 ? 0u : (r >> 8) % 8;
        const u32 q = Rand(seed, 30 + k);
        w.nv[k] = q % 10 == 0 ? std::min<u32>(w.n[k] + 1, 8u) : (q >> 8) % (w.n[k] + 1);
    }
    return w;
}
inline u32 NodeCoord(u32 seed, u32 k, u32 i, u32 c) {   // palabra baja = el int16
    const u32 v = Rand(seed, 1000 + k * 100 + i * 10 + c), mode = v % 8, w = v >> 3;
    if (mode < 4) return u32(i32(w % 41) - 20) * 8u & 0xFFFFu;
    if (mode < 6) return u32(i32(w % 3201) - 1600) & 0xFFFFu;
    if (mode == 6) return w & 0xFFFFu;
    static constexpr i32 t[5]{0, 1, -1, 32767, -32768};
    return u32(t[w % 5]) & 0xFFFFu;
}
inline u32 NodeFlags18(u32 seed, u32 k, u32 i) {
    const u32 v = Rand(seed, 2000 + k * 100 + i);
    const u32 nl = (v % 8 == 0) ? ((v >> 8) & 0xF) : ((v >> 8) & 0xF) % 5;
    return nl | (((v >> 12) & 1) << 4) | (((v >> 13) % 4 == 0 ? 1u : 0u) << 5) | (((v >> 14) & 1) << 6) | (((v >> 16) % 4 == 0 ? 1u : 0u) << 7);
}
inline u32 NodeFlags19(u32 seed, u32 k, u32 i) { return Rand(seed, 3000 + k * 100 + i) & 0xFF; }
inline i32 NodeLinkBase(u32 seed, u32 k, u32 i) { return i32(Rand(seed, 5000 + k * 100 + i) % 40) - 3; }
// entrada j (-3..50) de la tabla de enlaces del area k: area (poblada, sin poblar o con indice 72..79 de ranura nula) y nodo (0..7)
inline u32 LinkEntry(u32 seed, u32 k, i32 j) {
    const World w = MakeWorld(seed);
    const u32 v = Rand(seed, 6000 + k * 100 + u32(j + 3)), sel = v % 20;
    u32 area;
    if (sel < 14) area = w.id[(v >> 8) % w.na];
    else if (sel < 17) area = (v >> 8) % 72;
    else area = 72 + (v >> 8) % 8;
    return (area & 0xFFFFu) | ((((v >> 16) % 8) & 0xFFFFu) << 16);
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[92..95] = semillas de las arenas A, B, C y D
//   kPhoneFirst: a[4] = desplazamiento del peaton en C (>= 0x900), a[5] = dword +474h, a[6] = dword +478h, a[7] = arma guardada +70Ch, a[8] = ranura activa (byte +718h), a[9] = tipo de arma de la ranura (dword),
//                a[10] = datos de jugador (0 nulos, si no desplazamiento en D), a[11] = resultados de IsPlayer (bit 0 primera llamada, bit 1 segunda; el resto basura), a[12] = memoria de new (0 nula, 1 en B),
//                a[13] = desplazamiento en B, a[14] = dword que deja el falso del constructor de CTaskSimpleRunAnim, a[15] = byte +85h previo
//   kPhoneControl: a[4] = peaton (C), a[5] = clump (desplazamiento en D; byte +0 = 2 = rpCLUMP, que el DLL comprueba con un assert), a[6] = inteligencia (D), a[7] = datos de jugador (0 nulos, si no desplazamiento en D), a[8] = byte +84h, a[9] = byte +85h, a[10] = float +18h,
//                a[11] = subtarea (desplazamiento en C), a[12] = tipo de la subtarea, a[13] = asociacion 91h (0 no, si no desplazamiento en D), a[14] = asociacion 92h, a[15..16] = tiempo y paso de la 91h, a[17..18] = de la 92h,
//                a[19] = resultado de IsOutOfTime, a[20] = memoria de new (0 nula, 1 en B), a[21] = desplazamiento en B, a[22] = dword del falso del constructor, a[23] = FindTaskByType (0 nula, si no desplazamiento en D),
//                a[24] = modo de PlayerControlZelda (0 no cambia los datos de jugador, si no los cambia al desplazamiento a[25] en D), a[26] = bandera +1Dh previa... (sin uso), a[27] = tiempo del temporizador (sin uso)
//   kPhoneClone: a[4] = duracion (+0Ch), a[5] = memoria (0 nula, 1 en B), a[6] = desplazamiento en B, a[7] = dword que deja el falso de 61A3B0
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kSeqDtor: a[4] = indice (-1 o 0..63), a[5] = cuenta de referencias (+3Ch), a[6] = bandera (+38h), a[7] = dword que deja el falso de 61A3D0, a[8] = dword +0Ch de this (no se usa: el indice es a[4])
//   kSweep: a[4] = angulo, a[5..12] = desplazamientos de p1..p8 (p1..p4 en B, p5..p8 en C), a[13..36] = los 8 vectores (3 floats cada uno), a[37] = vx, a[38] = vy, a[39] = velocidad, a[40] = bandera (pila),
//           a[41] = mascara de resultados de Test2DLineAgainst2DLine (un bit por llamada), a[42] = basura de los bytes altos que devuelve el falso
//   kProbe: a[4] = avion (desplazamiento en C), a[5] = matriz (0 no, si no desplazamiento en D), a[6] = elevacion, a[7] = rumbo, a[8] = desplazamiento de la salida en B, a[9..11] = lo que deja el falso de Normalise,
//           a[12] = resultado de ProcessLineOfSight (dword; solo AL), a[13] = z del punto de choque, a[14] = entidad que deja, a[15..17] = posicion (matriz+30h o avion+4), a[18..20] = velocidad (+44h..+4Ch)
//   kClear: a[4] = avion (crudo), a[5] = rumbo, a[6] = aciertos de las sondas (bit i), a[7..12] = z de cada sonda, a[13] = basura de los bytes altos de AL
//   kPoolRef: a[4] = almacenamiento (crudo, [this]), a[5] = desplazamiento de las banderas en D (>= 0x300), a[6] = indice (con signo), a[7] = resto (0..0A17h, con el signo del indice), a[8] = byte de la tabla
//   kNop: a[4..20] = argumentos de pila (basura)
//   kNorm: a[4] = x, a[5] = y
//   kRecord: a[4] = semilla del mundo, a[5..7] = posicion, a[8] = tipo de nodo, a[9] = cuenta (con signo), a[10] = salida (desplazamiento en B), a[11] = distancia maxima, a[12..15] = banderas (dwords 0/1),
//            a[16..19] = nodos que devuelve el falso de FindNodeClosestToCoors (selector)
//   kCircle: a[4] = semilla del mundo, a[5..7] = centro (en B: a[8] = desplazamiento), a[9] = radio, a[10] = tipo de nodo, a[11] = maximo (con signo), a[12] = salida (B), a[13..14] = bajo trafico y argumento 7 (dwords con basura alta),
//            a[15] = bSkipMarked (argumento 8, dword), a[16] = bWater (argumento 9, dword), a[17] = sin uso
//   kPair: a[4] = semilla del mundo, a[5..7] = posicion, a[8] = tipo de nodo, a[9..11] = desplazamientos de outFirst, outSecond y outHeading en B, a[12] = longitud minima del enlace, a[13] = puntuacion maxima,
//          a[14] = bajo trafico (dword con basura alta), a[15] = sin uso, a[16] = bBoats (dword), a[17..19] = lo que deja el falso de Normalise, a[20..22] = relleno de las salidas (dwords previos)
//   kFind2: a[4..6] = posicion, a[7..8] = desplazamientos de las dos salidas en B, a[9] = bajo trafico (dword 0/1), a[10] = nodos que escribe el falso de Record (0..4), a[11..14] = selectores de nodos, a[15] = mascara de adyacencia,
//           a[16] = semilla del mundo de los selectores
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeedAt] = c.seed * 13u + 50u; c.a[kSeedAt + 1] = c.a[kSeedAt] + 1u; c.a[kSeedAt + 2] = c.a[kSeedAt] + 2u; c.a[kSeedAt + 3] = c.a[kSeedAt] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = 0x86E454u;
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86E518u, 0x86D4CCu, g.W(), g.W(), 0xDEADBEEFu, 0x86D48Cu});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kPhoneFirst:
        a[4] = 0x900 + 4 * g.R((0x2000 - 0x900) / 4);
        a[5] = g.Pk({0u, 0xFFFFFFFFu, 0x400000u, 0x2u, g.W(), g.W(), 0x00400000u, 0x80000000u});
        a[6] = g.Pk({0u, 0xFFFFFFFFu, 2u, 1u, g.W(), g.W(), 0x80000000u});
        a[7] = g.P(55) ? 0x37u : g.Pk({0u, 1u, 0x36u, 0x38u, 0xFFFFFFFFu, g.W(), g.R(60)});
        a[8] = g.P(70) ? g.R(13) : (g.P(50) ? u32(i32(g.R(256)) - 128) : g.Pk({0xFFu, 0x80u, 0x7Fu, 0u}));
        a[9] = g.Pk({0u, 1u, 22u, 0x37u, 0xFFFFFFFFu, g.W(), g.R(48)});
        a[10] = g.P(60) ? Aux(g, 0, u32(kVtbl), 0x90) : 0u;
        a[11] = (g.P(70) ? 1u : 0u) | (g.P(70) ? 2u : 0u) | (g.W() & 0xFFFFFF00u);
        a[12] = g.P(85) ? 1u : 0u;
        a[13] = AuxB(g, 0x20);
        a[14] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x86E454u});
        a[15] = g.R(256);
        break;
    case kPhoneControl: {
        a[4] = 0x900 + 4 * g.R((0x1800 - 0x900) / 4);
        a[5] = Aux(g, 0xE00, 0xEF0, 0x10);
        a[6] = Aux(g, 0, 0x100, 0x20);
        a[7] = g.P(70) ? Aux(g, 0x100, 0x400, 0x90) : 0u;
        a[8] = g.R(256); a[9] = g.R(256);
        a[10] = g.P(60) ? FloatQ(g) : g.Pk({0u, 0x80000000u, 0xBF800000u, 0x3F800000u, 0x80000001u, 0x00000001u});
        a[11] = Aux(g, 0x2400, 0x3C00, 0x40);
        a[12] = g.P(35) ? 0xCAu : g.Pk({0u, 0xC9u, 0xCBu, 0x642u, 0x643u, 0xFFFFFFFFu, g.W()});
        a[13] = g.P(55) ? Aux(g, 0x400, 0x600, 0x30) : 0u;
        a[14] = g.P(60) ? Aux(g, 0x600, 0x800, 0x30) : 0u;
        const auto timeNear = [&](u32 base) {   // tiempo de animacion cerca de un umbral
            const u32 k = g.R(8);
            if (k < 3) return base + g.R(5) - 2u;
            if (k == 3) return FloatQ(g);
            if (k == 4) return g.Pk({0u, 0x80000000u, 0x3F800000u, 0x3F000000u, 0x7F800000u, 0xFF800000u, 0x00000001u});
            return PosNormal(g, 125, 129) | (g.P(10) ? 0x80000000u : 0u);
        };
        const u32 b085 = 0x3F59999Au, b1566 = std::bit_cast<u32>(1.566f);
        a[15] = timeNear(b085); a[16] = g.P(50) ? 0x3A83126Fu + g.R(0x40000u) : FloatQ(g);
        a[17] = timeNear(b1566); a[18] = g.P(70) ? ((0x3C000000u + g.R(0x4000000u))) : FloatQ(g);
        a[19] = g.Pk({0u, 1u, 1u, 0x100u, 0xFF00u, 0xFFFFFF01u, g.W()});
        a[20] = g.P(85) ? 1u : 0u;
        a[21] = AuxB(g, 0x20);
        a[22] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x86E48Cu});
        a[23] = g.P(70) ? Aux(g, 0x800, 0xC00, 0x20) : 0u;
        a[24] = g.P(30) ? 1u : 0u;
        a[25] = Aux(g, 0xC00, 0xE00, 0x90);
        break;
    }
    case kPhoneClone:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), 1500u});
        a[5] = g.P(85) ? 1u : 0u;
        a[6] = AuxB(g, 0x20);
        a[7] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x86D4CCu});
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kSeqDtor:
        a[4] = g.P(25) ? 0xFFFFFFFFu : (g.P(40) ? g.R(4) : (g.P(30) ? 63u : g.R(64)));
        a[5] = g.Pk({1u, 1u, 1u, 0u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, g.W()});
        a[6] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.R(256)});
        a[7] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x86D48Cu});
        a[8] = g.W();
        break;
    case kSweep: {
        a[4] = Angle(g);
        for (u32 i = 0; i < 8; ++i) a[5 + i] = i < 4 ? (16u * g.R(200)) : (16u * g.R(900));
        const u32 mode = g.R(6);   // 0..3 coordenadas moderadas, 4 pocas distintas, 5 con especiales
        for (u32 i = 0; i < 24; ++i) {
            u32 v = Coord(g);
            if (mode == 4) v = g.Pk({0x3F800000u, 0x40000000u, 0xBF800000u, 0u, 0x40400000u, 0x3F000000u});
            if (mode == 5 && g.P(10)) v = FloatQ(g);
            a[13 + i] = v;
        }
        a[37] = Coord(g); a[38] = Coord(g); a[39] = Coord(g);
        a[40] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, g.W(), g.W() & ~0xFFu});
        const u32 m = g.R(6);   // todas falsas, una verdadera, varias
        a[41] = m == 0 ? 0u : (m <= 3 ? 1u << g.R(16) : (m == 4 ? g.W() & g.W() & g.W() & g.W() & 0xFFFFu : g.W() & 0xFFFFu));
        a[42] = g.W() & 0xFFFFFF00u;
        break;
    }
    case kProbe:
        a[4] = 0x100 + 4 * g.R((0x1000 - 0x100) / 4);
        a[5] = g.P(55) ? Aux(g, 0x100, 0x400, 0x40) : 0u;
        a[6] = Angle(g); a[7] = Angle(g);
        a[8] = AuxB(g, 4);
        a[9] = g.P(80) ? (g.P(50) ? 0x3F800000u : Coord(g)) : FloatQ(g); a[10] = g.P(80) ? Coord(g) : FloatQ(g); a[11] = g.P(70) ? 0u : Coord(g);
        a[12] = g.Pk({0u, 1u, 1u, 0x100u, 0xFFFFFF01u, 0xFFFFFF00u, 2u, 0x80u, g.W()});
        a[13] = Coord(g); a[14] = Raw(g);
        for (u32 i = 0; i < 3; ++i) { a[15 + i] = Coord(g); a[18 + i] = g.P(80) ? Coord(g) : FloatQ(g); }
        break;
    case kClear:
        a[4] = g.P(70) ? RawNz(g) : Raw(g); a[5] = Angle(g);
        a[6] = g.Pk({0u, 0u, 1u, 2u, 4u, 8u, 16u, 32u, 0x3Fu, 0x3Eu, g.R(64), g.R(64), g.R(64)});
        for (u32 i = 0; i < 6; ++i) a[7 + i] = g.P(88) ? Coord(g) : FloatBits(g);
        a[13] = g.W() & 0xFFFFFF00u;
        break;
    case kPoolRef: {
        a[4] = Raw(g);
        a[5] = 0x300 + 4 * g.R(0x80);
        const i32 idx = g.P(15) ? -i32(g.R(0x30)) : i32(g.R(0x400));
        a[6] = u32(idx);
        a[7] = g.Pk({0u, 1u, 0xA17u, 0xA16u, 0x500u, g.R(0xA18), g.R(0xA18)});
        a[8] = g.R(256);
        break;
    }
    case kNop:
        for (u32 i = 0; i < 17; ++i) a[4 + i] = g.W();
        break;
    case kNorm:
        a[4] = g.P(75) ? Coord(g) : FloatBits(g); a[5] = g.P(75) ? Coord(g) : FloatBits(g);
        if (g.P(10)) a[4] = g.Pk({0u, 0x80000000u});
        if (g.P(10)) a[5] = g.Pk({0u, 0x80000000u});
        if (g.P(8)) { a[4] = (0x40u + g.R(0x30)) << 23 | (g.W() & 0x7FFFFFu); a[5] = (0x40u + g.R(0x30)) << 23 | (g.W() & 0x7FFFFFu); }   // cuadrados diminutos
        break;
    case kRecord:
        a[4] = g.W() % 100000u;
        for (u32 i = 0; i < 3; ++i) a[5 + i] = Coord(g);
        a[8] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 2u, 3u, 0xFFu, 4u});
        a[9] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 4u, 0xFFFFFFFFu, 0x80000000u, 5u, 8u});
        a[10] = AuxB(g, 0x40);
        a[11] = g.Pk({0x497423FEu, 0x42C80000u, 0x7F800000u, 0u, FloatQ(g)});
        for (u32 i = 0; i < 4; ++i) a[12 + i] = g.P(35) ? 1u : 0u;
        for (u32 i = 0; i < 4; ++i) a[16 + i] = g.W();
        break;
    case kCircle:
        a[4] = g.W() % 100000u;
        for (u32 i = 0; i < 3; ++i) a[5 + i] = g.P(70) ? ((g.P(50) ? 0x80000000u : 0u) | PosNormal(g, 120, 134)) : FloatQ(g);
        a[8] = Aux(g, 0, 0x100, 0x10);
        a[9] = g.P(80) ? PosNormal(g, 120, 136) : FloatQ(g);
        a[10] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u});
        a[11] = g.Pk({0u, 1u, 2u, 3u, 5u, 100u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 4u, 8u});
        a[12] = Aux(g, 0x100, 0xF00, 0x80);
        a[13] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, 2u, 0x80u}) ; a[14] = g.W();
        a[15] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, g.W()});
        a[16] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, g.W()}); a[17] = g.W();
        break;
    case kPair:
        a[4] = g.W() % 100000u;
        for (u32 i = 0; i < 3; ++i) a[5 + i] = g.P(80) ? ((g.P(50) ? 0x80000000u : 0u) | PosNormal(g, 120, 134)) : FloatQ(g);
        a[8] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u});
        a[9] = 0x100u + 0x20u * g.R(8); a[10] = a[9] + 0x10u; a[11] = a[9] + 0x20u;
        a[12] = g.P(70) ? ((g.P(20) ? 0u : PosNormal(g, 118, 132))) : FloatQ(g);
        a[13] = g.P(50) ? PosNormal(g, 120, 140) : g.Pk({0x461C4000u, 0x461C3FFFu, 0u, 0xFF800000u, 0x7FC00000u, 0xBF800000u, 0x80000000u, 0x43480000u, 0x44FA0000u});
        a[14] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, 2u, 0x80u}); a[15] = g.W();
        a[16] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, g.W()});
        for (u32 i = 0; i < 3; ++i) a[17 + i] = g.P(70) ? (g.P(50) ? 0x80000000u : 0u) | PosNormal(g, 120, 127) : FloatQ(g);
        for (u32 i = 0; i < 3; ++i) a[20 + i] = g.W();
        break;
    case kFind2:
        for (u32 i = 0; i < 3; ++i) a[4 + i] = Coord(g);
        a[7] = 0x100u + 0x20u * g.R(8); a[8] = a[7] + 0x10u;
        a[9] = g.P(50) ? 1u : 0u;
        a[10] = g.Pk({0u, 1u, 2u, 3u, 4u, 4u});
        for (u32 i = 0; i < 4; ++i) a[11 + i] = g.W();
        a[15] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0xFFFFFFFFu, g.W() & 7u, g.W() & 7u});
        a[16] = g.W() % 100000u;
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
