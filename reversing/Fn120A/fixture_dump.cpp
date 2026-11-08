// Vuelca las filas `input` del banco Fn120A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F120AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F120AFix::kCases; ++n) std::printf("%s\n", F120AFix::JsonC(F120AFix::MakeC(fn, n)).c_str());
    return 0;
}
