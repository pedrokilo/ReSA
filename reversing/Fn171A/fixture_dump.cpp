// Vuelca las filas `input` del banco Fn171A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F171AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F171AFix::kCases; ++n) std::printf("%s\n", F171AFix::JsonC(F171AFix::MakeC(fn, n)).c_str());
    return 0;
}
