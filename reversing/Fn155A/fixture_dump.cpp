// Vuelca las filas `input` del banco Fn155A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F155AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F155AFix::kCases; ++n) std::printf("%s\n", F155AFix::JsonC(F155AFix::MakeC(fn, n)).c_str());
    return 0;
}
