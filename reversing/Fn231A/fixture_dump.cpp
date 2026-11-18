// Vuelca las filas `input` del banco Fn231A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F231AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F231AFix::kCases; ++n) std::printf("%s\n", F231AFix::JsonC(F231AFix::MakeC(fn, n)).c_str());
    return 0;
}
