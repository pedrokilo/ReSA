// Vuelca las filas `input` del banco Fn152A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F152AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F152AFix::kCases; ++n) std::printf("%s\n", F152AFix::JsonC(F152AFix::MakeC(fn, n)).c_str());
    return 0;
}
