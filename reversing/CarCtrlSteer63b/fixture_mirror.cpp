// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras del generador pseudoaleatorio (R) y constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace CS63BFix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("C\t%s\n", JsonSpec(MakeSpec(n)).c_str());
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2));
    std::printf("S\t{\"veh\":%u,\"pos\":%u,\"mat\":%u,\"speed\":%u,\"mission\":%u,\"recid\":%u,\"mtop\":%u,\"mpos\":%u,\"frame\":%u,\"paused\":%u,\"speedarr\":%u,\"size\":%u,\"index\":%u,\"buffer\":%u,\"kinds\":%u}\n",
                kVehSize, kVPos, kVMat, kVSpeed, kVMission, kVRecId, kMTop, kMPos, kFrameSize, kPausedAddr, kSpeedAddr, kSizeAddr, kIndexAddr, kBufferAddr, kKinds);
    return 0;
}
