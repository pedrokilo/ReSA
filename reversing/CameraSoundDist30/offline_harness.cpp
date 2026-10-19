// Arnes SIN juego (32 bits) del banco CameraSoundDist30: ejecuta los ayudantes REALES de X87Math.cpp (CamSoundPointCpp, CamSoundHeightCpp, CamSoundBlendCpp, compilados con
// /O2 /Oi /arch:IA32 /fp:fast como en el proyecto) dentro de una copia fiel del flujo de CCamera::UpdateSoundDistances (Camera.cpp) sobre la misma arena que Core.hpp, con el colaborador
// CWorld::ProcessVerticalLine sustituido por el mismo falso, y escribe una fila JSON por caso con la misma forma que `observed` de Core.hpp. offline_check.py compara cada fila con el oraculo.
// No sustituye al banco dentro del juego (no ejecuta el cuerpo compilado de Camera.cpp ni el EXE); comprueba los ayudantes x87 y el flujo contra el oraculo derivado del desensamblado.
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <float.h>
#include <string>
#include <vector>
#include "Fixtures.hpp"
#include "X87Math.h"
using namespace CameraSoundDist30Fix;

static std::vector<u32> trace;
static unsigned calls;
static u32 gHit, gHitZ;

// CWorld::ProcessVerticalLine (falso identico al de Core.hpp)
static bool FakeProcessVerticalLine(const float* origin, float distance, u32* colPoint, void** outEntity, bool b, bool v, bool p, bool o, bool d, bool s, void* poly) {
    ++calls;
    for (int i = 0; i < 3; ++i) { u32 w; std::memcpy(&w, origin + i, 4); trace.push_back(w); }
    u32 dw; std::memcpy(&dw, &distance, 4); trace.push_back(dw);
    for (const bool x : {b, v, p, o, d, s}) trace.push_back(x ? 1u : 0u);
    trace.push_back(u32(reinterpret_cast<std::uintptr_t>(poly)));
    colPoint[0] = 0x3F000000u; colPoint[1] = 0xBF000000u; colPoint[2] = gHitZ;
    *outEntity = nullptr;
    return gHit != 0;
}

template <class T> static T& Field(u8* camera, u32 offset) { return *reinterpret_cast<T*>(camera + offset); }

// Copia fiel de CCamera::UpdateSoundDistances (Camera.cpp) con la memoria cruda de la arena
static void UpdateSoundDistances(u8* camera, u32 frameCounter) {
    const u32 s5 = 0x40A00000u, sHalf = 0x3F000000u, twentyBits = 0x41A00000u, sixthBits = 0x3E2AAAABu, oneBits = 0x3F800000u;
    float scale; std::memcpy(&scale, &s5, 4);
    switch (static_cast<short>(Field<u32>(camera, kOffCams + camera[kOffActive] * kCamStride + kOffCamMode))) {
    case 0x10: case 0x07: case 0x27: case 0x28: case 0x34: case 0x2A: case 0x2B:
    case 0x29: case 0x2D: case 0x2E: case 0x22: case 0x08: case 0x33:
        if ((Field<u8*>(camera, kOffTarget)[kEntityTypeOffset] & 7) == 3) std::memcpy(&scale, &sHalf, 4);
        break;
    default:
        break;
    }
    float twenty, sixth, one; std::memcpy(&twenty, &twentyBits, 4); std::memcpy(&sixth, &sixthBits, 4); std::memcpy(&one, &oneBits, 4);
    u8* matrix = Field<u8*>(camera, kOffMatrix);
    const float* position = matrix ? reinterpret_cast<float*>(matrix + kMatrixPosOffset) : reinterpret_cast<float*>(camera + kOffPlacePos);
    float point[3], rayEnd;
    CamSoundPointCpp(&Field<float>(camera, kOffForward), scale, position, twenty, point, &rayEnd);
    const unsigned short phase = static_cast<unsigned short>(frameCounter % 12u);
    if (phase == 0) {
        Field<u32>(camera, kOffSoundOld) = Field<u32>(camera, kOffSoundNew);
        u32 colPoint[11]; void* hitEntity;
        if (FakeProcessVerticalLine(point, rayEnd, colPoint, &hitEntity, true, false, false, false, true, false, nullptr)) {
            float colZ; std::memcpy(&colZ, &colPoint[2], 4);
            Field<float>(camera, kOffSoundNew) = CamSoundHeightCpp(colZ, point[2]);
        } else {
            Field<u32>(camera, kOffSoundNew) = 0x41A00000u;
        }
    }
    Field<float>(camera, kOffSoundUp) = CamSoundBlendCpp(static_cast<int>(static_cast<short>(phase)) + 1, sixth, one, Field<float>(camera, kOffSoundOld), Field<float>(camera, kOffSoundNew));
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
        u8* const obj = arena + kObjOffset; u8* const entity = arena + kEntityOffset; u8* const matrix = arena + kMatrixOffset;
        std::memset(ref, 0xC3, sizeof ref);
        u8* const ro = ref + kObjOffset; u8* const re = ref + kEntityOffset; u8* const rm = ref + kMatrixOffset;
        FillWords(ro, kObjBytes, f.headSeed); FillWords(re, kEntityBytes, f.entSeed); FillWords(rm, kMatrixBytes, f.matSeed);
        auto put = [](u8* base, u32 off, u32 value) { std::memcpy(base + off, &value, 4); };
        for (u32 i = 0; i < 3; ++i) { put(ro, kOffPlacePos + 4 * i, f.placePos[i]); put(rm, kMatrixPosOffset + 4 * i, f.matPos[i]); put(ro, kOffForward + 4 * i, f.fwd[i]); }
        put(ro, kOffMatrix, f.hasMatrix ? u32(reinterpret_cast<std::uintptr_t>(matrix)) : 0u);
        ro[kOffActive] = u8(f.active);
        for (u32 i = 0, other = 0; i < 3; ++i) put(ro, kOffCams + i * kCamStride + kOffCamMode, i == f.active ? f.mode : f.otherModes[other++]);
        put(ro, kOffTarget, u32(reinterpret_cast<std::uintptr_t>(entity)));
        put(ro, kOffSoundUp, f.old158); put(ro, kOffSoundNew, f.old15C); put(ro, kOffSoundOld, f.old160);
        re[kEntityTypeOffset] = u8(f.etype);
        std::memcpy(arena, ref, kArena);
        trace.clear(); calls = 0; gHit = f.hit; gHitZ = f.hitZ;
        UpdateSoundDistances(obj, f.frame);
        bool rest = std::memcmp(obj, ro, kOffSoundUp) == 0 && std::memcmp(obj + kOffSoundUp + kWrittenBytes, ro + kOffSoundUp + kWrittenBytes, kObjBytes - kOffSoundUp - kWrittenBytes) == 0;
        bool guards = std::memcmp(arena, ref, kObjOffset) == 0 && std::memcmp(arena + kObjOffset + kObjBytes, ref + kObjOffset + kObjBytes, kGuard) == 0
                   && std::memcmp(arena + kEntityOffset + kEntityBytes, ref + kEntityOffset + kEntityBytes, kGuard) == 0 && std::memcmp(arena + kMatrixOffset + kMatrixBytes, ref + kMatrixOffset + kMatrixBytes, kGuard) == 0;
        std::printf("{\"case\":%u,\"written\":\"%s\",\"rest_ok\":%s,\"guard_ok\":%s,\"entity_ok\":%s,\"matrix_ok\":%s,\"frame_ok\":true,\"calls\":%u,\"trace\":[", n, Hex(obj + kOffSoundUp, kWrittenBytes).c_str(),
            rest ? "true" : "false", guards ? "true" : "false", std::memcmp(entity, re, kEntityBytes) == 0 ? "true" : "false", std::memcmp(matrix, rm, kMatrixBytes) == 0 ? "true" : "false", calls);
        for (std::size_t i = 0; i < trace.size(); ++i) std::printf(i ? ",%u" : "%u", trace[i]);
        std::printf("]}\n");
    }
    return 0;
}
