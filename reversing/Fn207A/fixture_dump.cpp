// Vuelca las filas `input` del banco Fn207A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F207AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F207AFix::kCases; ++n) std::printf("%s\n", F207AFix::JsonC(F207AFix::MakeC(fn, n)).c_str());
    return 0;
}
