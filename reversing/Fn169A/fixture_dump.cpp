// Vuelca las filas `input` del banco Fn169A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F169AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F169AFix::kCases; ++n) std::printf("%s\n", F169AFix::JsonC(F169AFix::MakeC(fn, n)).c_str());
    return 0;
}
