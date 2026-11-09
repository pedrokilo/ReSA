// Vuelca las filas `input` del banco Fn136A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F136AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F136AFix::kCases; ++n) std::printf("%s\n", F136AFix::JsonC(F136AFix::MakeC(fn, n)).c_str());
    return 0;
}
