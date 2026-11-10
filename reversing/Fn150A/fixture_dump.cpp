// Vuelca las filas `input` del banco Fn150A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F150AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F150AFix::kCases; ++n) std::printf("%s\n", F150AFix::JsonC(F150AFix::MakeC(fn, n)).c_str());
    return 0;
}
