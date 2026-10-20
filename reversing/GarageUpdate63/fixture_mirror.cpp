// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras del generador pseudoaleatorio (R) y constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace GU63Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("C\t%s\n", JsonSpec(MakeSpec(n)).c_str());
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2));
    std::printf("S\t{\"garage\":%u,\"veh\":%u,\"max_veh\":%u,\"left\":%u,\"right\":%u,\"front\":%u,\"back\":%u,\"posz\":%u,\"topz\":%u,\"door\":%u,\"time\":%u,\"target\":%u,\"type\":%u,\"state\":%u,\"flags\":%u,"
                "\"timer\":%u,\"frame\":%u,\"focus\":%u,\"money\":%u,\"nores\":%u,\"free\":%u,\"bombs\":%u,\"lastg\":%u,\"lasth\":%u,\"cam\":%u,\"camin\":%u,\"camout\":%u,\"bombshop\":%u,\"padw\":%u,\"padb\":%u,\"kinds\":%u}\n",
                kGarageSize, kVehSize, kMaxVeh, kGLeft, kGRight, kGFront, kGBack, kGPosZ, kGTopZ, kGDoorPos, kGTime, kGTarget, kGType, kGState, kGFlags, kTimerAddr, kFrameAddr, kFocusAddr, kMoneyBase,
                kNoResprays, kRespraysFree, kBombsFree, kLastGarage, kLastHelp, kCamOutside, kCamIn, kCamOut, kBombShop, kPadW, kPadB, kKinds);
    return 0;
}
