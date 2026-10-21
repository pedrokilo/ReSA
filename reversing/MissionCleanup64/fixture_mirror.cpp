// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`json`) con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Las ultimas lineas: muestras del generador pseudoaleatorio (R) y constantes de disposicion (S).
#include <cstdio>
#include "Fixtures.hpp"
using namespace MC64Fix;
int main() {
    for (u32 f = 0; f < kFns; ++f)
        for (u32 n = 0; n < (f ? kCasesC : kCasesP); ++n) std::printf("C\t%s\n", JsonSpec(MakeSpec(f, n)).c_str());
    for (u32 k = 0; k < 16; ++k) std::printf("R\t[%u,%u,%u,%u]\n", 0x1234567u + k, k, Rand(0x1234567u + k, k * 4 + 2), BgByte(0x1234567u + k, 0xA4448Au + k));
    std::printf("S\t{\"kinds\":%u,\"entries\":%u,\"slots\":%u,\"veh\":%u,\"ped\":%u,\"obj\":%u,\"regions\":[", kKinds, kEntries, kSlots, kVehSize, kPedSize, kObjSize);
    for (std::size_t i = 0; i < sizeof(kRegions) / sizeof(kRegions[0]); ++i) std::printf("%s[%u,%u]", i ? "," : "", kRegions[i].addr, kRegions[i].size);
    std::printf("],\"seq\":[%u,%u],\"fx\":%u,\"pad\":%u}\n", kSeqBase, kSeqStride, kFxBase, kPadBase);
    return 0;
}
