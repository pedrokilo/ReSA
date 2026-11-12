// Vuelca las filas `input` del banco Fn174A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F174AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F174AFix::kCases; ++n) std::printf("%s\n", F174AFix::JsonC(F174AFix::MakeC(fn, n)).c_str());
    return 0;
}
