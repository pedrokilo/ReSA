#pragma once
// Fixtures deterministas, arena y colaboradores sustituidos (stubs) del banco FxEmitterAddParticleMat31 (FxEmitter_c::AddParticle(matrix), EXE 0x4A4050, slot +0x10 de la vtable).
// Solo C++ estandar y sin tipos del juego: se incluye en el banco dentro del juego (Core.hpp) y en el arnes aislado de offline/ (bytes reales del EXE reubicados frente al C++).
// fixture_oracle.py repite exactamente las formulas y tablas de este archivo. Todos los valores float se dan como patrones de bits.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace AddMatFix {
using u32 = std::uint32_t;
using u8 = std::uint8_t;
constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline float F(u32 bits) { float f; std::memcpy(&f, &bits, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }

// ------------------------------------------------------------------------------------------------------------------------------ arena
// Disposicion (huecos de tamano fijo; los desplazamientos son los acumulados):
//   emisor   FxEmitter_c        0x000..0x040   (m_PrimBP +4, m_System +8)
//   prim     FxPrimBP_c         0x040..0x0C0   (m_FxInfoManager en +0x2C: lo recibe el stub de ProcessEmissionInfo como `this`)
//   sistema  FxSystem_c         0x0C0..0x140   (m_SystemBP +8, m_ParentMatrix +0xC, m_UseConstTime +0x52)
//   sysBP    FxSystemBP_c       0x140..0x180   (m_fLength +0xC)
//   padre    RwMatrix           0x180..0x1C0
//   mats     4 RwMatrix         0x1C0..0x2C0   (las que reparte el stub de FxRwMatrixCreate, en orden: A, B, C y la del prim)
//   mat      RwMatrix (entrada) 0x2C0..0x300   (argumento 1: se copia entera, 16 palabras, a la matriz C)
//   vel      CVector            0x300..0x310
//   mult     FxPrtMult_c        0x310..0x330
//   particula FxEmitterPrt_c    0x330..0x370   (la que devuelve el stub de CreateParticle cuando particleMode == 1)
// Total 0x370 (0x2C0 + 0x40 + 0x10 + 0x20 + 0x40). La memoria real va con 16 bytes de guarda a cada lado (kGuard).
constexpr std::size_t kEmitter = 0x000, kPrim = 0x040, kSystem = 0x0C0, kSystemBp = 0x140, kParent = 0x180, kMats = 0x1C0, kIn = 0x2C0, kVel = 0x300, kMult = 0x310, kPart = 0x330, kArena = 0x370;
static_assert(kPrim == kEmitter + 0x40 && kSystem == kPrim + 0x80 && kSystemBp == kSystem + 0x80 && kParent == kSystemBp + 0x40 && kMats == kParent + 0x40 && kIn == kMats + 4 * 0x40 &&
              kVel == kIn + 0x40 && kMult == kVel + 0x10 && kPart == kMult + 0x20 && kArena == kPart + 0x40, "arena");
constexpr std::size_t kGuard = 16;
constexpr std::size_t Mat(unsigned i) { return kMats + 0x40 * i; }
constexpr unsigned kEmissionWords = 26;
constexpr u32 kVtable = 0x0085A7A4;

inline constexpr std::array<u32, 40> kSpecial{
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7FFFFFFF, 0x7F7FFFFF, 0xFF7FFFFF,
    0x3F000000, 0x40000000, 0x41200000, 0x42C80000, 0x3A83126F, 0x437F0000, 0x477FFF00, 0x4B000000, 0x3F7FFFFF, 0x3F800001, 0x3C23D70A, 0x3E800000, 0x437F8000, 0x42C7FFFF, 0x7F000000, 0x00FFFFFF,
    0x3FC90FDB, 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x42B40000, 0x43B40000, 0x7149F2CA, 0x0DA24260};
// rotZ: valores sensibles a la conversion byte(ftol2(rotZ * 0.5f)) y a la comparacion rotZ >= 0
inline constexpr std::array<u32, 36> kRotZ{
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0xBF800000, 0xC2C80000, 0x3F800000, 0x40000000, 0x40400000, 0x437F0000, 0x43800000, 0x43FF0000, 0x44000000, 0x447FC000, 0x44800000,
    0x4B800000, 0x4F000000, 0x4F7FFFFF, 0x5F000000, 0x5F800000, 0x7F7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00001, 0x3F7FFFFF, 0x437FFFFF, 0x43FFFFFF, 0x3F000000, 0xBF000000, 0xBA83126F,
    0x42C80000, 0x4F800000, 0xCF000000, 0x007FFFFF, 0x7FC01234};

struct AddFx {
    unsigned family;
    u32 mat[16], vel[3];            // mat: matriz de entrada (argumento 1)
    u32 timeSince, rotZ, brightness;
    u32 mult[7];
    u32 length;                 // m_SystemBP->m_fLength
    unsigned useConst;          // m_UseConstTime (0 o 1)
    unsigned createLocal;       // 0 o 1
    unsigned parent;            // 1: el sistema tiene matriz padre
    unsigned particleMode;      // 0: CreateParticle devuelve null; 1: devuelve la particula del arena
    u32 emission[kEmissionWords];   // lo que escribe el stub de ProcessEmissionInfo
    u32 matInit[4][16];         // contenido previo de las cuatro matrices del pool
    u32 parentMat[16];
    u32 getRw[16];              // lo que escribe el stub de GetRWMatrix
    unsigned fill;
};
inline unsigned Family(unsigned c) { return c % 8; }
inline u32 Val(unsigned c, unsigned key, bool rotZ = false) {
    const unsigned fam = Family(c);
    const unsigned dice = Rand(c, key + 1000) % 100;
    unsigned special = 4, extreme = 3;
    if (fam == 4) { special = 40; extreme = 5; }
    else if (fam == 5) { special = 5; extreme = 40; }
    else if (fam == 6) { special = 20; extreme = 20; }
    const u32 r = Rand(c, key);
    if (rotZ && dice < 55) return kRotZ[r % kRotZ.size()];
    if (!rotZ && dice < special) return kSpecial[r % kSpecial.size()];
    const u32 mant = r & 0x7FFFFF, sign = (r >> 31) & 1;
    if (dice < special + extreme + (rotZ ? 55 : 0)) {
        const u32 e = (Rand(c, key + 2000) & 1) ? 1 + (Rand(c, key + 3000) % 24) : 231 + (Rand(c, key + 3000) % 24);
        return (sign << 31) | (e << 23) | mant;
    }
    const u32 e = rotZ ? 120 + Rand(c, key + 2000) % 16 : 118 + Rand(c, key + 2000) % 14;
    return (sign << 31) | (e << 23) | mant;
}
inline AddFx Make(unsigned c) {
    AddFx f{};
    f.family = Family(c);
    // matriz de entrada: 12 floats con Val; flags y los tres rellenos (palabras 3, 7, 11, 15) son basura de 32 bits (el EXE los copia tal cual)
    for (unsigned i = 0; i < 16; ++i) f.mat[i] = (i % 4 == 3) ? Rand(c, 400 + i) : Val(c, 80 + i);
    for (unsigned i = 0; i < 3; ++i) f.vel[i] = Val(c, 20 + i);
    f.timeSince = Val(c, 30);
    f.rotZ = Val(c, 31, true);
    f.brightness = Val(c, 32);
    for (unsigned i = 0; i < 7; ++i) f.mult[i] = Val(c, 40 + i);
    f.length = Val(c, 50);
    f.useConst = Rand(c, 51) & 1;
    f.createLocal = Rand(c, 52) & 1;
    f.parent = Rand(c, 53) % 3 != 0;
    f.particleMode = (Rand(c, 54) % 4) != 0;
    for (unsigned i = 0; i < kEmissionWords; ++i) f.emission[i] = Val(c, 100 + i);
    for (unsigned m = 0; m < 4; ++m) for (unsigned i = 0; i < 16; ++i) f.matInit[m][i] = Rand(c, 200 + 16 * m + i);   // basura de 32 bits (flags y rellenos incluidos)
    for (unsigned i = 0; i < 16; ++i) { f.parentMat[i] = Val(c, 300 + i); f.getRw[i] = Val(c, 320 + i); }
    f.fill = Rand(c, 60) % 4;
    return f;
}

inline void Put32(u8* base, std::size_t at, u32 v) { std::memcpy(base + at, &v, 4); }
inline u32 Get32(const u8* base, std::size_t at) { u32 v; std::memcpy(&v, base + at, 4); return v; }
inline u32 Ptr(const u8* base, std::size_t at) { return static_cast<u32>(reinterpret_cast<std::uintptr_t>(base + at)); }
inline void BuildArena(u8* base, const AddFx& f) {
    constexpr u8 fills[4] = {0x5C, 0xC3, 0xA5, 0x00};
    std::memset(base, fills[f.fill], kArena);
    Put32(base, kEmitter + 0, kVtable); Put32(base, kEmitter + 4, Ptr(base, kPrim)); Put32(base, kEmitter + 8, Ptr(base, kSystem));
    base[kEmitter + 0xC] = 1; Put32(base, kEmitter + 0x10, 0x3F000000);
    Put32(base, kSystem + 8, Ptr(base, kSystemBp)); Put32(base, kSystem + 0xC, f.parent ? Ptr(base, kParent) : 0u);
    base[kSystem + 0x52] = static_cast<u8>(f.useConst);
    Put32(base, kSystemBp + 0xC, f.length);
    for (unsigned i = 0; i < 16; ++i) Put32(base, kParent + 4 * i, f.parentMat[i]);
    for (unsigned m = 0; m < 4; ++m) for (unsigned i = 0; i < 16; ++i) Put32(base, Mat(m) + 4 * i, f.matInit[m][i]);
    for (unsigned i = 0; i < 16; ++i) Put32(base, kIn + 4 * i, f.mat[i]);
    for (unsigned i = 0; i < 3; ++i) Put32(base, kVel + 4 * i, f.vel[i]);
    for (unsigned i = 0; i < 7; ++i) Put32(base, kMult + 4 * i, f.mult[i]);
}

// ------------------------------------------------------------------------------------------------------------------------------ colaboradores sustituidos (trazados)
inline std::vector<u32> trace;
inline u8* traceBase{};
inline unsigned matrixCount{};
inline unsigned particleMode{};
inline u32 emissionWords[kEmissionWords]{};
inline u32 getRwWords[16]{};
inline u32 Off(const void* p) {
    if (!p) return 0xFFFFFFFFu;
    const auto* b = static_cast<const u8*>(p);
    return (b >= traceBase && b < traceBase + kArena) ? static_cast<u32>(b - traceBase) : 0xFFFFFFFEu;
}
inline u32 Fnv(const void* p, std::size_t n) { u32 h = 2166136261u; for (std::size_t i = 0; i < n; ++i) { h ^= static_cast<const u8*>(p)[i]; h *= 16777619u; } return h; }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
// FxInfoManager_c::ProcessEmissionInfo (0x4A4960, thiscall, 5 argumentos de pila, ret 0x14): this = &primBP->m_FxInfoManager
inline void __fastcall StubProcessEmissionInfo(void* self, void*, float currentTime, float mult, float totalTime, int useConst, void* out) {
    trace.insert(trace.end(), {1u, Off(self), Bits(currentTime), Bits(mult), Bits(totalTime), static_cast<u32>(useConst) & 0xFFu});
    std::memcpy(out, emissionWords, kEmissionWords * 4);
}
// FxManager_c::FxRwMatrixCreate (0x4A9440, thiscall, sin argumentos): reparte las cuatro matrices del arena en orden y vuelve a la primera
inline void* __fastcall StubCreate(void*, void*) {
    void* m = traceBase + Mat(matrixCount++ % 4);
    trace.insert(trace.end(), {2u, Off(m)});
    return m;
}
inline void __fastcall StubDestroy(void*, void*, void* m) { trace.insert(trace.end(), {3u, Off(m)}); }
// FxPrimBP_c::GetRWMatrix (0x4A9DC0, thiscall, 1 argumento, ret 4): escribe las 16 palabras
inline void __fastcall StubGetRWMatrix(void* self, void*, void* m) {
    trace.insert(trace.end(), {4u, Off(self), Off(m)});
    std::memcpy(m, getRwWords, 0x40);
}
// RwMatrixUpdate (0x7F18A0, cdecl)
inline void* __cdecl StubRwMatrixUpdate(void* m) { trace.insert(trace.end(), {5u, Off(m), Fnv(m, 0x40)}); return m; }
// RwMatrixMultiply (0x7F18B0, cdecl (dst, a, b)): dst[i] = hash + i
inline void* __cdecl StubRwMatrixMultiply(void* d, const void* a, const void* b) {
    trace.insert(trace.end(), {6u, Off(d), Off(a), Off(b), Fnv(a, 0x40), Fnv(b, 0x40)});
    auto* w = static_cast<u32*>(d);
    const u32 h = Fnv(a, 0x40) ^ (Fnv(b, 0x40) * 31u);
    for (u32 i = 0; i < 16; ++i) w[i] = h + i;
    return d;
}
// FxEmitter_c::CreateParticle (0x4A2580, thiscall, 7 argumentos de pila, ret 0x1C): devuelve null o la particula del arena
inline void* __fastcall StubCreateParticle(void* self, void*, const void* em, void* wld, const void* vel, float timeSince, const void* mult, float brightness, int createLocal) {
    trace.insert(trace.end(), {8u, Off(self), Fnv(em, 0x68), Off(wld), Fnv(wld, 0x40), Off(vel), Fnv(vel, 12), Bits(timeSince), Fnv(mult, 0x1C), Bits(brightness), static_cast<u32>(createLocal) & 0xFFu});
    return particleMode ? traceBase + kPart : nullptr;
}
}  // namespace AddMatFix
