// Vuelca las filas `input` del banco Fn96A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F96AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F96AFix::kCases; ++n) std::printf("%s\n", F96AFix::JsonC(F96AFix::MakeC(fn, n)).c_str());
    return 0;
}
