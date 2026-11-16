// Vuelca las filas `input` del banco Fn202A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F202AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F202AFix::kCases; ++n) std::printf("%s\n", F202AFix::JsonC(F202AFix::MakeC(fn, n)).c_str());
    return 0;
}
