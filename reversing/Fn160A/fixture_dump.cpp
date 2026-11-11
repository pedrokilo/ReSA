// Vuelca las filas `input` del banco Fn160A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F160AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F160AFix::kCases; ++n) std::printf("%s\n", F160AFix::JsonC(F160AFix::MakeC(fn, n)).c_str());
    return 0;
}
