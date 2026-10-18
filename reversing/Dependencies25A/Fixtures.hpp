#pragma once
// Fixtures del banco Dependencies25A (grupo g25a de BRIEF_21): nueve entradas del EXE de nivel "1 Muy facil" del cohorte 2 de dependencias originales.
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_oracle.py repite exactamente Mix/Rand/FillWords.
//
//   0  CTask::Destructor                               0x61A3A0  thiscall, sin pila, plano `ret`        mov [ecx], 0x86D48C
//   1  CVehiclePool::GetAt (CPool<CVehicle,CHeli>)     0x41CC10  thiscall, 1 arg de pila, `ret 4`       0 si bit 7 del estado, si no storage + idx * 0xA18
//   2  CAutoPilot::RemoveOnePathNode                   0x41B950  thiscall, sin pila, plano `ret`        desplaza m_aPathFindNodesInfo (contador de 16 bits en +0x88)
//   3  CPedGroups::RemoveAllFollowersFromGroup         0x5FB8A0  cdecl, 1 arg, `jmp 0x5FB7D0`           si ms_activeGroups[id], CPedGroup::RemoveAllFollowers del grupo
//   4  CDecisionMakerTypes::FlushDecisionMakerEventResponse 0x604490 thiscall, 2 args, `ret 8`          CDecision::SetDefault de la decision (a, evento b)
//   5  CGarages::CountCarsInHideoutGarage              0x44A210  cdecl, 1 arg (byte), EAX = cuenta       cuenta las 4 ranuras del refugio con modelo
//   6  FindReplacementUpgradeCB                        0x6D3490  cdecl, 2 args, EAX                      callback de RwFrameForAllObjects
//   7  GetAnimHierarchyFromFrameCallback               0x734A70  cdecl, 2 args, EAX                      callback de RwFrameForAllChildren (SIN hook: la DLL se llama directamente)
//   8  CAEStreamingDecoder scalar deleting destructor  0x4F2860  thiscall, 1 arg de pila, `ret 4`       vtable base, destruye el flujo, libera `this` si bit 0
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace Dependencies25AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 2048;   // por funcion
constexpr unsigned kFunctions = 9;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
// Bytes de un bloque: la palabra i (little endian) es Rand(seed, i); la cola de menos de 4 bytes toma los bytes bajos de la palabra siguiente.
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u64 Fnv64(const u8* p, std::size_t n) {
    u64 h = 0xCBF29CE484222325ull;
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 0x100000001B3ull; }
    return h;
}
// Semilla del contenido de la pila bajo el ESP de la llamada (las 8 primeras son valores notables).
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned fn, unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u * (fn + 1), 99); }

// ------------------------------------------------------------------------------------------------ 0 CTask::Destructor
constexpr u32 kTaskVtable = 0x86D48C;
struct F0 { u32 offset, first, eax, edx, seed, stack; };
inline constexpr std::array<u32, 10> kVptrs{0u, 0xFFFFFFFFu, 0x86D48Cu, 0x86D684u, 0x12345678u, 0x80000000u, 1u, 0x86D48Du, 0x85F418u, 0x0086D48Cu ^ 0x00FF0000u};
inline F0 MakeF0(unsigned n) {
    F0 f{};
    f.offset = n % 4;
    f.first = (n % 8) < 6 ? kVptrs[(n / 8 + n) % kVptrs.size()] : Rand(n, 1);
    f.eax = (n % 16) == 5 ? 0u : (n % 16) == 9 ? 0xFFFFFFFFu : Rand(n, 2);
    f.edx = (n % 16) == 6 ? 0u : (n % 16) == 10 ? 0xFFFFFFFFu : Rand(n, 3);
    f.seed = Rand(n, 4);
    f.stack = StackSeed(0, n);
    return f;
}
constexpr unsigned kF0Bytes = 48;

// ------------------------------------------------------------------------------------------------ 1 CVehiclePool::GetAt
constexpr unsigned kPoolSlots = 256;
constexpr u32 kPoolStride = 0xA18;
struct F1 { u32 idx, storage, stateKind, stateSeed, poolSeed, eax, edx, stack; };
inline constexpr std::array<u32, 8> kStorages{0u, 0x1000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFF000u, 0x0F000000u, 0xC0000A18u};
inline F1 MakeF1(unsigned n) {
    F1 f{};
    f.idx = n < kPoolSlots ? n : Rand(n, 1) % kPoolSlots;
    f.storage = (n % 3) == 0 ? Rand(n, 2) : kStorages[(n / 3 + n) % kStorages.size()];
    f.stateKind = n % 5;   // 0 todo libre, 1 todo usado, 2 aleatorio, 3 90% libre, 4 10% libre
    f.stateSeed = Rand(n, 3);
    f.poolSeed = Rand(n, 4);
    f.eax = Rand(n, 5); f.edx = Rand(n, 6);
    f.stack = StackSeed(1, n);
    return f;
}
inline void BuildStates(const F1& f, u8* states) {
    for (unsigned i = 0; i < kPoolSlots; ++i) {
        const u32 r = Rand(f.stateSeed, i);
        const u8 ref = u8(r & 0x7F);
        bool freeSlot = false;
        switch (f.stateKind) {
        case 0: freeSlot = true; break;
        case 1: freeSlot = false; break;
        case 2: freeSlot = (r >> 8) & 1; break;
        case 3: freeSlot = ((r >> 8) % 10) != 0; break;
        default: freeSlot = ((r >> 8) % 10) == 0; break;
        }
        states[i] = u8(ref | (freeSlot ? 0x80 : 0));
    }
}

// ------------------------------------------------------------------------------------------------ 2 CAutoPilot::RemoveOnePathNode
constexpr u32 kAutoPilotOffset = 0x40;       // desplazamiento del objeto dentro del arena
constexpr u32 kAutoPilotArena = 0x20400;
constexpr u32 kAutoPilotGuard = 64;
struct F2 { u32 count, w8c, seed, eax, edx, stack, window; };
inline constexpr std::array<u32, 14> kSmallCounts{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
inline constexpr std::array<u32, 12> kMidCounts{14, 16, 17, 20, 32, 64, 100, 255, 256, 512, 1000, 0x1000};
inline constexpr std::array<u32, 8> kBigCounts{0x8000u, 0x8001u, 0x7FFFu, 0x4000u, 0xFFFFu, 0xFFFEu, 0x2000u, 0x7FFEu};
inline constexpr std::array<u32, 14> kAliasWords{0u, 1u, 2u, 8u, 9u, 10u, 11u, 12u, 13u, 20u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x12345678u};
inline i16 DecCount(u32 count) { return i16(u16(count - 1)); }
inline F2 MakeF2(unsigned n) {
    F2 f{};
    const unsigned slot = n % 16;
    if (slot < 8) f.count = kSmallCounts[(n / 16 + slot * 2 + (n % 5)) % kSmallCounts.size()];
    else if (slot < 14) f.count = (n % 3) ? kSmallCounts[Rand(n, 1) % kSmallCounts.size()] : kMidCounts[Rand(n, 1) % kMidCounts.size()];
    else if (slot == 14) f.count = kMidCounts[Rand(n, 1) % kMidCounts.size()];
    else f.count = kBigCounts[(n / 16) % kBigCounts.size()];
    f.w8c = (n % 3) == 0 ? Rand(n, 2) : kAliasWords[Rand(n, 2) % kAliasWords.size()];
    f.seed = Rand(n, 3);
    f.eax = Rand(n, 4); f.edx = Rand(n, 5);
    f.stack = StackSeed(2, n);
    // Ventana de bytes tocados: el EXE lee hasta [obj + 0x6C + 4*(iteraciones)] y la copia llega a escribir sobre el contador cuando hay 9 o mas nodos.
    i32 iterations = DecCount(f.count) > 0 ? DecCount(f.count) : 0;
    if (DecCount(f.count) >= 9) { const i32 replaced = i16(u16(f.w8c)); if (replaced > iterations) iterations = replaced; }
    u32 window = 0x6C + 4u * u32(iterations + 2);
    if (window < 0xA0) window = 0xA0;
    f.window = window;
    return f;
}

// ------------------------------------------------------------------------------------------------ 3 CPedGroups::RemoveAllFollowersFromGroup
constexpr u32 kActiveGroups = 0xC098E0, kGroups = 0xC09920, kGroupStride = 0x2D4;
// Region guardada, rellenada por caso y restaurada: [0xC098D0, 0xC09920) = ScriptReferenceIndex, ms_activeGroups (+0x10), banderas de mision/muertes y relleno hasta ms_groups.
// Asi los identificadores fuera de 0..7 (que el EXE no comprueba) leen bytes conocidos y no solo ceros.
constexpr u32 kGroupRegion = 0xC098D0, kGroupRegionBytes = 0x50;
struct F3 { i32 id; u8 active[8]; u32 regionSeed, eax, ecx, edx, stack; };
inline constexpr std::array<i32, 24> kGroupIds{0, 1, 2, 3, 4, 5, 6, 7, -1, -2, -8, -16, 8, 9, 10, 15, 16, 31, 0x100, -0x100, 0x1000, -0x1000, 0x7F, 0x2D4};
inline constexpr std::array<u8, 8> kActiveBytes{0, 1, 0x80, 0xFF, 0x7F, 2, 0, 1};
inline F3 MakeF3(unsigned n) {
    F3 f{};
    f.id = n < 8 * 4 ? i32(n % 8) : (n % 5 < 3 ? i32(Rand(n, 1) % 8) : kGroupIds[Rand(n, 1) % kGroupIds.size()]);
    for (unsigned k = 0; k < 8; ++k) f.active[k] = (Rand(n, 10 + k) % 3) == 0 ? u8(Rand(n, 20 + k)) : kActiveBytes[Rand(n, 30 + k) % kActiveBytes.size()];
    f.regionSeed = Rand(n, 5);
    f.eax = Rand(n, 2); f.ecx = Rand(n, 3); f.edx = Rand(n, 4);
    f.stack = StackSeed(3, n);
    return f;
}
inline void BuildGroupRegion(const F3& f, u8* region) {
    FillWords(region, kGroupRegionBytes, f.regionSeed);
    std::memcpy(region + (kActiveGroups - kGroupRegion), f.active, 8);
}

// ------------------------------------------------------------------------------------------------ 4 CDecisionMakerTypes::FlushDecisionMakerEventResponse
constexpr u32 kTypesBytes = 0xF1C0, kTypesPad = 0x100, kEventIndices = 0xC034;
struct F4 { i32 a, b; u32 seed, eax, edx, stack; };
inline constexpr std::array<i32, 14> kDecisionIndex{20, 21, 40, 41, 100, 255, 256, 0x7FFFFFFF, i32(0x80000000), -1, -2, -20, 0x01234567, i32(0xFFFFFFFE)};
inline constexpr std::array<i32, 14> kEventIds{0, 1, 2, 5, 17, 40, 92, 93, 100, 255, 1000, 3000, 3170, 64};
inline constexpr std::array<i32, 8> kEventNeg{-1, -2, -100, -1000, -5000, -12301, -12000, -77};
inline F4 MakeF4(unsigned n) {
    F4 f{};
    f.a = (n % 4) != 3 ? i32((n / 4 + n) % 20) : kDecisionIndex[Rand(n, 1) % kDecisionIndex.size()];
    if ((n % 8) < 5) f.b = i32(Rand(n, 2) % 94);
    else if ((n % 8) < 7) f.b = kEventIds[Rand(n, 2) % kEventIds.size()];
    else f.b = kEventNeg[Rand(n, 2) % kEventNeg.size()];
    f.seed = Rand(n, 3); f.eax = Rand(n, 4); f.edx = Rand(n, 5);
    f.stack = StackSeed(4, n);
    return f;
}

// ------------------------------------------------------------------------------------------------ 5 CGarages::CountCarsInHideoutGarage
constexpr u32 kSafehouses = 0x96ABD8, kSafehouseBytes = 0x1400, kSafehouseModel = 0x12, kSafehouseStride = 0x100, kSlotStride = 0x40;
struct F5 { u32 typeArg; i32 index; u32 seed, zeroMask, eax, ecx, edx, stack; };
// Modelos distintos de cero que un comparador de 8 bits o con signo confundiria (byte bajo cero, bit alto, etc.)
inline constexpr std::array<u16, 7> kModelSpecial{1, 2, 0x100, 0x8000, 0xFF00, 0xFFFF, 0x00FF};
inline u16 StoredModel(u32 r) { return (r % 3) == 0 ? u16(0) : ((r >> 20) % 5) == 0 ? kModelSpecial[(r >> 24) % kModelSpecial.size()] : u16((r >> 8) | 1); }
inline constexpr std::array<i32, 10> kSafehouseEdge{-3, -2, -1, 20, 21, 22, 0, 19, 1, 10};
inline F5 MakeF5(unsigned n) {
    F5 f{};
    f.typeArg = (Rand(n, 1) & 0xFFFFFF00u) | (n % 256);
    f.index = (n % 7) != 6 ? i32(Rand(n, 2) % 20) : kSafehouseEdge[Rand(n, 2) % kSafehouseEdge.size()];
    f.seed = Rand(n, 3);
    f.zeroMask = Rand(n, 4);   // un bit por ranura: 1 = el modelo de esa ranura (del refugio probado) se pone a cero
    f.eax = Rand(n, 5); f.ecx = Rand(n, 6); f.edx = Rand(n, 7);
    f.stack = StackSeed(5, n);
    return f;
}

// ------------------------------------------------------------------------------------------------ 6 FindReplacementUpgradeCB
struct F6 { u8 type; u32 atomicId; i32 modelIndex; u32 objectSeed, dataSeed, eax, ecx, edx, stack; };
inline constexpr std::array<u32, 14> kAtomicIds{0u, 0x800u, 0x7FFu, 0xFFFFu, 0x1000u, 0x8000u, 0xFFFFFFFFu, 0x80000000u, 0x801u, 0xF7FFu, 1u, 2u, 0x8800u, 0x0u};
inline constexpr std::array<i32, 12> kModelIndexes{-1, -1, 0, 1, 0x7FFF, -2, 0x7FFFFFFF, i32(0x80000000), 0xFFFF, 1000, -32768, 184};
inline constexpr std::array<u8, 10> kObjectTypes{1, 1, 1, 1, 1, 0, 2, 3, 0xFF, 0x80};
inline F6 MakeF6(unsigned n) {
    F6 f{};
    f.type = (n % 8) < 5 ? u8(1) : (n % 8) < 7 ? kObjectTypes[Rand(n, 1) % kObjectTypes.size()] : u8(Rand(n, 1));
    f.atomicId = (n % 3) == 0 ? (Rand(n, 2) & ~0x800u) : (n % 3) == 1 ? (Rand(n, 2) | 0x800u) : kAtomicIds[Rand(n, 2) % kAtomicIds.size()];
    f.modelIndex = (n % 4) == 0 ? -1 : (n % 4) == 1 ? i32(i16(Rand(n, 3))) : kModelIndexes[Rand(n, 3) % kModelIndexes.size()];
    f.objectSeed = Rand(n, 4); f.dataSeed = Rand(n, 5);
    f.eax = Rand(n, 6); f.ecx = Rand(n, 7); f.edx = Rand(n, 8);
    f.stack = StackSeed(6, n);
    return f;
}

// ------------------------------------------------------------------------------------------------ 7 GetAnimHierarchyFromFrameCallback
constexpr unsigned kNodes = 8;
constexpr u32 kFrameStride = 0xA0;   // sizeof(RwFrame) aproximado: los falsos solo usan el cociente
struct F7 {
    unsigned nodeCount;
    u32 hierarchy[kNodes];
    int parent[kNodes];     // -1 para la raiz; el padre siempre tiene indice menor
    u32 found;
    u32 eax, ecx, edx, stack;
};
inline F7 MakeF7(unsigned n) {
    F7 f{};
    f.nodeCount = 1 + (n % 8);
    if (n % 11 == 0) f.nodeCount = 1;
    for (unsigned i = 0; i < kNodes; ++i) { f.hierarchy[i] = 0; f.parent[i] = -1; }
    for (unsigned i = 0; i < f.nodeCount; ++i) {
        const u32 r = Rand(n, 10 + i);
        const bool has = (r % 10) < ((n / 8) % 2 ? 4u : 2u);
        f.hierarchy[i] = has ? (Rand(n, 30 + i) | 1u) : 0u;
        if (i) f.parent[i] = int(Rand(n, 50 + i) % i);
    }
    // Una de cada 4 filas fuerza una jerarquia directa en la raiz; una de cada 4, ninguna en todo el arbol.
    if (n % 4 == 1) f.hierarchy[0] = Rand(n, 70) | 1u;
    if (n % 4 == 2) for (unsigned i = 0; i < kNodes; ++i) f.hierarchy[i] = 0;
    f.found = (n % 3) == 0 ? 0u : Rand(n, 71);
    f.eax = Rand(n, 72); f.ecx = Rand(n, 73); f.edx = Rand(n, 74);
    f.stack = StackSeed(7, n);
    return f;
}

// ------------------------------------------------------------------------------------------------ 8 CAEStreamingDecoder scalar deleting destructor
constexpr u32 kDecoderVtable = 0x85F418, kStreamBytes = 0x28;
struct F8 { u32 flags, streamPresent, filenameKind, decoderSeed, streamSeed, vptr, eax, edx, stack; };
inline constexpr std::array<u32, 14> kFlags{0u, 1u, 2u, 3u, 0x100u, 0x101u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000001u, 0x80000000u, 0x1FFu, 0xFFu, 0xFE, 0x12345679u};
inline F8 MakeF8(unsigned n) {
    F8 f{};
    f.flags = (n % 4) != 3 ? kFlags[(n + n / 4) % kFlags.size()] : Rand(n, 1);
    f.streamPresent = (n / 2) % 4 != 0;     // 75% con flujo
    f.filenameKind = (n / 8) % 4;           // 0 nulo, 1 puntero a bloque, 2 puntero arbitrario, 3 otro puntero arbitrario
    f.decoderSeed = Rand(n, 2); f.streamSeed = Rand(n, 3);
    f.vptr = (n % 5) == 0 ? kDecoderVtable : Rand(n, 4);
    f.eax = Rand(n, 5); f.edx = Rand(n, 6);
    f.stack = StackSeed(8, n);
    return f;
}
}   // namespace Dependencies25AFix
