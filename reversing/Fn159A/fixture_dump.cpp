// Vuelca las filas `input` del banco Fn159A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F159AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F159AFix::kCases; ++n) std::printf("%s\n", F159AFix::JsonC(F159AFix::MakeC(fn, n)).c_str());
    return 0;
}
