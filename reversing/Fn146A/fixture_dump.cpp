// Vuelca las filas `input` del banco Fn146A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F146AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F146AFix::kCases; ++n) std::printf("%s\n", F146AFix::JsonC(F146AFix::MakeC(fn, n)).c_str());
    return 0;
}
