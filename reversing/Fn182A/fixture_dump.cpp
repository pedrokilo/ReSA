// Vuelca las filas `input` del banco Fn182A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F182AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F182AFix::kCases; ++n) std::printf("%s\n", F182AFix::JsonC(F182AFix::MakeC(fn, n)).c_str());
    return 0;
}
