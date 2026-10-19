#pragma once
// Fixtures del banco CBDNRenderCB30: CCustomBuildingDNPipeline::CustomPipeRenderCB (0x5D6480, cdecl, `ret` normal, 4 argumentos: RwResEntry* entry, void* object, uint8 type, uint32 flags).
// Solo C++ estandar (sin tipos del juego): las incluyen LiveComparison.hpp/Core.hpp y fixture_dump.cpp (autoprueba cruzada con el oraculo Python).
//
// 6144 casos = 12 grupos x 512 (grupo = n % 12 = escenario del numero de mallas: 0, 1, 2, 3, 4, 1, 2, 3, 4, -1, INT_MIN, 0). El oraculo (fixture_oracle.py) trabaja SOLO con la fila `input`
// (imagenes previas en hexadecimal) y modela el desensamblado del EXE; este archivo solo decide QUE estados se prueban.
//
// Modelo de memoria (todo con guardas, comparado entero tras cada llamada: la funcion no escribe en ninguna):
//   res    : RwResEntry (0x18 bytes) + cabecera D3D9 (0x40) + 4 mallas de 0x24 bytes. Cabecera: +4 numMeshes, +8 indexBuffer, +0x10.. flujos, +0x30 useOffsets, +0x34 vertexDeclaration.
//            Malla: +8 puntero al material, +0xC alfa por vertice.
//   mat[4] : ventana de 0x80 bytes: +0 textura (valor crudo), +4 color RGBA (alfa en +7), +0xC propiedades de superficie, +0x10 banderas (bit 0 = mapa de entorno), palabra en +envOff =
//            puntero al dato de entorno (plugin 0x8D12C4, desplazamiento leido del juego en cada caso).
//   env[4] : datos de entorno de 16 bytes: +0 escala X (int8), +1 escala Y (int8), +4 brillo (byte), +8 textura (valor crudo).
//   obj    : objeto falso (solo su direccion se pasa al falso de _rwD3D9EnableClippingIfNeeded: nunca se desreferencia).
//   mtx    : 64 bytes previos de la matriz de textura global 0xC02C28 (solo se escribe su diagonal).
// Los punteros a texturas, buffers y declaraciones son VALORES CRUDOS (nunca desreferenciados); en la traza aparecen tal cual.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

namespace CBDNRenderFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kGroups = 12;
constexpr unsigned kPerGroup = 512;
constexpr unsigned kCases = kGroups * kPerGroup;   // 6144

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kMaxMeshes = 4;
constexpr u32 kEntryHdr = 0x18;                                       // RwResEntry
constexpr u32 kHeaderSize = 0x40;                                     // RxD3D9ResEntryHeader
constexpr u32 kMeshSize = 0x24;                                       // RxD3D9InstanceData
constexpr u32 kResSize = kEntryHdr + kHeaderSize + kMaxMeshes * kMeshSize;   // 0x18 + 0x40 + 0x90 = 0xE8
constexpr u32 kResGuard = 0x40;
constexpr u32 kResAt = kResGuard;
constexpr u32 kResArena = kResGuard + kResSize + kResGuard;           // 0x168
constexpr u32 kNumMats = 4;
constexpr u32 kMatWin = 0x80;
constexpr u32 kMatGuard = 0x20;
constexpr u32 kMatAt = kMatGuard;
constexpr u32 kMatArena = kMatGuard + kMatWin + kMatGuard;            // 0xC0
constexpr u32 kNumEnvs = 4;
constexpr u32 kEnvSize = 0x10;
constexpr u32 kEnvGuard = 0x20;
constexpr u32 kEnvAt = kEnvGuard;
constexpr u32 kEnvArena = kEnvGuard + kEnvSize + kEnvGuard;           // 0x50
constexpr u32 kObjSize = 0x20;
constexpr u32 kObjGuard = 0x20;
constexpr u32 kObjAt = kObjGuard;
constexpr u32 kObjArena = kObjGuard + kObjSize + kObjGuard;           // 0x60
constexpr u32 kObjProbeOffset = 0x10;                                 // el objeto que se pasa es obj + 0x10
constexpr u32 kMtxSize = 64;
constexpr u32 kGuardByte = 0xA7;
constexpr u32 kBogusBase = 0xC0DE0000u;                               // punteros basura (nunca desreferenciados)
constexpr u32 kEnvSlotNull = 0, kEnvSlotBase = 1;                     // etiqueta de la ranura de entorno: 0 nulo, 1..4 = env[i], otro = basura

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

struct Gen {
    u32 c{}, k{};
    explicit Gen(u32 seed) : c(seed) {}
    u32 operator()(u32 m) { return m ? Rand(c, k++) % m : (++k, 0u); }
    u32 Raw() { return Rand(c, k++); }
    bool One(u32 den) { return (*this)(den) == 0; }
};

struct Fx {
    unsigned n{};
    int group{};
    u32 numMeshes{};                       // palabra +4 de la cabecera (con signo en el EXE)
    u32 lighting{};                        // lo que devuelve el falso de RwD3D9GetRenderState(D3DRS_LIGHTING)
    u32 flags{};                           // 4.o argumento (rxGeoFlags)
    u32 typeArg{};                         // 3.er argumento tal como se empuja (solo importa el byte bajo)
    u32 objKind{};                         // 0 = obj + 0x10, 1 = nulo, 2 = valor crudo
    u32 indexBuffer{}, useOffsets{}, declaration{};
    std::array<u8, kResSize> res{};        // imagen previa de RwResEntry + cabecera + mallas (las palabras +4/+8/+0x30/+0x34 de la cabecera y +8 de cada malla ya puestas SALVO el puntero al material)
    std::array<u32, kMaxMeshes> meshMat{}; // indice de material de cada malla
    std::array<std::array<u8, kMatWin>, kNumMats> mat{};   // imagen previa (palabra en envOff a cero: ver matSlot)
    std::array<u32, kNumMats> matSlot{};   // etiqueta de la ranura de entorno de cada material
    std::array<std::array<u8, kEnvSize>, kNumEnvs> env{};
    std::array<u8, kMtxSize> mtx{};
    u32 eax{}, ecx{}, edx{}, stack{};
};

inline constexpr u32 kNumMeshesByGroup[12] = {0, 1, 2, 3, 4, 1, 2, 3, 4, 0xFFFFFFFFu, 0x80000000u, 0};
inline constexpr u32 kLighting[] = {0, 0, 0, 0, 1, 1, 1, 2, 0x100, 0xFFFFFFFFu, 0x80000000u};
inline constexpr u32 kFlags[] = {0, 8, 0x108, 0x100, 0x80000008u, 0xFFFFFFF7u, 0xF, 0xFFFFFFFFu, 4, 0x18, 0x48, 0x07};
inline constexpr u32 kVertexAlpha[] = {0, 0, 0, 1, 0x100, 0xFF, 0x80000000u, 0xFFFFFFFFu, 2};
inline constexpr u32 kScale[] = {0, 1, 0x7F, 0x80, 0xFF, 0xFE, 0x40, 0xC0, 8, 0xF8};
inline constexpr u32 kShin[] = {0, 1, 2, 3, 0x7F, 0x80, 0xFE, 0xFF};
inline constexpr u32 kAlpha[] = {0xFF, 0xFF, 0xFF, 0xFE, 0, 1, 0x80, 0x7F};
inline constexpr u32 kUseOffsets[] = {0, 1, 2, 0x100, 0xFFFFFFFFu, 0x80000000u};

inline Fx Make(unsigned n) {
    Fx f;
    f.n = n;
    f.group = int(n % kGroups);
    Gen g(n * 2654435761u + 91u);
    f.numMeshes = kNumMeshesByGroup[f.group];
    f.lighting = kLighting[g(sizeof(kLighting) / sizeof(u32))];
    f.flags = g(3) == 0 ? g.Raw() : kFlags[g(sizeof(kFlags) / sizeof(u32))];
    {
        const u32 low = g(4) == 0 ? g.Raw() & 0xFF : (g(2) ? 1u : 0xFFu);
        f.typeArg = g(2) ? (g.Raw() & 0xFFFFFF00u) | low : low;
    }
    {
        const u32 r = g(10);
        f.objKind = r == 0 ? 1 : (r == 1 ? 2 : 0);
    }
    f.indexBuffer = g(2) ? 0 : (kBogusBase + 0x100 + (g.Raw() & 0xFFF0u));
    f.useOffsets = g(2) ? kUseOffsets[g(sizeof(kUseOffsets) / sizeof(u32))] : g.Raw();
    f.declaration = g(4) == 0 ? 0 : (kBogusBase + 0x200 + (g.Raw() & 0xFFF0u));
    FillWords(f.res.data(), f.res.size(), n * 40503u + 1u);
    {
        auto put = [&](u32 off, u32 v) { std::memcpy(f.res.data() + off, &v, 4); };
        put(kEntryHdr + 4, f.numMeshes);
        put(kEntryHdr + 8, f.indexBuffer);
        put(kEntryHdr + 0x30, f.useOffsets);
        put(kEntryHdr + 0x34, f.declaration);
        for (u32 m = 0; m < kMaxMeshes; ++m) {
            const u32 at = kEntryHdr + kHeaderSize + m * kMeshSize;
            put(at + 8, 0);                                             // puntero al material: lo pone Core (segun meshMat)
            put(at + 0xC, kVertexAlpha[g(sizeof(kVertexAlpha) / sizeof(u32))]);
            if (g(5) == 0) put(at + 0xC, g.Raw());
        }
    }
    for (u32 m = 0; m < kMaxMeshes; ++m) f.meshMat[m] = (m > 0 && g(4) == 0) ? f.meshMat[m - 1] : g(kNumMats);
    for (u32 i = 0; i < kNumMats; ++i) {
        FillWords(f.mat[i].data(), kMatWin, n * 40503u + 10u + i);
        auto put = [&](u32 off, u32 v) { std::memcpy(f.mat[i].data() + off, &v, 4); };
        f.mat[i][7] = u8(kAlpha[g(sizeof(kAlpha) / sizeof(u32))]);
        if (g(6) == 0) f.mat[i][7] = u8(g.Raw());
        if (g(3) != 0) put(0, 0xB0000000u + (g.Raw() & 0xFFFFFu));      // textura cruda no nula
        else put(0, 0);
        u32 flagsWord = g(3) == 0 ? g.Raw() : (g(2) ? 0xFFFFFFFEu : 0u);
        flagsWord = (flagsWord & ~1u) | (g(2) ? 1u : 0u);               // bit 0 = mapa de entorno
        put(0x10, flagsWord);
        if (flagsWord & 1) f.matSlot[i] = kEnvSlotBase + g(kNumEnvs);   // con el bit: dato valido
        else {
            const u32 r = g(10);
            f.matSlot[i] = r < 3 ? kEnvSlotNull : (r < 6 ? kEnvSlotBase + g(kNumEnvs) : kBogusBase + 0x300 + (g.Raw() & 0xFFF0u));
        }
    }
    for (u32 j = 0; j < kNumEnvs; ++j) {
        FillWords(f.env[j].data(), kEnvSize, n * 40503u + 20u + j);
        f.env[j][0] = u8(g(3) == 0 ? g.Raw() : kScale[g(sizeof(kScale) / sizeof(u32))]);
        f.env[j][1] = u8(g(3) == 0 ? g.Raw() : kScale[g(sizeof(kScale) / sizeof(u32))]);
        f.env[j][4] = u8(g(3) == 0 ? g.Raw() : kShin[g(sizeof(kShin) / sizeof(u32))]);
        const u32 tex = g(4) == 0 ? 0 : (0xD0000000u + (g.Raw() & 0xFFFFFu));
        std::memcpy(f.env[j].data() + 8, &tex, 4);
    }
    FillWords(f.mtx.data(), f.mtx.size(), n * 40503u + 30u);
    f.eax = g.Raw(); f.ecx = g.Raw(); f.edx = g.Raw();
    f.stack = kStackSeeds[g(8)];
    return f;
}

// ---- imagen de la ventana de un material para un desplazamiento de plugin dado (la palabra de la ranura de entorno queda a cero)
inline std::array<u8, kMatWin> MatImage(const Fx& f, u32 i, u32 envOff) {
    auto m = f.mat[i];
    const u32 zero = 0;
    std::memcpy(m.data() + envOff, &zero, 4);
    return m;
}

inline std::string Hex(const u8* p, std::size_t n) {
    constexpr char d[] = "0123456789abcdef";
    std::string o;
    o.reserve(n * 2);
    for (std::size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; }
    return o;
}

// Fila `input` (JSON). La usan Core.hpp (nlohmann) y fixture_dump.cpp: el mismo texto en ambos sitios.
inline std::string InputJson(const Fx& f, u32 envOff) {
    std::string s = "{";
    char b[96];
    auto num = [&](const char* k, unsigned long long v) { std::snprintf(b, sizeof b, "\"%s\":%llu,", k, v); s += b; };
    auto hex = [&](const char* k, const u8* p, std::size_t len) { s += "\""; s += k; s += "\":\""; s += Hex(p, len); s += "\","; };
    num("group", unsigned(f.group)); num("n", f.n); num("env_off", envOff);
    num("num_meshes", f.numMeshes); num("lighting", f.lighting); num("flags", f.flags); num("type_arg", f.typeArg); num("obj_kind", f.objKind);
    hex("res", f.res.data(), f.res.size());
    s += "\"mesh_mat\":[";
    for (u32 m = 0; m < kMaxMeshes; ++m) { std::snprintf(b, sizeof b, "%s%u", m ? "," : "", f.meshMat[m]); s += b; }
    s += "],\"mat\":[";
    for (u32 i = 0; i < kNumMats; ++i) { if (i) s += ","; const auto m = MatImage(f, i, envOff); s += "\""; s += Hex(m.data(), m.size()); s += "\""; }
    s += "],\"mat_slot\":[";
    for (u32 i = 0; i < kNumMats; ++i) { std::snprintf(b, sizeof b, "%s%u", i ? "," : "", f.matSlot[i]); s += b; }
    s += "],\"env\":[";
    for (u32 j = 0; j < kNumEnvs; ++j) { if (j) s += ","; s += "\""; s += Hex(f.env[j].data(), kEnvSize); s += "\""; }
    s += "],";
    hex("mtx", f.mtx.data(), f.mtx.size());
    num("eax", f.eax); num("ecx", f.ecx); num("edx", f.edx);
    std::snprintf(b, sizeof b, "\"stack_seed\":%u", f.stack); s += b;
    s += "}";
    return s;
}
}  // namespace CBDNRenderFix
