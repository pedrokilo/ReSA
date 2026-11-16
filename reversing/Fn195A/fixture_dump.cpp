// Vuelca las filas `input` del banco Fn195A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F195AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F195AFix::kCases; ++n) std::printf("%s\n", F195AFix::JsonC(F195AFix::MakeC(fn, n)).c_str());
    return 0;
}
