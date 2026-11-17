// Vuelca las filas input del banco Fn227A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F227AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F227AFix::kCases; ++n) std::printf("%s\n", F227AFix::JsonC(F227AFix::MakeC(fn, n)).c_str());
    return 0;
}
