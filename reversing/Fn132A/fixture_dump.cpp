// Vuelca las filas `input` del banco Fn132A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F132AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F132AFix::kCases; ++n) std::printf("%s\n", F132AFix::JsonC(F132AFix::MakeC(fn, n)).c_str());
    return 0;
}
