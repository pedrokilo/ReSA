// Vuelca las filas `input` del banco Fn186A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F186AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F186AFix::kCases; ++n) std::printf("%s\n", F186AFix::JsonC(F186AFix::MakeC(fn, n)).c_str());
    return 0;
}
