#pragma once
// Fixtures del banco Helpers65C: cinco funciones pequenas del EXE que seguian como envolturas o llamadas por direccion.
//   0  Fx_c::TriggerWaterHydrant        0x4A0D70  thiscall, ECX = this (ignorado), (const CVector* pos), `ret 4`, void
//   1  Fx_c::TriggerTankFire            0x4A0FA0  thiscall, ECX = this, (const CVector* pos, const CVector* dir), `ret 8`, void
//   2  Fx_c::AddTyreBurst               0x49F300  thiscall, ECX = this (ignorado: usa el Fx_c global 0xA9AE00), (const CVector* posn, const CVector* velocity), `ret 8`, void
//   3  CProjectile::CProjectile(int32)  0x5A4030  thiscall, ECX = this, (int32 modelId), `ret 4`, devuelve this en EAX
//   4  CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB 0x63C840  cdecl, (CAnimBlendAssociation*, void* data = tarea), `ret`, void (callback de fin de animacion)
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
// Reparto: 4096 casos por funcion; familia = n % 16 (256 casos por familia y funcion).
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

namespace Helpers65CFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 5;
enum Func : unsigned { kHydrant = 0, kTankFire = 1, kTyreBurst = 2, kProjCtor = 3, kRollCb = 4 };

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float BitsF(u32 b) { return std::bit_cast<float>(b); }

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kGuard = 0x40;
// Fx (funciones 0..2): [guarda][pos 0x10][dir 0x10][matriz de la camara 0x40][matriz M 0x40][sistema falso 0x20][Fx_c falso 0x70][guarda]
constexpr u32 kFxPos = kGuard;                       // 0x040: CVector pos (12 bytes) + 4 de relleno
constexpr u32 kFxDir = kFxPos + 0x10;                // 0x050: CVector dir / velocidad
constexpr u32 kFxCamMat = kFxDir + 0x10;             // 0x060: matriz de la camara (CMatrix: la posicion esta en +0x30)
constexpr u32 kFxMatM = kFxCamMat + 0x40;            // 0x0A0: RwMatrix que devuelve el falso de FxRwMatrixCreate
constexpr u32 kFxSystem = kFxMatM + 0x40;            // 0x0E0: FxSystem_c falso (resultado de CreateFxSystem / m_SmokeII3expand)
constexpr u32 kFxThis = kFxSystem + 0x20;            // 0x100: Fx_c falso (argumento this)
constexpr u32 kFxThisBytes = 0x70;
constexpr u32 kFxEnd = kFxThis + kFxThisBytes;       // 0x170
constexpr u32 kFxArena = kFxEnd + kGuard;            // 0x1B0
static_assert(kFxArena == 0x1B0, "arena Fx: 0x40 + 0x10 + 0x10 + 0x40 + 0x40 + 0x20 + 0x70 + 0x40");
constexpr u32 kCamPlacement = 0xB6F02C;              // TheCamera.m_placement.m_vPosn (CVector)
constexpr u32 kCamMatrixPtr = 0xB6F03C;              // TheCamera.m_matrix (CMatrixLink*)
constexpr u32 kSmokeSlot = 0xA9AE24;                 // g_fx.m_SmokeII3expand
constexpr u32 kFxManAddr = 0xA9AE80;                 // g_fxMan (ECX de los colaboradores de FxManager_c)

// CProjectile (funcion 3): [guarda 0x40][relleno 0x10][objeto 0x17C][guarda 0x40]; el objeto esta en uno de cuatro desplazamientos
constexpr u32 kObjBytes = 0x17C;                     // sizeof(CProjectile) = sizeof(CObject)
constexpr u32 kPhysBytes = 0x138;                    // lo que escribe el falso del constructor base (+ vptr marcador en +0)
constexpr u32 kPrjArena = kGuard + 0x10 + kObjBytes + kGuard;
static_assert(kPrjArena == 0x20C, "arena CProjectile: 0x40 + 0x10 + 0x17C + 0x40");
constexpr u32 kObjOffsets[4]{0x40, 0x44, 0x48, 0x4C};
constexpr u32 kBaseVptrMarker = 0xBA5E0000u;         // lo que deja el falso del constructor base en +0
constexpr u32 kVtLabel = 0xC0B1EC70u;                // etiqueta del vptr propio de cada lado (0x867030 en el EXE, la vtable de la DLL en el candidato)
constexpr u32 kExeVptr = 0x867030u;
constexpr u32 kOffFlags1C = 0x1C, kOffFlags40 = 0x40, kOffMass = 0x8C, kOffTurnMass = 0x90, kOffAirRes = 0x98, kOffElasticity = 0x9C, kOffBuoyancy = 0xA0, kOffObjType = 0x13C, kOffModel = 0x22, kOffMark = 0x24;

// Callback (funcion 4): [guarda][tarea 0x60][relleno 0x20][3 x handling 0x100][3 x vehiculo 0x470][vtable 64 dwords][guarda]
constexpr u32 kTaskOff = kGuard;                     // 0x040
constexpr u32 kTaskBytes = 0x60;                     // sizeof(CTaskSimpleCarDrive)
constexpr u32 kHandOff = kTaskOff + kTaskBytes + 0x20; // 0x0C0
constexpr u32 kHandBytes = 0x100;
constexpr u32 kVehOff = kHandOff + 3 * kHandBytes;   // 0x3C0
constexpr u32 kVehBytes = 0x470;
constexpr u32 kVtabOff = kVehOff + 3 * kVehBytes;    // 0x1110
constexpr u32 kVtabSlots = 64;
constexpr u32 kRollArena = kVtabOff + kVtabSlots * 4 + kGuard;
static_assert(kRollArena == 0x1250, "arena callback: 0x40 + 0x60 + 0x20 + 3*0x100 + 3*0x470 + 0x100 + 0x40");
constexpr u32 kTaskVehicle = 0x08, kTaskAnim = 0x0C, kTaskFlags = 0x5C;
constexpr u32 kVehHandling = 0x384, kVehDriver = 0x460, kHandAnimGroup = 0xDE;
constexpr u32 kGroupTable = 0xC1CDC0, kGroupStride = 0x94;
constexpr u32 kSlotOpenDoor = 28;                    // [edx+0x70] / 4

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7F800001u, 0x7FA00000u, 0xFFA00000u, 0xFF800001u};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
inline u32 RandSign(u32 c, u32 k) { return Rand(c, k) & 0x80000000u; }
// Componente "con mezcla": NaN, infinito, ceros, subnormal, enorme, especial o normal.
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 8) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Rand(c, k + 100) & 0x80000000u;
    case 3: return Rand(c, k + 100) & 0x807FFFFFu;
    case 4: return RandFloat(c, k + 100, 60, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    case 6: return RandFloat(c, k + 100, -3, 6);
    default: return RandFloat(c, k + 100, -20, -3);
    }
}
// Un float normal de magnitud moderada, con signo aleatorio.
inline u32 Moderate(u32 c, u32 k, int lo, int hi) { return RandFloat(c, k, lo, hi); }
// Mueve un float finito `k` unidades en la ultima posicion (en el orden de los bits de magnitud), conservando el signo.
inline u32 Ulps(u32 bits, int k) {
    const u32 sign = bits & 0x80000000u;
    i32 mag = i32(bits & 0x7FFFFFFFu) + k;
    if (mag < 0) mag = 0;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return sign | u32(mag);
}

// ------------------------------------------------------------------------------------------------ Fx (funciones 0..2)
struct FxCase {
    u32 n, func, family;
    u32 camMode;          // 0: TheCamera.m_matrix == nullptr (se usa 0xB6F02C); 1: m_matrix apunta a la matriz de la arena (se usa matriz+0x30)
    u32 cam[3];           // posicion de camara USADA (en el modo 1, matriz+0x30; en el 0, 0xB6F02C)
    u32 other[3];         // posicion de camara NO usada (en el modo 1, 0xB6F02C; en el 0, matriz+0x30 que nadie lee)
    u32 pos[3], dir[3];   // argumentos (posn/pos y dir/velocity); dir son 32 bits cualesquiera (nadie los interpreta como float)
    u32 createKind;       // 0: CreateFxSystem devuelve nullptr; 1: devuelve el sistema falso de la arena
    u32 thisKind;         // 0: this = Fx_c falso de la arena; 1: this = 0xA9AE00 (g_fx); 2: this = ruido
    u32 mseed;            // semilla del patron que escribe el falso de CreateMatFromVec en la matriz M
    u32 mode, seedArena;  // relleno previo de la arena
    u32 eax, edx, stack;
};

// Reparte `d` (longitud ~ len) en tres componentes con signo.
inline void Split3(u32 c, u32 k, double len, double out[3]) {
    double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0;
    double b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
    double d = double(Rand(c, k + 2) % 2001) / 1000.0 - 1.0;
    double m = a * a + b * b + d * d;
    if (m < 1e-6) { a = 1.0; b = 0.0; d = 0.0; m = 1.0; }
    const double s = len / std::sqrt(m);
    out[0] = a * s; out[1] = b * s; out[2] = d * s;
}

inline FxCase MakeFx(unsigned func, unsigned n) {
    FxCase f{};
    f.n = n; f.func = func; f.family = n % kFamilies;
    const u32 c = n * 8u + func * 0x10001u;                       // semilla distinta por funcion
    f.camMode = Rand(c, 1) % 5 < 3 ? 0u : 1u;
    f.createKind = Rand(c, 2) % 3 == 0 ? 0u : 1u;
    f.thisKind = Rand(c, 3) % 4 == 0 ? 2u : Rand(c, 3) % 4 == 1 ? 1u : 0u;
    f.mseed = Rand(c, 4);
    f.mode = Rand(c, 5) % 6;
    f.seedArena = Rand(c, 6) | 1u;
    f.eax = Rand(c, 7) % 5 == 0 ? 0u : Rand(c, 8);
    f.edx = Rand(c, 9) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 10);
    f.stack = StackSeed(Rand(c, 11) % 12);
    for (int i = 0; i < 3; ++i) { f.other[i] = MixComponent(c, 20 + i); f.dir[i] = Rand(c, 30 + i); }
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 41 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) {          // pos = cam - d (redondeado a float), para que cam - pos ~ d
        for (int i = 0; i < 3; ++i) f.pos[i] = FBits(float(double(BitsF(f.cam[i])) - d[i]));
    };
    double d[3];
    switch (f.family) {
    case 0:                                                          // dentro de rango, cerca de la camara
        camNormal(11);
        Split3(c, 50, double(Rand(c, 53) % 2000) / 100.0 + 0.0, d);
        fromDelta(d);
        break;
    case 1:                                                          // alrededor del limite 25 (suma ~ 625), +-3 ulps en un componente
        camNormal(8);
        Split3(c, 50, 25.0 * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-6), d);
        fromDelta(d);
        f.pos[Rand(c, 54) % 3] = Ulps(f.pos[Rand(c, 54) % 3], int(Rand(c, 55) % 7) - 3);
        break;
    case 2: {                                                        // suma exactamente 625 (enteros), con signos y permutaciones; y +-1 ulp sobre un eje
        static constexpr int quads[][3] = {{25, 0, 0}, {0, 7, 24}, {0, 15, 20}, {12, 15, 16}, {7, 24, 0}, {15, 20, 0}, {20, 0, 15}, {24, 0, 7}, {0, 20, 15}, {16, 12, 15}};
        const auto& q = quads[Rand(c, 50) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 51) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 52 + i) & 1) ? -1.0 : 1.0);
            f.cam[i] = FBits(float(double(int(Rand(c, 60 + i) % 2001) - 1000)));
            f.pos[i] = FBits(float(double(BitsF(f.cam[i])) - v));
        }
        if (Rand(c, 56) % 3 == 0) f.pos[Rand(c, 57) % 3] = Ulps(f.pos[Rand(c, 57) % 3], Rand(c, 58) % 2 ? 1 : -1);
        break;
    }
    case 3:                                                          // lejos (1e3 .. 1e12)
        camNormal(11);
        for (int i = 0; i < 3; ++i) f.pos[i] = RandFloat(c, 70 + i, 10, 40);
        break;
    case 4:                                                          // enormes: los cuadrados (y a veces la resta) desbordan float
        for (int i = 0; i < 3; ++i) {
            f.cam[i] = RandFloat(c, 41 + i, 60, 127);
            f.pos[i] = Rand(c, 70 + i) % 3 == 0 ? Neg(f.cam[i]) : RandFloat(c, 70 + i, 60, 127);
        }
        break;
    case 5:                                                          // NaN en camara o en la posicion (incluye sNaN)
        camNormal(8);
        Split3(c, 50, 10.0, d);
        fromDelta(d);
        for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) {
            if (Rand(c, 57 + j) & 1) f.cam[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j);
            else f.pos[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j);
        }
        break;
    case 6:                                                          // infinitos (inf - inf = NaN cuando coinciden en signo)
        camNormal(8);
        Split3(c, 50, 10.0, d);
        fromDelta(d);
        for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
            const u32 inf = Pick(kInf, c, 62 + j);
            const unsigned axis = Rand(c, 60 + j) % 3;
            const u32 mode = Rand(c, 66 + j) % 4;
            if (mode == 0) f.cam[axis] = inf;
            else if (mode == 1) f.pos[axis] = inf;
            else if (mode == 2) { f.cam[axis] = inf; f.pos[axis] = inf; }
            else { f.cam[axis] = inf; f.pos[axis] = Neg(inf); }
        }
        break;
    case 7:                                                          // ceros, -0 y subnormales
        for (int i = 0; i < 3; ++i) {
            f.cam[i] = Pick(std::array<u32, 6>{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu}, c, 41 + i);
            f.pos[i] = Pick(std::array<u32, 6>{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu}, c, 70 + i);
        }
        break;
    case 8:                                                          // diferencias diminutas (cuadrados que underflowan float)
        camNormal(2);
        for (int i = 0; i < 3; ++i) f.pos[i] = Rand(c, 70 + i) % 3 == 0 ? f.cam[i] : Ulps(f.cam[i], int(Rand(c, 73 + i) % 9) - 4);
        break;
    case 9:                                                          // coordenadas grandes con diferencia pequena o moderada (la resta pierde bits)
        for (int i = 0; i < 3; ++i) {
            f.cam[i] = RandFloat(c, 41 + i, 14, 30);
            f.pos[i] = Ulps(f.cam[i], int(Rand(c, 73 + i) % 41) - 20);
        }
        break;
    case 10: {                                                       // un solo eje cerca del limite (25), los otros dos iguales
        camNormal(9);
        for (int i = 0; i < 3; ++i) f.pos[i] = f.cam[i];
        const unsigned axis = Rand(c, 50) % 3;
        const double v = 25.0 * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-7) * ((Rand(c, 54) & 1) ? -1.0 : 1.0);
        f.pos[axis] = FBits(float(double(BitsF(f.cam[axis])) - v));
        f.pos[axis] = Ulps(f.pos[axis], int(Rand(c, 55) % 5) - 2);
        break;
    }
    default:                                                         // 11..15: mezclas
        camNormal(7);
        for (int i = 0; i < 3; ++i) {
            f.pos[i] = Rand(c, 70 + i) % 4 == 0 ? MixComponent(c, 75 + i) : FBits(float(double(BitsF(f.cam[i])) + (double(Rand(c, 80 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 85 + i) % 7 == 0) f.cam[i] = MixComponent(c, 88 + i);
        }
        break;
    }
    return f;
}
// En los casos de mezcla, camaras y posiciones normales salen a menudo a 0 o cerca del limite: el reparto real se mide con fixture_selfcheck.py.

// ------------------------------------------------------------------------------------------------ CProjectile (funcion 3)
inline constexpr u32 kSpecialModels[]{
    0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFu, 0x10000u, 0x7FFFu, 0x8000u, 0xFFFF8000u, 0xFFFF7FFFu, 19999u, 20000u, 0xFFFFFF00u, 0xDEADBEEFu,
    0xFFFF0000u, 0x0000FF00u, 0x000000FFu, 0x100u, 0x80u, 0x7Fu, 0x8000FFFFu, 0xFFFFFFFEu, 0x80000001u, 0x7FFFFFFEu, 342u, 345u, 1337u, 0x00FFFFFFu, 0x01000000u};
struct PrjCase {
    u32 n, func, family, modelKind, mode, objOff, seedArena, seedP, modelId, eax, edx, stack;
};
inline PrjCase MakePrj(unsigned n) {
    PrjCase f{};
    const u32 c = n * 8u + kProjCtor * 0x10001u;
    f.n = n; f.func = kProjCtor; f.family = n % kFamilies;
    f.modelKind = Rand(c, 1) % 8;
    switch (f.modelKind) {
    case 0: f.modelId = Rand(c, 11) % 41u; break;
    case 1: case 2: f.modelId = Rand(c, 11) % 20000u; break;
    case 3: case 4: f.modelId = kSpecialModels[Rand(c, 11) % std::size(kSpecialModels)]; break;
    case 5: f.modelId = Rand(c, 11); break;
    case 6: f.modelId = u32(-i32(1 + Rand(c, 11) % 40000u)); break;
    default: f.modelId = Rand(c, 11) & 0xFFFFu; break;
    }
    f.mode = Rand(c, 3) % 6;
    f.objOff = kObjOffsets[Rand(c, 4) % 4];
    f.seedArena = Rand(c, 5) | 1u;
    f.seedP = Rand(c, 6);
    f.eax = Rand(c, 8) % 5 == 0 ? 0u : Rand(c, 9);
    f.edx = Rand(c, 10) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 13);
    f.stack = StackSeed(Rand(c, 14) % 12);
    return f;
}
// Relleno previo de la arena: 0 y 5 ruido, 1 ceros, 2 0xFF, 3 0xCD, 4 0xA5
inline void FillArena(u8* arena, u32 size, u32 mode, u32 seed) {
    switch (mode) {
    case 1: std::memset(arena, 0x00, size); break;
    case 2: std::memset(arena, 0xFF, size); break;
    case 3: std::memset(arena, 0xCD, size); break;
    case 4: std::memset(arena, 0xA5, size); break;
    default: FillWords(arena, size, seed); break;
    }
}

// ------------------------------------------------------------------------------------------------ callback (funcion 4)
struct RollCase {
    u32 n, func, family;
    u32 mode, seedArena;
    u32 taskVeh;            // vehiculo inicial de la tarea: 0..2 = V0..V2, 3 = nullptr
    u32 flags;              // byte +0x5C de la tarea
    u32 driver[3];          // conductor de cada vehiculo (0 = sin conductor)
    u32 handling[3];        // handling (0..2) de cada vehiculo
    u32 animGroup[3];       // byte +0xDE de cada handling
    u32 mutA, idxA;         // el falso de ClearGettingOutFlags re-apunta la tarea a V[idxA]
    u32 mutB, idxB;         // el falso de GetGroup re-apunta la tarea a V[idxB]
    u32 mutC, idxC, newDriver;   // el falso de GetGroup cambia el conductor de V[idxC] a newDriver
    u32 groupRet;           // lo que devuelve GetGroup
    u32 assoc;              // primer argumento (CAnimBlendAssociation*), ignorado
    u32 eax, ecx, edx, stack;
};
inline RollCase MakeRoll(unsigned n) {
    RollCase f{};
    const u32 c = n * 8u + kRollCb * 0x10001u;
    f.n = n; f.func = kRollCb; f.family = n % kFamilies;
    f.mode = Rand(c, 1) % 6;
    f.seedArena = Rand(c, 2) | 1u;
    // familias: 0..1 vehiculo nulo | 2..5 sin conductor | 6..9 con conductor sin mutaciones | 10..15 con mutaciones
    const u32 fam = f.family;
    f.taskVeh = fam <= 1 ? 3u : Rand(c, 3) % 3;
    f.flags = Rand(c, 4) % 4 == 0 ? 0xFFu : Rand(c, 4) % 4 == 1 ? 0x20u : Rand(c, 4) % 4 == 2 ? 0u : (Rand(c, 5) & 0xFFu);
    for (int i = 0; i < 3; ++i) {
        const u32 r = Rand(c, 10 + i);
        f.driver[i] = (fam >= 2 && fam <= 5) ? 0u : fam <= 1 ? (r & 1 ? 0u : (Rand(c, 13 + i) | 1u)) : (fam >= 6 && fam <= 9) ? (Rand(c, 13 + i) | 1u) : (r % 4 == 0 ? 0u : (Rand(c, 13 + i) | 1u));
        f.handling[i] = Rand(c, 16 + i) % 3;
        f.animGroup[i] = Rand(c, 19 + i) % 6 == 0 ? 0xFFu : Rand(c, 19 + i) % 6 == 1 ? 0u : (Rand(c, 22 + i) & 0xFFu);
    }
    const bool mut = fam >= 10;
    f.mutA = mut && Rand(c, 30) % 2; f.idxA = Rand(c, 31) % 3;
    f.mutB = mut && Rand(c, 32) % 2; f.idxB = Rand(c, 33) % 3;
    f.mutC = mut && Rand(c, 34) % 2; f.idxC = Rand(c, 35) % 3; f.newDriver = Rand(c, 36) % 3 == 0 ? 0u : (Rand(c, 37) | 1u);
    f.groupRet = Rand(c, 38) % 4 == 0 ? Rand(c, 39) % 200u : Rand(c, 39);
    f.assoc = Rand(c, 40);
    f.eax = Rand(c, 41) % 5 == 0 ? 0u : Rand(c, 42);
    f.ecx = Rand(c, 43);
    f.edx = Rand(c, 44) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 45);
    f.stack = StackSeed(Rand(c, 46) % 12);
    return f;
}
}
