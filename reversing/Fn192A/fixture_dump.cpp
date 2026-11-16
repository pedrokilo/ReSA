// Vuelca las filas `input` del banco Fn192A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F192AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F192AFix::kCases; ++n) std::printf("%s\n", F192AFix::JsonC(F192AFix::MakeC(fn, n)).c_str());
    return 0;
}
