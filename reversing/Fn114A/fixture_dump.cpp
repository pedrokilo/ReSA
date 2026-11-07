// Vuelca las filas `input` del banco Fn114A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F114AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F114AFix::kCases; ++n) std::printf("%s\n", F114AFix::JsonC(F114AFix::MakeC(fn, n)).c_str());
    return 0;
}
