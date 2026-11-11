// Vuelca las filas `input` del banco Fn161A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F161AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F161AFix::kCases; ++n) std::printf("%s\n", F161AFix::JsonC(F161AFix::MakeC(fn, n)).c_str());
    return 0;
}
