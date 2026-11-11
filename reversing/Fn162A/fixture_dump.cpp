// Vuelca las filas `input` del banco Fn162A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F162AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F162AFix::kCases; ++n) std::printf("%s\n", F162AFix::JsonC(F162AFix::MakeC(fn, n)).c_str());
    return 0;
}
