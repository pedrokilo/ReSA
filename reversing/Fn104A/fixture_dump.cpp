// Vuelca las filas `input` del banco Fn104A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F104AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F104AFix::kCases; ++n) std::printf("%s\n", F104AFix::JsonC(F104AFix::MakeC(fn, n)).c_str());
    return 0;
}
