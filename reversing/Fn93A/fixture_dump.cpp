// Vuelca las filas `input` del banco Fn93A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F93AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F93AFix::kCases; ++n) std::printf("%s\n", F93AFix::JsonC(F93AFix::MakeC(fn, n)).c_str());
    return 0;
}
