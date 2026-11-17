// Vuelca las filas `input` del banco Fn214A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F214AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F214AFix::kCases; ++n) std::printf("%s\n", F214AFix::JsonC(F214AFix::MakeC(fn, n)).c_str());
    return 0;
}
