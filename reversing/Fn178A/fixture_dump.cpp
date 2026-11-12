// Vuelca las filas `input` del banco Fn178A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F178AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F178AFix::kCases; ++n) std::printf("%s\n", F178AFix::JsonC(F178AFix::MakeC(fn, n)).c_str());
    return 0;
}
