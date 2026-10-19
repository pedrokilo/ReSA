// Volcado de Fixtures.hpp para la autoprueba cruzada con fixture_oracle.py (sin juego): constantes de disposicion y la fila `input` de cada caso.
// Compilar:  cl /nologo /EHsc /std:c++20 fixture_dump.cpp   (ver fixture_check.py, que lo hace y compara con el oraculo)
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace CBDNRenderFix;
int main(int argc, char** argv) {   // sin argumentos: desplazamiento de plugin variado por caso; con `envOff` (decimal o 0x..): fijo
    std::printf("{\"event\":\"constants\",\"kMaxMeshes\":%u,\"kEntryHdr\":%u,\"kHeaderSize\":%u,\"kMeshSize\":%u,\"kResSize\":%u,\"kResGuard\":%u,\"kResAt\":%u,\"kResArena\":%u,\"kNumMats\":%u,\"kMatWin\":%u,\"kMatGuard\":%u,"
                "\"kMatAt\":%u,\"kMatArena\":%u,\"kNumEnvs\":%u,\"kEnvSize\":%u,\"kEnvGuard\":%u,\"kEnvAt\":%u,\"kEnvArena\":%u,\"kObjSize\":%u,\"kObjGuard\":%u,\"kObjAt\":%u,\"kObjArena\":%u,"
                "\"kObjProbeOffset\":%u,\"kMtxSize\":%u,\"kGuardByte\":%u,\"kBogusBase\":%u,\"kEnvSlotNull\":%u,\"kEnvSlotBase\":%u,\"kCases\":%u,\"kGroups\":%u}\n",
                kMaxMeshes, kEntryHdr, kHeaderSize, kMeshSize, kResSize, kResGuard, kResAt, kResArena, kNumMats, kMatWin, kMatGuard, kMatAt, kMatArena, kNumEnvs, kEnvSize, kEnvGuard, kEnvAt, kEnvArena,
                kObjSize, kObjGuard, kObjAt, kObjArena, kObjProbeOffset, kMtxSize, kGuardByte, kBogusBase, kEnvSlotNull, kEnvSlotBase, kCases, kGroups);
    static const u32 offs[] = {0x28, 0x1C, 0x30, 0x7C, 0x14};
    for (unsigned n = 0; n < kCases; ++n) std::printf("%s\n", InputJson(Make(n), argc >= 2 ? u32(std::strtoul(argv[1], nullptr, 0)) : offs[n % 5]).c_str());
}
