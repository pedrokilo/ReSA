// Vuelca las filas `input` del banco Fn175A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F175AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F175AFix::kCases; ++n) std::printf("%s\n", F175AFix::JsonC(F175AFix::MakeC(fn, n)).c_str());
    return 0;
}
