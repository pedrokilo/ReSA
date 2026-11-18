// Vuelca las filas `input` del banco Fn234A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F234AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F234AFix::kCases; ++n) std::printf("%s\n", F234AFix::JsonC(F234AFix::MakeC(fn, n)).c_str());
    return 0;
}
