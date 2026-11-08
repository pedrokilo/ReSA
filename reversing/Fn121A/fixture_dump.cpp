// Vuelca las filas `input` del banco Fn121A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F121AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F121AFix::kCases; ++n) std::printf("%s\n", F121AFix::JsonC(F121AFix::MakeC(fn, n)).c_str());
    return 0;
}
