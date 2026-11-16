// Vuelca las filas `input` del banco Fn205A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F205AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F205AFix::kCases; ++n) std::printf("%s\n", F205AFix::JsonC(F205AFix::MakeC(fn, n)).c_str());
    return 0;
}
