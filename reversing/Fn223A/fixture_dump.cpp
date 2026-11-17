// Vuelca las filas input del banco Fn223A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F223AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F223AFix::kCases; ++n) std::printf("%s\n", F223AFix::JsonC(F223AFix::MakeC(fn, n)).c_str());
    return 0;
}
