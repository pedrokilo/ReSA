// Prueba aislada de las compuertas del banco (GateCdecl1/GateCdecl3/GateThis5, PrimeX87, SaveFP/RestoreFP) con los bytes reales del EXE reubicados.
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <array>
#include <vector>
#include <string>
#include <float.h>
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using int32 = std::int32_t; using int16 = std::int16_t;
#include "exebytes.inc"
#include "gates_extract.hpp"
void __cdecl QuaternionSlerpCpp(float* out, const float* from, const float* to, float theta, float invSin, float t);
static uint8* Alloc(size_t n) { return (uint8*)VirtualAlloc(nullptr, n, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); }
static float F(uint32_t b) { float f; memcpy(&f, &b, 4); return f; }
// Adaptador thiscall -> cdecl del candidato (equivale al salto del hook a CQuaternion::Slerp)
static void __declspec(naked) SlerpThiscallThunk() {
    __asm {
        // ecx = out; pila: ret, from, to, theta, inv, t  (ret 0x14)
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push ecx
        call QuaternionSlerpCpp
        add esp, 24
        ret 0x14
    }
}
int main() {
    using namespace Gates;
    uint8* code = Alloc(0x1000); memcpy(code, kSlerp, sizeof(kSlerp));
    static uint32_t consts[4] = {0, 0x3FC90FDB, 0x40490FDB, 0x3F800000};
    const uint32_t olds[4] = {0x858b50, 0x858fe4, 0x858cb8, 0x858624};
    for (size_t i = 0; i + 4 <= sizeof(kSlerp); ++i) for (int k = 0; k < 4; ++k) if (*(uint32_t*)(code + i) == olds[k]) *(uint32_t*)(code + i) = (uint32_t)&consts[k];
    alignas(16) std::array<uint8, 108> savedFP{}; SaveFP(savedFP.data());
    long bad = 0, n = 0;
    uint32_t TH[] = {0x3F800000, 0x40000000, 0x3E800000, 0x7FC00000, 0x00000000};
    for (uint32_t th : TH) for (int side = 0; side < 2; ++side) {
        alignas(16) uint8 buf[64]; memset(buf, 0xA5, sizeof buf);
        float from[4] = {0, 0, 0, 1}, to[4] = {0.7071068f, 0, 0, 0.7071068f};
        memcpy(buf + 16, from, 16); memcpy(buf + 32, to, 16);
        RestoreFP(savedFP.data()); PrimeX87(); _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
        uintptr_t entry = side == 0 ? (uintptr_t)code : (uintptr_t)&SlerpThiscallThunk;
        Reg r = GateThis5(entry, (uint32)(buf), (uint32)(buf + 16), (uint32)(buf + 32), th, 0x3F800000, 0x3F000000);
        alignas(16) std::array<uint8, 108> fp{}; SaveFP(fp.data()); RestoreFP(savedFP.data());
        printf("side=%d th=%08x stack=%d nv=%d out=%08x %08x %08x %08x fsw=%04x ftw=%04x ecx=%08x\n", side, th, r.stack, r.nonvolatile, *(uint32_t*)buf, *(uint32_t*)(buf + 4), *(uint32_t*)(buf + 8), *(uint32_t*)(buf + 12), *(uint16_t*)(fp.data() + 4), *(uint16_t*)(fp.data() + 8), r.ecx);
        (void)F; ++n; if (!r.stack || !r.nonvolatile) ++bad;
    }
    printf("gate pruebas=%ld fallos=%ld\n", n, bad);
    return 0;
}
