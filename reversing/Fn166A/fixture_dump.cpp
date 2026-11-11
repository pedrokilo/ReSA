// Vuelca las filas `input` del banco Fn166A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F166AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F166AFix::kCases; ++n) std::printf("%s\n", F166AFix::JsonC(F166AFix::MakeC(fn, n)).c_str());
    return 0;
}
