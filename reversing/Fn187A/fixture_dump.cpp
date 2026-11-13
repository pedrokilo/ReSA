// Vuelca las filas `input` del banco Fn187A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F187AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F187AFix::kCases; ++n) std::printf("%s\n", F187AFix::JsonC(F187AFix::MakeC(fn, n)).c_str());
    return 0;
}
