// Vuelca las filas `input` del banco Fn197A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F197AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F197AFix::kCases; ++n) std::printf("%s\n", F197AFix::JsonC(F197AFix::MakeC(fn, n)).c_str());
    return 0;
}
