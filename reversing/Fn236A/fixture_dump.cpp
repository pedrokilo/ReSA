// Vuelca las filas `input` del banco Fn236A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F236AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F236AFix::kCases; ++n) std::printf("%s\n", F236AFix::JsonC(F236AFix::MakeC(fn, n)).c_str());
    return 0;
}
