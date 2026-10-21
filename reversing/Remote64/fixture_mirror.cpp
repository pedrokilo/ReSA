// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras del generador pseudoaleatorio (R) y constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace R64Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn) {
        const u32 cases = fn == kGive ? kCasesGive : kCasesTake;
        for (u32 n = 0; n < cases; ++n) std::printf("C%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    }
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2));
    std::printf("J\t[%u,%u,%u,%u]\n", VehJunk(77u, 3u, 100u), MatJunk(77u, 3u, 5u), VehJunk(0xFFFFu, 5u, 645u), MatJunk(123456u, 2u, 23u));
    std::printf("S\t{\"veh\":%u,\"max_veh\":%u,\"mat\":%u,\"states\":%u,\"base\":%u,\"focus\":%u,\"timer\":%u,\"pool\":%u,\"players\":%u,\"stride\":%u,\"remote\":%u,\"dd\":%u,\"de\":%u,\"df\":%u,\"time\":%u,"
                "\"camera\":%u,\"cleanup\":%u,\"none\":%u,\"kinds\":%u}\n",
                kVehSize, kMaxVeh, kMatWords, kStatesBytes, kStatesBase, kFocusAddr, kTimerAddr, kPoolPtrAddr, kPlayers, kPlayerStride, kRemoteOff, kFlagDD, kFlagDE, kFlagDF, kTimeOff, kCamera, kMissionCleanup, kNone, kKinds);
    return 0;
}
