// Vuelca las filas `input` de los 16384 casos de Fixtures.hpp (el mismo archivo que usa el banco) y las constantes de arena, en JSON por lineas.
// Lo compila fixture_selfcheck.py con cl (solo C++ estandar).
#include <cstdio>
#include "Fixtures.hpp"

int main() {
    using namespace Leave70Fix;
    std::printf("{\"layout\":{\"guard\":%u,\"task\":%u,\"ped\":%u,\"veh\":%u,\"grp\":%u,\"ev\":%u,\"alloc\":%u,\"cases\":%u}}\n",
                kGuard, kTaskArena, kPedArena, kVehArena, kGrpArena, kEvArena, kAllocArena, kCases);
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"row\":%s}\n", JsonC(MakeC(n)).c_str());
    return 0;
}
