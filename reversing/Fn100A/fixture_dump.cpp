// Vuelca las filas `input` del banco Fn100A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F100AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F100AFix::kCases; ++n) std::printf("%s\n", F100AFix::JsonC(F100AFix::MakeC(fn, n)).c_str());
    return 0;
}
