// Vuelca las filas `input` del banco Fn188A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F188AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F188AFix::kCases; ++n) std::printf("%s\n", F188AFix::JsonC(F188AFix::MakeC(fn, n)).c_str());
    return 0;
}
