// Vuelca las filas `input` del banco Fn217A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F217AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F217AFix::kCases; ++n) std::printf("%s\n", F217AFix::JsonC(F217AFix::MakeC(fn, n)).c_str());
    return 0;
}
