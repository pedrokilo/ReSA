// Vuelca las filas `input` del banco Fn179A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F179AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F179AFix::kCases; ++n) std::printf("%s\n", F179AFix::JsonC(F179AFix::MakeC(fn, n)).c_str());
    return 0;
}
