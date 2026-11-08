// Vuelca las filas `input` del banco Fn122A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F122AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F122AFix::kCases; ++n) std::printf("%s\n", F122AFix::JsonC(F122AFix::MakeC(fn, n)).c_str());
    return 0;
}
