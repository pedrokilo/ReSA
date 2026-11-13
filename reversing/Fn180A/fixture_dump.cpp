// Vuelca las filas `input` del banco Fn180A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F180AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F180AFix::kCases; ++n) std::printf("%s\n", F180AFix::JsonC(F180AFix::MakeC(fn, n)).c_str());
    return 0;
}
