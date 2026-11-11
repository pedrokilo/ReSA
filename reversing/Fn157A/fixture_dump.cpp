// Vuelca las filas `input` del banco Fn157A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F157AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F157AFix::kCases; ++n) std::printf("%s\n", F157AFix::JsonC(F157AFix::MakeC(fn, n)).c_str());
    return 0;
}
