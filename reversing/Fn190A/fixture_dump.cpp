// Vuelca las filas `input` del banco Fn190A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F190AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F190AFix::kCases; ++n) std::printf("%s\n", F190AFix::JsonC(F190AFix::MakeC(fn, n)).c_str());
    return 0;
}
