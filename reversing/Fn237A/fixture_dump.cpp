// Vuelca las filas `input` del banco Fn237A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F237AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F237AFix::kCases; ++n) std::printf("%s\n", F237AFix::JsonC(F237AFix::MakeC(fn, n)).c_str());
    return 0;
}
