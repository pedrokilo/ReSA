// Vuelca las filas `input` del banco Fn99A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F99AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F99AFix::kCases; ++n) std::printf("%s\n", F99AFix::JsonC(F99AFix::MakeC(fn, n)).c_str());
    return 0;
}
