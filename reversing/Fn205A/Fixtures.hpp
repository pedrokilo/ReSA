#pragma once
// Fixtures del banco Fn205A (lote 205A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 205A SIN validar: reversing/PendienteValidar/205A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores). 32 asignadas: 32 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x4B6170   CEventSexyVehicle destructor con borrado (vtable 0x85B0F8)                         thiscall(flags) `ret 4`: cuerpo 4AF0A0; con el bit 0 libera la ranura del pool de eventos 0xB744AC EN LINEA (indice /0x44 con signo)
//    1   0x4B61C0   CEventPedToChase destructor con borrado (vtable 0x85B138)                          idem, cuerpo 4AF1C0
//    2   0x4B6210   CEventPedToFlee destructor con borrado (vtable 0x85B178)                           idem, cuerpo 4AF2D0
//    3   0x4B6260   CEventAttractor destructor con borrado (vtable 0x85B1B8)                           idem, cuerpo 4AF400
//    4   0x4B62B0   CEventVehicleToSteal destructor con borrado (vtable 0x85B210)                      idem, cuerpo 4AF700
//    5   0x4B6300   CEventAcquaintancePed destructor con borrado (vtable 0x85B250)                     idem, cuerpo 4AF890
//    6   0x4B6350   CEventVehicleThreat destructor con borrado (vtable 0x85B298)                       idem, cuerpo 4AFBF0
//    7   0x4B63A0   CEventKnockOffBike destructor con borrado (vtable 0x85B2E0)                        idem, cuerpo 4AFE70
//    8   0x4B63F0   CEventGunAimedAt destructor con borrado (vtable 0x85B330)                         idem, cuerpo 4B07B0
//    9   0x4B6440   CEventScriptCommand destructor con borrado (vtable 0x85B378)                      idem, cuerpo 4B0A50
//   10   0x66F650   CTaskComplexWanderGang destructor con borrado (vtable 0x8700D4)                   thiscall(flags) `ret 4`: cuerpo 66F670; delete 61A5B0 si bit 0
//   11   0x66F670   CTaskComplexWanderGang cuerpo del destructor                                      thiscall() `ret`: [this]=86FE84h; jmp 61A3D0
//   12   0x66F680   CTaskComplexAvoidOtherPedWhileWandering destructor con borrado (vtable 0x86FEC4)  cuerpo 66A1D0; delete 61A5B0 si bit 0
//   13   0x66F6A0   CTaskComplexAvoidEntity destructor con borrado (vtable 0x86FF00)                  cuerpo 66AAE0 (sin hook en esta base: la DLL lo llama por su direccion del EXE); delete 61A5B0 si bit 0
//   14   0x66F6C0   CTaskComplexAvoidEntity::ComputeSphere                                            thiscall (ECX sin leer) (salida, 16 peds) `ret 8`: x87 a 24 bits; CColSphere::Set 40FD10
//   15   0x66FD30   CTaskComplexAvoidEntity::ComputeRouteRoundSphere                                  thiscall(ped) `ret 4`: ComputeSphere 66F6C0 + ComputeRouteRoundSphere 5F1890
//   16   0x5E9680   CPedShelterAttractor::IsAtHeadOfQueue (V, ranura 2)                               thiscall(ped) `ret 4`: mov al,1
//   17   0x5EAFE0   CPedAttractor::operator delete                                                    cdecl(obj): libera la ranura del pool 0xB744C4 EN LINEA (indice /0xC4 con signo)
//   18   0x5EB140   std::copy_backward de 4 bytes (A)                                                  cdecl(out, first, last, destEnd): copia hacia atras; *out = destino final; eax = out
//   19   0x5EB170   envoltorio de copy_backward de 20 bytes                                            cdecl(out, first, last, destEnd): 5EAC50(out, first, last, destEnd, destEnd); eax = out
//   20   0x5EB1B0   std::copy_backward de 4 bytes (B)                                                  idem 18
//   21   0x5EB1E0   std::fill de 12 bytes                                                              cdecl(first, last, valor*): tres dwords por vuelta, valor releido
//   22   0x5EB220   envoltorio de copy_backward de 12 bytes                                            idem 19 hacia 5EACC0
//   23   0x5EB260   std::copy_backward de 4 bytes (C)                                                  idem 18
//   24   0x5EB2A0   std::uninitialized_fill_n de 4 bytes (A)                                           cdecl(dest, n, valor*, 2 de relleno): n sin signo; escribe solo si dest != nulo
//   25   0x5EB2E0   std::uninitialized_fill_n de 20 bytes                                              idem con rep movsd de 5 dwords
//   26   0x5EB310   std::uninitialized_fill_n de 4 bytes (B)                                           idem 24
//   27   0x5EB340   std::uninitialized_fill_n de 12 bytes                                              idem con tres dwords en orden
//   28   0x5EB390   std::uninitialized_fill_n de 4 bytes (C)                                           idem 24
//   29   0x5EB5F0   RemoveEffect<CPedAttractor>                                                          stdcall(fx, SArray*) `ret 8`: AbortPedTasks 5EAF60 por cada atractor con +4 == fx; devuelve 0
//   30   0x5EB640   IsPedRegistered<CPedAttractor>                                                       stdcall(ped, SArray*) `ret 8`: IsRegisteredWithPed 5EB4C0; verdadero al primer acierto
//   31   0x5EB690   IsPedRegisteredWithEffect<CPedAttractor>                                             stdcall(ped, fx, entidad, SArray*) `ret 10h`: +4 == fx y +8 == entidad; IsRegisteredWithPed 5EB4C0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this` en la arena A: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F205AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 32;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas B, C y D
constexpr std::size_t kWinA = 0x1000;    // ventana de la arena A (igual que las demas)
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

// Desplazamiento de un objeto de `span` bytes en la ventana de la arena A segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWinA);
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
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }   // distinto de cero y nunca una direccion de arena alineada del oraculo
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
    // magnitudes alrededor de la mitad del maximo (el doble de una diferencia desborda el float pero la suma posterior puede volver al rango)
    const u32 huge = sign | (0x7E800000u + (g.W() & 0x007FFFFFu));
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, huge, huge, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline u32 QuietSig(u32 b) { return (b | 0x00400000u) & 0x7FFFFFu; }   // significando de un NaN ya silenciado
// Un float finito normal positivo con exponente sesgado en [lo, hi)
inline u32 PosNormal(Gen& g, u32 lo, u32 hi) { return ((lo + g.R(hi - lo)) << 23) | (g.W() & 0x7FFFFFu); }


// Clases de funcion (forma)
enum Kind : u32 { kEvtSdd, kTaskSdd, kBody, kSphere, kRoute, kBool1, kAttrDel, kCopyBack, kWrap, kFill12, kFillN, kRemove, kIsReg, kIsRegEff };
inline constexpr std::array<Kind, kFunctions> kKind{kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd,
                                                    kTaskSdd, kBody, kTaskSdd, kTaskSdd, kSphere, kRoute, kBool1, kAttrDel,
                                                    kCopyBack, kWrap, kCopyBack, kFill12, kWrap, kCopyBack,
                                                    kFillN, kFillN, kFillN, kFillN, kFillN,
                                                    kRemove, kIsReg, kIsRegEff};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4B6170, 0x4B61C0, 0x4B6210, 0x4B6260, 0x4B62B0, 0x4B6300, 0x4B6350, 0x4B63A0, 0x4B63F0, 0x4B6440, 0x66F650, 0x66F670, 0x66F680, 0x66F6A0, 0x66F6C0, 0x66FD30,
                                                    0x5E9680, 0x5EAFE0, 0x5EB140, 0x5EB170, 0x5EB1B0, 0x5EB1E0, 0x5EB220, 0x5EB260, 0x5EB2A0, 0x5EB2E0, 0x5EB310, 0x5EB340, 0x5EB390, 0x5EB5F0, 0x5EB640, 0x5EB690};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85B0F8, 0x85B138, 0x85B178, 0x85B1B8, 0x85B210, 0x85B250, 0x85B298, 0x85B2E0, 0x85B330, 0x85B378, 0x8700D4, 0x86FE84, 0x86FEC4, 0x86FF00, 0, 0,
                                                     0x86C5B4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this` (arena A)
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x40, 0x40, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
// Dwords por elemento de las funciones uninitialized_fill_n (24..28)
inline constexpr std::array<u32, kFunctions> kElemDw{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 1, 3, 1, 0, 0, 0};
// Cuerpo (colaborador) de los destructores con borrado (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x4AF0A0, 0x4AF1C0, 0x4AF2D0, 0x4AF400, 0x4AF700, 0x4AF890, 0x4AFBF0, 0x4AFE70, 0x4B07B0, 0x4B0A50, 0x66F670, 0, 0x66A1D0, 0x66AAE0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Memoria global del juego tocada
inline constexpr u32 kEventPoolGlobal = 0xB744AC;                       // puntero al pool de eventos (destructores con borrado de eventos)
inline constexpr u32 kAttrPoolGlobal = 0xB744C4;                        // puntero al pool de atractores (CPedAttractor::operator delete)
inline constexpr u32 kModelTable = 0xA9B0C8;                            // CModelInfo::ms_modelInfoPtrs
inline constexpr i32 kModelLowIdx = -16, kModelHighIdx = 32767;         // indices de modelo (con signo de 16 bits) que el banco usa
inline constexpr std::size_t kModelRegionBytes = std::size_t(kModelHighIdx - kModelLowIdx + 1) * 4;
inline constexpr u32 kModelRegionStart = kModelTable + 4u * u32(kModelLowIdx);

// Disposicion de los objetos sinteticos de ComputeSphere / ComputeRouteRoundSphere (arenas C y D): 16 peatones de 0x40 bytes en C (+4 posicion, +0x14 matriz, +0x22 modelo), 16 matrices de 0x40 bytes en D
// (+0x30 posicion), 8 model infos de 0x20 bytes en D a partir de 0x400 (+0x14 CColModel) y 8 CColModel de 0x30 bytes en D a partir de 0x600 (+0x24 radio).
inline constexpr u32 kPedStride = 0x40, kMatStride = 0x40, kMiBase = 0x400, kMiStride = 0x20, kCmBase = 0x600, kCmStride = 0x30;
// Datos del caso de ComputeSphere en `c.s` (palabras u32 little endian): [0..15] seleccion por posicion del array (0 nulo, j + 1 = peaton j), luego 16 peatones de 8 palabras
// (x, y, z, tiene matriz, mx, my, mz, indice del modelo en el pool), luego el pool de 8 indices de modelo y el pool de 8 radios (bits).
inline constexpr u32 kSelWords = 16, kSlotWords = 8, kSphWordsTotal = kSelWords + 16 * kSlotWords + 16;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kEvtSdd, kAttrDel: a[4] = desplazamiento del pool en B, a[5] = distancia this - almacenamiento (con signo), a[6] = desplazamiento de los estados en D, a[7] = banderas (pila; solo kEvtSdd), a[8] = EAX del falso del cuerpo
//            (solo kEvtSdd), a[9] = dword que deja en [this] (solo kEvtSdd), a[10] = ultimo libre (+0Ch del pool), a[11] = byte previo del estado del slot, a[12] = tamano (+8 del pool)
//   kTaskSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kBody: a[4] = EAX del falso de 61A3D0, a[5] = dword que deja en [this]
//   kSphere: a[4] = desplazamiento de la salida en B, a[5] = perfil de flotantes; datos en c.s; el array de 16 punteros esta en A en a[0]
//   kRoute: a[4] = entidad (+0Ch de this, cruda), a[5] = desplazamiento del peaton en B, a[6..8] = posicion del peaton (bits), a[9] = matriz (0 nula, 1 en C), a[10] = desplazamiento de la matriz en C, a[11..13] = posicion de la
//           matriz (+30h..+38h), a[14..17] = lo que escribe el falso de ComputeSphere (centro x, y, z y radio), a[18] = EAX del falso de ComputeRouteRoundSphere
//   kBool1: a[4] = el peaton (pila, crudo)
//   kCopyBack: a[4] = modo de la salida (0 en B, 1 dentro del destino, 2 dentro del origen), a[5] = desplazamiento de la salida en B, a[6] = numero de dwords, a[7] = arena del origen, a[8] = desplazamiento del origen,
//              a[9] = desalineacion del origen (0..3), a[10] = modo del destino (0 otra arena, 1 misma arena solapado, 2 misma arena sin solape), a[11] = desplazamiento del destino (modo 0), a[12] = desalineacion del destino,
//              a[13] = desplazamiento (en dwords, con signo) del final del destino respecto al final del origen (modo 1), a[14] = indice de dword de la salida dentro del rango (modos 1 y 2)
//   kWrap: a[4..7] = out, first, last, destEnd (crudos), a[8..11] = si no son 0, ((arena + 1) << 16) | desplazamiento: el argumento es esa direccion de arena
//   kFill12: a[4] = desplazamiento de first en B, a[5] = numero de elementos, a[6] = modo del valor (0 en C, 1 dentro del destino, 2 crudo), a[7] = desplazamiento del valor en C o (modo 1) dword con signo respecto a first,
//            a[8] = desalineacion de first, a[9] = desalineacion del valor, a[10] = valor crudo (modo 2)
//   kFillN: a[4] = modo de dest (0 nulo, 1 en B), a[5] = desplazamiento de dest en B, a[6] = n, a[7] = modo del valor (0 en C, 1 dentro del destino, 2 crudo), a[8] = desplazamiento del valor en C o (modo 1) dword con signo
//           respecto a dest, a[9] = desalineacion de dest, a[10] = desalineacion del valor, a[11] = valor crudo (modo 2), a[12], a[13] = argumentos de relleno de la pila
//   kRemove, kIsReg, kIsRegEff: a[4] = desplazamiento del vector (SArray) en C, a[5] = desplazamiento de los punteros en D, a[6] = fx, a[7] = entidad, a[8] = peaton, a[9] = numero de elementos, a[10..17] = atractor (0..7) de cada
//           elemento, a[18..25] = modo de +4 de cada atractor (0 igual a fx, 1 distinto), a[26..33] = modo de +8 (0 igual a la entidad, 1 distinto), a[34] = mascara de resultados de IsRegisteredWithPed, a[35] = modo de los limites
//           del vector (0 normal, 1 inicio nulo, 2 fin antes del inicio, 3 fin = inicio, 4 fin muy por delante (cuenta negativa))
// Valores de las posiciones y radios de ComputeSphere. Dos NaN de cargas distintas en una misma operacion son ambiguos (la x87 elige segun el orden de los operandos): cada caso lleva como mucho UN NaN de
// usuario (perfil 6 impar); los infinitos solo producen el NaN indefinido, identico en todos los casos (perfil 6 par); el perfil 7 mezcla de todo SIN NaN de usuario.
inline u32 SphFloat(Gen& g, u32 profile, bool nanHere = false) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 moderate = sign | PosNormal(g, 110, 150);
    if (profile < 5) return g.P(6) ? sign : moderate;                                   // benigno: normales moderados y ceros con signo
    if (profile == 5) return g.Pk({moderate, moderate, sign | (g.W() & 0x7FFFFFu), sign | PosNormal(g, 1, 40), sign | PosNormal(g, 180, 254), sign | 0x7F7FFFFFu, sign | 0x00800000u, sign | 0x3F800000u, sign});   // denormales, muy pequenos, enormes
    if (profile == 6) {
        if (nanHere) return sign | 0x7FC00000u | (g.W() & 0x3FFFFFu) | (g.P(50) ? 0u : 0x80000000u);       // el unico NaN del caso (silencioso o, con el bit 22 a cero, senalizante)
        return g.P(15) ? (sign | 0x7F800000u) : moderate;                                                    // infinitos
    }
    u32 v;
    do { v = FloatBits(g); } while (IsNanBits(v));
    return v;
}
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    const auto putS = [&](u32 index, u32 v) { if (c.s.size() < (index + 1) * 4) c.s.resize((index + 1) * 4); std::memcpy(c.s.data() + index * 4, &v, 4); };
    switch (kKind[c.fn]) {
    case kEvtSdd:
    case kAttrDel: {
        const bool evt = kKind[c.fn] == kEvtSdd;
        const u32 stride = evt ? 0x44u : 0xC4u;
        const i32 idx = i32(g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 100u, 299u, g.R(300), g.R(300), g.R(300), 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFF8u, 0xFFFFFFF0u}));
        const u32 extra = g.Pk({0u, 1u, stride - 1u, g.R(stride), g.R(stride)});
        a[4] = AuxOffset(g, 0x20);
        a[5] = u32(i32(u32(idx) * stride) + (idx < 0 ? -i32(extra) : i32(extra)));
        a[6] = 0x40u + 4u * g.R((u32(kWin) - 0x40u - 0x300u - 0x40u) / 4u);
        a[7] = evt ? FlagsDword(g) : 0u;
        a[8] = evt ? Raw(g) : 0u; a[9] = evt ? Raw(g) : 0u;
        a[10] = g.Pk({u32(idx - 1), u32(idx), u32(idx + 1), 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 300u, g.W(), g.R(400), u32(idx - 1), u32(idx)});
        a[11] = g.Pk({0u, 0x80u, 0x7Fu, 0xFFu, g.R(256), g.R(256)});
        a[12] = g.Pk({300u, 512u, 1000u, g.W() | 0x100u});
        break;
    }
    case kTaskSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kBody:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kSphere: {
        a[4] = AuxOffset(g, 0x20);
        const u32 profile = n % 8;
        a[5] = profile;
        const u32 want = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, 12u, 16u, 16u, g.R(17), g.R(17)});
        // seleccion: `want` posiciones no nulas al azar (las demas, nulas); cada una apunta a un peaton al azar (con repeticiones)
        std::array<bool, 16> used{};
        u32 placed = 0;
        for (u32 guard = 0; placed < want && guard < 200; ++guard) { const u32 pos = g.R(16); if (!used[pos]) { used[pos] = true; ++placed; } }
        for (u32 i = 0; i < kSelWords; ++i) putS(i, used[i] ? 1u + (g.P(70) ? i : g.R(16)) : 0u);
        // pool de modelos: ocho indices distintos (como int16): pequenos, grandes, los ultimos de la tabla y negativos (movsx)
        std::array<i32, 8> pool{};
        for (u32 i = 0; i < 8; ++i) {
            for (;;) {
                const i32 v = i32(g.Pk({0u, 1u, 2u, 400u, 595u, 19999u, 20000u, 32767u, g.R(20000), g.R(20000), g.R(32768), g.R(32768), 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFF8u, 0xFFFFFFF0u, u32(-1 - i32(g.R(16)))}));
                if (v < kModelLowIdx || v > kModelHighIdx) continue;
                bool dup = false;
                for (u32 k = 0; k < i; ++k) dup |= pool[k] == v;
                if (!dup) { pool[i] = v; break; }
            }
        }
        const u32 nanAt = g.R(16) * 7 + (g.R(2) ? 0u : 1u) + (g.P(30) ? 4u : 0u);   // posicion (ped, coordenada x o y, ped o matriz) del unico NaN de los casos con NaN
        for (u32 j = 0; j < 16; ++j) {
            const u32 base = kSelWords + j * kSlotWords;
            const auto sf = [&](u32 k) { return SphFloat(g, profile, profile == 6 && (n / 8) % 2 == 1 && j * 7 + k == nanAt); };
            putS(base + 0, sf(0)); putS(base + 1, sf(1)); putS(base + 2, sf(2));
            putS(base + 3, g.Pk({0u, 1u, 1u}));
            putS(base + 4, sf(4)); putS(base + 5, sf(5)); putS(base + 6, sf(6));
            putS(base + 7, g.R(8));
        }
        for (u32 i = 0; i < 8; ++i) putS(kSelWords + 16 * kSlotWords + i, u32(pool[i]));
        for (u32 i = 0; i < 8; ++i) putS(kSelWords + 16 * kSlotWords + 8 + i, profile == 6 ? SphFloat(g, 0) : SphFloat(g, profile));   // radios: nunca NaN con las posiciones NaN (dos NaN distintos son ambiguos)
        break;
    }
    case kRoute:
        a[4] = Raw(g);
        a[5] = AuxOffset(g, 0x20);
        for (u32 i = 0; i < 3; ++i) a[6 + i] = FloatBits(g);
        a[9] = g.Pk({0u, 1u, 1u});
        a[10] = AuxOffset(g, 0x40);
        for (u32 i = 0; i < 3; ++i) a[11 + i] = FloatBits(g);
        for (u32 i = 0; i < 4; ++i) a[14 + i] = FloatBits(g);
        a[18] = Raw(g);
        break;
    case kBool1:
        a[4] = Raw(g);
        break;
    case kCopyBack: {
        a[4] = g.Pk({0u, 0u, 0u, 1u, 2u});
        a[5] = AuxOffset(g, 8);
        a[6] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 16u, 32u, 40u, g.R(41), g.R(41)});
        a[7] = g.R(4);
        a[8] = 0x100u + 4u * g.R(0x180);
        a[9] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[10] = g.Pk({0u, 0u, 1u, 2u});
        a[11] = 0x100u + 4u * g.R(0x180);
        a[12] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[13] = u32(i32(g.R(2 * a[6] + 1)) - i32(a[6]));
        a[14] = a[6] ? g.R(a[6]) : 0u;
        break;
    }
    case kWrap:
        for (u32 i = 0; i < 4; ++i) {
            a[4 + i] = Raw(g);
            a[8 + i] = g.P(50) ? ((1u + g.R(4)) << 16) | AuxOffset(g, 8) : 0u;
        }
        break;
    case kFill12: {
        a[4] = 0x100u + 4u * g.R(0x180);
        a[5] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, 16u, g.R(17), g.R(17)});
        a[6] = a[5] == 0 ? g.Pk({0u, 1u, 2u, 2u}) : g.Pk({0u, 0u, 1u, 1u, 1u});
        a[7] = a[6] == 1 ? u32(i32(g.R(3 * a[5] + 7)) - 3) : AuxOffset(g, 0x10);
        a[8] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[9] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[10] = Raw(g);
        break;
    }
    case kFillN: {
        const u32 e = kElemDw[c.fn];
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = 0x100u + 4u * g.R(0x180);
        u32 cnt = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, 12u, g.R(25), g.R(25)});
        if (a[4] == 0 && cnt > 1) a[4] = 1u;   // el destino nulo solo se salta la primera vuelta (despues avanza a 4, 8, ...): con n > 1 escribiria en la pagina cero
        a[6] = cnt;
        a[7] = (a[4] == 0 || cnt == 0) ? g.Pk({0u, 1u, 2u, 2u, 2u}) : g.Pk({0u, 0u, 1u, 1u, 1u});
        if (a[4] == 0 && a[7] == 1) a[7] = 0;                                          // un valor dentro del destino exige destino
        a[8] = a[7] == 1 ? u32(i32(g.R(cnt * e + 7)) - 3) : AuxOffset(g, 0x20);
        a[9] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[10] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[11] = Raw(g);
        a[12] = Raw(g); a[13] = Raw(g);
        break;
    }
    case kRemove:
    case kIsReg:
    case kIsRegEff: {
        a[4] = AuxOffset(g, 0x20);
        a[5] = AuxOffset(g, 0x40);
        a[6] = Raw(g); a[7] = Raw(g); a[8] = Raw(g);
        a[9] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, g.R(9)});
        for (u32 i = 0; i < 8; ++i) a[10 + i] = g.R(8);
        for (u32 i = 0; i < 8; ++i) a[18 + i] = g.Pk({0u, 0u, 1u});
        for (u32 i = 0; i < 8; ++i) a[26 + i] = g.Pk({0u, 0u, 1u});
        a[34] = g.Pk({0u, 0xFFFFFFFFu, 1u, 2u, 4u, g.W(), g.W(), 0x55555555u, 0x80u});
        a[35] = g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u});
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
