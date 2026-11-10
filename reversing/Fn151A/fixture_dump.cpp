// Vuelca las filas `input` del banco Fn151A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F151AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F151AFix::kCases; ++n) std::printf("%s\n", F151AFix::JsonC(F151AFix::MakeC(fn, n)).c_str());
    return 0;
}
