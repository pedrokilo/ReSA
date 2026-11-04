// Vuelca las filas `input` del banco Fn92A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F92AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F92AFix::kCases; ++n) std::printf("%s\n", F92AFix::JsonC(F92AFix::MakeC(fn, n)).c_str());
    return 0;
}
