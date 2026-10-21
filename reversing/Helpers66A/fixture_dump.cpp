// Vuelca las filas `input` de los 5 x 4096 casos de Fixtures.hpp (el mismo archivo que usa el banco) y las constantes de arena, en JSON por lineas.
// Lo compila fixture_selfcheck.py con cl (solo C++ estandar).
#include <cstdio>
#include "Fixtures.hpp"

int main() {
    using namespace Hp66Fix;
    std::printf("{\"layout\":{\"guard\":%u,\"ent\":%u,\"mat\":%u,\"vec\":%u,\"ped\":%u,\"player\":%u,\"task\":%u,\"cam\":%u,\"smoke\":%u,\"groups\":%u,\"cases\":%u}}\n",
                kGuard, kEntArena, kMatArena, kVecArena, kPedArena, kPlayerArena, kTaskArena, kCamArena, kSmokeArena, kGroupsBytes, kCases);
    for (unsigned n = 0; n < kCases; ++n) {
        std::printf("{\"func\":0,\"row\":%s}\n", JsonC0(MakeC0(n)).c_str());
        std::printf("{\"func\":1,\"row\":%s}\n", JsonC1(MakeC1(n)).c_str());
        std::printf("{\"func\":2,\"row\":%s}\n", JsonC2(MakeC2(n)).c_str());
        std::printf("{\"func\":3,\"row\":%s}\n", JsonC3(MakeC3(n)).c_str());
        std::printf("{\"func\":4,\"row\":%s}\n", JsonC4(MakeC4(n)).c_str());
    }
    return 0;
}
