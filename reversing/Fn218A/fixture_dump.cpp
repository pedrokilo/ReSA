// Vuelca las filas `input` del banco Fn218A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F218AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F218AFix::kCases; ++n) std::printf("%s\n", F218AFix::JsonC(F218AFix::MakeC(fn, n)).c_str());
    return 0;
}
