// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras del generador pseudoaleatorio (R) y constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace CS63DFix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("C\t%s\n", JsonSpec(MakeSpec(n)).c_str());
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2));
    std::printf("S\t{\"veh\":%u,\"mat\":%u,\"speed\":%u,\"ent\":%u,\"entmat\":%u,\"matwords\":%u,\"kinds\":%u}\n", kVehSize, kVMat, kVSpeed, kEntSize, kEMat, kMatWords, kKinds);
    return 0;
}
