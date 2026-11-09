// Vuelca las filas `input` del banco Fn139A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F139AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F139AFix::kCases; ++n) std::printf("%s\n", F139AFix::JsonC(F139AFix::MakeC(fn, n)).c_str());
    return 0;
}
