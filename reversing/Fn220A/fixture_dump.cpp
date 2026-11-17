// Vuelca las filas `input` del banco Fn220A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F220AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F220AFix::kCases; ++n) std::printf("%s\n", F220AFix::JsonC(F220AFix::MakeC(fn, n)).c_str());
    return 0;
}
