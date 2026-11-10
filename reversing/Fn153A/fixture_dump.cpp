// Vuelca las filas `input` del banco Fn153A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F153AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F153AFix::kCases; ++n) std::printf("%s\n", F153AFix::JsonC(F153AFix::MakeC(fn, n)).c_str());
    return 0;
}
