// Arnes SIN juego (32 bits) del banco CameraShake30: ejecuta el ayudante REAL de X87Math.cpp (CamShakeOrientCpp sobre FlcX87, compilado con /O2 /Oi /arch:IA32 /fp:fast como en el proyecto)
// dentro de una copia fiel del flujo de CCamera::ProcessShake (Camera.cpp) sobre la misma arena que Core.hpp, con la tabla de CHandShaker y su bandera en memoria propia y CHandShaker::Process
// sustituido por el mismo falso. La lista `put(...)` de inicializacion se extrae del propio Camera.cpp (offline_check.py genera init_list.inc). Escribe una fila JSON por caso con la forma de
// `observed` de Core.hpp; offline_check.py compara cada fila con el oraculo. No sustituye al banco dentro del juego: comprueba el ayudante x87 y el flujo contra el oraculo derivado del desensamblado.
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <float.h>
#include <string>
#include <vector>
#include "Fixtures.hpp"
#include "X87Math.h"
using namespace CameraShake30Fix;

alignas(16) static u8 table[kTableBytes];
static u8 flagByte;
static std::vector<u32> trace;
static unsigned calls;
static u32 gAngZ, gMat[9];

// CHandShaker::Process (falso identico al de Core.hpp)
static void FakeProcess(u8* self, float degree) {
    ++calls;
    trace.push_back(u32((static_cast<int>(reinterpret_cast<std::uintptr_t>(self)) - static_cast<int>(reinterpret_cast<std::uintptr_t>(table))) / static_cast<int>(kEntryBytes)));
    u32 d; std::memcpy(&d, &degree, 4); trace.push_back(d);
    std::memcpy(self + kEntryAngZ, &gAngZ, 4);
    for (u32 i = 0; i < 9; ++i) std::memcpy(self + kEntryMatrix + 4 * kMatrixWordIndex[i], &gMat[i], 4);
}

template <class T> static T& Field(u8* camera, u32 offset) { return *reinterpret_cast<T*>(camera + offset); }

// Copia fiel de CCamera::ProcessShake (Camera.cpp) con la tabla y la bandera en memoria propia
static void ProcessShake(u8* camera) {
    if (!flagByte) {
        const auto put = [](unsigned address, u32 bits) { std::memcpy(table + (address - kTableAddress), &bits, 4); };
#include "init_list.inc"
        flagByte = 1;
    }
    u8* const cam = camera + kOffCams + camera[kOffActive] * kCamStride;
    u8* const shaker = table + static_cast<u32>(Field<int>(camera, kOffType)) * kEntryBytes;
    const float intensity = Field<float>(camera, kOffIntensity);
    FakeProcess(shaker, intensity);
    CamShakeOrientCpp(reinterpret_cast<float*>(cam + kOffFront), reinterpret_cast<float*>(cam + kOffUp), reinterpret_cast<const float*>(shaker + kEntryMatrix),
                      *reinterpret_cast<const float*>(shaker + kEntryAngZ), intensity);
}

static std::string Hex(const void* p, std::size_t n) {
    static const char d[] = "0123456789abcdef";
    std::string o;
    for (std::size_t i = 0; i < n; ++i) { const u8 b = static_cast<const u8*>(p)[i]; o += d[b >> 4]; o += d[b & 15]; }
    return o;
}

alignas(16) static u8 arena[kArena];

int main() {
    _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
    static u8 ref[kArena];
    for (unsigned n = 0; n < kCases; ++n) {
        const F f = MakeF(n);
        u8* const obj = arena + kObjOffset;
        const u32 camBase = kOffCams + f.active * kCamStride;
        std::memset(ref, 0xC3, sizeof ref);
        u8* const ro = ref + kObjOffset;
        FillWords(ro, kObjBytes, f.headSeed);
        auto put = [](u8* base, u32 off, u32 value) { std::memcpy(base + off, &value, 4); };
        ro[kOffActive] = u8(f.active);
        put(ro, kOffIntensity, f.intensity); put(ro, kOffType, f.type);
        for (u32 i = 0; i < 3; ++i) { put(ro, camBase + kOffFront + 4 * i, f.front[i]); put(ro, camBase + kOffUp + 4 * i, f.up[i]); }
        u8 tableRef[kTableBytes];
        FillWords(tableRef, kTableBytes, f.tableSeed);
        std::memcpy(arena, ref, kArena);
        std::memcpy(table, tableRef, kTableBytes); flagByte = u8(f.flag);
        trace.clear(); calls = 0; gAngZ = f.angZ; for (u32 i = 0; i < 9; ++i) gMat[i] = f.mat[i];
        ProcessShake(obj);
        bool guards = std::memcmp(arena, ref, kObjOffset) == 0 && std::memcmp(arena + kObjOffset + kObjBytes, ref + kObjOffset + kObjBytes, kGuard) == 0;
        u8 a[kObjBytes], b[kObjBytes];
        std::memcpy(a, obj, kObjBytes); std::memcpy(b, ro, kObjBytes);
        std::memset(a + camBase + kOffFront, 0, 12); std::memset(b + camBase + kOffFront, 0, 12); std::memset(a + camBase + kOffUp, 0, 12); std::memset(b + camBase + kOffUp, 0, 12);
        const bool rest = std::memcmp(a, b, kObjBytes) == 0;
        std::printf("{\"case\":%u,\"front\":\"%s\",\"up\":\"%s\",\"rest_ok\":%s,\"guard_ok\":%s,\"table\":\"%s\",\"flag\":%d,\"calls\":%u,\"trace\":[", n, Hex(obj + camBase + kOffFront, 12).c_str(), Hex(obj + camBase + kOffUp, 12).c_str(),
            rest ? "true" : "false", guards ? "true" : "false", Hex(table, kTableBytes).c_str(), int(flagByte), calls);
        for (std::size_t i = 0; i < trace.size(); ++i) std::printf(i ? ",%u" : "%u", trace[i]);
        std::printf("]}\n");
    }
    return 0;
}
