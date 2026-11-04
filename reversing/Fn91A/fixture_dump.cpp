// Vuelca las filas `input` del banco Fn91A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F91AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F91AFix::kCases; ++n) std::printf("%s\n", F91AFix::JsonC(F91AFix::MakeC(fn, n)).c_str());
    return 0;
}
