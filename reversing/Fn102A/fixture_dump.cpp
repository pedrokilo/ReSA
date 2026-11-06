// Vuelca las filas `input` del banco Fn102A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F102AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F102AFix::kCases; ++n) std::printf("%s\n", F102AFix::JsonC(F102AFix::MakeC(fn, n)).c_str());
    return 0;
}
