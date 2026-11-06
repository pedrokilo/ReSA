// Vuelca las filas `input` del banco Fn108A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F108AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F108AFix::kCases; ++n) std::printf("%s\n", F108AFix::JsonC(F108AFix::MakeC(fn, n)).c_str());
    return 0;
}
