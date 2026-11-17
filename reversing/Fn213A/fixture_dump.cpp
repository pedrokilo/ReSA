// Vuelca las filas `input` del banco Fn213A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F213AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F213AFix::kCases; ++n) std::printf("%s\n", F213AFix::JsonC(F213AFix::MakeC(fn, n)).c_str());
    return 0;
}
