// Vuelca las filas `input` de los 16384 casos de Fixtures.hpp (el mismo archivo que usa el banco) y las constantes de arena, en JSON por lineas.
// Lo compila fixture_selfcheck.py con cl (solo C++ estandar).
#include <cstdio>
#include "Fixtures.hpp"

int main() {
    using namespace Lc70Fix;
    std::printf("{\"layout\":{\"guard\":%u,\"task\":%u,\"sub\":%u,\"veh\":%u,\"ped\":%u,\"mat\":%u,\"ev\":%u,\"cases\":%u}}\n",
                kGuard, kTaskArena, kSubArena, kVehArena, kPedArena, kMatArena, kEvArena, kCases);
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"row\":%s}\n", JsonC(MakeC(n)).c_str());
    return 0;
}
