// Vuelca las filas `input` de los 16384 casos de Fixtures.hpp (el mismo archivo que usa el banco) y las constantes de arena, en JSON por lineas.
// Lo compila fixture_selfcheck.py con cl (solo C++ estandar).
#include <cstdio>
#include "Fixtures.hpp"

int main() {
    using namespace Pu68Fix;
    std::printf("{\"layout\":{\"guard\":%u,\"task\":%u,\"sub\":%u,\"ped\":%u,\"mat\":%u,\"pad\":%u,\"block\":%u,\"groups\":%u,\"cases\":%u}}\n",
                kGuard, kTaskArena, kSubArena, kPedArena, kMatArena, kPadArena, kBlockArena, kGroupsArena, kCases);
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"row\":%s}\n", JsonC(MakeC(n)).c_str());
    return 0;
}
