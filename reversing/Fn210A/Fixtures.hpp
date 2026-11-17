#pragma once
// Fixtures del banco Fn210A (lote 210A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 210A SIN validar: ver reversing/PendienteValidar/210A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores).
//   idx  direccion  nombre real                                                                         forma
//    0   0x670A60   CTaskComplexFollowPatrolRoute::GetTaskType                                          thiscall() `ret`: mov eax,3A3h
//    1   0x670A70   CTaskComplexFollowPatrolRoute::GetNextSubTaskType                                    thiscall() `ret`: contador de la ruta, nodo (+12h), modo (+0Ch), pasadas (+0Eh); Reverse 66D550
//    2   0x670B00   CTaskComplexFollowPatrolRoute::CreateSubTask                                        thiscall(tipo) `ret 4`: 384 GoToPoint 667CD0, 191 RunNamedAnim 61A990, C8 CTaskSimple 61A390 + vtable 859CFC, 387 668120, 38A 66EA30
//    3   0x670D80   CTaskComplexFollowPatrolRoute::CreateNextSubTask                                      thiscall(ped) `ret 4`: tipo de la subtarea (+8, virtual 10h) hasta tres veces; CreateSubTask 670B00 y GetNextSubTaskType 670A70
//    4   0x670E20   CTaskComplexFollowPatrolRoute::CreateFirstSubTask                                     thiscall(ped) `ret 4`: busca el segmento mas cercano (x87 24 bits; Normalise 59C910) y los puntos; CreateSubTask / GetNextSubTaskType
//    5   0x672BB0   CTaskComplexFollowPatrolRoute destructor con borrado (vtable 0x870114)               thiscall(flags) `ret 4`: cuerpo 6709F0; delete 61A5B0 si bit 0 del BYTE; devuelve this
//    6   0x670830   CTaskComplexTurnToFaceEntityOrCoord destructor con borrado (vtable 0x86FF94)        idem con cuerpo 66B960
//    7   0x6711F0   CTaskComplexGotoDoorAndOpen destructor con borrado (vtable 0x86FFC0)                 idem con cuerpo 66BC00
//    8   0x6712C0   CTaskSimpleSlideToCoord destructor con borrado (vtable 0x86FFEC)                     idem con cuerpo 66C4C0
//    9   0x6712E0   CTaskComplexInvestigateDisturbance destructor con borrado (vtable 0x870010)         idem con cuerpo 66C7A0
//   10   0x671330   CTaskComplexSeekEntity<FixedPos>::Clone                                              thiscall() `ret`: new(58h) 61A5A0; constructor 66CA40 (8 argumentos); copia +44h..+4Ch, +50h, bit 3 de +54h y +20h (fld/fstp)
//   11   0x673070   CTaskComplexSeekEntity<FixedPos>::CreateSubTask                                      thiscall(tipo, ped) `ret 8`: DB Tired 630F20, CB StandStill 62F310, F4 Sequence 632BD0/632D10, 2C0 LeaveCar 63B8C0, 2CF CarDriveTimed 5FF940, 38A 66EA30, 387 668120, 398 66B890
//   12   0x673540   CTaskComplexSeekEntity<FixedPos>::ControlSubTask                                     thiscall(ped) `ret 4`: barco comun (AreInSameGroup 5F7F40, Attach 5E7CB0...), temporizadores, SetTarget 671750, GoToPoint 46FE60, Say 5EFFE0 (x87 24 bits)
//   13   0x673BA0   CTaskComplexSeekEntity<FixedPos>::GetSeekPos                                          thiscall(salida, ped) `ret 8`: llamada virtual (ranura 0) del calculador +40h; copia el temporal
//   14   0x66B780   CTaskComplexGoToPointAnyMeans::GetTaskType (V)                                       thiscall() `ret`: mov eax,396h
//   15   0x6A2F80   CAutomobile::DebugCode                                                               thiscall() `ret`: handling del modelo si es el vehiculo del jugador; arrastre / 1000 * 0.5 (x87)
//   16   0x6A3030   CAutomobile::RemoveRefsToVehicle                                                     thiscall(entidad) `ret 4`: pone a cero las ruedas de colision (+8E8h..+8F4h) iguales a la entidad
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40; C mide 0x4000) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F210AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 17;
constexpr unsigned kArenas = 4;
constexpr unsigned kPayload = 128;      // a[0..127]; las semillas de las arenas van en a[124..127]
constexpr unsigned kSeedAt = 124;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;     // ventana de las arenas A, B y D
constexpr std::size_t kWinC = 0x4000;    // ventana de la arena C (pool de peatones: 0x7C4 bytes por peaton)
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

// Desplazamiento de un objeto de `span` bytes en la ventana de la arena A segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
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
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }   // distinto de cero y nunca una direccion de arena alineada del oraculo
inline u32 ZeroOr(Gen& g, u32 pctZero) { return g.P(pctZero) ? 0u : RawNz(g); }
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    const u32 huge = sign | (0x7E800000u + (g.W() & 0x007FFFFFu));
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, huge, g.W() & 0xFF7FFFFFu});
}
// Como FloatBits pero sin NaN senalizantes (un NaN senalizante cargado con fld se silencia y uno en memoria no): para los valores que el EXE carga en la x87 o recibe por ST0
inline u32 FloatQ(Gen& g) {
    u32 v = FloatBits(g);
    if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu) != 0) v |= 0x00400000u;
    return v;
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float finito normal positivo con exponente sesgado en [lo, hi)
inline u32 PosNormal(Gen& g, u32 lo, u32 hi) { return ((lo + g.R(hi - lo)) << 23) | (g.W() & 0x7FFFFFu); }
// Una posicion (bits): normalmente de magnitud moderada (para que las distancias sean comparables), a veces cualquiera
inline u32 Coord(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    if (g.P(80)) return sign | PosNormal(g, 120, 136);
    return FloatQ(g);
}
// Un byte con basura alrededor del valor: se escribe solo el byte
inline u32 ByteOf(Gen& g, std::initializer_list<u32> l) { return g.P(75) ? *(l.begin() + g.R(u32(l.size()))) : g.R(256); }


// Clases de funcion (forma)
enum Kind : u32 { kConst, kSdd, kFpNext, kFpCreate, kFpNextSub, kFpFirst, kClone, kSeekCreate, kSeekControl, kSeekPos, kDebug, kRemove };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kFpNext, kFpCreate, kFpNextSub, kFpFirst, kSdd, kSdd, kSdd, kSdd, kSdd, kClone, kSeekCreate, kSeekControl, kSeekPos, kConst, kDebug, kRemove};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x670A60, 0x670A70, 0x670B00, 0x670D80, 0x670E20, 0x672BB0, 0x670830, 0x6711F0, 0x6712C0, 0x6712E0, 0x671330, 0x673070, 0x673540, 0x673BA0, 0x66B780, 0x6A2F80, 0x6A3030};
// Cuerpo (colaborador) que llaman los destructores con borrado (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0x6709F0, 0x66B960, 0x66BC00, 0x66C4C0, 0x66C7A0};
inline constexpr std::array<u32, kFunctions> kConstVal{0x3A3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x396};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x870114, 0x870114, 0x870114, 0x870114, 0x870114, 0x870114, 0x86FF94, 0x86FFC0, 0x86FFEC, 0x870010, 0x87004C, 0x87004C, 0x87004C, 0x87004C, 0x86FF68, 0x871120, 0x871120};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x24, 0x24, 0x24, 0x24, 4, 4, 4, 4, 4, 0x58, 0x58, 0x58, 0x44, 4, 0xA8, 0x8F8};

// Memoria global del juego tocada
inline constexpr u32 kModelTable = 0xA9B0C8;                  // CModelInfo::ms_modelInfoPtrs (indice con signo de 16 bits)
inline constexpr int kModelLo = -3, kModelHi = 7;             // indices cubiertos por la region
inline constexpr u32 kModelSlots = kModelHi - kModelLo + 1;   // 11
inline constexpr u32 kPlayerInFocus = 0xB7CD74;               // CWorld::PlayerInFocus (byte)
inline constexpr u32 kPlayers = 0xB7CD98;                     // CWorld::Players (0x190 bytes cada uno; +0xB0 = vehiculo remoto)
inline constexpr u32 kPlayerStride = 0x190, kRemoteAt = 0xB0, kPlayerCount = 4;
inline constexpr u32 kHandling = 0xC2B9DC;                    // gHandlingDataMgr: entradas de 0xE0 bytes
inline constexpr u32 kHandlingStride = 0xE0;
inline constexpr std::array<u32, 8> kHandlingIds{0, 1, 2, 5, 0x40, 0x80, 0xFE, 0xFF};
inline constexpr u32 kTimeMs = 0xB7CB84;                      // CTimer::m_snTimeInMilliseconds
inline constexpr u32 kFrame = 0xB7CB4C;                       // CTimer::m_FrameCounter

// ------------------------------------------------------------------------------------------------ generadores auxiliares
inline u32 Hw(Gen& g, int lo, int hi) { return u32(i32(lo) + i32(g.R(u32(hi - lo + 1)))) & 0xFFFFu; }            // un int16 entre lo y hi (como bits bajos)
inline u32 NodeCount(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 1u, 2u, 3u, 4u, 8u, 8u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 9u, 12u, g.R(13), g.W()}); }
inline u32 NodeIdx(Gen& g, u32 n, bool narrow) {
    const u32 r = g.R(100);
    if (r < 22) return (n - 1) & 0xFFFFu;
    if (r < 44) return n & 0xFFFFu;
    if (r < 54) return (n + 1) & 0xFFFFu;
    if (r < 62) return (n + 5) & 0xFFFFu;
    if (r < 68) return (n - 2) & 0xFFFFu;
    if (r < 76) return 0u;
    if (r < 80) return 0xFFFFu;
    if (r < 90) return g.R(10);
    return narrow ? g.R(10) : (g.W() & 0xFFFFu);
}
inline u32 PatrolMode(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 4u, 0xFFFFu, 0x8000u, 0x7FFFu, g.W() & 0xFFFFu}); }
inline u32 Passes(Gen& g) { return g.Pk({0u, 0u, 1u, 1u, 2u, 0x7FFFu, 0xFFFFu, 0xFFFEu, 0x8000u, g.W() & 0xFFFFu}); }
inline u32 MoveStateW(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 6u, 6u, 0xFFFFu, 0x8000u, g.W() & 0xFFFFu}); }
// Una coordenada de una ruta: casi siempre moderada; a veces cualquiera
inline u32 Pt(Gen& g) { return g.P(75) ? Coord(g) : FloatQ(g); }
// Una coordenada "pequena" (denormales y exponentes muy negativos) para los productos que desbordan por debajo
inline u32 Tiny(Gen& g) { const u32 sign = g.P(50) ? 0x80000000u : 0u; return sign | (g.R(100) < 30 ? (g.W() & 0x7FFFFFu) : (PosNormal(g, 1, 60))); }
// Una coordenada finita y moderada (sin denormales ni NaN: la aritmetica del enganche al barco en SSE no coincide con la x87 del EXE en esos casos)
inline u32 Fin(Gen& g) { return (g.P(50) ? 0x80000000u : 0u) | PosNormal(g, 120, 136); }
inline float F32(u32 b) { return std::bit_cast<float>(b); }
inline bool FiniteBits(u32 b) { return (b & 0x7F800000u) != 0x7F800000u; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[124..127] = semillas de las arenas A, B, C y D
//   kConst: a[3] crudo
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kFpNext (GetNextSubTaskType): a[4] = desplazamiento de la ruta en B, a[5] = contador de nodos (+0 de la ruta), a[6] = nodo (+0x12), a[7] = modo (+0xC), a[8] = pasadas (+0xE), a[9] = word +0x10, a[10] = banderas (+0x20)
//   kFpCreate (CreateSubTask): a[4] = tipo (pila), a[5] = ruta en B, a[6] = contador, a[7] = modo, a[8] = pasadas, a[9] = estado de movimiento (+0x10), a[10] = nodo, a[11] = radio (+0x14), a[12] = radio de estado (+0x18), a[13] = banderas,
//             a[14] = hay memoria, a[15] = desplazamiento de la memoria en D, a[16] = dword que dejan los constructores
//   kFpNextSub (CreateNextSubTask): a[4] = subtarea en C, a[5] = tipo que devuelve su GetTaskType, a[6] = ped (pila, crudo), a[7] = EAX del falso de CreateSubTask, a[8] = EAX del falso de GetNextSubTaskType, a[9] = ruta en B, a[10] = nodo, a[11] = primer byte del nombre del nodo
//   kFpFirst (CreateFirstSubTask): a[4] = ped (desplazamiento en C), a[5] = modo de matriz, a[6] = matriz en D, a[7..9] = posicion del ped, a[10..12] = posicion de la matriz, a[13] = contador, a[14] = ruta en B, a[15..50] = 12 puntos,
//             a[51..86] = salidas del falso de Normalise (una por llamada), a[87] = banderas, a[88] = nodo, a[89] = modo, a[90] = pasadas, a[91] = estado de movimiento, a[92] = EAX de CreateSubTask, a[93] = EAX de GetNextSubTaskType
//   kClone: a[4..10] = +0xC, +0x10, +0x14, +0x18, +0x1C, +0x20, +0x24; a[11..13] = +0x44..+0x4C; a[14] = +0x50; a[15] = byte +0x54; a[16] = memoria en D; a[17] = dword que deja el constructor
//   kSeekCreate: a[4] = tipo (pila), a[5] = ped en C, a[6] = entidad (+0xC, cruda), a[7] = +0x18, a[8] = +0x1C, a[9] = +0x24, a[10] = +0x50, a[11] = byte +0x54, a[12] = vehiculo del ped (+0x58C), a[13..15] = lo que escribe el calculador,
//             a[16 + 2i] = hay memoria i, a[17 + 2i] = desplazamiento en D (i = 0..2), a[22] = dword que dejan los constructores
//   kSeekControl: ver MakeControl (MakeAll lo llama)
//   kSeekPos: a[4] = +0xC (entidad, cruda), a[5] = ped (pila, crudo), a[6..8] = lo que escribe el calculador falso, a[9] = desplazamiento de la salida en B
//   kDebug: a[4] = indice de modelo (+0x22), a[5] = seleccion del id de handling (0..7), a[6] = jugador en foco, a[7..10] = vehiculo remoto de cada jugador (0 nulo, 1 this, 2 otro), a[11] = vehiculo de FindPlayerVehicle (0, 1, 2),
//             a[12..43] = masa, masa de giro, arrastre y flotabilidad de cada una de las 8 entradas, a[44] = el "otro" puntero, a[46..56] = id de handling de cada modelo (indice en kHandlingIds)
//   kRemove: a[4] = entidad (cruda), a[5..8] = las cuatro ruedas (0 = 0, 1 = la entidad, 2 = otro valor), a[9] = el otro valor
inline void MakeControl(C& c, Gen& g);
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeedAt] = c.seed * 13u + 50u; c.a[kSeedAt + 1] = c.a[kSeedAt] + 1u; c.a[kSeedAt + 2] = c.a[kSeedAt] + 2u; c.a[kSeedAt + 3] = c.a[kSeedAt] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
        a[3] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kFpNext: {
        a[4] = Aux(g, 0x100, 0x800, 0x1A4);
        a[5] = NodeCount(g);
        a[6] = NodeIdx(g, a[5], false);
        a[7] = PatrolMode(g); a[8] = Passes(g);
        a[9] = g.W() & 0xFFFFu; a[10] = g.W();
        if (g.P(14)) { a[6] = a[5] & 0xFFFFu; a[7] = g.Pk({1u, 1u, 2u, 3u}); a[8] = g.P(65) ? 0u : Passes(g); }   // se pasa del ultimo nodo con pocas pasadas (primera vuelta del modo 1)
        if (g.P(6)) a[5] = 0u;
        break;
    }
    case kFpCreate: {
        a[4] = g.Pk({0x384u, 0x384u, 0x191u, 0x191u, 0xC8u, 0xC8u, 0x387u, 0x387u, 0x38Au, 0x38Au, 0x38Au, 0x516u, 0u, 1u, 0x383u, 0x385u, 0x386u, 0x388u, 0x389u, 0x38Bu, 0x190u, 0x192u, 0xC7u, 0xC9u,
                     0xFFFFFFFFu, 0x80000384u, 0x10000384u, 0x80000387u, 0x7FFFFFFFu, g.W()});
        a[5] = Aux(g, 0x180, 0x800, 0x1A4);
        a[6] = NodeCount(g);
        a[7] = PatrolMode(g); a[8] = Passes(g); a[9] = MoveStateW(g);
        a[10] = NodeIdx(g, a[6], a[4] == 0x38Au);
        if (a[4] == 0x387u && g.P(35)) { a[7] = 1u; a[8] = 0u; }   // modo 1 con cero pasadas: el radio de estado pasa a 0.0
        if (a[4] == 0x38Au) { const i32 kk = i32(i16(a[10])); if (kk < -8 || kk > 20) a[10] = g.R(10); }   // la ruta se lee de verdad: solo nodos dentro de la arena
        a[11] = FloatBits(g); a[12] = FloatBits(g); a[13] = g.W();
        a[14] = g.P(85) ? 1u : 0u;
        a[15] = AuxD(g, 0x70);
        a[16] = Raw(g);
        break;
    }
    case kFpNextSub: {
        a[4] = AuxC(g, 0x40);
        a[5] = g.Pk({0x38Au, 0x38Au, 0xC8u, 0xC8u, 0x191u, 0x191u, 0x384u, 0x387u, 0x516u, 0u, 0x190u, 0x192u, 0xFFFFFFFFu, 0x1000038Au, 0x100000C8u, g.W()});
        a[6] = Raw(g); a[7] = Raw(g); a[8] = Raw(g);
        a[9] = Aux(g, 0x180, 0x700, 0x1A4);
        a[10] = g.P(80) ? Hw(g, -4, 12) : (g.W() & 0xFFFFu);
        if (a[10] > 12 && a[10] < 0xFFFCu) a[10] = Hw(g, -4, 12);   // el nombre del nodo se lee de verdad: solo nodos dentro de la arena
        a[11] = ByteOf(g, {0u, 0u, 0u, 0u, 1u, 0x41u, 0xFFu, 0x80u});
        break;
    }
    case kFpFirst: {
        a[4] = AuxC(g, 0x20);
        a[5] = g.Pk({0u, 1u, 1u});
        a[6] = AuxD(g, 0x40);
        const u32 nr = g.R(100);
        if (nr < 4) a[13] = 0u;
        else if (nr < 34) a[13] = 1u + g.R(3);
        else if (nr < 54) a[13] = 4u + g.R(2);
        else if (nr < 79) a[13] = 6u + g.R(3);
        else if (nr < 94) a[13] = 9u + g.R(4);
        else a[13] = g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0xFFFFFF00u});
        a[14] = Aux(g, 0x40, 0xE00, 0x1E0);
        const u32 pr = g.R(100);
        const u32 pts = pr < 8 ? 2u : (pr < 18 ? 1u : 0u);   // 0: puntos sueltos, 1: todos iguales, 2: pequenos
        for (u32 p = 0; p < 12; ++p) for (u32 k = 0; k < 3; ++k) a[15 + 3 * p + k] = pts == 2 ? Tiny(g) : Pt(g);
        if (pts == 1) {   // todos los puntos iguales: segmentos de longitud cero
            const u32 x0 = Coord(g), y0 = Coord(g), z0 = Coord(g);
            for (u32 p = 0; p < 12; ++p) { a[15 + 3 * p] = x0; a[16 + 3 * p] = y0; a[17 + 3 * p] = z0; }
        }
        // posicion del ped: sobre un punto, cerca de un segmento o suelta
        const u32 pm = g.R(100);
        for (u32 k = 0; k < 3; ++k) a[7 + k] = pts == 2 ? Tiny(g) : Pt(g);
        for (u32 k = 0; k < 3; ++k) a[10 + k] = pts == 2 ? Tiny(g) : Pt(g);
        const u32 nn = std::min<u32>(a[13] < 13 ? a[13] : 4u, 12u);
        if (nn > 0 && pm < 20) {   // el peaton coincide con un punto (por la posicion simple y por la matriz)
            const u32 p = g.R(nn);
            for (u32 k = 0; k < 3; ++k) { a[7 + k] = a[15 + 3 * p + k]; a[10 + k] = a[15 + 3 * p + k]; }
        } else if (nn > 1 && pm < 55) {   // cerca de un segmento
            const u32 p = g.R(nn), q = (p + 1) % nn;
            const float tt = float(g.R(1000)) / 1000.0f;
            for (u32 k = 0; k < 3; ++k) {
                const float x = F32(a[15 + 3 * p + k]), y = F32(a[15 + 3 * q + k]);
                const float v = x + (y - x) * tt + float(i32(g.R(9)) - 4) * 0.25f;
                const u32 vb = std::bit_cast<u32>(v);
                if (FiniteBits(vb)) { a[7 + k] = vb; a[10 + k] = vb; }
            }
        }
        // salidas del falso de Normalise: la direccion unitaria del segmento (a veces), o cualquier cosa
        const bool consistent = g.P(55);
        for (u32 i = 0; i < 12; ++i) {
            const u32 nz = nn ? nn : 1u;
            const u32 ib = i % nz, jb = (i + 1) % nz;
            bool ok = consistent;
            double d[3]{};
            for (u32 k = 0; k < 3; ++k) {
                const u32 pa = a[15 + 3 * jb + k], pb = a[15 + 3 * ib + k];
                if (!FiniteBits(pa) || !FiniteBits(pb)) ok = false;
                d[k] = double(F32(pa)) - double(F32(pb));
            }
            const double len = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
            if (ok && len > 1e-30 && std::isfinite(len)) {
                for (u32 k = 0; k < 3; ++k) a[51 + 3 * i + k] = std::bit_cast<u32>(float(d[k] / len));
            } else {
                for (u32 k = 0; k < 3; ++k) a[51 + 3 * i + k] = g.P(70) ? std::bit_cast<u32>(float(double(i32(g.R(2001)) - 1000) / 1000.0)) : FloatQ(g);
            }
        }
        a[87] = g.W(); a[88] = g.W(); a[89] = PatrolMode(g); a[90] = Passes(g); a[91] = MoveStateW(g);
        a[92] = Raw(g); a[93] = Raw(g);
        break;
    }
    case kClone:
        a[4] = Raw(g); a[5] = g.W(); a[6] = g.W();
        a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = FloatQ(g); a[10] = FloatBits(g);
        a[11] = g.W(); a[12] = g.W(); a[13] = g.W(); a[14] = g.W();
        a[15] = g.P(70) ? ByteOf(g, {0u, 1u, 2u, 3u, 8u, 9u, 0xAu, 0xBu, 0x0Fu, 0xF0u, 0xFFu}) : g.R(256);
        a[16] = AuxD(g, 0x60);
        a[17] = Raw(g);
        break;
    case kSeekCreate: {
        a[4] = g.Pk({0x398u, 0x398u, 0x387u, 0x387u, 0x38Au, 0x38Au, 0xF4u, 0xF4u, 0x2C0u, 0x2C0u, 0x2CFu, 0x2CFu, 0xCBu, 0xCBu, 0xDBu, 0xDBu, 0x516u, 0u, 1u,
                     0x397u, 0x399u, 0x386u, 0x388u, 0x389u, 0x38Bu, 0x515u, 0x517u, 0xDAu, 0xDCu, 0xCAu, 0xCCu, 0xF3u, 0xF5u, 0x2BFu, 0x2C1u, 0x2CEu, 0x2D0u, 0xFFFFFFFFu, 0x80000387u, 0x100000F4u, g.W()});
        a[5] = Aux(g, 0, u32(kWinC), 0x600);
        a[6] = Raw(g);
        a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = FloatBits(g);
        a[10] = g.Pk({4u, 5u, 6u, 7u, 6u, 6u, 0u, 0xFFFFFFFFu, g.W()});
        a[11] = g.P(70) ? ByteOf(g, {0u, 2u, 8u, 0xAu, 1u, 3u, 0xFFu, 0xF7u}) : g.R(256);
        a[12] = Raw(g);
        for (u32 k = 0; k < 3; ++k) a[13 + k] = g.P(50) ? g.W() : FloatBits(g);
        for (u32 i = 0; i < 3; ++i) { a[16 + 2 * i] = g.P(85) ? 1u : 0u; a[17 + 2 * i] = 0x200u * i + 4u * g.R(0x30); }
        if (a[4] == 0x387u || a[4] == 0xF4u) a[16] = 1u;   // sin memoria el EXE sigue con EAX = 0 y escribe en [eax + 24h] (o llama a AddTask con nulo): fuera del banco
        a[22] = Raw(g);
        break;
    }
    case kSeekControl:
        MakeControl(c, g);
        break;
    case kSeekPos:
        a[4] = Raw(g); a[5] = Raw(g);
        for (u32 k = 0; k < 3; ++k) a[6 + k] = g.P(50) ? g.W() : FloatBits(g);
        a[9] = AuxB(g, 0xC);
        break;
    case kDebug:
        a[4] = Hw(g, kModelLo, kModelHi);
        a[5] = g.R(8);
        a[6] = g.R(kPlayerCount);
        for (u32 k = 0; k < 4; ++k) a[7 + k] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[11] = g.Pk({0u, 1u, 1u, 1u, 2u});
        for (u32 e = 0; e < 8; ++e) {
            a[12 + 4 * e] = FloatBits(g); a[13 + 4 * e] = FloatBits(g);
            a[14 + 4 * e] = g.P(35) ? g.Pk({0u, 0x3C23D70Au, 0x3C23D709u, 0x3C23D70Bu, 0x80000000u, 0x7F800000u, 0x7FC00000u, 0x7F800001u, 0x3F800000u, 0x447A0000u}) : FloatBits(g);
            a[15 + 4 * e] = FloatBits(g);
        }
        a[44] = Raw(g) | 1u;
        for (u32 k = 0; k < kModelSlots; ++k) a[46 + k] = g.R(8);
        break;
    case kRemove: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 0x1000u, g.W() | 1u});
        a[9] = g.P(50) ? 0u : (g.W() | 1u);
        for (u32 k = 0; k < 4; ++k) a[5 + k] = g.R(3);
        if (g.P(40)) for (u32 k = 0; k < 4; ++k) if (g.P(50)) a[5 + k] = 1u;
        break;
    }
    }
}

// ------------------------------------------------------------------------------------------------ ControlSubTask de CTaskComplexSeekEntity<FixedPos> (0x673540)
//   a[4] = entidad nula (1), a[5] = entidad en C, a[6] = byte +36h de la entidad, a[7] = banderas +40h de la entidad, a[8] y a[9] = lo que apuntan +568h y +584h de la entidad (0 nulo, 1 = V1, 2 = V2, 3 = la entidad),
//   a[10] y a[11] = V1 y V2 en C, a[12] y a[14] = byte +36h de V1 y V2, a[13] y a[15] = tipo de vehiculo (+590h) de V1 y V2,
//   a[16] = ped en C, a[17] y a[18] = lo que apuntan +568h y +584h del ped (0 nulo, 1 = V1, 2 = V2, 3 = crudo), a[19] = +0FCh del ped, a[20] = modo de matriz del ped, a[21] = matriz del ped en D, a[22..24] = posicion del ped,
//   a[25..27] = posicion de su matriz, a[28] = +534h del ped, a[29] = word +20h, a[30] = ranura de arma (+718h), a[31] = tipo de arma de esa ranura,
//   a[32..34] = posicion de V1, a[35] = modo de matriz de V1, a[36] = matriz de V1 en D, a[37..39] = posicion de su matriz; a[40..47] = lo mismo de V2,
//   a[48] = reloj (ms), a[49] = contador de fotogramas, a[50] = inicio del temporizador de escaneo (+28h), a[51] = su intervalo (+2Ch), a[52] = su byte de iniciado (+30h), a[53] = su byte de forzado (+31h),
//   a[54] = inicio del temporizador de busqueda (+34h), a[55] = intervalo (+38h), a[56] = iniciado (+3Ch), a[57] = forzado (+3Dh),
//   a[58] = +14h, a[59] = +18h, a[60] = +1Ch, a[61] = +20h, a[62] = +24h, a[63] = +50h, a[64] = byte +54h,
//   a[65] = subtarea en D, a[66] = su tipo (+30h), a[67] = su byte +24h, a[68] = mascara de resultados de MakeAbortable (un bit por llamada), a[69..71] = lo que escribe el calculador, a[72] = EAX de IsPlayer,
//   a[73] = EAX de AreInSameGroup, a[74] = EAX de IsMember, a[75] = hay memoria, a[76] = memoria en D, a[77] = dword que dejan los constructores, a[78] = jugador en C, a[79] = sus datos en D, a[80] = su grupo (+38h),
//   a[81..83] = salida del falso de Difference, a[84..86] = de Normalise, a[87] = EAX de FindZCoorForPed, a[88] = la Z que escribe, a[89..91] = salida de FromMultiply3x3, a[92] = EAX del falso de CreateSubTask,
//   a[93] = resultado de GetRadianAngleBetweenPoints, a[94] = de LimitRadianAngle
inline void MakeControl(C& c, Gen& g) {
    auto& a = c.a;
    const u32 base = g.R(0x30) * 4u;
    a[5] = 0x000u + base; a[10] = 0x800u + g.R(0x30) * 4u; a[11] = 0x1000u + g.R(0x30) * 4u; a[16] = 0x1800u + g.R(0x30) * 4u; a[78] = 0x2000u + g.R(0x30) * 4u;
    a[21] = 0x200u + g.R(0x30) * 4u; a[36] = 0x400u + g.R(0x30) * 4u; a[44] = 0x600u + g.R(0x30) * 4u; a[65] = g.R(0x30) * 4u; a[76] = 0x800u + g.R(0x30) * 4u; a[79] = 0xA00u + g.R(0x30) * 4u;
    const bool boatScenario = g.P(38);
    a[4] = g.P(12) ? 1u : 0u;
    a[6] = g.P(78) ? 3u | (g.P(20) ? (g.R(32) << 3) : 0u) : ByteOf(g, {2u, 4u, 0u, 0xFFu, 3u});
    a[7] = g.P(75) ? 0u : g.Pk({0x100u, 0x100u, g.W(), g.W() & ~0x100u, g.W() | 0x100u, 0xFFFFFFFFu});
    a[8] = g.R(4); a[9] = g.R(4);
    a[12] = g.P(80) ? 2u | (g.P(20) ? (g.R(32) << 3) : 0u) : ByteOf(g, {3u, 4u, 0u, 0xFFu});
    a[14] = g.P(80) ? 2u | (g.P(20) ? (g.R(32) << 3) : 0u) : ByteOf(g, {3u, 4u, 0u, 0xFFu});
    a[13] = g.Pk({5u, 5u, 5u, 5u, 4u, 0u, 6u, 0xFFFFFFFFu, g.W()});
    a[15] = g.Pk({5u, 5u, 5u, 5u, 4u, 0u, 6u, 0xFFFFFFFFu, g.W()});
    a[17] = g.R(4); a[18] = g.R(4);
    a[19] = g.P(70) ? 0u : (g.W() | 1u);
    a[20] = g.Pk({0u, 1u, 1u});
    for (u32 k = 0; k < 3; ++k) { a[22 + k] = Fin(g); a[25 + k] = Fin(g); a[32 + k] = Fin(g); a[37 + k] = Fin(g); a[40 + k] = Fin(g); a[45 + k] = Fin(g); }
    a[28] = g.Pk({4u, 5u, 6u, 7u, 5u, 6u, 7u, 8u, 0u, 0xFFFFFFFFu, g.W()});
    a[29] = g.Pk({0u, 1u, 0xFFFFu, 0x3FFFu, 0x4000u, g.W() & 0xFFFFu, g.W() & 0xFFFFu});
    a[30] = ByteOf(g, {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u}) % 13u;
    a[31] = g.Pk({0u, 0x22u, 0x1Fu, 0x26u, g.W(), g.W()});
    a[35] = g.Pk({0u, 1u, 1u}); a[43] = g.Pk({0u, 1u, 1u});
    // tiempo: el reloj, los temporizadores (en tiempo o no) y el contador de fotogramas
    a[48] = g.Pk({g.W(), g.W(), 0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 100000u, 0x80000000u, 5000u + g.R(100000)});
    const auto timer = [&](u32 startAt, u32 intervalAt) {
        const u32 now = a[48];
        if (g.P(8)) { a[startAt] = g.W(); a[intervalAt] = g.W(); return; }
        const u32 diff = g.R(2000);
        a[startAt] = now - diff;
        a[intervalAt] = g.P(50) ? g.R(diff + 1) : diff + 1 + g.R(2000);
    };
    timer(50, 51); timer(54, 55);
    a[52] = ByteOf(g, {1u, 1u, 1u, 1u, 0u, 2u, 0xFFu}); a[53] = ByteOf(g, {0u, 0u, 0u, 1u, 2u, 0xFFu});
    a[56] = ByteOf(g, {1u, 1u, 0u, 1u, 2u, 0xFFu}); a[57] = ByteOf(g, {0u, 0u, 0u, 1u, 2u, 0xFFu});
    a[49] = g.P(30) ? (((0u - (a[29] & 0xFFFFu)) & 0x3FFFu) + 0x4000u * g.R(4)) : g.Pk({g.W(), 0u, 1u, 0x3FFFu, 0xFFFFFFFFu});
    a[58] = g.W();
    a[59] = g.P(30) ? PosNormal(g, 120, 136) : FloatBits(g);
    a[60] = g.P(30) ? PosNormal(g, 120, 136) : FloatBits(g);
    a[61] = g.P(35) ? 0u : (g.P(15) ? g.Pk({0x80000000u, 0x7FC00000u, 0x3F800000u, 0x3F7FFFFFu, 0x40000000u}) : (g.P(75) ? PosNormal(g, 120, 136) : (g.P(50) ? 0x80000000u | PosNormal(g, 120, 136) : PosNormal(g, 100, 140))));
    a[62] = FloatBits(g);
    a[63] = g.Pk({4u, 5u, 6u, 7u, 6u, 6u, 0u, 0xFFFFFFFFu, g.W()});
    a[64] = g.P(70) ? ByteOf(g, {0u, 1u, 2u, 8u, 9u, 0xAu, 0xFFu, 3u}) : g.R(256);
    a[66] = g.Pk({0x38Au, 0x38Au, 0x38Au, 0x387u, 0x387u, 0x387u, 0xCBu, 0x2C0u, 0u, 0x384u, 0x389u, 0x38Bu, 0xFFFFFFFFu, 0x1000038Au, g.W()});
    a[67] = g.P(70) ? ByteOf(g, {0u, 0u, 0x10u, 0x10u, 0xFFu, 0xEFu, 1u}) : g.R(256);
    a[68] = g.W();
    for (u32 k = 0; k < 3; ++k) a[69 + k] = Fin(g);   // acotado: con magnitudes de ~1e19 o mas los cuadrados desbordan un float (la x87 del EXE no)
    a[72] = g.P(60) ? 1u : 0u;
    a[73] = g.P(70) ? 1u : 0u;
    a[74] = g.P(60) ? 1u : 0u;
    a[75] = g.P(87) ? 1u : 0u;
    a[77] = Raw(g);
    a[80] = g.Pk({0u, 1u, 2u, 3u, 7u, 8u, g.R(100), g.W()});
    for (u32 k = 0; k < 3; ++k) { a[81 + k] = Fin(g); a[84 + k] = Fin(g); a[89 + k] = Fin(g); }
    a[87] = g.P(55) ? 1u : 0u;
    a[88] = Fin(g);
    a[92] = Raw(g);
    a[93] = g.P(70) ? std::bit_cast<u32>(float(double(i32(g.R(6283)) - 3141) / 1000.0)) : FloatQ(g);
    // el angulo se convierte con _ftol2 (el EXE) o cvttss2si (la DLL): solo coinciden dentro del rango de un int; el real devuelve un angulo en [-pi, pi]
    a[94] = g.P(85) ? std::bit_cast<u32>(float(double(i32(g.R(6283)) - 3141) / 1000.0)) : g.Pk({0u, 0x80000000u, 0x4EFFFFFFu, 0xCEFFFFFFu, 0x4E800000u, 0x7FC00000u, 0x3F800000u, 0xBF800000u, 0x00000001u, 0x40490FDBu, 0xC0490FDBu});
    if (a[94] != 0 && ((a[94] >> 23) & 0xFF) != 0xFF && ((a[94] >> 23) & 0xFF) > 127 + 30) a[94] = 0x41F00000u;   // |x| < 2^31
    a[95] = g.W();
    // 38%: escenario del barco comun (la entidad es un peaton del grupo, sin sumergir, de pie en V1 o en contacto con V2, V1 y V2 barcos, el ped en las mismas)
    if (boatScenario) {
        a[4] = 0u; a[6] = 3u; a[7] = g.P(90) ? 0u : 0x100u;
        a[73] = 1u;
        a[12] = 2u; a[14] = 2u; a[13] = g.P(80) ? 5u : a[13]; a[15] = g.P(80) ? 5u : a[15];
        const u32 which = g.R(4);
        a[8] = which == 0 ? 1u : (which == 1 ? 2u : (which == 2 ? 0u : 1u)); a[9] = which == 0 ? 2u : (which == 1 ? 1u : (which == 2 ? 2u : 0u));
        a[17] = 1u + g.R(2); a[18] = 1u + g.R(2);
        if (g.P(25)) a[17] = g.R(4);
        if (g.P(25)) a[18] = g.R(4);
        a[19] = g.P(75) ? 0u : (g.W() | 1u);
        a[66] = g.P(75) ? g.Pk({0x38Au, 0x387u, 0x384u, 0u}) : 0xCBu;
        a[68] = g.W() | 1u;
    }
    // 30%: escenario de escaneo (subtarea de seguimiento de ruta o de ir al punto, entidad presente, el temporizador de busqueda en hora y el de escaneo vencido)
    else if (g.P(40)) {
        a[4] = 0u; a[66] = g.Pk({0x38Au, 0x387u, 0x38Au, 0x387u});
        a[6] = g.P(60) ? 3u : a[6];
        a[7] = 0u; a[73] = g.P(50) ? 0u : a[73];
        a[56] = g.P(60) ? 0u : a[56];
        a[52] = 1u; a[53] = g.P(50) ? 0u : 1u;
        if (a[56] == 1u && a[57] == 0u) { a[54] = a[48] - g.R(100); a[55] = 1000u + g.R(2000); }
        a[50] = a[48] - (100u + g.R(2000)); a[51] = g.R(100);
        a[67] = g.P(70) ? 0u : a[67];
        const u32 sm = g.R(4);
        if (sm == 0) {   // el peaton esta mas cerca de la posicion de busqueda que (minimo - 1): se cancela y se crea la ruta de ir al punto
            const u32 base = a[20] ? 25u : 22u;
            for (u32 k = 0; k < 2; ++k) a[69 + k] = a[base + k];
            a[61] = PosNormal(g, 128, 134);
        } else if (sm == 1) {   // mas lejos que (minimo + 1): se cancela y se crea la ruta de nodos
            a[69] = std::bit_cast<u32>(1.0e6f * float(g.R(100) + 1)); a[70] = std::bit_cast<u32>(-1.0e6f * float(g.R(100) + 1));
            a[61] = g.P(50) ? 0x40000000u : PosNormal(g, 123, 129);
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
