// Vuelca las filas `input` del banco Fn135A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F135AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F135AFix::kCases; ++n) std::printf("%s\n", F135AFix::JsonC(F135AFix::MakeC(fn, n)).c_str());
    return 0;
}
