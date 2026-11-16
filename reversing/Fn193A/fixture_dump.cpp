// Vuelca las filas `input` del banco Fn193A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F193AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F193AFix::kCases; ++n) std::printf("%s\n", F193AFix::JsonC(F193AFix::MakeC(fn, n)).c_str());
    return 0;
}
