// Vuelca las filas `input` del banco Fn101A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F101AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F101AFix::kCases; ++n) std::printf("%s\n", F101AFix::JsonC(F101AFix::MakeC(fn, n)).c_str());
    return 0;
}
