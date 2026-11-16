#pragma once
// Fixtures del banco Fn198A (lote 198A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 198A SIN validar: ver reversing/PendienteValidar/198A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores).
//   idx  direccion  nombre real                                                                         forma
//    0   0x638590   CTaskComplexWaitAtAttractor destructor con borrado (vtable 0x86E108)                thiscall(flags) `ret 4`: cuerpo 632630; delete 61A5B0 si bit 0 del BYTE; devuelve this
//    1   0x6385B0   CTaskComplexUseAttractor destructor con borrado (vtable 0x86E134)                   idem con cuerpo 632720
//    2   0x639E20   CTaskComplexSequence destructor con borrado (vtable 0x86E200)                       idem con cuerpo 6389F0
//    3   0x65A220   CTaskComplexDriveFireTruck destructor con borrado (vtable 0x86F570)                 idem con cuerpo 6593A0
//    4   0x66DC70   CTaskComplexGoToPointAndStandStillTimed destructor con borrado (vtable 0x86FDD4)    idem con cuerpo 668630 (el de la base)
//    5   0x65A0F0   CTaskComplexPresentIDToCop destructor con borrado (vtable 0x86F544)                 idem con cuerpo 659240 (funcion 15 de este lote)
//    6   0x5FFE60   CVehicleScanner::ScanForVehiclesInRange                                              thiscall(ped) `ret 4`: IsAlive 5E0170; vehiculo +58Ch y bit 8 de +46Ch: mision (+484h == 2) -> 5FFA20(0); +4Ch: CleanUp 571A00 / Register 571B70
//    7   0x5FFF30   CObjectScanner::ScanForObjectsInRange                                                thiscall(ped) `ret 4`: vehiculo y bit 8 -> nada (sin IsAlive); si no IsAlive y 5FFA20(2)
//    8   0x6034B0   CAttractorScanner::ScanEntityList                                                   thiscall(lista, ped) `ret 8`: recorre nodos; objetos (tipo 4): BL = bit 24 de +140h, descartes por +1Ch / +140h; modelo con signo; Get2dEffect 4C4C70; 5FFFD0 por direccion
//    9   0x603570   CAttractorScanner::FindClosestPedForEffect (cdecl)                                   cdecl(efecto): pool 0xB74490 de peatones; tarea activa 681720 (tipo E9 salta); x87 (dz*dz + dy*dy) + dx*dx; GetIndex 6F9F60; IsApproachable 5EA220
//   10   0x603A30   CPedAcquaintanceScanner::CanScanPed                                                  thiscall(ped) `ret 4`: IsAlive; permiso (+484h, +0Ch/+0Dh/+0Eh, 608750); evento actual 4B8CA0 de tipo 24h: +C4h, 4B5730, 600CF0
//   11   0x658C20   CTaskComplexMedicTreatInjuredPed::GetTaskType (V)                                     thiscall() `ret`: mov eax,64h
//   12   0x658C30   CTaskComplexMedicTreatInjuredPed cuerpo del destructor                               vtable 86F518; accidente +18h (tratado +4 a 0); CleanUp 571A00 de +0Ch y +10h; 61A3D0
//   13   0x6591D0   CTaskComplexPresentIDToCop::Constructor                                               thiscall(policia) `ret 4`: 61A3B0; +0Ch = policia; vtable 86F544; Register 571B70
//   14   0x659230   CTaskComplexPresentIDToCop::GetTaskType (V)                                           thiscall() `ret`: mov eax,6Ah
//   15   0x659240   CTaskComplexPresentIDToCop cuerpo del destructor                                      vtable 86F544; CleanUp 571A00 de +0Ch si existe; 61A3D0
//   16   0x6592A0   CTaskComplexPresentIDToCop::ComputeHeading                                             thiscall(ped) `ret 4`: float en ST0; 53CBE0 con (policia.x, policia.y, ped.x, ped.y); 53CB50
//   17   0x659B60   CTaskComplexPresentIDToCop::Clone (V)                                                 thiscall() `ret`: new(10h) 61A5A0; constructor 6591D0 con +0Ch; devuelve el EAX del constructor
//   18   0x65A110   CTaskComplexPresentIDToCop::CreateSubTask                                            thiscall(tipo, ped) `ret 8`: 19D new(34h) + 61ABE0 + vtable 85A29C; 386 new(18h) + ComputeHeading + 667E20
//   19   0x65AFE0   CTaskComplexPresentIDToCop::CreateNextSubTask (V)                                    thiscall(ped) `ret 4`: tipo de la subtarea (+8, virtual 10h): 19D -> CreateSubTask(516); 386 -> Say + CreateSubTask(19D)
//   20   0x65B050   CTaskComplexPresentIDToCop::CreateFirstSubTask (V)                                    thiscall(ped) `ret 4`: CreateSubTask(386, ped)
//   21   0x65B070   CTaskComplexPresentIDToCop::ControlSubTask (V)                                        thiscall(ped) `ret 4`: policia +0Ch: subtarea (+8) o CreateSubTask(516)
//   22   0x46FEE0   CTaskComplexSeekEntityStandard::GetSeekPos                                             thiscall(salida, ped) `ret 8`: llamada virtual (ranura 0) del calculador +40h; copia el temporal
//   23   0x470070   CPed::SetKindaStayInSamePlace                                                         thiscall(byte) `ret 4`: bit 22 de +46Ch = bit 0 del byte
//   24   0x4700E0   CStreaming::SetMissionDoesntRequireStreamedScript                                     cdecl(indice): salta a 409C90 con indice + 6676h
//   25   0x470100   CBoundingBox::SetMinMax                                                              thiscall(6 floats) `ret 18h`: copia 6 dwords; devuelve this
//   26   0x668240   CTaskComplexGoToPointAndStandStill copia del cuerpo del destructor                    thiscall() : [this] = 86FDA8; jmp 61A3D0
//   27   0x668250   CTaskComplexGoToPointAndStandStill::SelectMoveState                                    thiscall(tarea, ped, r1, r2) `ret 10h`: x87 24 bits; +8 = 4 (andar), 6 (correr), 7 (esprintar)
//   28   0x6682D0   CTaskComplexGoToPointAndStandStill::CreateSubTask                                     thiscall(tipo, ped) `ret 8`: GO_TO_POINT, LEAVE_CAR, PAUSE, STAND_STILL (x87 de la mezcla)
//   29   0x620B00   CTaskComplexReactToAttack::Constructor                                                thiscall(5 dwords) `ret 14h`: 61A3B0; campos; vtable 86D868; Register 571B70 si hay agresor
//   30   0x620B80   CTaskComplexReactToAttack::GetTaskType                                                thiscall() `ret`: mov eax,3EEh
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40; C mide 0x4000 por el pool de peatones) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F198AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 31;
constexpr unsigned kArenas = 4;
constexpr unsigned kPayload = 96;       // a[0..95]; las semillas de las arenas van en a[92..95]
constexpr unsigned kSeedAt = 92;

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
enum Kind : u32 { kSdd, kConst, kScanVeh, kScanObj, kEntList, kClosest, kCanScan, kMedicDtor, kPresentCtor, kPresentDtor, kHeading, kClone, kPresentSub, kNext, kFirst, kControl, kSeekPos, kStay, kStream,
                  kMinMax, kBodyCopy, kSelect, kGoTo, kReactCtor };
inline constexpr std::array<Kind, kFunctions> kKind{kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kScanVeh, kScanObj, kEntList, kClosest, kCanScan, kConst, kMedicDtor, kPresentCtor, kConst, kPresentDtor, kHeading, kClone,
                                                    kPresentSub, kNext, kFirst, kControl, kSeekPos, kStay, kStream, kMinMax, kBodyCopy, kSelect, kGoTo, kReactCtor, kConst};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x638590, 0x6385B0, 0x639E20, 0x65A220, 0x66DC70, 0x65A0F0, 0x5FFE60, 0x5FFF30, 0x6034B0, 0x603570, 0x603A30, 0x658C20, 0x658C30, 0x6591D0, 0x659230,
                                                    0x659240, 0x6592A0, 0x659B60, 0x65A110, 0x65AFE0, 0x65B050, 0x65B070, 0x46FEE0, 0x470070, 0x4700E0, 0x470100, 0x668240, 0x668250, 0x6682D0, 0x620B00, 0x620B80};
// Cuerpo (colaborador) que llaman los destructores con borrado (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x632630, 0x632720, 0x6389F0, 0x6593A0, 0x668630, 0x659240};
inline constexpr std::array<u32, kFunctions> kConstVal{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x3EE};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86E108, 0x86E134, 0x86E200, 0x86F570, 0x86FDD4, 0x86F544, 0, 0, 0, 0, 0, 0x86F518, 0x86F518, 0x86F544, 0x86F544, 0x86F544, 0x86F544, 0x86F544, 0x86F544, 0x86F544,
                                                     0x86F544, 0x86F544, 0x859DF8, 0, 0, 0, 0x86FDA8, 0x86FDA8, 0x86FDA8, 0x86D868, 0x86D868};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 0x50, 4, 4, 4, 0x10, 4, 0x1C, 0x10, 4, 0x10, 0x10, 0x10, 4, 0x10, 4, 0x10, 0x44, 0x470, 4, 0x18, 4, 4, 0x28, 0x24, 4};

// Memoria global del juego tocada
inline constexpr u32 kModelTable = 0xA9B0C8;                  // CModelInfo::ms_modelInfoPtrs (indice con signo de 16 bits)
inline constexpr int kModelLo = -3, kModelHi = 7;             // indices cubiertos por la region
inline constexpr u32 kModelSlots = kModelHi - kModelLo + 1;   // 11
inline constexpr u32 kPedPool = 0xB74490;                     // CPools::ms_pPedPool
inline constexpr u32 kUserLists = 0xC3A200;                   // CScripted2dEffects::ms_userLists (0x24 bytes cada una)
inline constexpr u32 kUserCount = 8;
inline constexpr u32 kTimeStep = 0xB7CB5C;                    // CTimer::ms_fTimeStep
inline constexpr u32 kPedStride = 0x7C4;
inline constexpr u32 kEntStride = 0x150;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[92..95] = semillas de las arenas A, B, C y D
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kConst: nada mas
//   kScanVeh: a[4] = desplazamiento del peaton en C, a[5] = vehiculo (+58Ch, valor crudo: 0 o no nulo), a[6] = banderas +46Ch, a[7] = byte +484h, a[8] = +4Ch previo del escaner (0 nulo, otro crudo), a[9] = EAX del falso de IsAlive
//   kScanObj: a[4..7, 9] como kScanVeh
//   kEntList: a[4] = desplazamiento de la lista en B, a[5] = base de los nodos en B, a[6] = base de las entidades en C, a[7] = numero de nodos (0..5), a[8 + 4e .. 11 + 4e] (e = 0..4) = byte +36h, banderas +1Ch, banderas +140h,
//             indice de modelo (con signo, -3..7); a[28] = peaton (pila, crudo); a[40..50] = n2dfx de cada modelo (indices -3..7); a[52..57] = tipo de cada efecto 2D (+0Ch); a[58..63] = byte +37h de cada efecto
//   kClosest: a[4] = desplazamiento del pool en B, a[5] = base de los peatones en C, a[6] = desplazamiento del mapa en D, a[7] = tamano del pool (0..5), a[8..12] = byte del mapa de cada peaton,
//             a[13 + 11p .. 23 + 11p] (p = 0..4) = indice de modelo, tipo de peaton, modo de matriz, modo de tarea (0 ninguna, 1 tipo 0xE9, 2 otro), tipo de la tarea, x, y, z del peaton, x, y, z de la matriz;
//             a[68..70] = x, y, z del efecto (en C, +0..+8), a[71] = indice de GetIndex (0..7), a[72..75] = los cuatro modelos de la lista de usuarios, a[76] = byte +20h de la lista, a[77..80] = tipos de peaton (+10h),
//             a[81] = mascara de IsApproachable (un bit por llamada)
//   kCanScan: a[4] = desplazamiento del peaton en C, a[5] = byte +484h, a[6] = banderas +46Ch, a[7] = desplazamiento de la inteligencia en D, a[8..10] = bytes +0Ch, +0Dh, +0Eh del escaner, a[11] = EAX de IsAlive,
//             a[12] = EAX de GetStatus (dword), a[13] = evento (0 nulo, 1 en B), a[14] = desplazamiento del evento en B, a[15] = tipo del evento, a[16] = +C4h de la inteligencia, a[17] = EAX de ComputeResponseTaskOfType,
//             a[18] = EAX de FindRespectedFriendInInformRange
//   kMedicDtor: a[4] = accidente (0 nulo, 1 en B), a[5] = desplazamiento en B, a[6] = primer dword del accidente, a[7] = byte +4, a[8] = byte +5, a[9] = +0Ch del objeto, a[10] = +10h del objeto, a[11] = dword que deja el falso de 61A3D0
//   kPresentCtor: a[4] = policia (pila, crudo), a[5] = dword que deja el falso de 61A3B0
//   kPresentDtor: a[4] = +0Ch del objeto, a[5] = dword que deja el falso de 61A3D0
//   kHeading: a[4] = policia (desplazamiento en C), a[5] = peaton (desplazamiento en C, en la segunda mitad), a[6] = modo de la matriz del policia, a[7] = desplazamiento en D, a[8] = modo de la matriz del peaton, a[9] = desplazamiento en D,
//             a[10..12] = posicion del policia, a[13..15] = matriz del policia, a[16..18] = posicion del peaton, a[19..21] = matriz del peaton, a[22] = resultado de GetRadianAngleBetweenPoints, a[23] = resultado de LimitRadianAngle
//   kClone: a[4] = +0Ch del objeto, a[5] = memoria (0 nula, 1 en B), a[6] = desplazamiento en B, a[7] = dword que deja el falso del constructor
//   kPresentSub: a[4] = tipo (pila), a[5] = ped (pila, crudo), a[6] = memoria (0 nula, 1 en B), a[7] = desplazamiento en B, a[8] = resultado de ComputeHeading (float), a[9] = dword que dejan los falsos de los constructores
//   kNext: a[4] = desplazamiento de la subtarea en C, a[5] = tipo que devuelve su GetTaskType, a[6] = ped (pila, crudo), a[7] = EAX del falso de CreateSubTask
//   kFirst: a[4] = ped (pila, crudo), a[5] = EAX del falso de CreateSubTask
//   kControl: a[4] = +0Ch del objeto, a[5] = +8 del objeto (crudo), a[6] = ped (pila, crudo), a[7] = EAX del falso de CreateSubTask
//   kSeekPos: a[4] = +0Ch del objeto (entidad, cruda), a[5] = ped (pila, crudo), a[6..8] = lo que escribe el calculador falso, a[9] = desplazamiento de la salida en B
//   kStay: a[4] = palabra previa +46Ch, a[5] = argumento (pila)
//   kStream: a[4] = argumento (pila)
//   kMinMax: a[4..9] = los seis floats (pila)
//   kBodyCopy: a[4] = dword que deja el falso de 61A3D0
//   kSelect: a[4] = desplazamiento de la tarea en B, a[5] = desplazamiento del peaton en C, a[6] = modo de matriz, a[7] = desplazamiento en D, a[8..9] = x, y del peaton, a[10..11] = x, y de la matriz, a[12..13] = x, y del destino,
//            a[14] = radio de andar, a[15] = radio de correr
//   kGoTo: a[4] = subtarea (0 nula, 1 en B), a[5] = desplazamiento de la subtarea en B, a[6] = tipo que devuelve su GetTaskType, a[7] = +0Ch, a[8..10] = +10h..+18h, a[11] = +1Ch, a[12] = +20h, a[13] = byte +24h, a[14] = tipo (pila),
//          a[15] = peaton (desplazamiento en C), a[16] = modo de matriz, a[17] = desplazamiento en D, a[18..20] = posicion del peaton, a[21..23] = matriz, a[24] = +4DCh del peaton, a[25] = vehiculo (+58Ch), a[26] = resultado del producto
//          escalar falso, a[27..29] = resultado de la resta falsa, a[30] = memoria (0 nula, 1 en B), a[31] = desplazamiento de la memoria en B, a[32] = paso de tiempo, a[33] = dword que dejan los constructores falsos
//   kReactCtor: a[4..8] = los cinco argumentos de pila, a[9] = dword que deja el falso de 61A3B0
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
    const auto aliveRet = [&] { return g.Pk({0u, 0u, 1u, 1u, 1u, 0x100u, 0xFF00u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80u, g.W(), g.W() | 1u, g.W() & ~0xFFu}); };
    switch (kKind[c.fn]) {
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kConst:
        a[3] = Raw(g);
        break;
    case kScanVeh:
    case kScanObj:
        a[4] = AuxC(g, 0x600);
        a[5] = g.P(33) ? 0u : RawNz(g);
        a[6] = g.Pk({0u, 0x100u, 0x100u, 0xFFFFFFFFu, 0xFFFFFEFFu, g.W() | 0x100u, g.W() & ~0x100u, 0x200u, 0x80u, 0x10000u});
        a[7] = ByteOf(g, {2u, 2u, 2u, 0u, 1u, 3u, 0xFFu});
        a[8] = g.P(50) ? 0u : RawNz(g);
        a[9] = aliveRet();
        break;
    case kEntList: {
        a[4] = Aux(g, 0x800, 0x1000, 4);
        a[5] = Aux(g, 0x100, 0x400, 0x30);
        a[6] = Aux(g, 0, u32(kWinC), 5 * kEntStride);
        a[7] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u, 5u});
        for (u32 e = 0; e < 5; ++e) {
            a[8 + 4 * e] = g.P(50) ? ((g.P(70) ? 4u : g.R(8)) | (g.P(30) ? (g.R(32) << 3) : 0u)) : g.W() & 0xFFu;
            a[9 + 4 * e] = g.Pk({0u, 4u, 0x40000u, 0x40004u, g.W(), g.W() | 4u, g.W() & ~0x40004u, 0xFFFFFFFFu, 0x40000u, 4u});
            a[10 + 4 * e] = g.Pk({0u, 0x1000000u, 0x40u, 0x1000040u, g.W(), g.W() & ~0x1000040u, g.W() | 0x1000000u, 0xFFFFFFFFu, 0x1000000u, 0u});
            a[11 + 4 * e] = u32(i32(g.R(kModelSlots)) + kModelLo);
        }
        a[28] = Raw(g);
        for (u32 k = 0; k < kModelSlots; ++k) a[40 + k] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 2u, 3u, 1u});
        for (u32 i = 0; i < 6; ++i) { a[52 + i] = ByteOf(g, {3u, 3u, 3u, 3u, 0u, 1u, 2u, 4u, 7u, 0xFFu}); a[58 + i] = ByteOf(g, {0u, 1u, 0u, 1u, 2u, 0xFEu, 0xFFu}); }
        break;
    }
    case kClosest: {
        a[4] = AuxB(g, 0x10);
        a[5] = Aux(g, 0, 0x3000, 5 * kPedStride);
        a[6] = Aux(g, 0, 0x100, 8);
        a[7] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u, 5u});
        for (u32 p = 0; p < 5; ++p) a[8 + p] = ByteOf(g, {0u, 0u, 0u, 0u, 0x80u, 0xFFu, 0x7Fu});
        for (u32 p = 0; p < 5; ++p) {
            const u32 b = 13 + 11 * p;
            a[b] = u32(i32(g.R(14)) - 3);                                              // modelo: -3..10 (con signo de 16 bits)
            a[b + 1] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFEu, g.W()});        // tipo de peaton
            a[b + 2] = g.Pk({0u, 1u, 1u});
            a[b + 3] = g.Pk({0u, 0u, 1u, 2u});
            a[b + 4] = g.Pk({0xE9u, 0xE8u, 0xEAu, 0u, g.W(), 0x1F4u});
            for (u32 k = 0; k < 3; ++k) { a[b + 5 + k] = Coord(g); a[b + 8 + k] = Coord(g); }
        }
        for (u32 k = 0; k < 3; ++k) a[68 + k] = Coord(g);
        if (g.P(30)) {   // distancias muy parecidas: el orden de las sumas importa
            for (u32 p = 0; p < 5; ++p) for (u32 k = 0; k < 3; ++k) { const u32 b = 13 + 11 * p; if (g.P(60)) { a[b + 5 + k] = a[68 + k] + g.R(3) - 1u; a[b + 8 + k] = a[68 + k] + g.R(3) - 1u; } }
        }
        a[71] = g.R(kUserCount);
        for (u32 k = 0; k < 4; ++k) a[72 + k] = g.Pk({0xFFFFFFFEu, 0xFFFFFFFEu, 0xFFFFFFFFu, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, g.W()});
        a[76] = ByteOf(g, {0u, 0u, 1u, 1u, 1u, 2u, 0xFFu});
        for (u32 k = 0; k < 4; ++k) a[77 + k] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFEu, g.W()});
        a[81] = g.W();
        break;
    }
    case kCanScan:
        a[4] = AuxC(g, 0x600);
        a[5] = ByteOf(g, {2u, 2u, 2u, 0u, 1u, 3u, 0xFFu});
        a[6] = g.Pk({0u, 0x100u, 0x100u, 0xFFFFFFFFu, 0xFFFFFEFFu, g.W() | 0x100u, g.W() & ~0x100u, 0x200u});
        a[7] = AuxD(g, 0xE0);
        for (u32 k = 0; k < 3; ++k) a[8 + k] = ByteOf(g, {0u, 1u, 0u, 1u, 2u, 0xFFu});
        a[11] = aliveRet();
        a[12] = g.Pk({0u, 1u, 1u, 2u, 0x100u, 0xFFFFFFFFu, g.W(), g.W() | 1u});
        a[13] = g.Pk({0u, 1u, 1u, 1u});
        a[14] = AuxB(g, 0x40);
        a[15] = g.Pk({0x24u, 0x24u, 0x24u, 0x24u, 0x23u, 0x25u, 0u, 0x124u, g.W()});
        a[16] = g.Pk({0u, 1u, 2u, 0x100u, 0xFFFFFFFFu, g.W(), g.W()});
        a[17] = aliveRet();
        a[18] = g.Pk({0u, 1u, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, g.W() & ~0xFFu, (g.W() & ~0xFFu) | 1u, 1u, 0u});   // bool canonico en AL (los 0x600CF0 del EXE y de la DLL devuelven 0 o 1); los bytes altos de EAX son basura
        break;
    case kMedicDtor:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxB(g, 0x10);
        a[6] = g.P(40) ? 0u : RawNz(g);
        a[7] = ByteOf(g, {0u, 1u, 0xFFu});
        a[8] = ByteOf(g, {0u, 0u, 1u, 2u, 0x80u, 0xFFu});
        a[9] = g.P(40) ? 0u : RawNz(g);
        a[10] = g.P(40) ? 0u : RawNz(g);
        a[11] = Raw(g);
        break;
    case kPresentCtor:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kPresentDtor:
        a[4] = g.P(40) ? 0u : RawNz(g); a[5] = Raw(g);
        break;
    case kHeading:
        a[4] = Aux(g, 0, 0x1000, 0x40);
        a[5] = Aux(g, 0x1000, 0x2000, 0x40);
        a[6] = g.Pk({0u, 1u, 1u}); a[7] = Aux(g, 0, 0x700, 0x40);
        a[8] = g.Pk({0u, 1u, 1u}); a[9] = Aux(g, 0x700, 0xE00, 0x40);
        for (u32 k = 0; k < 12; ++k) a[10 + k] = g.P(15) ? FloatQ(g) : Coord(g);
        a[22] = FloatQ(g); a[23] = FloatQ(g);
        break;
    case kClone:
        a[4] = Raw(g);
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = AuxB(g, 0x40);
        a[7] = Raw(g);
        break;
    case kPresentSub:
        a[4] = g.Pk({0x19Du, 0x19Du, 0x386u, 0x386u, 0x516u, 0u, 1u, 0xFFFFFFFFu, 0x19Cu, 0x19Eu, 0x385u, 0x387u, 0x515u, 0x517u, 0x1019Du, 0x10386u, g.W()});
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u});
        a[7] = AuxB(g, 0x40);
        a[8] = FloatQ(g);
        a[9] = Raw(g);
        break;
    case kNext:
        a[4] = AuxC(g, 0x40);
        a[5] = g.Pk({0x19Du, 0x19Du, 0x386u, 0x386u, 0u, 1u, 0x19Cu, 0x387u, 0xFFFFFFFFu, 0x1019Du, g.W()});
        a[6] = Raw(g); a[7] = Raw(g);
        break;
    case kFirst:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kControl:
        a[4] = g.P(40) ? 0u : RawNz(g);
        a[5] = Raw(g); a[6] = Raw(g); a[7] = Raw(g);
        break;
    case kSeekPos:
        a[4] = Raw(g); a[5] = Raw(g);
        for (u32 k = 0; k < 3; ++k) a[6 + k] = g.P(50) ? g.W() : FloatBits(g);
        a[9] = AuxB(g, 0xC);
        break;
    case kStay:
        a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x400000u, 0xFFBFFFFFu, g.W(), g.W(), g.W() | 0x400000u, g.W() & ~0x400000u});
        a[5] = g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF01u, 0xFFFFFF00u, 0x80u, 0x81u, 0xFEu, 0xFFFFFFFFu, g.W(), g.W()});
        break;
    case kStream:
        a[4] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFF9989u, 0xFFFF998Au, g.R(1000), g.R(100), g.W(), g.W(), 0x8000u, 0xFFFFu, 0x10000u});
        break;
    case kMinMax:
        for (u32 k = 0; k < 6; ++k) a[4 + k] = g.P(50) ? g.W() : FloatBits(g);
        break;
    case kBodyCopy:
        a[4] = Raw(g);
        break;
    case kSelect: {
        a[4] = AuxB(g, 0x14);
        a[5] = Aux(g, 0, 0x1000, 0x40);
        a[6] = g.Pk({0u, 1u, 1u});
        a[7] = AuxD(g, 0x40);
        for (u32 k = 0; k < 8; ++k) a[8 + k] = (k < 6 && g.P(30)) ? FloatQ(g) : Coord(g);
        a[14] = g.P(30) ? FloatQ(g) : (g.P(50) ? PosNormal(g, 120, 136) : 0u);
        a[15] = g.P(30) ? FloatQ(g) : (g.P(50) ? PosNormal(g, 120, 140) : 0u);
        if (g.P(35)) {   // distancia y radios casi iguales: el cuadrado y la suma se redondean a 24 bits
            const double dx = double(std::bit_cast<float>(a[8])) - double(std::bit_cast<float>(a[12]));
            const double dy = double(std::bit_cast<float>(a[9])) - double(std::bit_cast<float>(a[13]));
            const double r = std::sqrt(dx * dx + dy * dy);
            if (std::isfinite(r)) {
                const u32 base = std::bit_cast<u32>(float(r)) + g.R(3) - 1u;
                if (g.P(50)) a[14] = base; else a[15] = base;
            }
        }
        break;
    }
    case kGoTo: {
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = Aux(g, 0, 0x400, 0x40);
        a[6] = g.Pk({0x384u, 0x384u, 0x384u, 0x385u, 0x383u, 0u, 0x2C0u, 0x1000384u, g.W()});
        a[7] = g.Pk({6u, 7u, 4u, 5u, 0u, 1u, 6u, 7u, 0xFFFFFFFFu, g.W()});
        for (u32 k = 0; k < 3; ++k) a[8 + k] = g.W();
        a[11] = g.W();
        if (IsNanBits(a[11])) a[11] |= 0x00400000u;   // radio de la subtarea: se pasa al constructor como float (la x87 silencia un NaN senalizante; el EXE lo pasa como dword)
        a[12] = g.P(40) ? FloatQ(g) : (g.P(50) ? PosNormal(g, 120, 136) : g.W());
        if (IsNanBits(a[12])) a[12] |= 0x00400000u;   // se pasa a SelectMoveState como float: la conversion de la x87 silencia un NaN senalizante (el EXE lo pasa como dword)
        a[13] = ByteOf(g, {0u, 1u, 2u, 3u, 8u, 9u, 0xAu, 0xBu, 0xFFu});
        a[14] = g.Pk({0x2C0u, 0x384u, 0x384u, 0xCAu, 0xCBu, 0xCBu, 0x516u, 0u, 0x2BFu, 0x2C1u, 0x385u, 0xC9u, 0xCCu, 0x80000384u, 0x1000384u, g.W()});
        a[15] = Aux(g, 0x1000, 0x3000, 0x600);
        a[16] = g.Pk({0u, 1u, 1u});
        a[17] = AuxD(g, 0x40);
        for (u32 k = 0; k < 3; ++k) { a[18 + k] = Coord(g); a[21 + k] = Coord(g); }
        a[24] = g.Pk({0u, 0x80000000u, 0x3C23D70Au, 0x3C23D709u, 0x3C23D70Bu, 0x3C23D70Au, FloatQ(g), FloatQ(g), PosNormal(g, 100, 130), PosNormal(g, 110, 128), PosNormal(g, 115, 127), 0x3F800000u, 0x7F800000u, 0x7FC00000u});
        a[25] = Raw(g);
        a[26] = g.P(50) ? FloatQ(g) : g.Pk({0x3C23D70Au, 0x3C23D70Bu, 0x3C23D709u, 0u, 0x3F800000u, PosNormal(g, 110, 140), PosNormal(g, 120, 135)});
        for (u32 k = 0; k < 3; ++k) a[27 + k] = g.W();
        a[30] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[31] = Aux(g, 0x400, 0x1000, 0x40);
        a[32] = g.P(30) ? FloatQ(g) : g.Pk({0x3F800000u, 0x3F000000u, 0x3E800000u, PosNormal(g, 110, 130), PosNormal(g, 120, 135), 0x41F00000u, 0x3D800000u, 0u, 0x80000000u});
        a[33] = Raw(g);
        if (g.P(45)) {   // escenario de STAND_STILL con parada exacta: subtarea GO_TO_POINT y las distintas ramas de la velocidad de mezcla
            a[14] = 0xCBu; a[13] |= 2u; a[4] = 1u; a[6] = 0x384u;
            switch (g.R(8)) {
            case 0: a[24] = g.P(50) ? PosNormal(g, 100, 121) : 0x3C23D709u; break;                                                       // shift < 0.01: 8.0
            case 1: a[26] = g.Pk({0u, 0x3C23D70Au, 0x80000000u | PosNormal(g, 110, 130), 0x3C23D709u, 0x7FC00000u | g.R(0x3FFFFF)}); a[24] = PosNormal(g, 122, 130); break;   // dot <= 0.01 o NaN: 16.0
            case 2: case 3: case 4: a[24] = PosNormal(g, 122, 127); a[26] = PosNormal(g, 123, 129); a[32] = PosNormal(g, 120, 128); break;   // calculado: unas veces recortado y otras no
            case 5: a[24] = PosNormal(g, 122, 127); a[26] = PosNormal(g, 123, 129); a[32] = PosNormal(g, 90, 118); break;                 // paso de tiempo diminuto: el cociente desborda el float
            case 6: a[24] = 0x7FC00000u | g.R(0x3FFFFF) | (g.P(50) ? 0x80000000u : 0u); a[26] = PosNormal(g, 123, 129); break;             // NaN en la velocidad: mezcla NaN
            default: a[24] = g.Pk({0x7F800000u, 0x3C23D70Au, 0x3C23D70Bu, PosNormal(g, 120, 123), 0x00000001u, 0x007FFFFFu}); a[26] = g.Pk({0x7F800000u, PosNormal(g, 123, 129), PosNormal(g, 126, 140)}); a[32] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0x7F800000u, PosNormal(g, 100, 128)}); break;
            }
        }
        if (a[14] == 0x384u) a[30] = 1u;       // con memoria nula el EXE llamaria a SelectMoveState y a `or byte [eax+20h]` con un puntero nulo: fuera del banco
        if (a[30] == 0u) a[13] &= ~8u;
        break;
    }
    case kReactCtor:
        for (u32 k = 0; k < 5; ++k) a[4 + k] = Raw(g);
        a[9] = Raw(g);
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
