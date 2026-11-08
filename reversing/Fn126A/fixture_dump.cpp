// Vuelca las filas `input` del banco Fn126A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F126AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F126AFix::kCases; ++n) std::printf("%s\n", F126AFix::JsonC(F126AFix::MakeC(fn, n)).c_str());
    return 0;
}
