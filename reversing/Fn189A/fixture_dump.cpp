// Vuelca las filas `input` del banco Fn189A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F189AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F189AFix::kCases; ++n) std::printf("%s\n", F189AFix::JsonC(F189AFix::MakeC(fn, n)).c_str());
    return 0;
}
