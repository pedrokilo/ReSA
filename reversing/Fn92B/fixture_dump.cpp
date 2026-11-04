// Vuelca las filas `input` del banco Fn92B (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F92BFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F92BFix::kCases; ++n) std::printf("%s\n", F92BFix::JsonC(F92BFix::MakeC(fn, n)).c_str());
    return 0;
}
