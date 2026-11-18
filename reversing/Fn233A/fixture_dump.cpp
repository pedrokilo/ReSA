// Vuelca las filas `input` del banco Fn233A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F233AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F233AFix::kCases; ++n) std::printf("%s\n", F233AFix::JsonC(F233AFix::MakeC(fn, n)).c_str());
    return 0;
}
