// Vuelca las filas `input` del banco Fn212A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F212AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F212AFix::kCases; ++n) std::printf("%s\n", F212AFix::JsonC(F212AFix::MakeC(fn, n)).c_str());
    return 0;
}
