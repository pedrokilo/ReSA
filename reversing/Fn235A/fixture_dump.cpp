// Vuelca las filas `input` del banco Fn235A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F235AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F235AFix::kCases; ++n) std::printf("%s\n", F235AFix::JsonC(F235AFix::MakeC(fn, n)).c_str());
    return 0;
}
