// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp
// (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de disposicion y muestras del generador de transformaciones.
#include <cstdio>
#include "Fixtures.hpp"
using namespace G63Fix;
int main() {
    for (u32 fn = 0; fn < kFns; ++fn)
        for (u32 n = 0; n < kCases; ++n) std::printf("%u\t%s\n", fn, JsonSpec(MakeSpec(fn, n)).c_str());
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2));
    std::printf("S\t{\"garage\":%u,\"veh\":%u,\"max_veh\":%u,\"left\":%u,\"right\":%u,\"front\":%u,\"back\":%u,\"posz\":%u,\"topz\":%u,\"door\":%u,\"time\":%u,\"type\":%u,\"state\":%u,\"flags\":%u,\"pool\":%u,"
                "\"pos\":%u,\"matrix\":%u,\"b36\":%u,\"speed\":%u,\"f428\":%u,\"vtype\":%u,\"sph_stride\":%u,\"sph_words\":%u,\"mat_words\":%u}\n",
                kGarageSize, kVehSize, kMaxVeh, kGLeft, kGRight, kGFront, kGBack, kGPosZ, kGTopZ, kGDoorPos, kGTime, kGType, kGState, kGFlags, kPoolAddr, kOffPos, kOffMatrix, kOffB36, kOffSpeed, kOffFlags428,
                kOffVType, kSphWords * 4, kSphWords, kMatWords);
    return 0;
}
