// Vuelca las filas `input` del banco Fn198A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F198AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F198AFix::kCases; ++n) std::printf("%s\n", F198AFix::JsonC(F198AFix::MakeC(fn, n)).c_str());
    return 0;
}
