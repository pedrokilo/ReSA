// Vuelca las filas `input` del banco Fn165A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py.
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned fn = 0; fn < F165AFix::kFunctions; ++fn)
        for (unsigned n = 0; n < F165AFix::kCases; ++n) std::printf("%s\n", F165AFix::JsonC(F165AFix::MakeC(fn, n)).c_str());
    return 0;
}
