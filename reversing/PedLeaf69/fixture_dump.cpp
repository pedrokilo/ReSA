// Vuelca las filas `input` de los 4 x 4096 casos de Fixtures.hpp (el mismo archivo que usa el banco) y las constantes de arena, en JSON por lineas.
// Lo compila fixture_selfcheck.py con cl (solo C++ estandar).
#include <cstdio>
#include "Fixtures.hpp"

int main() {
    using namespace Pl69Fix;
    std::printf("{\"layout\":{\"guard\":%u,\"intel\":%u,\"ent\":%u,\"mat\":%u,\"vec\":%u,\"stats\":%u,\"cases\":%u}}\n", kGuard, kIntelArena, kEntArena, kMatArena, kVecArena, kStatsArena, kCases);
    for (unsigned n = 0; n < kCases; ++n) {
        std::printf("{\"func\":0,\"row\":%s}\n", JsonC0(MakeC0(n)).c_str());
        std::printf("{\"func\":1,\"row\":%s}\n", JsonC1(MakeC1(n)).c_str());
        std::printf("{\"func\":2,\"row\":%s}\n", JsonC2(MakeC2(n)).c_str());
        std::printf("{\"func\":3,\"row\":%s}\n", JsonC3(MakeC3(n)).c_str());
    }
    return 0;
}
