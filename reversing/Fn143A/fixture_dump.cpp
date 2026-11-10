// Vuelca las filas `input` del banco Fn143A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F143AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F143AFix::kCases; ++n) std::printf("%s\n", F143AFix::JsonC(F143AFix::MakeC(fn, n)).c_str());
    return 0;
}
