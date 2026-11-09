// Vuelca las filas `input` del banco Fn137A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F137AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F137AFix::kCases; ++n) std::printf("%s\n", F137AFix::JsonC(F137AFix::MakeC(fn, n)).c_str());
    return 0;
}
