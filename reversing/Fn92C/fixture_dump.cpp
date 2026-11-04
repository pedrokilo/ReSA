// Vuelca las filas `input` del banco Fn92C (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F92CFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F92CFix::kCases; ++n) std::printf("%s\n", F92CFix::JsonC(F92CFix::MakeC(fn, n)).c_str());
    return 0;
}
