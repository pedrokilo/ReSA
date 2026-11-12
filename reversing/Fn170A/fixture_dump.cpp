// Vuelca las filas `input` del banco Fn170A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F170AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F170AFix::kCases; ++n) std::printf("%s\n", F170AFix::JsonC(F170AFix::MakeC(fn, n)).c_str());
    return 0;
}
