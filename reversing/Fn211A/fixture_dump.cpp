// Vuelca las filas `input` del banco Fn211A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F211AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F211AFix::kCases; ++n) std::printf("%s\n", F211AFix::JsonC(F211AFix::MakeC(fn, n)).c_str());
    return 0;
}
