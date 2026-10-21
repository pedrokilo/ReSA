#pragma once
// Fixtures del banco Helpers66D: cuatro funciones que seguian como envolturas al EXE (lote 66).
//   0  RpClumpGetBoundingSphere      0x734FC0  cdecl (RpClump* clump, RwSphere* sphere, bool bUseLTM como dword), `ret`, devuelve el clump (o 0) en EAX
//   1  CGarage::FindDoorsWithGarage  0x449FF0  thiscall, ECX = garage, (CObject** primera, CObject** segunda), `ret 8`, void (busca las dos puertas mas cercanas a una esquina del garaje)
//   2  CCam::GetArrestCamCopSourceSide 0x516010 thiscall, ECX = CCam (sin usar), (CEntity* target, CPed* cop, const CVector* dest, CVector* source), `ret 10h`, devuelve bool en AL
//   3  CHeli::CHeli(int32 modelIndex, eVehicleCreatedBy createdBy) 0x6C4190 thiscall, ECX = this, `ret 8`, devuelve this en EAX
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

namespace H66Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;
enum Func : unsigned { kClump = 0, kGarage = 1, kCop = 2, kHeli = 3 };

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

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
// Solo NaN silenciosos, con carga distinta de cero y significandos distintos entre si (la x87 elige el de mayor significando cuando los dos operandos son NaN: sin empates de distinto signo).
inline constexpr std::array<u32, 8> kNan{0x7FC00001u, 0xFFC00002u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFEu, 0x7FC00007u, 0xFFC0F00Du};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
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
// Mueve un float finito `k` unidades en la ultima posicion, conservando el signo.
inline u32 Ulps(u32 bits, int k) {
    const u32 sign = bits & 0x80000000u;
    i32 mag = i32(bits & 0x7FFFFFFFu) + k;
    if (mag < 0) mag = 0;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return sign | u32(mag);
}
// Un float "bien portado" (moderado) con signo aleatorio.
inline u32 Calm(u32 c, u32 k, int lo = -6, int hi = 7) { return RandFloat(c, k, lo, hi); }

constexpr u32 kGuard = 0x40;

// ------------------------------------------------------------------------------------------------ 0: RpClumpGetBoundingSphere
// Arena: [guarda 0x40][clump 0x20][marco 0x60 (matriz local en +0x10)][LTM 0x40][esfera 0x10][relleno 0x10][guarda 0x40]
constexpr u32 kClumpOff = kGuard;                       // 0x040: RpClump falso (solo se lee +4 = marco padre)
constexpr u32 kClumpBytes = 0x20;
constexpr u32 kFrameOff = kClumpOff + kClumpBytes;      // 0x060: RwFrame falso (matriz local en +0x10)
constexpr u32 kFrameBytes = 0x60;
constexpr u32 kLtmOff = kFrameOff + kFrameBytes;        // 0x0C0: matriz que devuelve el falso de RwFrameGetLTM
constexpr u32 kSphereOff = kLtmOff + 0x40;              // 0x100: RwSphere de salida
constexpr u32 kSphereEnd = kSphereOff + 0x10;           // 0x110
constexpr u32 kClumpArena = kSphereEnd + 0x10 + kGuard; // 0x160
static_assert(kClumpArena == 0x160, "arena RpClumpGetBoundingSphere: 0x40 + 0x20 + 0x60 + 0x40 + 0x10 + 0x10 + 0x40");
constexpr u32 kGarbageFrame = 0xBADF00Du;               // marco padre que el banco solo usa como etiqueta (con la LTM del falso)

struct ClumpCase {
    u32 n, family;
    u32 clumpKind;               // 0 nulo, 1 arena
    u32 sphereKind;              // 0 nula, 1 arena
    u32 useLtm;                  // tercer argumento (dword completo; el EXE guarda su byte bajo en 0x8D60BC)
    i32 count;                   // lo que devuelve el falso de RpClumpGetNumAtomics
    u32 sum[3];                  // lo que escribe el falso del callback 0x734970 (suma de centros)
    u32 radMode, rad;            // el falso del callback 0x734ED0 escribe el radio (modo 1) o lo deja como esta (0)
    u32 cmodMode, cmod[3];       // y, en el modo 1, tambien el centro
    u32 parentKind;              // 0 marco de la arena, 1 nulo, 2 basura (solo con la LTM del falso)
    u32 invMask, trMask[3];      // mascaras que aplican los falsos de RwMatrixInvert y RwV3dTransformPoints
    u32 mode, seedArena;
    u32 eax, ecx, edx, stack;
};
inline constexpr std::array<u32, 12> kUseLtm{0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0x80u, 0x7Fu, 0xFFFFFFFFu, 0x01000000u, 0x00FF0000u};
inline ClumpCase MakeClump(unsigned n) {
    ClumpCase f{};
    const u32 c = n * 8u + kClump * 0x10001u;
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    f.clumpKind = fam == 0 ? (Rand(c, 1) % 4 == 0 ? 1u : 0u) : 1u;
    f.sphereKind = fam == 1 ? 0u : (fam == 0 && Rand(c, 2) % 2) ? 0u : 1u;
    f.useLtm = Rand(c, 3) % 2 ? kUseLtm[Rand(c, 4) % kUseLtm.size()] : (Rand(c, 5) % 2 ? 0u : 1u);
    if (fam == 8 || fam == 9) f.useLtm = Rand(c, 6) % 3 == 0 ? Rand(c, 7) : kUseLtm[Rand(c, 8) % kUseLtm.size()];
    // numero de atomos
    switch (fam) {
    case 2: {
        static constexpr i32 neg[]{0, -1, -2, i32(0x80000000u), i32(0x80000001u), -1000, -16777216, -16777217};
        f.count = Rand(c, 10) % 3 == 0 ? i32(Rand(c, 11) | 0x80000000u) : neg[Rand(c, 12) % std::size(neg)];
        if (Rand(c, 13) % 5 == 0) f.count = 0;
        break;
    }
    case 4: {
        static constexpr i32 big[]{16777216, 16777217, 16777219, 16777220, 0x7FFFFFFF, 0x7FFFFFFE, 1 << 30, 33554433, 33554435, 0x01000001, 0x40000001, 0x3FFFFFFF, 1 << 24, 0x00FFFFFF};
        f.count = Rand(c, 10) % 2 ? big[Rand(c, 11) % std::size(big)] : i32(Rand(c, 12) & 0x7FFFFFFFu);
        break;
    }
    default:
        f.count = Rand(c, 10) % 8 == 0 ? i32(1 + Rand(c, 11) % 100) : i32(1 + Rand(c, 11) % 12);
        if (Rand(c, 14) % 40 == 0) f.count = i32(Rand(c, 15) | 0x80000000u);
        break;
    }
    // suma de los centros
    for (int i = 0; i < 3; ++i) {
        switch (fam) {
        case 5: f.sum[i] = Rand(c, 20 + i) % 3 == 0 ? (Rand(c, 30 + i) & 0x807FFFFFu) : RandFloat(c, 40 + i, -130, -110); break;
        case 6: f.sum[i] = RandFloat(c, 20 + i, 100, 127); break;
        case 7: f.sum[i] = MixComponent(c, 20 + i); break;
        case 3: f.sum[i] = Calm(c, 20 + i, -8, 9); break;
        default: f.sum[i] = Rand(c, 25 + i) % 5 == 0 ? MixComponent(c, 20 + i) : Calm(c, 20 + i, -20, 20); break;
        }
    }
    if (fam == 5 && Rand(c, 50) % 2) f.count = i32(1 + Rand(c, 51) % 40);
    if (fam == 6 && Rand(c, 50) % 2) f.count = Rand(c, 52) % 2 ? 1 : 2;
    f.radMode = fam == 9 ? Rand(c, 60) % 3 : (Rand(c, 61) % 3 != 0 ? 1u : 0u);
    f.rad = fam == 9 ? MixComponent(c, 62) : (Rand(c, 63) % 6 == 0 ? MixComponent(c, 62) : Calm(c, 62, -3, 12) & 0x7FFFFFFFu);
    f.cmodMode = fam == 10 ? 1u : (Rand(c, 64) % 16 == 0 ? 1u : 0u);
    for (int i = 0; i < 3; ++i) f.cmod[i] = MixComponent(c, 65 + i);
    f.parentKind = 0;
    if (fam == 11 && (f.useLtm & 0xFF) != 0) f.parentKind = 1 + Rand(c, 70) % 2;
    if (fam == 12 && (f.useLtm & 0xFF) != 0 && Rand(c, 71) % 3 == 0) f.parentKind = 1 + Rand(c, 72) % 2;
    f.invMask = Rand(c, 73);
    for (int i = 0; i < 3; ++i) f.trMask[i] = Rand(c, 74 + i) % 4 == 0 ? 0u : Rand(c, 78 + i);
    f.mode = Rand(c, 80) % 6;
    f.seedArena = Rand(c, 81) | 1u;
    f.eax = Rand(c, 82) % 5 == 0 ? 0u : Rand(c, 83);
    f.ecx = Rand(c, 84);
    f.edx = Rand(c, 85) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 86);
    f.stack = StackSeed(Rand(c, 87) % 12);
    return f;
}

// ------------------------------------------------------------------------------------------------ 1: CGarage::FindDoorsWithGarage
// Arena: [guarda][primera, segunda, relleno 0x10][garaje 0x40 (en 4 desplazamientos 0x50..0x5C)][pool 0x10][banderas 0x10][matrices 12 x 0x40][objetos 12 x 0x19C][guarda]
constexpr u32 kMaxSlots = 12;
constexpr u32 kObjBytes = 0x19C;
constexpr u32 kDoorsOff = kGuard;                       // 0x040: *primera (+0), *segunda (+4)
constexpr u32 kGarageOff = kDoorsOff + 0x10;            // 0x050 (hasta 4 desplazamientos de 4 bytes)
constexpr u32 kGarageBytes = 0x40;
constexpr u32 kPoolOff = kGarageOff + kGarageBytes;     // 0x090: {objetos, banderas, tamano}
constexpr u32 kFlagsOff = kPoolOff + 0x10;              // 0x0A0
constexpr u32 kMatOff = kFlagsOff + 0x10;               // 0x0B0
constexpr u32 kObjOff = kMatOff + kMaxSlots * 0x40;     // 0x3B0
constexpr u32 kGarageEnd = kObjOff + kMaxSlots * kObjBytes;   // 0x1700
constexpr u32 kGarageArena = kGarageEnd + kGuard;       // 0x1740
static_assert(kGarageArena == 0x1740, "arena CGarage: 0x40 + 0x10 + 0x40 + 0x10 + 0x10 + 12*0x40 + 12*0x19C + 0x40");
constexpr u32 kObjByte = 0x147;                         // CObject +0x147: indice del garaje al que pertenece la puerta
constexpr u32 kGarageReadBytes = 0x28;                  // lo que el EXE lee del garaje (x, y, dirA, dirB, ancho, alto)
constexpr u32 kObjectPool = 0xB7449C;                   // CPools::ms_pObjectPool (puntero)
constexpr u32 kGarageBase = 0x96C048;                   // CGarages::aGarages (elementos de sizeof(CGarage) = 0xD8 bytes)
constexpr u32 kRdataGarageAddr[6]{0x858624u, 0x858C80u, 0x85999Cu, 0x859998u, 0x8CC7F0u, 0x86A6ACu};   // datos constantes del EXE leidos como garaje (el oraculo recibe los 0x28 bytes)

struct DoorSlot {
    u32 occupied;     // 0: ranura libre (bit 7 de la bandera), 1: ocupada
    u32 flagByte;
    u32 matKind;      // 0: sin matriz (posicion en +4), 1: matriz de la arena (posicion en +0x30)
    u32 pos[3];       // posicion que se usa
    u32 other[3];     // la que no se usa
    u32 match;        // 1: byte +0x147 igual al indice del garaje; 0: distinto
    u32 delta;        // 1..255: desplazamiento del byte cuando no coincide
};
struct GarageCase {
    u32 n, family;
    u32 garageKind;   // 0: garaje de la arena, 1: bytes de datos constantes del EXE (rdata)
    u32 garageDelta;  // desplazamiento del garaje en la arena (0, 4, 8, 12)
    u32 rdata;        // direccion de datos constantes (garageKind 1)
    u32 g[10];        // x, y, z, dirAx, dirAy, dirBx, dirBy, topZ, ancho, alto (garaje de la arena)
    u32 size;         // tamano del pool (0..12)
    u32 baseMode;     // 0: base de objetos real; 1: la ranura `nullSlot` cae en la direccion 0
    u32 nullSlot;
    DoorSlot s[kMaxSlots];
    u32 firstPre, secondPre;
    u32 mode, seedArena;
    u32 eax, ecx, edx, stack;
};
inline GarageCase MakeGarage(unsigned n) {
    GarageCase f{};
    const u32 c = n * 8u + kGarage * 0x10001u;
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    f.garageKind = fam == 12 ? 1u : 0u;
    f.garageDelta = 4 * (Rand(c, 1) % 4);
    f.rdata = kRdataGarageAddr[Rand(c, 2) % 6];
    // geometria del garaje
    for (int i = 0; i < 10; ++i) f.g[i] = Calm(c, 10 + i, -4, 9);
    f.g[3] = Rand(c, 30) % 3 ? FBits(float(int(Rand(c, 31) % 3) - 1)) : Calm(c, 32, -2, 1);                // direcciones tipicas (-1, 0, 1)
    f.g[4] = Rand(c, 33) % 3 ? FBits(float(int(Rand(c, 34) % 3) - 1)) : Calm(c, 35, -2, 1);
    f.g[5] = Rand(c, 36) % 3 ? FBits(float(int(Rand(c, 37) % 3) - 1)) : Calm(c, 38, -2, 1);
    f.g[6] = Rand(c, 39) % 3 ? FBits(float(int(Rand(c, 40) % 3) - 1)) : Calm(c, 41, -2, 1);
    f.g[8] = Calm(c, 42, 0, 5) & 0x7FFFFFFFu;
    f.g[9] = Calm(c, 43, 0, 5) & 0x7FFFFFFFu;
    if (fam == 8) { for (int j = 0; j < 1 + int(Rand(c, 44) % 3); ++j) f.g[Rand(c, 45 + j) % 10] = Pick(kNan, c, 50 + j); }
    if (fam == 10) {
        for (int i = 0; i < 10; ++i) f.g[i] = Rand(c, 60 + i) % 2 ? RandFloat(c, 70 + i, 60, 127) : (Rand(c, 80 + i) % 2 ? (Rand(c, 90 + i) & 0x807FFFFFu) : RandFloat(c, 100 + i, -130, -100));
    }
    if (fam == 9 && Rand(c, 46) % 2) f.g[Rand(c, 47) % 10] = Pick(kInf, c, 48);
    // tamano del pool y ocupacion
    f.size = fam == 0 ? 0u : (fam == 1 ? 1 + Rand(c, 110) % 12 : Rand(c, 110) % 13);
    if (fam == 2) f.size = 1 + Rand(c, 111) % 12;
    f.baseMode = fam == 11 ? 1u : 0u;
    f.nullSlot = f.size ? Rand(c, 112) % f.size : 0u;
    // esquina aproximada (doble), para colocar las puertas cerca
    const double hw = double(BitsF(f.g[8])) * 0.5, hh = double(BitsF(f.g[9])) * 0.5;
    const double cx = double(BitsF(f.g[0])) + double(BitsF(f.g[3])) * hw + double(BitsF(f.g[5])) * hh;
    const double cy = double(BitsF(f.g[1])) + double(BitsF(f.g[4])) * hw + double(BitsF(f.g[6])) * hh;
    const unsigned matches = fam == 1 ? 0u : fam == 2 ? 1u : fam == 3 ? 2u : 3u + Rand(c, 113) % 6;
    unsigned placed = 0;
    for (u32 j = 0; j < kMaxSlots; ++j) {
        DoorSlot& s = f.s[j];
        const u32 r = Rand(c, 120 + j);
        s.occupied = j < f.size && (fam == 11 ? j == f.nullSlot : (fam == 13 ? r % 3 != 0 : r % 5 != 0));
        s.flagByte = s.occupied ? (Rand(c, 130 + j) & 0x7Fu) : (0x80u | (Rand(c, 130 + j) & 0x7Fu));
        s.matKind = Rand(c, 140 + j) % 2;
        s.delta = 1 + Rand(c, 150 + j) % 255;
        const bool wantMatch = fam == 5 || fam == 6 ? true : (fam == 1 ? false : (placed < matches));
        s.match = s.occupied ? ((wantMatch || Rand(c, 160 + j) % 4 == 0) ? 1u : 0u) : Rand(c, 165 + j) % 2;   // las ranuras libres tambien llevan el byte (50 %): solo la bandera debe saltarlas
        if (fam == 1 && s.occupied) s.match = 0;
        if (s.occupied && s.match) ++placed;
        for (int i = 0; i < 3; ++i) s.other[i] = MixComponent(c, 170 + 3 * j + i);
        const double radius = double(1 + Rand(c, 220 + j) % 40);
        const double ang = double(Rand(c, 230 + j) % 6283) / 1000.0;
        double px = cx + radius * std::cos(ang), py = cy + radius * std::sin(ang);
        s.pos[0] = FBits(float(px)); s.pos[1] = FBits(float(py)); s.pos[2] = Calm(c, 240 + j);
        switch (fam) {
        case 5: s.pos[0] = FBits(float(cx + double(j + 1) * 3.0)); s.pos[1] = FBits(float(cy)); break;                                  // distancias distintas y crecientes con j
        case 6: {                                                                                                                       // empates: pocas posiciones distintas
            const u32 q = Rand(c, 250 + j) % 3;
            s.pos[0] = FBits(float(cx + double(q + 1) * 2.0)); s.pos[1] = FBits(float(cy - double(q))); break;
        }
        case 7: if (Rand(c, 260 + j) % 3 == 0) s.pos[Rand(c, 261 + j) % 2] = Pick(kNan, c, 262 + j); break;
        case 9: if (Rand(c, 260 + j) % 3 == 0) s.pos[Rand(c, 261 + j) % 2] = Pick(kInf, c, 262 + j); break;
        case 10: s.pos[0] = Rand(c, 270 + j) % 2 ? RandFloat(c, 271 + j, 60, 127) : (Rand(c, 272 + j) & 0x807FFFFFu); s.pos[1] = Rand(c, 273 + j) % 2 ? RandFloat(c, 274 + j, 60, 127) : (Rand(c, 275 + j) & 0x807FFFFFu); break;
        case 14: s.pos[0] = FBits(float(cx + 1e7 * (1 + Rand(c, 280 + j) % 5))); s.pos[1] = FBits(float(cy)); break;                      // muy lejos (> 99999.9)
        case 15: if (Rand(c, 285 + j) % 3 == 0) { s.pos[0] = FBits(float(cx)); s.pos[1] = FBits(float(cy)); } break;                   // distancia cero
        default: if (Rand(c, 290 + j) % 9 == 0) s.pos[Rand(c, 291 + j) % 3] = MixComponent(c, 292 + j); break;
        }
    }
    f.firstPre = Rand(c, 300) % 3 == 0 ? 0u : Rand(c, 301);
    f.secondPre = Rand(c, 302) % 3 == 0 ? 0u : Rand(c, 303);
    f.mode = Rand(c, 304) % 6;
    f.seedArena = Rand(c, 305) | 1u;
    f.eax = Rand(c, 306) % 5 == 0 ? 0u : Rand(c, 307);
    f.edx = Rand(c, 308) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 309);
    f.stack = StackSeed(Rand(c, 310) % 12);
    return f;
}

// ------------------------------------------------------------------------------------------------ 2: CCam::GetArrestCamCopSourceSide
// Arena: [guarda][cop 0x20 (+4 posicion, +0x14 matriz)][matriz del cop 0x40 (+0x30 posicion)][dest 0x10][source 0x10][relleno 0x10][guarda]
constexpr u32 kCopOff = kGuard;                         // 0x040
constexpr u32 kCopBytes = 0x20;
constexpr u32 kCopMatOff = kCopOff + kCopBytes;         // 0x060
constexpr u32 kDestOff = kCopMatOff + 0x40;             // 0x0A0
constexpr u32 kSourceOff = kDestOff + 0x10;             // 0x0B0
constexpr u32 kCopEnd = kSourceOff + 0x10 + 0x10;       // 0x0D0
constexpr u32 kCopArena = kCopEnd + kGuard;             // 0x110
static_assert(kCopArena == 0x110, "arena CCam: 0x40 + 0x20 + 0x40 + 0x10 + 0x10 + 0x10 + 0x40");
constexpr u32 kTargetFake = 0x1234F00u;                 // CEntity* de mentira (solo se comprueba que no sea nulo)

struct CopCase {
    u32 n, family;
    u32 targetKind;     // 0 nulo, 1 no nulo
    u32 copKind;        // 0 nulo, 1 arena
    u32 matKind;        // 0 sin matriz (posicion en +4), 1 con matriz (posicion en +0x30)
    u32 copPos[3], copOther[3], dest[3], sourcePre[3];
    u32 nrmMode;        // 0: el falso de Normalise no toca el vector, 1: lo sustituye por nrm
    u32 nrm[3];
    u32 cross[3];       // lo que escribe el falso de CrossProduct
    u32 ground;         // lo que devuelve el falso de FindGroundZFor3DCoord (float)
    u32 foundMode;      // 0: no escribe *found; 1: escribe foundByte
    u32 foundByte;
    u32 mode, seedArena;
    u32 eax, ecx, edx, stack;
};
inline CopCase MakeCop(unsigned n) {
    CopCase f{};
    const u32 c = n * 8u + kCop * 0x10001u;
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    f.targetKind = fam == 0 ? (Rand(c, 1) % 4 == 0 ? 1u : 0u) : 1u;
    f.copKind = fam == 1 ? 0u : (fam == 0 && Rand(c, 2) % 2 ? 0u : 1u);
    f.matKind = Rand(c, 3) % 2;
    auto vec = [&](u32* out, u32 k, int lo, int hi) { for (int i = 0; i < 3; ++i) out[i] = RandFloat(c, k + i, lo, hi); };
    vec(f.copPos, 10, -4, 10); vec(f.copOther, 14, -4, 10); vec(f.dest, 18, -4, 10); vec(f.sourcePre, 22, -4, 10);
    // vector normalizado y producto cruzado tipicos
    auto unit = [&](u32* out, u32 k) {
        double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0, b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
        double m = std::sqrt(a * a + b * b);
        if (m < 1e-3) { a = 1.0; b = 0.0; m = 1.0; }
        out[0] = FBits(float(a / m)); out[1] = FBits(float(b / m)); out[2] = 0u;
    };
    f.nrmMode = Rand(c, 30) % 4 == 0 ? 0u : 1u;
    unit(f.nrm, 31);
    unit(f.cross, 34);
    f.ground = RandFloat(c, 40, -4, 9);
    f.foundMode = Rand(c, 41) % 4 == 0 ? 0u : 1u;
    f.foundByte = Rand(c, 42) % 2 ? 1u : (Rand(c, 43) % 3 == 0 ? 0u : (Rand(c, 44) % 255 + 1));
    switch (fam) {
    case 2: f.foundMode = 1; f.foundByte = 1; break;                                           // suelo encontrado
    case 3: f.foundMode = 1; f.foundByte = 0; break;                                           // no encontrado
    case 4: f.foundMode = 1; f.foundByte = 2u + Rand(c, 45) % 254; break;                      // byte distinto de 0/1
    case 5: f.foundMode = 0; break;                                                            // el falso no escribe: queda el 0 inicial
    case 6: for (int i = 0; i < 3; ++i) f.dest[i] = Rand(c, 46 + i) % 3 ? f.dest[i] : MixComponent(c, 50 + i); break;
    case 7: for (int i = 0; i < 3; ++i) f.copPos[i] = Rand(c, 46 + i) % 3 ? f.copPos[i] : MixComponent(c, 50 + i); break;
    case 8: for (int i = 0; i < 3; ++i) { f.nrm[i] = MixComponent(c, 56 + i); f.cross[i] = MixComponent(c, 60 + i); } f.nrmMode = 1; break;
    case 9: f.ground = MixComponent(c, 64); f.foundMode = 1; f.foundByte = 1; break;
    case 10: for (int i = 0; i < 3; ++i) { f.dest[i] = RandFloat(c, 66 + i, 100, 127); f.copPos[i] = RandFloat(c, 69 + i, 100, 127); } break;
    case 11: for (int i = 0; i < 3; ++i) { f.dest[i] = Rand(c, 72 + i) % 2 ? (Rand(c, 75 + i) & 0x807FFFFFu) : RandFloat(c, 78 + i, -140, -100); f.copPos[i] = RandFloat(c, 81 + i, -140, -100); } break;
    case 12: for (int i = 0; i < 3; ++i) { f.nrm[i] = RandFloat(c, 84 + i, -140, -100); f.cross[i] = RandFloat(c, 87 + i, -140, -100); f.sourcePre[i] = RandFloat(c, 90 + i, 100, 127); } break;
    case 13: for (int i = 0; i < 3; ++i) { f.dest[i] = RandFloat(c, 93 + i, 120, 127); f.nrm[i] = RandFloat(c, 96 + i, 120, 127); } f.foundMode = 1; f.foundByte = 1; f.ground = RandFloat(c, 99, 120, 127); break;
    case 14: for (int i = 0; i < 3; ++i) f.dest[i] = Rand(c, 100 + i) % 2 ? f.copPos[i] : FBits(float(int(Rand(c, 103 + i) % 5) - 2)); f.nrmMode = 0; break;           // delta nulo o pequeno
    case 15: for (int i = 0; i < 3; ++i) { f.copPos[i] = MixComponent(c, 106 + i); f.dest[i] = MixComponent(c, 109 + i); f.sourcePre[i] = MixComponent(c, 112 + i); } break;
    default: break;
    }
    f.mode = Rand(c, 120) % 6;
    f.seedArena = Rand(c, 121) | 1u;
    f.eax = Rand(c, 122) % 5 == 0 ? 0u : Rand(c, 123);
    f.ecx = Rand(c, 124);
    f.edx = Rand(c, 125) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 126);
    f.stack = StackSeed(Rand(c, 127) % 12);
    return f;
}

// ------------------------------------------------------------------------------------------------ 3: CHeli::CHeli
// Arena: [guarda 0x40][relleno 0x10][objeto 0xA18][guarda 0x40]; el objeto esta en uno de cuatro desplazamientos
constexpr u32 kHeliBytes = 0xA18;
constexpr u32 kHeliArena = kGuard + 0x10 + kHeliBytes + kGuard;
static_assert(kHeliArena == 0xAA8, "arena CHeli: 0x40 + 0x10 + 0xA18 + 0x40");
constexpr u32 kHeliOffsets[4]{0x40, 0x44, 0x48, 0x4C};
constexpr u32 kTimerAddr = 0xB7CB84;                    // CTimer::m_snTimeInMilliseconds
constexpr u32 kExeHeliVptr = 0x871680u;
constexpr u32 kBaseVptrMarker = 0xBA5E0000u;            // lo que deja el falso de CAutomobile::CAutomobile en +0
constexpr u32 kVtLabel = 0xC0B1EC70u;                   // etiqueta del vptr propio de cada lado
constexpr u32 kModelHunter = 0x1A9;
constexpr u32 kSetDoorPattern0 = 0xD00D0001u, kSetDoorPattern1 = 0xD00D0002u;

struct HeliCase {
    u32 n, family;
    u32 model;          // dword (i32)
    u32 createdBy;      // 0..255 (eVehicleCreatedBy es uint8)
    u32 timer;          // CTimer::m_snTimeInMilliseconds
    u32 randRet;        // lo que devuelve el rand() del EXE (i32)
    u32 objOff, mode, seedArena, seedP;
    u32 eax, edx, stack;
};
inline HeliCase MakeHeli(unsigned n) {
    HeliCase f{};
    const u32 c = n * 8u + kHeli * 0x10001u;
    f.n = n; f.family = n % kFamilies;
    const u32 fam = f.family;
    static constexpr u32 models[]{0x1A9u, 0x1A8u, 0x1AAu, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 497u, 417u, 425u, 447u, 488u, 548u, 563u, 0x1A9u + 0x100u, 0x1A9u + 0x10000u, 0xFFFFFFFFu - 0x1A9u};
    f.model = fam < 8 ? 0x1A9u : (Rand(c, 1) % 3 == 0 ? Rand(c, 2) : (Rand(c, 3) % 2 ? models[Rand(c, 4) % std::size(models)] : Rand(c, 5) % 700u));
    if (fam >= 8 && Rand(c, 6) % 4 == 0) f.model = 0x1A9u;
    f.createdBy = Rand(c, 7) % 3 == 0 ? 1u + Rand(c, 8) % 4 : (Rand(c, 9) % 2 ? Rand(c, 10) & 0xFFu : (Rand(c, 11) % 2 ? 0xFFu : 0u));
    f.timer = Rand(c, 12) % 8 == 0 ? (Rand(c, 13) % 2 ? 0u : 0xFFFFFFFFu) : Rand(c, 14);
    static constexpr u32 rnd[]{0u, 1u, 32767u, 16383u, 16384u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x8000u, 0xFFFFu, 0x10000u, 0x00FFFFFFu, 0x01000000u, 0x01000001u, 0x40000001u, 0xFFFFFFFEu};
    f.randRet = Rand(c, 15) % 3 == 0 ? rnd[Rand(c, 16) % std::size(rnd)] : (Rand(c, 17) % 2 ? Rand(c, 18) % 32768u : Rand(c, 19));
    f.objOff = kHeliOffsets[Rand(c, 20) % 4];
    f.mode = Rand(c, 21) % 6;
    f.seedArena = Rand(c, 22) | 1u;
    f.seedP = Rand(c, 23);
    f.eax = Rand(c, 24) % 5 == 0 ? 0u : Rand(c, 25);
    f.edx = Rand(c, 26) % 5 == 0 ? 0xFFFFFFFFu : Rand(c, 27);
    f.stack = StackSeed(Rand(c, 28) % 12);
    return f;
}

// ------------------------------------------------------------------------------------------------ imagenes de referencia de cada arena (las usan Core.hpp y fixture_dump.cpp)
inline void PutW(u8* a, std::size_t at, u32 v) { std::memcpy(a + at, &v, 4); }
inline void BuildClumpRef(const ClumpCase& f, u32 base, u8* ref) {
    FillArena(ref, kClumpArena, f.mode, f.seedArena);
    PutW(ref, kClumpOff + 4, f.parentKind == 0 ? base + kFrameOff : f.parentKind == 1 ? 0u : kGarbageFrame);
}
// Devuelve la direccion de `this` y el byte de indice del garaje (low8 de (this - 0x96C048) / 0xD8 con division con signo).
inline void BuildGarageRef(const GarageCase& f, u32 base, u8* ref, u32& thisAddr, u32& gb) {
    FillArena(ref, kGarageArena, f.mode, f.seedArena);
    const u32 garageInArena = kGarageOff + f.garageDelta;
    thisAddr = f.garageKind == 0 ? base + garageInArena : f.rdata;
    if (f.garageKind == 0) for (int i = 0; i < 10; ++i) PutW(ref, garageInArena + 4 * i, f.g[i]);
    gb = u32(u8(i32(i32(thisAddr - kGarageBase) / 0xD8)));
    PutW(ref, kDoorsOff, f.firstPre); PutW(ref, kDoorsOff + 4, f.secondPre);
    PutW(ref, kPoolOff, f.baseMode == 0 ? base + kObjOff : u32(0u - f.nullSlot * kObjBytes));
    PutW(ref, kPoolOff + 4, base + kFlagsOff);
    PutW(ref, kPoolOff + 8, f.size);
    for (u32 j = 0; j < kMaxSlots; ++j) {
        const DoorSlot& s = f.s[j];
        ref[kFlagsOff + j] = u8(s.flagByte);
        u8* obj = ref + kObjOff + j * kObjBytes;
        u8* mat = ref + kMatOff + j * 0x40;
        for (int i = 0; i < 3; ++i) { PutW(obj, 4 + 4 * i, s.matKind == 0 ? s.pos[i] : s.other[i]); PutW(mat, 0x30 + 4 * i, s.matKind == 1 ? s.pos[i] : s.other[i]); }
        PutW(obj, 0x14, s.matKind == 1 ? base + kMatOff + j * 0x40 : 0u);
        obj[kObjByte] = s.match ? u8(gb) : u8(gb ^ s.delta);
    }
}
inline void BuildCopRef(const CopCase& f, u32 base, u8* ref) {
    FillArena(ref, kCopArena, f.mode, f.seedArena);
    for (int i = 0; i < 3; ++i) {
        PutW(ref, kCopOff + 4 + 4 * i, f.matKind == 0 ? f.copPos[i] : f.copOther[i]);
        PutW(ref, kCopMatOff + 0x30 + 4 * i, f.matKind == 1 ? f.copPos[i] : f.copOther[i]);
        PutW(ref, kDestOff + 4 * i, f.dest[i]);
        PutW(ref, kSourceOff + 4 * i, f.sourcePre[i]);
    }
    PutW(ref, kCopOff + 0x14, f.matKind == 1 ? base + kCopMatOff : 0u);
}
inline void BuildHeliRef(const HeliCase& f, u8* ref) { FillArena(ref, kHeliArena, f.mode, f.seedArena); }
}
