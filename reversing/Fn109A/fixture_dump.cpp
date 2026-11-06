// Vuelca las filas `input` del banco Fn109A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F109AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F109AFix::kCases; ++n) std::printf("%s\n", F109AFix::JsonC(F109AFix::MakeC(fn, n)).c_str());
    return 0;
}
