// Volcado de las fixtures (solo para contrastar Fixtures.hpp con fixture_mirror.py; sin juego). Compilar: cl /EHsc /std:c++20 fixture_dump.cpp
#include <cstdio>
#include "Fixtures.hpp"
using namespace PedSaveExtract28Fix;
int main() {
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        std::printf("%u %d ", n, f.family);
        for (u8 b : f.save) std::printf("%02x", b);
        std::printf(" %d %u %u %u %u %u %u %u %u %u %d %d %u %u %u %u", int(f.hasMatrix), f.pedMode, f.pedSeed, f.pedConst, f.matMode, f.matSeed, f.matConst, f.statesMode, f.statesSeed, f.statesConst,
                    int(f.stateValid), f.exitIdx, f.stateByte, f.eax, f.edx, f.stack);
        for (const auto& i : f.infos) std::printf(" %u,%d,%d", i.type, i.model1, i.model2);
        std::printf("\n");
    }
}
