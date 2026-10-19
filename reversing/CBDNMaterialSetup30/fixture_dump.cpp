// Volcado de Fixtures.hpp para la autoprueba cruzada con fixture_oracle.py (sin juego): constantes de disposicion y la fila `input` de cada caso.
// Compilar:  cl /nologo /EHsc /std:c++20 fixture_dump.cpp   (ver fixture_check.py, que lo hace y compara con el oraculo)
#include <cstdio>
#include <cstdlib>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace CBDNMatFix;
int main(int argc, char** argv) {   // sin argumentos: desplazamientos de plugin variados por caso; con `envOff mfxOff` (decimal o 0x..): fijos
    std::printf("{\"event\":\"constants\",\"kMatGuard\":%u,\"kMatAt\":%u,\"kMatWin\":%u,\"kMatArena\":%u,\"kEnvGuard\":%u,\"kEnvAt\":%u,\"kEnvSize\":%u,\"kEnvArena\":%u,\"kTexGuard\":%u,\"kTexAt\":%u,\"kTexSize\":%u,\"kTexArena\":%u,\"kMfxGuard\":%u,\"kMfxAt\":%u,\"kMfxSize\":%u,\"kMfxArena\":%u,\"kGuardByte\":%u,\"kCases\":%u,\"kGroups\":%u}\n",
                kMatGuard, kMatAt, kMatWin, kMatArena, kEnvGuard, kEnvAt, kEnvSize, kEnvArena, kTexGuard, kTexAt, kTexSize, kTexArena, kMfxGuard, kMfxAt, kMfxSize, kMfxArena, kGuardByte, kCases, kGroups);
    static const u32 offs[][2] = {{0x28, 0x1C}, {0x1C, 0x20}, {0x30, 0x2C}, {0x7C, 0x14}, {0x14, 0x18}};
    for (unsigned n = 0; n < kCases; ++n) {
        u32 e = offs[n % 5][0], m = offs[n % 5][1];
        if (argc >= 3) { e = u32(std::strtoul(argv[1], nullptr, 0)); m = u32(std::strtoul(argv[2], nullptr, 0)); }
        std::printf("%s\n", InputJson(Make(n), e, m).c_str());
    }
}
